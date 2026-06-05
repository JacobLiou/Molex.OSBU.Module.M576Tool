#include "Peak1DSweepRecenter.h"
#include "M576Peak1DConstants.h"

#include <algorithm>
#include <cmath>

namespace M576
{
	const char* SweepTrendName(SweepTrend t)
	{
		switch (t)
		{
		case SweepTrend::Flat: return "Flat";
		case SweepTrend::StrictInc: return "StrictInc";
		case SweepTrend::StrictDec: return "StrictDec";
		case SweepTrend::NonMono: return "NonMono";
		default: return "Unknown";
		}
	}

	namespace
	{
		static double EpsMonoFromSpan(double spanPts)
		{
			return (std::max)((double)M576_PEAK1D_MIN_ABS_EPS_DB, spanPts * (double)M576_PEAK1D_EPS_REL_OF_SPAN);
		}

		static bool IsStrictIncreasing(const std::vector<double>& ys, double epsMono)
		{
			if (ys.size() < 2)
				return false;
			for (size_t k = 1; k < ys.size(); ++k)
			{
				if (ys[k] <= ys[k - 1] + epsMono)
					return false;
			}
			return true;
		}

		static bool IsStrictDecreasing(const std::vector<double>& ys, double epsMono)
		{
			if (ys.size() < 2)
				return false;
			for (size_t k = 1; k < ys.size(); ++k)
			{
				if (ys[k] >= ys[k - 1] - epsMono)
					return false;
			}
			return true;
		}

		static double LinearSlopePerIndex(const std::vector<int>& idx, const std::vector<double>& ys)
		{
			if (idx.size() < 2 || idx.size() != ys.size())
				return 0.0;
			const size_t n = idx.size();
			double sumX = 0.0;
			double sumY = 0.0;
			double sumXX = 0.0;
			double sumXY = 0.0;
			for (size_t k = 0; k < n; ++k)
			{
				const double x = (double)idx[k];
				const double y = ys[k];
				sumX += x;
				sumY += y;
				sumXX += x * x;
				sumXY += x * y;
			}
			const double dn = (double)n;
			const double denom = dn * sumXX - sumX * sumX;
			if (std::abs(denom) < 1e-18)
				return 0.0;
			return (dn * sumXY - sumX * sumY) / denom;
		}

		static double ClampRecenterDeltaDac(double deltaDac, int dacRange)
		{
			const double maxShift = (double)M576_PEAK1D_SWEEP_RECENTER_MAX_SHIFT_FRAC * (double)dacRange;
			if (deltaDac > maxShift)
				return maxShift;
			if (deltaDac < -maxShift)
				return -maxShift;
			return deltaDac;
		}

		static double IndexShiftToDeltaDac(double deltaIndex, int sampleCount, int dacRange)
		{
			if (sampleCount <= 1 || dacRange < 1)
				return 0.0;
			const double step = (2.0 * dacRange) / (double)(sampleCount - 1);
			return deltaIndex * step;
		}

		/// Legacy heuristic: trend + edge argmax + attempt-index frac.
		static double HeuristicDeltaIndex(
			const SweepProfile& profile,
			int sampleCount,
			int attemptIndex)
		{
			if (sampleCount <= 1)
				return 0.0;
			if (attemptIndex < 0)
				attemptIndex = 0;

			double frac = (double)M576_PEAK1D_SWEEP_RECENTER_BASE_FRAC
				+ (double)M576_PEAK1D_SWEEP_RECENTER_ATTEMPT_STEP_FRAC * (double)attemptIndex;

			double deltaIndex = 0.0;
			if (profile.trend == SweepTrend::StrictDec)
			{
				if (profile.argmaxIndex == 0)
					frac += (double)M576_PEAK1D_SWEEP_RECENTER_EDGE_BONUS_FRAC;
				deltaIndex = -frac * (double)(sampleCount - 1);
			}
			else if (profile.trend == SweepTrend::StrictInc)
			{
				if (profile.argmaxIndex == (sampleCount - 1))
					frac += (double)M576_PEAK1D_SWEEP_RECENTER_EDGE_BONUS_FRAC;
				deltaIndex = frac * (double)(sampleCount - 1);
			}
			else if (profile.argmaxIndex == 0)
			{
				deltaIndex = -(double)M576_PEAK1D_SWEEP_RECENTER_EDGE_ARGMAX_FRAC * (double)(sampleCount - 1);
			}
			else if (profile.argmaxIndex == (sampleCount - 1))
			{
				deltaIndex = (double)M576_PEAK1D_SWEEP_RECENTER_EDGE_ARGMAX_FRAC * (double)(sampleCount - 1);
			}
			else if (profile.argmaxIndex <= 1)
			{
				// Near-left-edge quasi-monotone: likely true peak outside left bound.
				deltaIndex = -(double)M576_PEAK1D_SWEEP_RECENTER_EDGE_ARGMAX_FRAC * (double)(sampleCount - 1);
			}
			else if (profile.argmaxIndex >= (sampleCount - 2))
			{
				// Near-right-edge quasi-monotone: likely true peak outside right bound.
				deltaIndex = (double)M576_PEAK1D_SWEEP_RECENTER_EDGE_ARGMAX_FRAC * (double)(sampleCount - 1);
			}
			return deltaIndex;
		}

