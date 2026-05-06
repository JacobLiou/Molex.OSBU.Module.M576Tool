#include <vector>
#include <algorithm>
#include <cmath>
#include <utility>
#include "PeakFinder2D.h"
#include "M576Peak1DConstants.h"
// 格点邻域/全局最大；RECAL3/5 一维：预处理对称窗 + 三阶 LSQ + 区间最大，连续峰位下标十字寻峰。

namespace M576
{
	bool IsRecal1DPowerInvalidValue(double v);

	namespace
	{
		static const double kDetEps = 1e-18;
		static const double kTRangeEps = 1e-9;
		static const double kCubAbsWeak = 1e-17;
		static const double kDerivRejectScale = 0.05;
		static const double kFlatSpanMult = 1000; // spanAll≤MIN_ABS*k => 视同常值线拒绝

		static bool MinMaxRange(const std::vector<double>& p, double& outMin, double& outMax)
		{
			if (p.empty())
				return false;
			outMin = outMax = p[0];
			for (size_t i = 1; i < p.size(); ++i)
			{
				outMin = (std::min)(outMin, p[i]);
				outMax = (std::max)(outMax, p[i]);
			}
			return true;
		}

		static void ResetAndFillTraceGlobalMax(const std::vector<double>& p, Peak1DFitTrace* trace)
		{
			if (!trace)
				return;
			trace->fitIndex.clear();
			trace->fitY.clear();
			trace->globalMaxIndex = -1;
			trace->globalMaxY = 0;
			const int n = (int)p.size();
			bool have = false;
			for (int i = 0; i < n; ++i)
			{
				if (IsRecal1DPowerInvalidValue(p[(size_t)i]))
					continue;
				const double v = p[(size_t)i];
				if (!have || v > trace->globalMaxY)
				{
					trace->globalMaxY = v;
					trace->globalMaxIndex = i;
					have = true;
				}
			}
			if (!have)
			{
				trace->globalMaxIndex = -1;
				trace->globalMaxY = 0;
			}
		}

		static bool MinMaxRangeExcludingInvalid(const std::vector<double>& p, double& outMin, double& outMax, int& outCountValid)
		{
			outCountValid = 0;
			bool have = false;
			for (size_t i = 0; i < p.size(); ++i)
			{
				if (IsRecal1DPowerInvalidValue(p[i]))
					continue;
				++outCountValid;
				if (!have)
				{
					outMin = outMax = p[i];
					have = true;
				}
				else
				{
					outMin = (std::min)(outMin, p[i]);
					outMax = (std::max)(outMax, p[i]);
				}
			}
			return have;
		}

		static bool Solve4x4(std::vector<std::vector<double>>& A, std::vector<double>& B, double coeff[4])
		{
			const int nn = 4;
			for (int kcol = 0; kcol < nn; ++kcol)
			{
				int pivotRow = kcol;
				double amax = std::abs(A[(size_t)kcol][(size_t)kcol]);
				for (int r = kcol + 1; r < nn; ++r)
				{
					const double t = std::abs(A[(size_t)r][(size_t)kcol]);
					if (t > amax)
					{
						amax = t;
						pivotRow = r;
					}
				}
				if (amax < kDetEps)
					return false;
				if (pivotRow != kcol)
				{
					std::swap(A[(size_t)kcol], A[(size_t)pivotRow]);
					std::swap(B[(size_t)kcol], B[(size_t)pivotRow]);
				}
				for (int r = kcol + 1; r < nn; ++r)
				{
					const double f = A[(size_t)r][(size_t)kcol] / A[(size_t)kcol][(size_t)kcol];
					for (int c = kcol; c < nn; ++c)
						A[(size_t)r][(size_t)c] -= f * A[(size_t)kcol][(size_t)c];
					B[(size_t)r] -= f * B[(size_t)kcol];
				}
			}
			for (int i = nn - 1; i >= 0; --i)
			{
				double sum = B[(size_t)i];
				for (int j = i + 1; j < nn; ++j)
					sum -= A[(size_t)i][(size_t)j] * coeff[j];
				if (std::abs(A[(size_t)i][(size_t)i]) < kDetEps)
					return false;
				coeff[i] = sum / A[(size_t)i][(size_t)i];
			}
			return std::isfinite(coeff[0]) && std::isfinite(coeff[1]) && std::isfinite(coeff[2]) && std::isfinite(coeff[3]);
		}

