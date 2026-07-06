#pragma once

#include "PeakFinder2D.h"
#include <vector>

namespace M576
{
	enum class SweepTrend
	{
		Unknown,
		Flat,
		StrictInc,
		StrictDec,
		NonMono
	};

	enum class SweepRetryAction
	{
		GiveUp,
		JumpFlatMax,
		MonoCoarseShift,
		FlatAtMaxShift,
		ShiftOnly,
		FineRefine,
	};

	struct SweepProfile
	{
		SweepTrend trend = SweepTrend::Unknown;
		int argmaxIndex = -1;
		int validCount = 0;
		double span = 0.0;
		double slopePerIndex = 0.0;
	};

	/// Context from the failed fit attempt used to refine the next hardware sweep center.
	struct SweepRecenterFailureInfo
	{
		Peak1DValidateCode code = Peak1DValidateCode::Empty;
		double tPeak = 0.0;
		bool hasTPeak = false;
		int prevArgmaxIndex = -1;
		bool hasPrevAttempt = false;
	};

	struct SweepRecenterSessionState
	{
		int uiFineRange = 64;
		int movingBase = 0;
		int attemptRange = 64;
		bool flatJumpedToMax = false;
		bool inCoarsePhase = false;
		bool fineConsumed = false;
		int prevArgmax = -1;
		int lastBase = 0;
		int prevDeltaSign = 0;
		int flatShiftCount = 0;
		bool oscillationDetected = false;
	};

	struct SweepRetryPlan
	{
		SweepRetryAction action = SweepRetryAction::GiveUp;
		int nextRange = 0;
		int nextBase = 0;
	};

	const char* SweepTrendName(SweepTrend t);
	const char* SweepRetryActionLogTag(SweepRetryAction action);

	void InitSweepRecenterSessionState(
		SweepRecenterSessionState& state,
		int uiFineRange,
		int movingBase);

	SweepProfile AnalyzeRecal1DSweepProfile(const std::vector<double>& powY);

	bool IsFlatSweepFailure(Peak1DValidateCode code, const SweepProfile& profile);

	/// StrictInc/Dec or edge NonMono + ParabolaNotDownward / NotEnoughValidSamples (not Flat).
	bool IsMonotoneSweepFailure(Peak1DValidateCode code, const SweepProfile& profile, int sampleCount);

	/// Double current offset up to maxRange; returns 0 if no expansion possible.
	int SuggestFlatRetryDacRange(int currentRange, int maxRange);

	/// Flat retry: jump directly to maxRange (not ×2 steps).
	int SuggestJumpMaxDacRange(int currentRange, int maxRange);

	bool IsCoarsePeakHint(
		Peak1DValidateCode code,
		const SweepProfile& profile,
		int sampleCount,
		const SweepRecenterFailureInfo* failure);

	/// JumpFlatMax@coarse 后：内峰 + 左肩充分 + 右肩被窗截断 → planner 直接 FineRefine。
	bool IsCoarseExpandedInteriorPeak(
		const SweepRecenterSessionState& state,
		const SweepProfile& profile,
		const std::vector<double>& powSamples,
		int sampleCount);

	bool NeedsFineRefineAfterSuccess(int attemptRange, int uiFineRange);

	/// After ApplySweepRetryPlan(FineRefine): next sweep uses relaxed peak validation.
	bool IsFineRefineSweepAttempt(const SweepRecenterSessionState& state);

	/// Y/X 扫频结果已在 UI fine range（粗扫+FineRefine 或首轮即 fine）：交叉寻峰与预扫同用 FineRefineRelaxed。
	Peak1DFitPolicy Peak1DFitPolicyForSweepResult(int dacRangeUsed, int uiFineRange);

	/// 交叉轴：X retry 精扫 attempt，或扫频结果已在 uiFineRange。
	Peak1DFitPolicy Peak1DFitPolicyForCrossAxis(
		const SweepRecenterSessionState& retryState,
		int dacRangeUsed,
		int uiFineRange);

