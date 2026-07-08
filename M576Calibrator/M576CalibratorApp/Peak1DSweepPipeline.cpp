#include "Peak1DSweepPipeline.h"
#include "M576Peak1DConstants.h"
#include "PeakFinder2D.h"

#include <algorithm>
#include <cmath>
#include <map>

namespace M576
{
	namespace
	{
		static int ClampRecalBase(int v)
		{
			if (v < M576_RECAL_DAC_MIN)
				return M576_RECAL_DAC_MIN;
			if (v > M576_RECAL_DAC_MAX)
				return M576_RECAL_DAC_MAX;
			return v;
		}

		static void AppendSegment(
			Recal1DSweepPipelineState& state,
			double col0,
			int halfRange,
			int movingBase,
			int stitchK,
			const std::vector<double>& pow)
		{
			Recal1DSweepSegment seg = {};
			seg.col0 = col0;
			seg.halfRange = halfRange;
			seg.movingBase = movingBase;
			seg.stitchK = stitchK;
			seg.pow = pow;
			state.segments.push_back(seg);
		}

		static bool SegmentSpanRaw(const std::vector<double>& pow, double& span)
		{
			span = 0.0;
			double lo = 0.0, hi = 0.0;
			bool any = false;
			for (size_t i = 0; i < pow.size(); ++i)
			{
				if (IsRecal1DPowerInvalidValue(pow[i]))
					continue;
				if (!any)
				{
					lo = hi = pow[i];
					any = true;
				}
				else
				{
					lo = (std::min)(lo, pow[i]);
					hi = (std::max)(hi, pow[i]);
				}
			}
			if (!any)
				return false;
			span = hi - lo;
			return true;
		}

		static void BeginFineRefine(
			Recal1DSweepPipelineState& state,
			double col0,
			int halfRange,
			double tPeak,
			int peakIdx,
			int sampleCount)
		{
			state.pendingFineBase = PeakBaseFromCoarseHint(
				col0, tPeak, std::isfinite(tPeak), peakIdx, sampleCount, halfRange);
			state.hasPendingFineBase = true;
			state.phase = Recal1DPipelinePhase::FineRefine;
			state.failedPhaseTag = "fineRefine";
		}

		static bool SegmentsHaveStitchK(const std::vector<Recal1DSweepSegment>& segments, int k)
		{
			for (const Recal1DSweepSegment& seg : segments)
			{
				if (seg.stitchK == k)
					return true;
			}
			return false;
		}

		static bool SegmentsHaveSymmetricTrio(const std::vector<Recal1DSweepSegment>& segments)
		{
			return SegmentsHaveStitchK(segments, 0)
				&& SegmentsHaveStitchK(segments, 1)
				&& SegmentsHaveStitchK(segments, 2);
		}

		static bool SegmentsHaveExploreSweep(const std::vector<Recal1DSweepSegment>& segments)
		{
			for (const Recal1DSweepSegment& seg : segments)
			{
				if (seg.stitchK >= 3)
					return true;
			}
			return false;
		}

		static bool TryMergeAndBeginFineRefine(Recal1DSweepPipelineState& state, int dacStep)
		{
			double peakDac = 0.0;
			double tMerged = 0.0;
			int idxMerged = 0;
			Peak1DValidateCode mergeCode = Peak1DValidateCode::Empty;
			double spanRaw = 0.0;
			Peak1DPlateauTrace plateauTrace = {};
			if (!FindPeakDacOnMerged(
					state.segments,
					dacStep,
					peakDac,
					tMerged,
					idxMerged,
					mergeCode,
					nullptr,
					spanRaw,
					&plateauTrace))
			{
				state.mergedSpanRaw = spanRaw;
				state.lastCode = mergeCode;
				state.lastMergeKneeLeft = plateauTrace.kneeLeftIdx;
				state.lastMergeKneeRight = plateauTrace.kneeRightIdx;
				state.lastMergeTPeak = tMerged;
				state.hasLastMergePlateau = plateauTrace.usedDualKnee;
				return false;
			}

			state.mergedSpanRaw = spanRaw;
			state.lastCode = Peak1DValidateCode::Ok;
			state.lastMergeKneeLeft = plateauTrace.kneeLeftIdx;
			state.lastMergeKneeRight = plateauTrace.kneeRightIdx;
			state.lastMergeTPeak = tMerged;
			state.hasLastMergePlateau = plateauTrace.usedDualKnee;
			double mCol0 = 0.0;
			std::vector<double> merged;
			if (!MergeRecal1DSweepSegments(state.segments, dacStep, mCol0, merged))
				return false;

			BeginFineRefine(
				state,
				mCol0,
				state.coarseRange,
				tMerged,
				idxMerged,
				(int)merged.size());
			state.movingBase = state.pendingFineBase;
			return true;
		}
	}