		static double CubicEval(double a, double b, double c, double d, double x)
		{
			return (((a * x + b) * x) + c) * x + d;
		}

		static double CubicDeriv(double a, double b, double c, double x)
		{
			return ((3 * a * x + 2 * b) * x) + c;
		}

		static bool FitCubicLeastSquares(const std::vector<double>& xs, const std::vector<double>& ys, double outABCD[4])
		{
			const int n = (int)xs.size();
			if (n != (int)ys.size() || n < (int)M576_PEAK1D_CUBIC_MIN_SAMPLES)
				return false;
			std::vector<std::vector<double>> A(4, std::vector<double>(4, 0));
			std::vector<double> B(4, 0);
			for (int i = 0; i < n; ++i)
			{
				const double xi = xs[(size_t)i];
				double xi2 = xi * xi;
				double xi3 = xi2 * xi;
				double xi4 = xi3 * xi;
				double xi5 = xi4 * xi;
				double xi6 = xi5 * xi;
				const double yi = ys[(size_t)i];

				A[0][0] += xi6; A[0][1] += xi5; A[0][2] += xi4; A[0][3] += xi3;
				A[1][0] += xi5; A[1][1] += xi4; A[1][2] += xi3; A[1][3] += xi2;
				A[2][0] += xi4; A[2][1] += xi3; A[2][2] += xi2; A[2][3] += xi;
				A[3][0] += xi3; A[3][1] += xi2; A[3][2] += xi;  A[3][3] += 1;

				B[0] += xi3 * yi;
				B[1] += xi2 * yi;
				B[2] += xi * yi;
				B[3] += yi;
			}
			return Solve4x4(A, B, outABCD);
		}

		static void CubicArgmaxOnClosedInterval(double a, double b, double c, double d, double xa, double xb,
			double& bestX, double& bestY)
		{
			struct Cand { double x, y; };
			std::vector<Cand> cand;
			cand.reserve(6);

			auto tryAdd = [&](double x)
			{
				if (x >= xa - kTRangeEps && x <= xb + kTRangeEps && std::isfinite(x))
				{
					double y = CubicEval(a, b, c, d, x);
					if (std::isfinite(y))
						cand.push_back({ x, y });
				}
			};

			if (std::abs(a) >= kCubAbsWeak)
			{
				const double delta = 4 * b * b - 12 * a * c;
				if (delta >= 0 && std::isfinite(delta))
				{
					const double sdel = std::sqrt(delta);
					const double denom = 6 * a;
					if (std::abs(denom) >= kCubAbsWeak)
					{
						tryAdd((-2 * b + sdel) / denom);
						tryAdd((-2 * b - sdel) / denom);
					}
				}
			}
			else if (std::abs(2 * b) >= kDetEps)
			{
				tryAdd(-c / (2 * b));
			}

			tryAdd(xa);
			tryAdd(xb);

			bestX = xa;
			bestY = CubicEval(a, b, c, d, xa);
			for (const Cand& w : cand)
			{
				if (w.y > bestY || (std::abs(w.y - bestY) < kTRangeEps && w.x < bestX))
				{
					bestY = w.y;
					bestX = w.x;
				}
			}
		}

		/// xs/ys：原格点下标递增；跨度 span 与该批样本 min/max。
		static bool FitsAreStrictMonotoneAlongIndex(const std::vector<double>& xs, const std::vector<double>& ys, double epsMono)
		{
			if (xs.size() < 2)
				return false;
			bool strictInc = true;
			for (size_t k = 1; k < xs.size(); ++k)
			{
				double dy = ys[k] - ys[k - 1];
				if (dy <= epsMono)
					strictInc = false;
			}
			// 仅在沿下标严格上升时拒绝（整体向扫频末端爬升无虚峰）；单侧下降翼或洞左仅保留右翼仍有效。
			return strictInc;
		}

