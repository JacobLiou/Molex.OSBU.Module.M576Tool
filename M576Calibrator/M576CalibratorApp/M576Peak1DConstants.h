#pragma once
// 仅含 1D 寻峰/预处理与三阶拟合门限宏，无 TCHAR/MFC。供 PeakFinder2D、CrossPeakTest 与 CalibConstants 共用。

/// ValidateUnimodal1DAtArgmax：span=max-min 严格小于本值则 LowSpan。ParabolaVertexMax1D（三阶主路径）不据此早退。
#ifndef M576_PEAK1D_MIN_SPAN_DB
#define M576_PEAK1D_MIN_SPAN_DB 2.5
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
