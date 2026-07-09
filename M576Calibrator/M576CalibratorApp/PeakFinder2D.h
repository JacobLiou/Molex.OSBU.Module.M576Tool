#pragma once
// 二维/一维简单寻峰，用于 RECAL 3/5 两向扫频后的交叉峰（与 M576 定标流程配合）。

#include <vector>
#include "PmRangeValidation.h"

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
		PmRangeMismatch,        ///< 全局有效极大值 dBm 不在界面 PM 挡位内（RECAL 3）
	};

	/// Strict：粗扫 / jump-max / mono shift 全部门限。FineRefineRelaxed：粗扫 OK 后的 uiFineRange 精扫（跳过平坦门控；三阶失败可 argmax 回退）。
	enum class Peak1DFitPolicy
	{
		Strict,
		FineRefineRelaxed,
	};

	// --- 历史名保留：预处理 + P(i)≈a i^3+b i^2+c i+d（下标 i 为原扫频格点）；固件占位不参与。区间内求 P 最大得 t* ---
	/// 固件 P 行中无效功率占位，仅二者之一。
	bool IsRecal1DPowerInvalidValue(double v);

	/// dB 门限 → 固件 raw 功率差（INV-16：dBm = raw/10000）；算法内 span/肩点比较一律用本函数。
	inline double Peak1DDbToRawDelta(double db)
	{
		return db * M576_RECAL_RAW_TO_DBM_SCALE;
	}

	/// 当前生效的平坦/突出度门限 (dB)：离群剔除后 useOk 点 span 下限；INI MinProminenceDb 或默认 M576_PEAK1D_MIN_PROMINENCE_DB。
	double Peak1DGetMinProminenceDb();
	/// 平坦门限 raw 功率差（Peak1DDbToRawDelta(Peak1DGetMinProminenceDb())）。
	inline double Peak1DMinFlatSpanRaw()
	{
		return Peak1DDbToRawDelta(Peak1DGetMinProminenceDb());
	}
	/// 产线/单测注入；非法值会被 clamp 到 [MIN, MAX]。fromIni=true 时 Get 带来源标记供日志。
	void Peak1DSetMinProminenceDb(double db, bool fromIni = false);
	void Peak1DResetMinProminenceDb();
	bool Peak1DMinProminenceDbFromIni();

	/// argmax 格点两侧（跳过无效功率）是否均达到 minPromDb 落差；供 recenter Flat 判定。
	bool Peak1DArgmaxHasBilateralProminence(const std::vector<double>& powY, int argmaxIndex, double minPromDb);

	/// argmax 左侧是否达到 minPromDb 落差（跳过无效功率）。
	bool Peak1DArgmaxHasLeftProminence(const std::vector<double>& powY, int argmaxIndex, double minPromDb);

	/// argmax 右侧是否达到 minPromDb 落差（跳过无效功率）。
	bool Peak1DArgmaxHasRightProminence(const std::vector<double>& powY, int argmaxIndex, double minPromDb);

	/// 可选调试输出：全序列有效点全局 argmax（平局先出现者）与进入三阶拟合的格点样本。
	struct Peak1DFitTrace
	{
		int globalMaxIndex = -1;
		double globalMaxY = 0;
		std::vector<int> fitIndex;
		std::vector<double> fitY;
		bool usedArgmaxFallback = false;
	};

	/// merged 平台型双拐点定峰 trace（stitch merge 路径）。
	struct Peak1DPlateauTrace
	{
		int kneeLeftIdx = -1;
		int kneeRightIdx = -1;
		int argmaxIdx = -1;
		bool usedDualKnee = false;
	};

	/// merged 曲线是否为平台峰（mesa）：NonMono + 内峰 + 平顶宽度；保守判定，避免尖峰误判。
	bool IsMergedMesaProfile(const std::vector<double>& merged);

	/// merged mesa 曲线：上升/下降沿最大斜率膝点中点为 t*（非 Strict 单段尖峰路径）。
	bool FindPlateauDualKneePeak1D(
		const std::vector<double>& p,
		double& outT,
		Peak1DValidateCode& outCode,
		Peak1DPlateauTrace* trace = nullptr);

	/// 失败时 f 非 Ok；成功时 outT 为连续峰位下标（相对整条扫频，含间隙格点下标）。
	/// trace 非空时：入口即填全局最大；预处理成功后追加拟合点（即使后续拟合失败亦保留，便于追溯）。
	bool ParabolaVertexMax1D(
		const std::vector<double>& p,
		double& outT,
		Peak1DValidateCode& f,
		Peak1DFitTrace* trace = nullptr,
		Peak1DFitPolicy policy = Peak1DFitPolicy::Strict);

	/// 在已有 argmax 下标 i 上校验（历史接口）。
	bool ValidateUnimodal1DAtArgmax(const std::vector<double>& data, int i, Peak1DValidateCode& f);

	/// 三阶拟合峰位；outIdx = lround(t*)。outTParabola 非空时回写连续 t*，供 DAC 用。
	bool FindUnimodalPeak1DIndex(
		const std::vector<double>& data,
		int& outIdx,
		Peak1DValidateCode& f,
		double* outTParabola = nullptr,
		Peak1DFitTrace* trace = nullptr,
		Peak1DFitPolicy policy = Peak1DFitPolicy::Strict);

	/// Mesa Y@merge + X@fine：Y 峰位来自 merge t*（pipeline 已验证），X 单独 ParabolaVertexMax1D。
	bool PeakCrossFromMesaMergedYAndFineX(
		double mergeTPeak,
		const std::vector<double>& powX,
		int& outRow,
		int& outCol,
		Peak1DValidateCode* xDetail = nullptr,
		double* outTY = nullptr,
		double* outTX = nullptr,
		Peak1DFitTrace* traceX = nullptr,
		Peak1DFitPolicy policyX = Peak1DFitPolicy::Strict);

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
		Peak1DFitTrace* traceX = nullptr,
		Peak1DFitPolicy policyY = Peak1DFitPolicy::Strict,
		Peak1DFitPolicy policyX = Peak1DFitPolicy::Strict);
}