		static bool Peak1DSamplesFromPreprocessed(
			const std::vector<double>& p,
			double& spanAllValid,
			std::vector<double>& xs,
			std::vector<double>& ys,
			Peak1DValidateCode& f)
		{
			f = Peak1DValidateCode::Ok;
			xs.clear();
			ys.clear();

			const int n = (int)p.size();
			int nValid = 0;
			double vmin = 0, vmax = 0;
			if (!MinMaxRangeExcludingInvalid(p, vmin, vmax, nValid))
			{
				f = Peak1DValidateCode::Empty;
				return false;
			}
			if (nValid < (int)M576_PEAK1D_CUBIC_MIN_SAMPLES)
			{
				f = Peak1DValidateCode::NotEnoughValidSamples;
				return false;
			}

			spanAllValid = vmax - vmin;
			const double eps = (std::max)((double)M576_PEAK1D_MIN_ABS_EPS_DB,
				spanAllValid * (double)M576_PEAK1D_EPS_REL_OF_SPAN);
			const double outlierThrRel = spanAllValid * (double)M576_PEAK1D_OUTLIER_MIN_SPAN_FRAC;
			const double outlierThr = (std::max)((double)M576_PEAK1D_OUTLIER_MULT * eps, outlierThrRel);

			std::vector<unsigned char> baseOk((size_t)n, 0);
			std::vector<unsigned char> useOk((size_t)n, 0);
			for (int i = 0; i < n; ++i)
			{
				if (!IsRecal1DPowerInvalidValue(p[(size_t)i]))
					baseOk[(size_t)i] = 1;
			}

			for (int i = 0; i < n; ++i)
			{
				if (!baseOk[(size_t)i])
					continue;
				double nbrMax = 0;
				bool hn = false;
				const int hl = M576_PEAK1D_OUTLIER_LOCAL_HALF;
				for (int d = -hl; d <= hl; ++d)
				{
					if (d == 0)
						continue;
					const int j = i + d;
					if (j < 0 || j >= n)
						continue;
					if (!baseOk[(size_t)j])
						continue;
					const double v = p[(size_t)j];
					if (!hn)
					{
						nbrMax = v;
						hn = true;
					}
					else
						nbrMax = (std::max)(nbrMax, v);
				}
				if (!hn)
				{
					useOk[(size_t)i] = 1;
					continue;
				}
				const double yi = p[(size_t)i];
				if (yi > nbrMax + outlierThr)
					useOk[(size_t)i] = 0;
				else
					useOk[(size_t)i] = 1;
			}

			int bestI = -1;
			double bestY = -(1e300);
			for (int i = 0; i < n; ++i)
			{
				if (!useOk[(size_t)i])
					continue;
				if (p[(size_t)i] > bestY)
				{
					bestY = p[(size_t)i];
					bestI = i;
				}
			}
			if (bestI < 0)
			{
				f = Peak1DValidateCode::Empty;
				return false;
			}
			const int i0 = bestI;

			int leftBound = i0;
			for (int i = i0 - 1; i >= 0; --i)
			{
				if (!useOk[(size_t)i])
					break;
				if (!(p[(size_t)i] <= p[(size_t)(i + 1)] + eps))
					break;
				leftBound = i;
			}
			int rightBound = i0;
			for (int j = i0 + 1; j < n; ++j)
			{
				if (!useOk[(size_t)j])
					break;
				if (!(p[(size_t)j] <= p[(size_t)(j - 1)] + eps))
					break;
				rightBound = j;
			}

			const int halfW = (std::min)(i0 - leftBound, rightBound - i0);
			auto collectWindow = [&](int lo, int hi)
			{
				xs.clear();
				ys.clear();
				for (int i = lo; i <= hi; ++i)
				{
					if (!useOk[(size_t)i])
						continue;
					xs.push_back((double)i);
					ys.push_back(p[(size_t)i]);
				}
			};
			collectWindow(i0 - halfW, i0 + halfW);
			if (xs.size() < (size_t)M576_PEAK1D_CUBIC_MIN_SAMPLES || halfW == 0)
				collectWindow(leftBound, rightBound);
			if (xs.size() < (size_t)M576_PEAK1D_CUBIC_MIN_SAMPLES)
			{
				f = Peak1DValidateCode::NotEnoughValidSamples;
				xs.clear();
				ys.clear();
				return false;
			}
			std::vector<std::pair<double, double>> sortBuf;
			sortBuf.reserve(xs.size());
			for (size_t k = 0; k < xs.size(); ++k)
				sortBuf.push_back(std::make_pair(xs[k], ys[k]));
			std::sort(sortBuf.begin(), sortBuf.end(),
				[](const std::pair<double, double>& u, const std::pair<double, double>& v) { return u.first < v.first; });
			for (size_t k = 0; k < sortBuf.size(); ++k)
			{
				xs[k] = sortBuf[k].first;
				ys[k] = sortBuf[k].second;
			}

			double loPts = ys.front(), hiPts = ys.front();
			for (size_t k = 1; k < ys.size(); ++k)
			{
				loPts = (std::min)(loPts, ys[k]);
				hiPts = (std::max)(hiPts, ys[k]);
			}
			const double spanPts = hiPts - loPts;
			const double epsMono = (std::max)((double)M576_PEAK1D_MIN_ABS_EPS_DB, spanPts * (double)M576_PEAK1D_EPS_REL_OF_SPAN);

			if (FitsAreStrictMonotoneAlongIndex(xs, ys, epsMono))
			{
				f = Peak1DValidateCode::ParabolaNotDownward;
				xs.clear();
				ys.clear();
				return false;
			}
			return true;
		}