		static int PeakOutsideSign(const SweepProfile& profile, int sampleCount)
		{
			if (sampleCount <= 1)
				return 0;
			if (profile.trend == SweepTrend::StrictDec)
				return -1;
			if (profile.trend == SweepTrend::StrictInc)
				return +1;
			if (profile.argmaxIndex == 0)
				return -1;
			if (profile.argmaxIndex == (sampleCount - 1))
				return +1;
			if (profile.argmaxIndex <= 1)
				return -1;
			if (profile.argmaxIndex >= (sampleCount - 2))
				return +1;
			return 0;
		}

		static bool IsMonotoneMissFailure(
			Peak1DValidateCode code,
			const SweepProfile& profile)
		{
			return code == Peak1DValidateCode::ParabolaNotDownward
				&& (profile.trend == SweepTrend::StrictInc || profile.trend == SweepTrend::StrictDec);
		}

		static bool ArgmaxStuckOnSameEdge(
			const SweepProfile& profile,
			int sampleCount,
			const SweepRecenterFailureInfo& failure)
		{
			if (!failure.hasPrevAttempt || sampleCount <= 1)
				return false;
			if (profile.argmaxIndex != failure.prevArgmaxIndex)
				return false;
			return profile.argmaxIndex == 0 || profile.argmaxIndex == (sampleCount - 1);
		}

		static double DeltaIndexFromTPeak(double tPeak, int sampleCount)
		{
			const double mid = (double)(sampleCount - 1) * 0.5;
			return tPeak - mid;
		}

		static double ApplyAttemptGrowth(double deltaIndex, int attemptIndex)
		{
			if (deltaIndex == 0.0 || attemptIndex <= 0)
				return deltaIndex;
			const double growth = 1.0 + (double)M576_PEAK1D_SWEEP_RECENTER_ATTEMPT_GROWTH * (double)attemptIndex;
			return deltaIndex * growth;
		}

		static double EnforcePeakDirection(double deltaIndex, int outsideSign)
		{
			if (outsideSign == 0 || deltaIndex == 0.0)
				return deltaIndex;
			const int sign = (deltaIndex > 0.0) ? 1 : ((deltaIndex < 0.0) ? -1 : 0);
			if (sign == 0 || sign == outsideSign)
				return deltaIndex;
			return (double)outsideSign * std::abs(deltaIndex);
		}

		static bool CollectValidSamples(
			const std::vector<double>& powY,
			std::vector<int>& outIdx,
			std::vector<double>& outY)
		{
			outIdx.clear();
			outY.clear();
			const int n = (int)powY.size();
			outIdx.reserve((size_t)n);
			outY.reserve((size_t)n);
			for (int i = 0; i < n; ++i)
			{
				if (IsRecal1DPowerInvalidValue(powY[(size_t)i]))
					continue;
				outIdx.push_back(i);
				outY.push_back(powY[(size_t)i]);
			}
			return !outY.empty();
		}

		static SweepTrend TrendFromValidYSeries(const std::vector<int>& idx, const std::vector<double>& ys)
		{
			if (ys.size() < (size_t)M576_PEAK1D_CUBIC_MIN_SAMPLES)
				return SweepTrend::Flat;
			double vmin = ys[0];
			double vmax = ys[0];
			for (double y : ys)
			{
				vmin = (std::min)(vmin, y);
				vmax = (std::max)(vmax, y);
			}
			const double span = vmax - vmin;
			const double maxAbs = (std::max)(std::abs(vmin), std::abs(vmax));
			const bool relFlat = (maxAbs > 1e-6 && span / maxAbs < (double)M576_PEAK1D_FLAT_REL_SPAN_FRAC);
			if (span < (double)M576_PEAK1D_MIN_SPAN_DB || relFlat)
				return SweepTrend::Flat;
			const double epsMono = EpsMonoFromSpan(span);
			if (IsStrictIncreasing(ys, epsMono))
				return SweepTrend::StrictInc;
			if (IsStrictDecreasing(ys, epsMono))
				return SweepTrend::StrictDec;
			return SweepTrend::NonMono;
		}

