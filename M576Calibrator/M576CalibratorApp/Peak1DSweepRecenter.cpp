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

	bool IsRetryablePeakFailure(Peak1DValidateCode code, const SweepProfile& profile, int sampleCount)
	{
		if (sampleCount <= 0 || profile.trend == SweepTrend::Flat)
			return false;

		switch (code)
		{
		case Peak1DValidateCode::ParabolaNotDownward:
			return profile.trend == SweepTrend::StrictInc || profile.trend == SweepTrend::StrictDec;
		case Peak1DValidateCode::NotEnoughValidSamples:
			return profile.trend == SweepTrend::StrictInc || profile.trend == SweepTrend::StrictDec;
		case Peak1DValidateCode::VertexOutOfRange:
		case Peak1DValidateCode::EdgeNotAllowed:
			if (sampleCount <= 1)
				return false;
			return profile.argmaxIndex == 0 || profile.argmaxIndex == (sampleCount - 1);
		case Peak1DValidateCode::LowSpan:
		case Peak1DValidateCode::Empty:
			return false;
		default:
			return false;
		}
	}

	double SuggestSweepRecenterDeltaDac(
		const SweepProfile& profile,
		int sampleCount,
		int dacRange,
		int attemptIndex)
	{
		if (sampleCount <= 1 || dacRange < 1)
			return 0.0;
		if (attemptIndex < 0)
			attemptIndex = 0;

		const double step = (2.0 * dacRange) / (double)(sampleCount - 1);
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
		else
		{
			return 0.0;
		}

		double deltaDac = deltaIndex * step;
		const double maxShift = (double)M576_PEAK1D_SWEEP_RECENTER_MAX_SHIFT_FRAC * (double)dacRange;
		if (deltaDac > maxShift)
			deltaDac = maxShift;
		else if (deltaDac < -maxShift)
			deltaDac = -maxShift;
		return deltaDac;
	}

	int SuggestSweepRecenterNewBase(
		double centerDac,
		const SweepProfile& profile,
		int sampleCount,
		int dacRange,
		int attemptIndex)
	{
		const double delta = SuggestSweepRecenterDeltaDac(profile, sampleCount, dacRange, attemptIndex);
		const double next = centerDac + delta;
		int iNext = (int)floor(next + 0.5);
		if (iNext < 0)
			iNext = 0;
		if (iNext > 65535)
			iNext = 65535;
		return iNext;
	}
}
