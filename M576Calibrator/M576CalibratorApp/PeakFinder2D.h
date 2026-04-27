#pragma once
// 二维/一维简单寻峰，用于 RECAL 3/5 两向扫频后的交叉峰（与 M576 定标流程配合）。

#include <vector>

namespace M576
{
	/// Cross (+) neighborhood peak on row-major grid [rows x cols].
	// 行主序矩阵上取邻域十字峰（中心及四邻，按实现）。
	bool PeakCross2D(const std::vector<double>& data, int rows, int cols, int& outRow, int& outCol);

	/// Global maximum.
	// 全网格最大点。
	bool PeakMax2D(const std::vector<double>& data, int rows, int cols, int& outRow, int& outCol);

	/// Index of maximum element (ties: first).
	// 一维最大下标，平局取先出现者。
	bool PeakMax1D(const std::vector<double>& data, int& outIdx);

	/// RECAL 3 0 (Y sweep at fixed X) + RECAL 3 1 (X sweep at fixed Y): discrete cross peak.
	/// outRow = argmax(powY), outCol = argmax(powX). Fails if lengths differ or empty.
	// 用两次一维扫频的功率序列做离散“交叉”峰：outRow/Col 对应用 powY/powX 的 argmax。
	bool PeakCrossFrom1DScans(const std::vector<double>& powY, const std::vector<double>& powX, int& outRow, int& outCol);
}
