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

	/// 1D 拟合/校验：预处理对称窗 + 三阶最小二乘 + 区间内全局最大（与 CalibConstants 门限配合）。
	enum class Peak1DValidateCode
	{
		Ok = 0,
		Empty,
		LowSpan,              ///< max-min 太小，平线/无动态
		NotStrictLocal,       ///< 旧 argmax 邻域（保留接口）
		EdgeNotAllowed,       ///< 顶点在扫频端（宏 REJECT_EDGE_MAX=1 时按策略拒绝）
		MultiLocalMax,        ///< 严格局部峰过多（仅旧路径）
		NotEnoughSamples,       ///< 全序列 n<M576_PEAK1D_CUBIC_MIN_SAMPLES
		NotEnoughValidSamples,  ///< 预处理后可用格点<M576_PEAK1D_CUBIC_MIN_SAMPLES
		ParabolaNotDownward,    ///< 无峰形单调或边界上升/下降占优等（历史名）
		ParabolaFitSingular,    ///< 正规方程接近奇异（历史名）
		VertexOutOfRange,       ///< 峰位 t* 不在 [0, n-1]（含边）
	};

	// --- 历史名保留：预处理 + P(i)≈a i^3+b i^2+c i+d（下标 i 为原扫频格点）；固件占位不参与。区间内求 P 最大得 t* ---
	/// 固件 P 行中无效功率占位，仅二者之一。
	bool IsRecal1DPowerInvalidValue(double v);

	/// 可选调试输出：全序列有效点全局 argmax（平局先出现者）与进入三阶拟合的格点样本。
	struct Peak1DFitTrace
	{
		int globalMaxIndex = -1;
		double globalMaxY = 0;
		std::vector<int> fitIndex;
		std::vector<double> fitY;
	};

	/// 失败时 f 非 Ok；成功时 outT 为连续峰位下标（相对整条扫频，含间隙格点下标）。
	/// trace 非空时：入口即填全局最大；预处理成功后追加拟合点（即使后续拟合失败亦保留，便于追溯）。
	bool ParabolaVertexMax1D(const std::vector<double>& p, double& outT, Peak1DValidateCode& f, Peak1DFitTrace* trace = nullptr);

	/// 在已有 argmax 下标 i 上校验（历史接口）。
	bool ValidateUnimodal1DAtArgmax(const std::vector<double>& data, int i, Peak1DValidateCode& f);

	/// 三阶拟合峰位；outIdx = lround(t*)。outTParabola 非空时回写连续 t*，供 DAC 用。
	bool FindUnimodalPeak1DIndex(
		const std::vector<double>& data,
		int& outIdx,
		Peak1DValidateCode& f,
		double* outTParabola = nullptr,
		Peak1DFitTrace* trace = nullptr);

	/// 两向各自三阶拟合寻峰；outRow/outCol = lround(tY), lround(tX)。outTY/outTX 供 SweepCol0+ 连续下标算 DAC。
	bool PeakCrossFrom1DScans(
		const std::vector<double>& powY,
		const std::vector<double>& powX,
		int& outRow,
		int& outCol,
		Peak1DValidateCode* yDetail = nullptr,
		Peak1DValidateCode* xDetail = nullptr,
		double* outTY = nullptr,
		double* outTX = nullptr,
		Peak1DFitTrace* traceY = nullptr,
		Peak1DFitTrace* traceX = nullptr);
}
