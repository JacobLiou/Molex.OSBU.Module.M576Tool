#pragma once
// PM 挡位防呆：RECAL 扫频原始功率 /10000 -> dBm，校验全局有效极大值是否在界面所选挡位内。

#include <vector>

namespace M576
{
#ifndef M576_RECAL_RAW_TO_DBM_SCALE
#define M576_RECAL_RAW_TO_DBM_SCALE 10000.0
#endif

	/// 固件 RECAL P 行原始值 -> dBm。
	inline double RecalRawToDbm(double raw)
	{
		return raw / M576_RECAL_RAW_TO_DBM_SCALE;
	}

	/// pm_range 0..3：输出半开/闭区间边界；4(auto) 返回 false（调用方应跳过校验）。
	bool GetPmRangeDbmBounds(int pmRangeIndex, double& outLoDbm, double& outHiDbm);

	/// 日志/错误信息用，如 `[-14,6]`、`[-34,-14)`。
	const char* PmRangeDbmIntervalDesc(int pmRangeIndex);

	/// 在有效（非无效占位）样本中取全局最大功率下标；无有效点返回 -1。
	int FindGlobalMaxValidPowerIndex(const std::vector<double>& powers);

	/// peakIdxHint >= 0 使用该下标；否则用 FindGlobalMaxValidPowerIndex。
	/// pm_range==4 或越界索引：返回 true（跳过）。
	bool ValidatePeakPowerInPmRange(
		int pmRangeIndex,
		const std::vector<double>& powers,
		int peakIdxHint,
		double& outPeakRaw,
		double& outPeakDbm,
		double& outLoDbm,
		double& outHiDbm,
		int& outPeakIdx);

	/// `opm 4 1` / `opm 5 1` 应答（ASCII，可含首尾空白）：0..4 或 -1。
	int ParseOpmPmRangeReplyAscii(const char* lineAscii);
}
