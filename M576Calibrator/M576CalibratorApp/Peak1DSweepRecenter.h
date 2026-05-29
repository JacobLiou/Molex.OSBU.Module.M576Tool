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

	const char* SweepTrendName(SweepTrend t);

	SweepProfile AnalyzeRecal1DSweepProfile(const std::vector<double>& powY);

	bool IsRetryablePeakFailure(Peak1DValidateCode code, const SweepProfile& profile, int sampleCount);

	// Signed DAC shift from current sweep center; clamped to +/- MAX_SHIFT_FRAC * dacRange.
	double SuggestSweepRecenterDeltaDac(
		const SweepProfile& profile,
		int sampleCount,
		int dacRange,
		int attemptIndex);

	// centerDac + delta, rounded; caller passes col0 + dacRange for moving-axis center.
	int SuggestSweepRecenterNewBase(
		double centerDac,
		const SweepProfile& profile,
		int sampleCount,
		int dacRange,
		int attemptIndex);
}
