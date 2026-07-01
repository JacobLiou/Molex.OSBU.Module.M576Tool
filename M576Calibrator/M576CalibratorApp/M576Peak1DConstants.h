#pragma once
// 仅含 1D 寻峰/预处理与三阶拟合门限宏，无 TCHAR/MFC。供 PeakFinder2D、CrossPeakTest 与 CalibConstants 共用。

/// ValidateUnimodal1DAtArgmax / recenter Flat：全序列 span=max-min 严格小于本值则 LowSpan 或 trend=Flat。
#ifndef M576_PEAK1D_MIN_SPAN_DB
#define M576_PEAK1D_MIN_SPAN_DB 2.5
#endif
/// 峰突出度 (dB)：全局微平坦、argmax 双侧落差、对称拟合窗边界。运行时可由 M576Calibrator.ini 覆盖（见 Peak1DGetMinProminenceDb）。
#ifndef M576_PEAK1D_MIN_PROMINENCE_DB
#define M576_PEAK1D_MIN_PROMINENCE_DB 0.3
#endif
/// INI MinProminenceDb 合法范围（产线可调，重启生效）。
#ifndef M576_PEAK1D_MIN_PROMINENCE_DB_MIN
#define M576_PEAK1D_MIN_PROMINENCE_DB_MIN 0.05
#endif
#ifndef M576_PEAK1D_MIN_PROMINENCE_DB_MAX
#define M576_PEAK1D_MIN_PROMINENCE_DB_MAX 2.0
#endif
/// 拟合窗 span / 全序列 span 低于本比例时视为 plateau 假峰（comm expand128 类）。
#ifndef M576_PEAK1D_MIN_FIT_SPAN_FRAC
#define M576_PEAK1D_MIN_FIT_SPAN_FRAC 0.05
#endif
#ifndef M576_PEAK1D_EPS_REL_OF_SPAN
#define M576_PEAK1D_EPS_REL_OF_SPAN 1.0e-4
#endif
#ifndef M576_PEAK1D_MIN_ABS_EPS_DB
#define M576_PEAK1D_MIN_ABS_EPS_DB 1.0e-9
#endif
#ifndef M576_PEAK1D_REJECT_EDGE_MAX
#define M576_PEAK1D_REJECT_EDGE_MAX 0
#endif
#ifndef M576_PEAK1D_MAX_STRICT_LOCAL_MAXIMA
#define M576_PEAK1D_MAX_STRICT_LOCAL_MAXIMA 0
#endif
/// 固件 RECAL3/5 行里 P1..Pn 的无效功率占位，拟合/span 时剔除，仅作二者精确匹配。
#ifndef M576_RECAL_POW_INVALID_1
#define M576_RECAL_POW_INVALID_1 (-999999.0)
#endif
#ifndef M576_RECAL_POW_INVALID_2
#define M576_RECAL_POW_INVALID_2 (-999900.0)
#endif
/// Host-set RECAL 3/5 Base DAC (when not 9999): signed int16, same as LUT DAC pair range.
#ifndef M576_RECAL_DAC_MIN
#define M576_RECAL_DAC_MIN (-32768)
#endif
#ifndef M576_RECAL_DAC_MAX
#define M576_RECAL_DAC_MAX 32767
#endif
/// 三阶拟合至少需要参与回归的样本数（含预处理后的格点）。
#ifndef M576_PEAK1D_CUBIC_MIN_SAMPLES
#define M576_PEAK1D_CUBIC_MIN_SAMPLES 4
#endif
/// 孤立尖峰：功率异常偏高（优于邻域最大值）的突破量超过 max(mult*eps, MIN_SPAN_FRAC*span)；eps=max(MIN_ABS_EPS, span*EPS_REL)。
#ifndef M576_PEAK1D_OUTLIER_MULT
#define M576_PEAK1D_OUTLIER_MULT 8.0
#endif
/// 尖峰判别时，两侧各向内取的有效样本格数（总长 2*half+1 的中心除外）。
#ifndef M576_PEAK1D_OUTLIER_LOCAL_HALF
#define M576_PEAK1D_OUTLIER_LOCAL_HALF 2
#endif
/// 尖峰相对全有效序列跨度：|y-邻域中位数| 须同时超过 mult*eps 与本比例*span，避免光滑曲线上端点被误判。
#ifndef M576_PEAK1D_OUTLIER_MIN_SPAN_FRAC
#define M576_PEAK1D_OUTLIER_MIN_SPAN_FRAC 0.45
#endif
/// UI 日志中拟合点列表：打印前 N 个与后 M 个，中间省略。
#ifndef M576_PEAK1D_LOG_FIT_FIRST
#define M576_PEAK1D_LOG_FIT_FIRST 16
#endif
#ifndef M576_PEAK1D_LOG_FIT_LAST
#define M576_PEAK1D_LOG_FIT_LAST 8
#endif
/// RECAL 3/5 monotone sweep recenter: 1 initial + (MAX-1) hardware re-sweeps.
#ifndef M576_PEAK1D_SWEEP_RECENTER_MAX_ATTEMPTS
#define M576_PEAK1D_SWEEP_RECENTER_MAX_ATTEMPTS 12
#endif
#ifndef M576_PEAK1D_COARSE_DAC_RANGE
#define M576_PEAK1D_COARSE_DAC_RANGE M576_MAX_DAC_RANGE
#endif
#ifndef M576_PEAK1D_FLAT_EXPAND_FACTOR
#define M576_PEAK1D_FLAT_EXPAND_FACTOR 2
#endif
/// Flat/cross Y resweep: max sweep offset (matches UI `M576_MAX_DAC_RANGE` in CalibConstants.h).
#ifndef M576_MAX_DAC_RANGE
#define M576_MAX_DAC_RANGE 200
#endif
#ifndef M576_PEAK1D_SWEEP_RECENTER_MAX_SHIFT_FRAC
#define M576_PEAK1D_SWEEP_RECENTER_MAX_SHIFT_FRAC 0.35
#endif
#ifndef M576_PEAK1D_SWEEP_RECENTER_BASE_FRAC
#define M576_PEAK1D_SWEEP_RECENTER_BASE_FRAC 0.20
#endif
#ifndef M576_PEAK1D_SWEEP_RECENTER_ATTEMPT_STEP_FRAC
#define M576_PEAK1D_SWEEP_RECENTER_ATTEMPT_STEP_FRAC 0.10
#endif
#ifndef M576_PEAK1D_SWEEP_RECENTER_EDGE_ARGMAX_FRAC
#define M576_PEAK1D_SWEEP_RECENTER_EDGE_ARGMAX_FRAC 0.25
#endif
#ifndef M576_PEAK1D_SWEEP_RECENTER_EDGE_BONUS_FRAC
#define M576_PEAK1D_SWEEP_RECENTER_EDGE_BONUS_FRAC 0.05
#endif
/// Blend failed-fit t* vs heuristic index shift (VertexOutOfRange / edge).
#ifndef M576_PEAK1D_SWEEP_RECENTER_TSTAR_WEIGHT
#define M576_PEAK1D_SWEEP_RECENTER_TSTAR_WEIGHT 0.65
#endif
/// Per retry attempt: multiply |shift| by (1 + GROWTH * attemptIndex).
#ifndef M576_PEAK1D_SWEEP_RECENTER_ATTEMPT_GROWTH
#define M576_PEAK1D_SWEEP_RECENTER_ATTEMPT_GROWTH 0.12
#endif
/// Same edge argmax as prior attempt: extra |shift| multiplier.
#ifndef M576_PEAK1D_SWEEP_RECENTER_STAGNATION_GAIN
#define M576_PEAK1D_SWEEP_RECENTER_STAGNATION_GAIN 0.18
#endif
#ifndef M576_PEAK1D_FLAT_REL_SPAN_FRAC
#define M576_PEAK1D_FLAT_REL_SPAN_FRAC 0.002
#endif
/// 单调包络窗不足时，以 argmax 为中心固定半窗兜底：half = clamp((n-1)*FRAC, MIN, MAX)。
#ifndef M576_PEAK1D_FIT_HALF_MIN
#define M576_PEAK1D_FIT_HALF_MIN 4
#endif
#ifndef M576_PEAK1D_FIT_HALF_MAX
#define M576_PEAK1D_FIT_HALF_MAX 16
#endif
#ifndef M576_PEAK1D_FIT_HALF_FRAC
#define M576_PEAK1D_FIT_HALF_FRAC 0.25
#endif
