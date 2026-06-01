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

	const char* SweepTrendName(SweepTrend t);

	SweepProfile AnalyzeRecal1DSweepProfile(const std::vector<double>& powY);

	bool IsRetryablePeakFailure(Peak1DValidateCode code, const SweepProfile& profile, int sampleCount);

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
