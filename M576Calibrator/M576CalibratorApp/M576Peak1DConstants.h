#pragma once
// 仅含 1D 寻峰/抛物线门限宏，无 TCHAR/MFC。供 PeakFinder2D、CrossPeakTest 与 CalibConstants 共用。

/// ValidateUnimodal1DAtArgmax：span=max-min 严格小于本值则 LowSpan。抛物线主路径（ParabolaVertexMax1D）不再据此早退。
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