		static bool IsStrictLocalMaxAt(const std::vector<double>& p, int i, double eps)
		{
			const int nnn = (int)p.size();
			if (nnn <= 0 || i < 0 || i >= nnn)
				return false;
			if (nnn == 1)
				return false;
			if (i > 0 && i < nnn - 1)
				return p[(size_t)i] > p[(size_t)i - 1] + eps && p[(size_t)i] > p[(size_t)i + 1] + eps;
			if (i == 0)
				return p[0] > p[1] + eps;
			return p[(size_t)i] > p[(size_t)i - 1] + eps;
		}

		static int CountStrictLocalMaxima(const std::vector<double>& p, double eps)
		{
			const int nnn = (int)p.size();
			int c = 0;
			for (int ii = 0; ii < nnn; ++ii)
			{
				if (IsStrictLocalMaxAt(p, ii, eps))
					++c;
			}
			return c;
		}
	} // namespace


	static bool InRange(int rr, int cc, int rows, int cols)
	{
		return rr >= 0 && cc >= 0 && rr < rows && cc < cols;
	}

	bool PeakCross2D(const std::vector<double>& data, int rows, int cols, int& outRow, int& outCol)
	{
		if (rows <= 0 || cols <= 0 || (size_t)rows * (size_t)cols != data.size())
			return false;
		double best = -1e300;
		outRow = outCol = 0;
		for (int rr = 0; rr < rows; ++rr)
		{
			for (int cc = 0; cc < cols; ++cc)
			{
				double v = data[(size_t)rr * (size_t)cols + (size_t)cc];
				double sum = v;
				int cnt = 1;
				static const int dr[4] = { -1, 1, 0, 0 };
				static const int dc[4] = { 0, 0, -1, 1 };
				for (int k = 0; k < 4; ++k)
				{
					int r2 = rr + dr[k], c2 = cc + dc[k];
					if (InRange(r2, c2, rows, cols))
					{
						sum += data[(size_t)r2 * (size_t)cols + (size_t)c2];
						cnt++;
					}
				}
				double score = sum / cnt;
				if (score > best)
				{
					best = score;
					outRow = rr;
					outCol = cc;
				}
			}
		}
		return true;
	}

	bool PeakMax2D(const std::vector<double>& data, int rows, int cols, int& outRow, int& outCol)
	{
		if (rows <= 0 || cols <= 0 || (size_t)rows * (size_t)cols != data.size())
			return false;
		double best = -1e300;
		outRow = outCol = 0;
		for (int rr = 0; rr < rows; ++rr)
		{
			for (int cc = 0; cc < cols; ++cc)
			{
				double v = data[(size_t)rr * (size_t)cols + (size_t)cc];
				if (v > best)
				{
					best = v;
					outRow = rr;
					outCol = cc;
				}
			}
		}
		return true;
	}

	bool PeakMax1D(const std::vector<double>& data, int& outIdx)
	{
		if (data.empty())
			return false;
		outIdx = 0;
		double best = data[0];
		for (size_t i = 1; i < data.size(); ++i)
		{
			if (data[i] > best)
			{
				best = data[i];
				outIdx = (int)i;
			}
		}
		return true;
	}

