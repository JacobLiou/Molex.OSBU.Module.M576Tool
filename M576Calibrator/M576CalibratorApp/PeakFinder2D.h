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

	/// 1D 拟合/校验：抛物线 + 可辨识度（与 CalibConstants 中门限配合）。
	enum class Peak1DValidateCode
	{
		Ok = 0,
		Empty,
		LowSpan,              ///< max-min 太小，平线/无动态
		NotStrictLocal,       ///< 旧 argmax 邻域（保留接口）
		EdgeNotAllowed,       ///< 顶点在扫频端（宏 REJECT_EDGE_MAX=1 时按策略拒绝）
		MultiLocalMax,        ///< 严格局部峰过多（仅旧路径）
		NotEnoughSamples,       ///< 全序列 n<3，无法拟合
		NotEnoughValidSamples,  ///< 剔除固件无效占位后有效点<3
		ParabolaNotDownward,  ///< 最小二乘 a>=0，无有限极大
		ParabolaFitSingular,  ///< 正规方程接近奇异
		VertexOutOfRange,     ///< 顶点 t* 不在 [0, n-1]（含边）
	};

	// --- 最小二乘 P(i)≈a i^2+b i+c，下标为原格点 0..n-1，固件无效点(-999999/-999900)不参与。顶点 t*=-b/(2a) ---
	/// 固件 P 行中无效功率占位，仅二者之一。
	bool IsRecal1DPowerInvalidValue(double v);
	/// 失败时 f 非 Ok；成功时 outT 为连续峰位下标（相对整条扫频，含间隙格点下标）。
	bool ParabolaVertexMax1D(const std::vector<double>& p, double& outT, Peak1DValidateCode& f);

	/// 在已有 argmax 下标 i 上校验（历史接口；当前主路径已改为抛物线）。
	bool ValidateUnimodal1DAtArgmax(const std::vector<double>& data, int i, Peak1DValidateCode& f);

	/// 抛物线顶点；outIdx = lround(t*)。outTParabola 非空时回写连续 t*，供 DAC 用。
	bool FindUnimodalPeak1DIndex(
		const std::vector<double>& data,
		int& outIdx,
		Peak1DValidateCode& f,
		double* outTParabola = nullptr);

	/// 两向独立抛物线；outRow/outCol = lround(tY), lround(tX)。outTY/outTX 供 SweepCol0+ 连续下标算 DAC（勿先取整再算步进）。
	bool PeakCrossFrom1DScans(
		const std::vector<double>& powY,
		const std::vector<double>& powX,
		int& outRow,
		int& outCol,
		Peak1DValidateCode* yDetail = nullptr,
		Peak1DValidateCode* xDetail = nullptr,
		double* outTY = nullptr,
		double* outTX = nullptr);
}