	int StitchMovingBaseFromAnchor(int anchorBase, int stitchK)
	{
		if (stitchK < 1)
			return anchorBase;
		const int unit = (int)M576_PEAK1D_STITCH_UNIT_DAC;
		if (stitchK == 1)
			return ClampRecalBase(anchorBase - unit);
		if (stitchK == 2)
			return ClampRecalBase(anchorBase + unit);
		if (stitchK == 3)
			return ClampRecalBase(anchorBase - 2 * unit);
		if (stitchK == 4)
			return ClampRecalBase(anchorBase + 2 * unit);
		return anchorBase;
	}

	int StitchAnchorCenterFromCoarseSweep(double col0, int halfRange)
	{
		if (halfRange < 1)
			return ClampRecalBase((int)floor(col0 + 0.5));
		return ClampRecalBase((int)floor(col0 + (double)halfRange + 0.5));
	}

	bool IsStitchLeft(int stitchK)
	{
		return (stitchK % 2) == 1;
	}

	double DacAtSampleIndex(double col0, int index, int sampleCount, int halfRange)
	{
		if (sampleCount <= 1)
			return col0;
		const double step = (2.0 * (double)halfRange) / (double)(sampleCount - 1);
		return col0 + (double)index * step;
	}

	bool MergeRecal1DSweepSegments(
		const std::vector<Recal1DSweepSegment>& segments,
		int dacStep,
		double& outCol0,
		std::vector<double>& outPow)
	{
		outPow.clear();
		outCol0 = 0.0;
		if (segments.empty() || dacStep < 1)
			return false;

		std::map<int, std::pair<double, int>> acc;
		for (const Recal1DSweepSegment& seg : segments)
		{
			const int n = (int)seg.pow.size();
			if (n <= 0)
				continue;
			for (int i = 0; i < n; ++i)
			{
				if (IsRecal1DPowerInvalidValue(seg.pow[(size_t)i]))
					continue;
				const double dacF = DacAtSampleIndex(seg.col0, i, n, seg.halfRange);
				const int dacKey = (int)floor(dacF + 0.5);
				auto it = acc.find(dacKey);
				if (it == acc.end())
					acc[dacKey] = std::make_pair(seg.pow[(size_t)i], 1);
				else
				{
					it->second.first += seg.pow[(size_t)i];
					it->second.second += 1;
				}
			}
		}
		if (acc.empty())
			return false;

		const int dacMin = acc.begin()->first;
		const int dacMax = acc.rbegin()->first;
		outCol0 = (double)dacMin;
		const int count = (dacMax - dacMin) / dacStep + 1;
		if (count < 1)
			return false;
		outPow.reserve((size_t)count);
		for (int k = 0; k < count; ++k)
		{
			const int dac = dacMin + k * dacStep;
			auto it = acc.find(dac);
			if (it != acc.end() && it->second.second > 0)
				outPow.push_back(it->second.first / (double)it->second.second);
			else
				outPow.push_back((double)M576_RECAL_POW_INVALID_1);
		}
		return !outPow.empty();
	}

	bool FindPeakOnPow(
		const std::vector<double>& pow,
		Peak1DFitPolicy policy,
		int& outPeakIdx,
		double& outTPeak,
		Peak1DValidateCode& outCode,
		Peak1DFitTrace* trace)
	{
		return FindUnimodalPeak1DIndex(pow, outPeakIdx, outCode, &outTPeak, trace, policy);
	}