	bool IsRecal1DPowerInvalidValue(double v)
	{
		return v == (double)M576_RECAL_POW_INVALID_1 || v == (double)M576_RECAL_POW_INVALID_2;
	}

	bool ParabolaVertexMax1D(const std::vector<double>& p, double& outT, Peak1DValidateCode& f, Peak1DFitTrace* trace)
	{
		f = Peak1DValidateCode::Ok;
		outT = 0;
		ResetAndFillTraceGlobalMax(p, trace);
		const int n = (int)p.size();
		if (n < (int)M576_PEAK1D_CUBIC_MIN_SAMPLES)
		{
			f = Peak1DValidateCode::NotEnoughSamples;
			return false;
		}

		std::vector<double> xs;
		std::vector<double> ys;
		double spanAll = 0;
		if (!Peak1DSamplesFromPreprocessed(p, spanAll, xs, ys, f))
			return false;
		if (trace)
		{
			trace->fitIndex.clear();
			trace->fitY.clear();
			trace->fitIndex.reserve(xs.size());
			trace->fitY.reserve(ys.size());
			for (size_t k = 0; k < xs.size(); ++k)
			{
				trace->fitIndex.push_back((int)std::lround(xs[k]));
				trace->fitY.push_back(ys[k]);
			}
		}
		if (spanAll <= (double)M576_PEAK1D_MIN_ABS_EPS_DB * kFlatSpanMult)
		{
			f = Peak1DValidateCode::ParabolaNotDownward;
			return false;
		}

		const double xmin = xs.front();
		const double xmax = xs.back();

		double abcd[4] = {};
		if (!FitCubicLeastSquares(xs, ys, abcd))
		{
			f = Peak1DValidateCode::ParabolaFitSingular;
			return false;
		}

		double a = abcd[0], coefb = abcd[1], c = abcd[2], dcoef = abcd[3];

		double tStar = xmin;
		double yStar = CubicEval(a, coefb, c, dcoef, xmin);
		CubicArgmaxOnClosedInterval(a, coefb, c, dcoef, xmin, xmax, tStar, yStar);
		if (!std::isfinite(tStar) || !std::isfinite(yStar))
		{
			f = Peak1DValidateCode::ParabolaFitSingular;
			return false;
		}

		const double spanRef = spanAll <= 0 ? 1.0 : spanAll;
		const double derivReject = spanRef * (double)M576_PEAK1D_EPS_REL_OF_SPAN * kDerivRejectScale;
		const bool attachSeqEnd = std::abs(tStar - xmax) <= kTRangeEps || tStar >= xmax - kTRangeEps;
		if ((double)n >= (double)M576_PEAK1D_CUBIC_MIN_SAMPLES && attachSeqEnd && xmax >= (double)(n - 1) - kTRangeEps)
		{
			const double xh = xmax - (std::max)(kTRangeEps * 100, (xmax - xmin) * 1e-9 + kTRangeEps);
			if (xh > xmin + kTRangeEps && CubicDeriv(a, coefb, c, xh) > derivReject)
			{
				f = Peak1DValidateCode::ParabolaNotDownward;
				return false;
			}
		}

		outT = tStar;
		if (!std::isfinite(outT) || outT < -kTRangeEps || outT > (double)(n - 1) + kTRangeEps)
		{
			f = Peak1DValidateCode::VertexOutOfRange;
			return false;
		}

#if M576_PEAK1D_REJECT_EDGE_MAX
		if (outT <= kTRangeEps || outT >= (double)(n - 1) - kTRangeEps)
		{
			f = Peak1DValidateCode::EdgeNotAllowed;
			return false;
		}
#endif
		return true;
	}

