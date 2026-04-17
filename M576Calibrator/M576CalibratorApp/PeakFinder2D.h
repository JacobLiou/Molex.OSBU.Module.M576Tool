#pragma once

#include <vector>

namespace M576
{
	/// Cross (+) neighborhood peak on row-major grid [rows x cols].
	bool PeakCross2D(const std::vector<double>& data, int rows, int cols, int& outRow, int& outCol);

	/// Global maximum.
	bool PeakMax2D(const std::vector<double>& data, int rows, int cols, int& outRow, int& outCol);

	/// Index of maximum element (ties: first).
	bool PeakMax1D(const std::vector<double>& data, int& outIdx);

	/// RECAL 3 0 (Y sweep at fixed X) + RECAL 3 1 (X sweep at fixed Y): discrete cross peak.
	/// outRow = argmax(powY), outCol = argmax(powX). Fails if lengths differ or empty.
	bool PeakCrossFrom1DScans(const std::vector<double>& powY, const std::vector<double>& powX, int& outRow, int& outCol);
}