	bool FindPeakDacOnMerged(
		const std::vector<Recal1DSweepSegment>& segments,
		int dacStep,
		double& outPeakDac,
		double& outTPeak,
		int& outPeakIdx,
		Peak1DValidateCode& outCode,
		Peak1DFitTrace* trace,
		double& outMergedSpanRaw,
		Peak1DPlateauTrace* plateauTrace)
	{
		outMergedSpanRaw = 0.0;
		outPeakDac = 0.0;
		outTPeak = 0.0;
		outPeakIdx = 0;
		outCode = Peak1DValidateCode::Empty;
		if (plateauTrace != nullptr)
			*plateauTrace = Peak1DPlateauTrace();

		double col0 = 0.0;
		std::vector<double> merged;
		if (!MergeRecal1DSweepSegments(segments, dacStep, col0, merged))
			return false;
		(void)SegmentSpanRaw(merged, outMergedSpanRaw);
		if (outMergedSpanRaw < Peak1DMinFlatSpanRaw())
		{
			outCode = Peak1DValidateCode::LowSpan;
			return false;
		}

		Peak1DPlateauTrace localPlateau = {};
		Peak1DPlateauTrace* pt = plateauTrace ? plateauTrace : &localPlateau;
		double tMerged = 0.0;
		const bool forceRelaxed = SegmentsHaveExploreSweep(segments);
		const bool symmetricTrio = SegmentsHaveSymmetricTrio(segments);
		const bool useDualKnee = !forceRelaxed && symmetricTrio && IsMergedMesaProfile(merged);

		if (useDualKnee)
		{
			if (!FindPlateauDualKneePeak1D(merged, tMerged, outCode, pt)
				|| outCode != Peak1DValidateCode::Ok)
			{
				return false;
			}
		}
		else
		{
			pt->usedDualKnee = false;
			if (!FindPeakOnPow(
					merged,
					Peak1DFitPolicy::FineRefineRelaxed,
					outPeakIdx,
					tMerged,
					outCode,
					trace)
				|| outCode != Peak1DValidateCode::Ok)
			{
				return false;
			}
		}

		outTPeak = tMerged;
		outPeakIdx = (int)std::lround(tMerged);
		const int n = (int)merged.size();
		if (outPeakIdx < 0)
			outPeakIdx = 0;
		if (outPeakIdx >= n)
			outPeakIdx = n - 1;
		const int half = segments.empty() ? M576_MAX_DAC_RANGE : segments.back().halfRange;
		outPeakDac = DacAtSampleIndex(col0, outPeakIdx, n, half);
		if (std::isfinite(outTPeak))
			outPeakDac = DacAtSampleIndex(col0, outTPeak, n, half);
		return true;
	}

	void InitRecal1DSweepPipeline(
		Recal1DSweepPipelineState& state,
		int uiFineRange,
		int movingBase)
	{
		state = {};
		state.uiFineRange = (uiFineRange >= 1) ? uiFineRange : 1;
		state.fineHalfRange = state.uiFineRange;
		state.coarseRange = M576_MAX_DAC_RANGE;
		state.movingBase = movingBase;
		state.anchorBase = movingBase;
		state.phase = Recal1DPipelinePhase::Fine64;
		state.failedPhaseTag = "fine64";
	}

	bool GetNextPipelineSweepCommand(
		const Recal1DSweepPipelineState& state,
		Recal1DSweepCommand& outCmd)
	{
		if (state.phase == Recal1DPipelinePhase::Succeeded
			|| state.phase == Recal1DPipelinePhase::Failed)
		{
			return false;
		}

		outCmd = {};
		switch (state.phase)
		{
		case Recal1DPipelinePhase::Fine64:
			outCmd.movingBase = state.movingBase;
			outCmd.halfRange = state.uiFineRange;
			outCmd.fitPolicy = Peak1DFitPolicy::Strict;
			outCmd.phaseLogTag = "fine64";
			return true;
		case Recal1DPipelinePhase::Coarse200:
			outCmd.movingBase = state.movingBase;
			outCmd.halfRange = state.coarseRange;
			outCmd.fitPolicy = Peak1DFitPolicy::Strict;
			outCmd.phaseLogTag = "coarse200";
			return true;
		case Recal1DPipelinePhase::Stitch:
		{
			const int k = state.stitchK;
			if (k < 1 || k > (int)M576_PEAK1D_STITCH_MAX_RETRIES)
				return false;
			outCmd.movingBase = StitchMovingBaseFromAnchor(state.anchorBase, k);
			outCmd.halfRange = state.coarseRange;
			outCmd.fitPolicy = Peak1DFitPolicy::Strict;
			if (k <= (int)M576_PEAK1D_STITCH_SYMMETRIC_RETRIES)
				outCmd.phaseLogTag = IsStitchLeft(k) ? "stitch left" : "stitch right";
			else
				outCmd.phaseLogTag = IsStitchLeft(k) ? "stitch explore left" : "stitch explore right";
			return true;
		}
		case Recal1DPipelinePhase::FineRefine:
			outCmd.movingBase = state.hasPendingFineBase ? state.pendingFineBase : state.movingBase;
			outCmd.halfRange = state.uiFineRange;
			outCmd.fitPolicy = Peak1DFitPolicy::FineRefineRelaxed;
			outCmd.phaseLogTag = "fineRefine";
			return true;
		default:
			return false;
		}
	}