	bool ValidateUnimodal1DAtArgmax(const std::vector<double>& data, int ii, Peak1DValidateCode& f)
	{
		f = Peak1DValidateCode::Ok;
		if (data.empty() || ii < 0 || ii >= (int)data.size())
		{
			f = Peak1DValidateCode::Empty;
			return false;
		}
		const int nnn = (int)data.size();
		double lo = 0, hi = 0;
		if (!MinMaxRange(data, lo, hi))
		{
			f = Peak1DValidateCode::Empty;
			return false;
		}
		const double span = hi - lo;
		if (span < (double)M576_PEAK1D_MIN_SPAN_DB)
		{
			f = Peak1DValidateCode::LowSpan;
			return false;
		}
		const double eps = (std::max)((double)M576_PEAK1D_MIN_ABS_EPS_DB, span * (double)M576_PEAK1D_EPS_REL_OF_SPAN);

		if (M576_PEAK1D_MAX_STRICT_LOCAL_MAXIMA > 0)
		{
			const int cmax = CountStrictLocalMaxima(data, eps);
			if (cmax > (int)M576_PEAK1D_MAX_STRICT_LOCAL_MAXIMA)
			{
				f = Peak1DValidateCode::MultiLocalMax;
				return false;
			}
		}

#if M576_PEAK1D_REJECT_EDGE_MAX
		if (ii == 0 || ii == nnn - 1)
		{
			f = Peak1DValidateCode::EdgeNotAllowed;
			return false;
		}
#endif
		if (ii > 0 && ii < nnn - 1)
		{
			if (data[(size_t)ii] > data[(size_t)ii - 1] + eps && data[(size_t)ii] > data[(size_t)ii + 1] + eps)
				return true;
			f = Peak1DValidateCode::NotStrictLocal;
			return false;
		}
		if (ii == 0)
		{
			if (nnn >= 2 && data[0] > data[1] + eps)
				return true;
			f = Peak1DValidateCode::NotStrictLocal;
			return false;
		}
		if (nnn >= 2 && data[(size_t)ii] > data[(size_t)ii - 1] + eps)
			return true;
		f = Peak1DValidateCode::NotStrictLocal;
		return false;
	}

	bool FindUnimodalPeak1DIndex(
		const std::vector<double>& data,
		int& outIdx,
		Peak1DValidateCode& f,
		double* outTParabola,
		Peak1DFitTrace* trace)
	{
		f = Peak1DValidateCode::Empty;
		if (outTParabola)
			*outTParabola = 0.0;
		double t = 0.0;
		if (!ParabolaVertexMax1D(data, t, f, trace))
			return false;
		if (outTParabola)
			*outTParabola = t;
		outIdx = (int)std::lround(t);
		if (outIdx < 0)
			outIdx = 0;
		if (outIdx >= (int)data.size())
			outIdx = (int)data.size() - 1;
		return true;
	}

	bool PeakCrossFrom1DScans(
		const std::vector<double>& powY,
		const std::vector<double>& powX,
		int& outRow,
		int& outCol,
		Peak1DValidateCode* yDetail,
		Peak1DValidateCode* xDetail,
		double* outTY,
		double* outTX,
		Peak1DFitTrace* traceY,
		Peak1DFitTrace* traceX)
	{
		if (yDetail)
			*yDetail = Peak1DValidateCode::Ok;
		if (xDetail)
			*xDetail = Peak1DValidateCode::Ok;
		if (outTY)
			*outTY = 0.0;
		if (outTX)
			*outTX = 0.0;
		if (powY.size() != powX.size() || powY.empty())
		{
			if (yDetail)
				*yDetail = Peak1DValidateCode::Empty;
			if (xDetail)
				*xDetail = Peak1DValidateCode::Empty;
			return false;
		}
		double tY = 0, tX = 0;
		Peak1DValidateCode cy = Peak1DValidateCode::Ok, cx = Peak1DValidateCode::Ok;
		const bool okY = ParabolaVertexMax1D(powY, tY, cy, traceY);
		const bool okX = ParabolaVertexMax1D(powX, tX, cx, traceX);
		if (yDetail)
			*yDetail = okY ? Peak1DValidateCode::Ok : cy;
		if (xDetail)
			*xDetail = okX ? Peak1DValidateCode::Ok : cx;
		if (!okY || !okX)
			return false;
		outRow = (int)std::lround(tY);
		outCol = (int)std::lround(tX);
		const int ns = (int)powY.size();
		if (outRow < 0)
			outRow = 0;
		if (outRow >= ns)
			outRow = ns - 1;
		if (outCol < 0)
			outCol = 0;
		if (outCol >= ns)
			outCol = ns - 1;
		if (outTY)
			*outTY = tY;
		if (outTX)
			*outTX = tX;
		return true;
	}
}