		static SweepTrend AnalyzeRecal1DSweepTailTrend(const std::vector<double>& powY, int minTailSamples)
		{
			std::vector<int> validIdx;
			std::vector<double> validY;
			if (!CollectValidSamples(powY, validIdx, validY))
				return SweepTrend::Flat;
			const int n = (int)validY.size();
			if (n < minTailSamples)
				return SweepTrend::Unknown;
			const int tailCount = (std::max)(minTailSamples, n / 2);
			const int start = n - tailCount;
			std::vector<int> tailIdx(validIdx.begin() + start, validIdx.end());
			std::vector<double> tailY(validY.begin() + start, validY.end());
			return TrendFromValidYSeries(tailIdx, tailY);
		}

		static bool IsPeakOutsideLeftWindow(const SweepRecenterFailureInfo* failure)
		{
			if (!failure || !failure->hasTPeak || !std::isfinite(failure->tPeak))
				return false;
			return failure->tPeak < -0.01;
		}

		static bool IsPeakOutsideRightWindow(const SweepRecenterFailureInfo* failure, int sampleCount)
		{
			if (!failure || !failure->hasTPeak || !std::isfinite(failure->tPeak))
				return false;
			return failure->tPeak > (double)(sampleCount - 1) + 0.01;
		}

		static bool IsVertexOutsideLeft(
			const SweepRecenterFailureInfo* failure,
			int sampleCount)
		{
			if (!IsPeakOutsideLeftWindow(failure))
				return false;
			return failure->code == Peak1DValidateCode::VertexOutOfRange;
		}

		static bool IsVertexOutsideRight(
			const SweepRecenterFailureInfo* failure,
			int sampleCount)
		{
			if (!IsPeakOutsideRightWindow(failure, sampleCount))
				return false;
			return failure->code == Peak1DValidateCode::VertexOutOfRange;
		}
	}

	SweepProfile AnalyzeRecal1DSweepProfile(const std::vector<double>& powY)
	{
		SweepProfile out;
		const int n = (int)powY.size();
		if (n <= 0)
		{
			out.trend = SweepTrend::Flat;
			return out;
		}

		std::vector<int> validIdx;
		std::vector<double> validY;
		validIdx.reserve((size_t)n);
		validY.reserve((size_t)n);
		for (int i = 0; i < n; ++i)
		{
			if (IsRecal1DPowerInvalidValue(powY[(size_t)i]))
				continue;
			validIdx.push_back(i);
			validY.push_back(powY[(size_t)i]);
		}

		out.validCount = (int)validIdx.size();
		if (out.validCount <= 0)
		{
			out.trend = SweepTrend::Flat;
			return out;
		}

		double vmin = validY[0];
		double vmax = validY[0];
		int argmaxLocal = 0;
		for (int k = 0; k < out.validCount; ++k)
		{
			if (validY[(size_t)k] > vmax)
			{
				vmax = validY[(size_t)k];
				argmaxLocal = k;
			}
			vmin = (std::min)(vmin, validY[(size_t)k]);
		}
		out.argmaxIndex = validIdx[(size_t)argmaxLocal];
		out.span = vmax - vmin;
		out.slopePerIndex = LinearSlopePerIndex(validIdx, validY);

		const double maxAbs = (std::max)(std::abs(vmin), std::abs(vmax));
		const bool relFlat = (maxAbs > 1e-6 && out.span / maxAbs < (double)M576_PEAK1D_FLAT_REL_SPAN_FRAC);
		if (out.validCount < (int)M576_PEAK1D_CUBIC_MIN_SAMPLES
			|| out.span < (double)M576_PEAK1D_MIN_SPAN_DB
			|| relFlat)
		{
			out.trend = SweepTrend::Flat;
			return out;
		}

		const double epsMono = EpsMonoFromSpan(out.span);
		if (IsStrictIncreasing(validY, epsMono))
			out.trend = SweepTrend::StrictInc;
		else if (IsStrictDecreasing(validY, epsMono))
			out.trend = SweepTrend::StrictDec;
		else
			out.trend = SweepTrend::NonMono;
		return out;
	}