	bool AdvanceRecal1DSweepPipeline(
		Recal1DSweepPipelineState& state,
		double col0,
		const std::vector<double>& pow,
		bool peakOk,
		Peak1DValidateCode code,
		double tPeak,
		int peakIdx,
		int dacStep)
	{
		state.sweepCount++;
		state.lastCode = code;

		switch (state.phase)
		{
		case Recal1DPipelinePhase::Fine64:
			if (peakOk)
			{
				state.phase = Recal1DPipelinePhase::Succeeded;
				return true;
			}
			state.failedPhaseTag = "fine64";
			state.phase = Recal1DPipelinePhase::Coarse200;
			return false;

		case Recal1DPipelinePhase::Coarse200:
			if (peakOk)
			{
				BeginFineRefine(state, col0, state.coarseRange, tPeak, peakIdx, (int)pow.size());
				return false;
			}
			state.anchorBase = StitchAnchorCenterFromCoarseSweep(col0, state.coarseRange);
			AppendSegment(state, col0, state.coarseRange, state.movingBase, 0, pow);
			state.stitchK = 1;
			state.lastStitchK = 0;
			state.failedPhaseTag = "coarse200";
			state.phase = Recal1DPipelinePhase::Stitch;
			return false;

		case Recal1DPipelinePhase::Stitch:
		{
			AppendSegment(state, col0, state.coarseRange, state.movingBase, state.stitchK, pow);
			state.lastStitchK = state.stitchK;
			state.failedPhaseTag = "stitch_k";

			if (TryMergeAndBeginFineRefine(state, dacStep))
				return false;

			if (state.stitchK >= (int)M576_PEAK1D_STITCH_MAX_RETRIES)
			{
				state.phase = Recal1DPipelinePhase::Failed;
				state.failedPhaseTag = "stitch_k4";
				return true;
			}
			state.stitchK++;
			return false;
		}

		case Recal1DPipelinePhase::FineRefine:
			if (peakOk)
			{
				state.phase = Recal1DPipelinePhase::Succeeded;
				return true;
			}
			state.failedPhaseTag = "fineRefine";
			state.phase = Recal1DPipelinePhase::Failed;
			return true;

		default:
			return true;
		}
	}

	PeakPipelineFailureReport BuildPeakPipelineFailureReport(const Recal1DSweepPipelineState& state)
	{
		PeakPipelineFailureReport r = {};
		r.anchorBase = state.anchorBase;
		r.sweepCount = state.sweepCount;
		r.lastCode = state.lastCode;
		r.failedPhase = state.failedPhaseTag;
		r.lastStitchK = state.lastStitchK;
		r.mergedSpanRaw = state.mergedSpanRaw;
		r.mergeKneeLeft = state.lastMergeKneeLeft;
		r.mergeKneeRight = state.lastMergeKneeRight;
		r.mergeTPeak = state.lastMergeTPeak;
		r.segments = state.segments;
		return r;
	}

	bool PlanRecalYCrossResweepPipeline(
		Peak1DValidateCode crossCode,
		const std::vector<double>& powY,
		double sweepCol0,
		int sweepHalfRange,
		int& ioMovingBase,
		double crossTPeak,
		bool hasCrossTPeak)
	{
		if (crossCode == Peak1DValidateCode::Ok || crossCode == Peak1DValidateCode::PmRangeMismatch)
			return false;
		const int n = (int)powY.size();
		if (n <= 0 || sweepHalfRange < 1)
			return false;

		const SweepProfile profile = AnalyzeRecal1DSweepProfile(powY);
		const int hintBase = PeakBaseFromCoarseHint(
			sweepCol0,
			crossTPeak,
			hasCrossTPeak,
			profile.argmaxIndex,
			n,
			sweepHalfRange);
		ioMovingBase = hintBase;
		return true;
	}
}
