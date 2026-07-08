#pragma once

#include "Peak1DSweepRecenter.h"
#include <string>
#include <vector>

namespace M576
{
	struct Recal1DSweepSegment
	{
		double col0 = 0.0;
		int halfRange = 0;
		int movingBase = 0;
		int stitchK = 0;
		std::vector<double> pow;
	};

	enum class Recal1DPipelinePhase
	{
		Fine64,
		Coarse200,
		Stitch,
		FineRefine,
		Succeeded,
		Failed,
	};

	struct Recal1DSweepPipelineState
	{
		int uiFineRange = 64;
		/// @deprecated FineRefine ??? uiFineRange??? m_dacRange???????????????????
		int fineHalfRange = 64;
		int coarseRange = 200;
		int movingBase = 0;
		/// First coarse@200 sweep center DAC (floor(col0 + halfRange)).
		int anchorBase = 0;
		int stitchK = 0;
		int sweepCount = 0;
		Recal1DPipelinePhase phase = Recal1DPipelinePhase::Fine64;
		std::vector<Recal1DSweepSegment> segments;
		Peak1DValidateCode lastCode = Peak1DValidateCode::Empty;
		const char* failedPhaseTag = "fine64";
		int lastStitchK = 0;
		double mergedSpanRaw = 0.0;
		int pendingFineBase = 0;
		bool hasPendingFineBase = false;
		int lastMergeKneeLeft = -1;
		int lastMergeKneeRight = -1;
		double lastMergeTPeak = 0.0;
		bool hasLastMergePlateau = false;
	};

	struct PeakPipelineFailureReport
	{
		const char* axisTag = "";
		const char* recalStage = "";
		int pathLine1Based = 0;
		int fileSlot = 0;
		int anchorBase = 0;
		int sweepCount = 0;
		Peak1DValidateCode lastCode = Peak1DValidateCode::Empty;
		const char* failedPhase = "fine64";
		int lastStitchK = 0;
		double mergedSpanRaw = 0.0;
		int mergeKneeLeft = -1;
		int mergeKneeRight = -1;
		double mergeTPeak = 0.0;
		std::vector<Recal1DSweepSegment> segments;
	};

	struct Recal1DSweepCommand
	{
		int movingBase = 0;
		int halfRange = 0;
		Peak1DFitPolicy fitPolicy = Peak1DFitPolicy::Strict;
		const char* phaseLogTag = "fine64";
	};

	/// k odd: anchor - offset; k even: anchor + offset (anchor = coarse sweep center).
	int StitchMovingBaseFromAnchor(int anchorBase, int stitchK);

	/// First coarse@200 sweep center: floor(col0 + halfRange).
	int StitchAnchorCenterFromCoarseSweep(double col0, int halfRange);

	bool IsStitchLeft(int stitchK);

	double DacAtSampleIndex(double col0, int index, int sampleCount, int halfRange);

	bool MergeRecal1DSweepSegments(
		const std::vector<Recal1DSweepSegment>& segments,
		int dacStep,
		double& outCol0,
		std::vector<double>& outPow);

	bool FindPeakOnPow(
		const std::vector<double>& pow,
		Peak1DFitPolicy policy,
		int& outPeakIdx,
		double& outTPeak,
		Peak1DValidateCode& outCode,
		Peak1DFitTrace* trace);

	bool FindPeakDacOnMerged(
		const std::vector<Recal1DSweepSegment>& segments,
		int dacStep,
		double& outPeakDac,
		double& outTPeak,
		int& outPeakIdx,
		Peak1DValidateCode& outCode,
		Peak1DFitTrace* trace,
		double& outMergedSpanRaw,
		Peak1DPlateauTrace* plateauTrace = nullptr);

	void InitRecal1DSweepPipeline(
		Recal1DSweepPipelineState& state,
		int uiFineRange,
		int movingBase);

	bool GetNextPipelineSweepCommand(
		const Recal1DSweepPipelineState& state,
		Recal1DSweepCommand& outCmd);

	/// ?????????????????????????? true ??? pipeline ??????????????????
	bool AdvanceRecal1DSweepPipeline(
		Recal1DSweepPipelineState& state,
		double col0,
		const std::vector<double>& pow,
		bool peakOk,
		Peak1DValidateCode code,
		double tPeak,
		int peakIdx,
		int dacStep);

	PeakPipelineFailureReport BuildPeakPipelineFailureReport(const Recal1DSweepPipelineState& state);

	/// Y cross ?????? cross ???? hint ??? base??????????? Fine64 ?????
	bool PlanRecalYCrossResweepPipeline(
		Peak1DValidateCode crossCode,
		const std::vector<double>& powY,
		double sweepCol0,
		int sweepHalfRange,
		int& ioMovingBase,
		double crossTPeak,
		bool hasCrossTPeak);
}