	bool IsFlatSweepFailure(Peak1DValidateCode code, const SweepProfile& profile)
	{
		if (profile.trend != SweepTrend::Flat)
			return false;
		return code == Peak1DValidateCode::ParabolaNotDownward
			|| code == Peak1DValidateCode::LowSpan
			|| code == Peak1DValidateCode::NotEnoughValidSamples;
	}

	bool IsMonotoneSweepFailure(Peak1DValidateCode code, const SweepProfile& profile, int sampleCount)
	{
		if (profile.trend == SweepTrend::Flat || sampleCount <= 0)
			return false;
		if (code != Peak1DValidateCode::ParabolaNotDownward
			&& code != Peak1DValidateCode::NotEnoughValidSamples)
		{
			return false;
		}
		if (profile.trend == SweepTrend::StrictInc || profile.trend == SweepTrend::StrictDec)
			return true;
		if (profile.trend == SweepTrend::NonMono && sampleCount > 1)
		{
			return profile.argmaxIndex <= 1 || profile.argmaxIndex >= (sampleCount - 2);
		}
		return false;
	}

	int SuggestFlatRetryDacRange(int currentRange, int maxRange)
	{
		if (currentRange < 1 || maxRange < 1)
			return 0;
		const int expanded = currentRange * M576_PEAK1D_FLAT_EXPAND_FACTOR;
		const int next = (expanded > maxRange) ? maxRange : expanded;
		if (next <= currentRange)
			return 0;
		return next;
	}

	bool PlanRecalYCrossResweep(
		Peak1DValidateCode crossCode,
		const std::vector<double>& powY,
		double sweepCenterDac,
		int roundIndex,
		int& ioMovingBase,
		int& ioDacRange,
		int& ioPrevArgmax,
		double crossTPeak,
		bool hasCrossTPeak,
		bool& outUsedExpandRange,
		bool& ioMonoRangeExpanded)
	{
		outUsedExpandRange = false;
		if (crossCode == Peak1DValidateCode::Ok || crossCode == Peak1DValidateCode::PmRangeMismatch)
			return false;
		const int n = (int)powY.size();
		if (n <= 0 || ioDacRange < 1)
			return false;

		const SweepProfile profile = AnalyzeRecal1DSweepProfile(powY);

		if (IsFlatSweepFailure(crossCode, profile))
		{
			const int next = SuggestFlatRetryDacRange(ioDacRange, M576_MAX_DAC_RANGE);
			if (next > ioDacRange)
			{
				ioDacRange = next;
				outUsedExpandRange = true;
				return true;
			}
			return false;
		}

		if (IsMonotoneSweepFailure(crossCode, profile, n) && !ioMonoRangeExpanded)
		{
			const int next = SuggestFlatRetryDacRange(ioDacRange, M576_MAX_DAC_RANGE);
			if (next > ioDacRange)
			{
				ioDacRange = next;
				ioMonoRangeExpanded = true;
				outUsedExpandRange = true;
				return true;
			}
		}

		if (IsRetryablePeakFailure(crossCode, profile, n))
		{
			M576::SweepRecenterFailureInfo failInfo = {};
			failInfo.code = crossCode;
			failInfo.tPeak = crossTPeak;
			failInfo.hasTPeak = hasCrossTPeak;
			failInfo.prevArgmaxIndex = ioPrevArgmax;
			failInfo.hasPrevAttempt = (roundIndex > 0);
			ioMovingBase = SuggestSweepRecenterNewBase(
				sweepCenterDac, profile, n, ioDacRange, roundIndex, failInfo);
			ioPrevArgmax = profile.argmaxIndex;
			return true;
		}

		// Pre passed but cross rejected (shallow / weak peak): expand offset first, else nudge base.
		{
			const int next = SuggestFlatRetryDacRange(ioDacRange, M576_MAX_DAC_RANGE);
			if (next > ioDacRange)
			{
				ioDacRange = next;
				outUsedExpandRange = true;
				return true;
			}
		}
		if (profile.trend != SweepTrend::Flat)
		{
			M576::SweepRecenterFailureInfo failInfo = {};
			failInfo.code = crossCode;
			failInfo.tPeak = crossTPeak;
			failInfo.hasTPeak = hasCrossTPeak;
			failInfo.prevArgmaxIndex = ioPrevArgmax;
			failInfo.hasPrevAttempt = (roundIndex > 0);
			ioMovingBase = SuggestSweepRecenterNewBase(
				sweepCenterDac, profile, n, ioDacRange, roundIndex, failInfo);
			ioPrevArgmax = profile.argmaxIndex;
			return true;
		}
		return false;
	}

