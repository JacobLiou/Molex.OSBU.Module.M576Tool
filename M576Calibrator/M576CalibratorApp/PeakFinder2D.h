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
}