	/// Moving-axis DAC at coarse peak index: col0 + t* * gridStep.
	int PeakBaseFromCoarseHint(
		double sweepCol0,
		double tPeak,
		bool hasTPeak,
		int argmaxIndex,
		int sampleCount,
		int halfRange);

	SweepProfile AdjustProfileForFlatAtMaxShift(const SweepProfile& profile, int sampleCount);

	/// Flat@max + FlatAtMaxShift 闸门：内峰 argmax 或内峰 t*。
	bool IsInteriorPeakHint(
		const SweepProfile& profile,
		int sampleCount,
		const SweepRecenterFailureInfo& failure);

	/// FlatAtMaxShift 下一 base：内峰用 PeakBaseFromCoarseHint，贴边保留启发式。
	int SuggestFlatAtMaxShiftBase(
		double sweepCenterDac,
		const SweepProfile& profile,
		int sampleCount,
		int dacRange,
		int attemptIndex,
		const SweepRecenterFailureInfo& failure);

	/// 仅 Flat@max 链路：连续反向 FlatAtMaxShift 后应强制 FineRefine。
	bool DetectSweepRecenterOscillation(const SweepRecenterSessionState& state);

	SweepRetryPlan PlanNextRecal1DSweepAttempt(
		const SweepRecenterSessionState& state,
		Peak1DValidateCode code,
		const SweepProfile& profile,
		const std::vector<double>& powSamples,
		double sweepCenterDac,
		int attemptIndex,
		bool lastAttempt,
		const SweepRecenterFailureInfo& failure);

	SweepRetryPlan PlanFineRefineAfterCoarseSuccess(
		const SweepRecenterSessionState& state,
		double sweepCol0,
		double tPeak,
		int sampleCount,
		int coarseRange);

	void ApplySweepRetryPlan(SweepRecenterSessionState& state, const SweepRetryPlan& plan);

	/// After Y cross fit fails, plan next RECAL 3/5 mode-0 re-sweep (recenter baseY; pipeline restarts @ ioDacRange).
	bool PlanRecalYCrossResweep(
		Peak1DValidateCode crossCode,
		const std::vector<double>& powY,
		double sweepCol0,
		int sweepHalfRange,
		int roundIndex,
		int& ioMovingBase,
		int& ioDacRange,
		int& ioPrevArgmax,
		double crossTPeak,
		bool hasCrossTPeak,
		bool& outUsedExpandRange,
		bool& ioMonoRangeExpanded);

	bool IsRetryablePeakFailure(
		Peak1DValidateCode code,
		const SweepProfile& profile,
		int sampleCount,
		bool afterFlatExpandRange = false,
		const std::vector<double>* powSamples = nullptr,
		const SweepRecenterFailureInfo* failure = nullptr);

	/// Map plateau+tail or vertex-outside-left to StrictDec/Inc for mono recenter (optional failure context).
	SweepProfile AdjustProfileForMonoRecenter(
		const SweepProfile& profile,
		const std::vector<double>& powSamples,
		bool afterFlatExpandRange,
		const SweepRecenterFailureInfo* failure = nullptr);

	// Signed DAC shift from current sweep center; clamped to +/- MAX_SHIFT_FRAC * dacRange.
	double SuggestSweepRecenterDeltaDac(
		const SweepProfile& profile,
		int sampleCount,
		int dacRange,
		int attemptIndex);

	double SuggestSweepRecenterDeltaDac(
		const SweepProfile& profile,
		int sampleCount,
		int dacRange,
		int attemptIndex,
		const SweepRecenterFailureInfo& failure);

	// centerDac + delta, rounded to int16 RECAL base (signed; not clamped to 0).
	int SuggestSweepRecenterNewBase(
		double centerDac,
		const SweepProfile& profile,
		int sampleCount,
		int dacRange,
		int attemptIndex);

	int SuggestSweepRecenterNewBase(
		double centerDac,
		const SweepProfile& profile,
		int sampleCount,
		int dacRange,
		int attemptIndex,
		const SweepRecenterFailureInfo& failure);
}