	bool IsRetryablePeakFailure(
		Peak1DValidateCode code,
		const SweepProfile& profile,
		int sampleCount,
		bool afterFlatExpandRange,
		const std::vector<double>* powSamples,
		const SweepRecenterFailureInfo* failure)
	{
		if (sampleCount <= 0)
			return false;
		if (profile.trend == SweepTrend::Flat)
			return false;
		if (code == Peak1DValidateCode::PmRangeMismatch)
			return false;

		auto baseRetryable = [&]() -> bool
		{
			switch (code)
			{
			case Peak1DValidateCode::ParabolaNotDownward:
				if (profile.trend == SweepTrend::StrictInc || profile.trend == SweepTrend::StrictDec)
					return true;
				if (profile.trend == SweepTrend::NonMono && sampleCount > 1)
				{
					return profile.argmaxIndex <= 1 || profile.argmaxIndex >= (sampleCount - 2);
				}
				return false;
			case Peak1DValidateCode::NotEnoughValidSamples:
				return profile.trend == SweepTrend::StrictInc || profile.trend == SweepTrend::StrictDec;
			case Peak1DValidateCode::EdgeNotAllowed:
				if (sampleCount <= 1)
					return false;
				return profile.argmaxIndex == 0 || profile.argmaxIndex == (sampleCount - 1);
			case Peak1DValidateCode::VertexOutOfRange:
				if (sampleCount <= 1)
					return false;
				if (profile.argmaxIndex == 0 || profile.argmaxIndex == (sampleCount - 1))
					return true;
				// Plateau + peak left of window (e.g. Step 206): t*<0, argmax on left platform.
				if (IsVertexOutsideLeft(failure, sampleCount))
				{
					if (profile.argmaxIndex >= 0 && profile.argmaxIndex <= 1)
						return true;
					if (profile.argmaxIndex <= sampleCount / 3)
						return true;
				}
				if (IsVertexOutsideRight(failure, sampleCount))
				{
					if (profile.argmaxIndex >= sampleCount - 2)
						return true;
					if (profile.argmaxIndex >= (sampleCount * 2) / 3)
						return true;
					// Quasi-monotone, t*>n-1, argmax on left flank (Step 478): peak right of window.
					if (profile.argmaxIndex >= 0 && profile.argmaxIndex <= 1)
						return true;
					if (profile.argmaxIndex <= sampleCount / 3)
						return true;
				}
				return false;
			case Peak1DValidateCode::LowSpan:
			case Peak1DValidateCode::Empty:
				return false;
			default:
				return false;
			}
		};

		if (baseRetryable())
			return true;

		if (!afterFlatExpandRange || powSamples == nullptr || powSamples->empty())
			return false;
		if (code != Peak1DValidateCode::ParabolaNotDownward
			&& code != Peak1DValidateCode::NotEnoughValidSamples
			&& code != Peak1DValidateCode::VertexOutOfRange)
		{
			return false;
		}

		const SweepTrend tailTrend = AnalyzeRecal1DSweepTailTrend(*powSamples, 8);
		if (tailTrend == SweepTrend::StrictDec || tailTrend == SweepTrend::StrictInc)
			return true;
		if (profile.argmaxIndex >= 0 && profile.argmaxIndex <= sampleCount / 3)
			return true;
		if (profile.argmaxIndex >= (sampleCount * 2) / 3)
			return true;
		return false;
	}

	SweepProfile AdjustProfileForMonoRecenter(
		const SweepProfile& profile,
		const std::vector<double>& powSamples,
		bool afterFlatExpandRange,
		const SweepRecenterFailureInfo* failure)
	{
		const int n = (int)powSamples.size();
		if (failure != nullptr && n > 0)
		{
			if (IsPeakOutsideLeftWindow(failure))
			{
				SweepProfile out = profile;
				out.trend = SweepTrend::StrictDec;
				out.argmaxIndex = 0;
				return out;
			}
			if (IsPeakOutsideRightWindow(failure, n))
			{
				SweepProfile out = profile;
				out.trend = SweepTrend::StrictInc;
				out.argmaxIndex = n - 1;
				return out;
			}
		}

		if (!afterFlatExpandRange)
			return profile;
		if (profile.trend == SweepTrend::StrictInc || profile.trend == SweepTrend::StrictDec)
			return profile;

		SweepProfile out = profile;
		const SweepTrend tailTrend = AnalyzeRecal1DSweepTailTrend(powSamples, 8);
		if (tailTrend == SweepTrend::StrictDec)
		{
			out.trend = SweepTrend::StrictDec;
			out.argmaxIndex = 0;
			return out;
		}
		if (tailTrend == SweepTrend::StrictInc)
		{
			out.trend = SweepTrend::StrictInc;
			if (n > 0)
				out.argmaxIndex = n - 1;
			return out;
		}
		if (n > 0 && profile.argmaxIndex >= 0 && profile.argmaxIndex <= n / 3)
		{
			out.trend = SweepTrend::StrictDec;
			out.argmaxIndex = 0;
		}
		else if (n > 0 && profile.argmaxIndex >= (n * 2) / 3)
		{
			out.trend = SweepTrend::StrictInc;
			out.argmaxIndex = n - 1;
		}
		return out;
	}

	double SuggestSweepRecenterDeltaDac(
		const SweepProfile& profile,
		int sampleCount,
		int dacRange,
		int attemptIndex)
	{
		SweepRecenterFailureInfo empty;
		return SuggestSweepRecenterDeltaDac(profile, sampleCount, dacRange, attemptIndex, empty);
	}

	double SuggestSweepRecenterDeltaDac(
		const SweepProfile& profile,
		int sampleCount,
		int dacRange,
		int attemptIndex,
		const SweepRecenterFailureInfo& failure)
	{
		if (sampleCount <= 1 || dacRange < 1)
			return 0.0;
		if (attemptIndex < 0)
			attemptIndex = 0;

		const int outsideSign = PeakOutsideSign(profile, sampleCount);
		const bool monoMiss = IsMonotoneMissFailure(failure.code, profile);

		double deltaIndex = HeuristicDeltaIndex(profile, sampleCount, attemptIndex);
		if (deltaIndex == 0.0 && outsideSign == 0)
			return 0.0;

		const bool useTStar = failure.hasTPeak
			&& std::isfinite(failure.tPeak)
			&& !monoMiss
			&& (failure.code == Peak1DValidateCode::VertexOutOfRange
				|| failure.code == Peak1DValidateCode::EdgeNotAllowed
				|| failure.code == Peak1DValidateCode::NotEnoughValidSamples
				|| profile.trend == SweepTrend::NonMono);

		if (useTStar)
		{
			const double deltaT = DeltaIndexFromTPeak(failure.tPeak, sampleCount);
			const double w = (double)M576_PEAK1D_SWEEP_RECENTER_TSTAR_WEIGHT;
			deltaIndex = w * deltaT + (1.0 - w) * deltaIndex;
		}

		deltaIndex = EnforcePeakDirection(deltaIndex, outsideSign);
		deltaIndex = ApplyAttemptGrowth(deltaIndex, attemptIndex);

		if (ArgmaxStuckOnSameEdge(profile, sampleCount, failure))
		{
			const double stag = 1.0 + (double)M576_PEAK1D_SWEEP_RECENTER_STAGNATION_GAIN;
			deltaIndex *= stag;
		}

		return ClampRecenterDeltaDac(IndexShiftToDeltaDac(deltaIndex, sampleCount, dacRange), dacRange);
	}

	int SuggestSweepRecenterNewBase(
		double centerDac,
		const SweepProfile& profile,
		int sampleCount,
		int dacRange,
		int attemptIndex)
	{
		SweepRecenterFailureInfo empty;
		return SuggestSweepRecenterNewBase(centerDac, profile, sampleCount, dacRange, attemptIndex, empty);
	}

	int SuggestSweepRecenterNewBase(
		double centerDac,
		const SweepProfile& profile,
		int sampleCount,
		int dacRange,
		int attemptIndex,
		const SweepRecenterFailureInfo& failure)
	{
		const double delta = SuggestSweepRecenterDeltaDac(profile, sampleCount, dacRange, attemptIndex, failure);
		const double next = centerDac + delta;
		int iNext = (int)floor(next + 0.5);
		if (iNext < M576_RECAL_DAC_MIN)
			iNext = M576_RECAL_DAC_MIN;
		if (iNext > M576_RECAL_DAC_MAX)
			iNext = M576_RECAL_DAC_MAX;
		return iNext;
	}
}
