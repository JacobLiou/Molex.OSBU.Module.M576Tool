#include <vector>
#include <algorithm>
#include <cmath>
#include "PeakFinder2D.h"
#include "M576Peak1DConstants.h"
// 格点邻域/全局最大；RECAL3/5 一维为全样本二次最小二乘 + 顶点 t*（连续下标）十字寻峰。

namespace M576
{
	bool IsRecal1DPowerInvalidValue(double v);

	namespace
	{
		static const double kDetEps = 1e-18;
		static const double kAmaxEps = 1e-20; // 拟合 a 须 < -kAmaxEps 才视为有顶
		static const double kTRangeEps = 1e-9; // 顶点在 [0,n-1] 的容差

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

		/// 仅对非无效占位点取 min/max；至少 1 个有效点时返回 true。
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

		/// 3x3: 高斯消元为上三角，再回代；A,b 被就地修改。
		static bool Solve3x3(double A[3][3], double b[3], double x[3])
		{
			for (int p = 0; p < 3; ++p)
			{
				int pivot = p;
				double amax = std::abs(A[p][p]);
				for (int r = p + 1; r < 3; ++r)
				{
					const double t = std::abs(A[r][p]);
					if (t > amax)
					{
						amax = t;
						pivot = r;
					}
				}
				if (amax < kDetEps)
					return false;
				if (pivot != p)
				{
					for (int c = 0; c < 3; ++c)
						std::swap(A[p][c], A[pivot][c]);
					std::swap(b[p], b[pivot]);
				}
				for (int r = p + 1; r < 3; ++r)
				{
					const double f = A[r][p] / A[p][p];
					for (int c = p; c < 3; ++c)
						A[r][c] -= f * A[p][c];
					b[r] -= f * b[p];
				}
			}
			if (std::abs(A[2][2]) < kDetEps)
				return false;
			for (int i = 2; i >= 0; --i)
			{
				double s = b[i];
				for (int j = i + 1; j < 3; ++j)
					s -= A[i][j] * x[j];
				if (std::abs(A[i][i]) < kDetEps)
					return false;
				x[i] = s / A[i][i];
			}
			return std::isfinite(x[0]) && std::isfinite(x[1]) && std::isfinite(x[2]);
		}

		/// Strict local max (kept for ValidateUnimodal1DAtArgmax)
		static bool IsStrictLocalMaxAt(const std::vector<double>& p, int i, double eps)
		{
			const int n = (int)p.size();
			if (n <= 0 || i < 0 || i >= n)
				return false;
			if (n == 1)
				return false;
			if (i > 0 && i < n - 1)
				return p[(size_t)i] > p[(size_t)i - 1] + eps && p[(size_t)i] > p[(size_t)i + 1] + eps;
			if (i == 0)
				return p[0] > p[1] + eps;
			return p[(size_t)i] > p[(size_t)i - 1] + eps;
		}

		static int CountStrictLocalMaxima(const std::vector<double>& p, double eps)
		{
			const int n = (int)p.size();
			int c = 0;
			for (int i = 0; i < n; ++i)
			{
				if (IsStrictLocalMaxAt(p, i, eps))
					++c;
			}
			return c;
		}
	} // namespace

	static bool InRange(int r, int c, int rows, int cols)
	{
		return r >= 0 && c >= 0 && r < rows && c < cols;
	}

	bool PeakCross2D(const std::vector<double>& data, int rows, int cols, int& outRow, int& outCol)
	{
		if (rows <= 0 || cols <= 0 || (size_t)rows * (size_t)cols != data.size())
			return false;
		double best = -1e300;
		outRow = outCol = 0;
		for (int r = 0; r < rows; ++r)
		{
			for (int c = 0; c < cols; ++c)
			{
				double v = data[(size_t)r * (size_t)cols + (size_t)c];
				double sum = v;
				int cnt = 1;
				static const int dr[4] = { -1, 1, 0, 0 };
				static const int dc[4] = { 0, 0, -1, 1 };
				for (int k = 0; k < 4; ++k)
				{
					int rr = r + dr[k], cc = c + dc[k];
					if (InRange(rr, cc, rows, cols))
					{
						sum += data[(size_t)rr * (size_t)cols + (size_t)cc];
						cnt++;
					}
				}
				double score = sum / cnt;
				if (score > best)
				{
					best = score;
					outRow = r;
					outCol = c;
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
		for (int r = 0; r < rows; ++r)
		{
			for (int c = 0; c < cols; ++c)
			{
				double v = data[(size_t)r * (size_t)cols + (size_t)c];
				if (v > best)
				{
					best = v;
					outRow = r;
					outCol = c;
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

	bool ParabolaVertexMax1D(const std::vector<double>& p, double& outT, Peak1DValidateCode& f)
	{
		f = Peak1DValidateCode::Ok;
		const int n = (int)p.size();
		if (n < 3)
		{
			f = Peak1DValidateCode::NotEnoughSamples;
			return false;
		}
		int nValid = 0;
		double lo = 0, hi = 0;
		if (!MinMaxRangeExcludingInvalid(p, lo, hi, nValid) || nValid < 1)
		{
			f = Peak1DValidateCode::Empty;
			return false;
		}
		if (nValid < 3)
		{
			f = Peak1DValidateCode::NotEnoughValidSamples;
			return false;
		}
		(void)lo;
		(void)hi;

		// 掩码最小二乘：下标为原格点 0..n-1，仅对有效 p(i) 累加。
		double s1 = 0, s2 = 0, s3 = 0, s4 = 0, t0 = 0, t1 = 0, t2 = 0;
		for (int i = 0; i < n; ++i)
		{
			const double vi = p[(size_t)i];
			if (IsRecal1DPowerInvalidValue(vi))
				continue;
			const double di = (double)i;
			const double i2 = di * di;
			const double i3 = i2 * di;
			const double i4 = i2 * i2;
			s1 += di;
			s2 += i2;
			s3 += i3;
			s4 += i4;
			t0 += vi;
			t1 += vi * di;
			t2 += vi * i2;
		}
		double A[3][3] = {
			{ s4, s3, s2 },
			{ s3, s2, s1 },
			{ s2, s1, (double)nValid } };
		double bvec[3] = { t2, t1, t0 };
		double abc[3] = { 0, 0, 0 };
		if (!Solve3x3(A, bvec, abc))
		{
			f = Peak1DValidateCode::ParabolaFitSingular;
			return false;
		}
		const double a = abc[0], bcoef = abc[1];
		if (a >= -kAmaxEps)
		{
			f = Peak1DValidateCode::ParabolaNotDownward;
			return false;
		}
		outT = -0.5 * bcoef / a;
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

	bool ValidateUnimodal1DAtArgmax(const std::vector<double>& data, int i, Peak1DValidateCode& f)
	{
		f = Peak1DValidateCode::Ok;
		if (data.empty() || i < 0 || i >= (int)data.size())
		{
			f = Peak1DValidateCode::Empty;
			return false;
		}
		const int n = (int)data.size();
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
		if (i == 0 || i == n - 1)
		{
			f = Peak1DValidateCode::EdgeNotAllowed;
			return false;
		}
#endif
		if (i > 0 && i < n - 1)
		{
			if (data[(size_t)i] > data[(size_t)i - 1] + eps && data[(size_t)i] > data[(size_t)i + 1] + eps)
				return true;
			f = Peak1DValidateCode::NotStrictLocal;
			return false;
		}
		if (i == 0)
		{
			if (n >= 2 && data[0] > data[1] + eps)
				return true;
			f = Peak1DValidateCode::NotStrictLocal;
			return false;
		}
		if (n >= 2 && data[(size_t)i] > data[(size_t)i - 1] + eps)
			return true;
		f = Peak1DValidateCode::NotStrictLocal;
		return false;
	}

	bool FindUnimodalPeak1DIndex(
		const std::vector<double>& data,
		int& outIdx,
		Peak1DValidateCode& f,
		double* outTParabola)
	{
		f = Peak1DValidateCode::Empty;
		if (outTParabola)
			*outTParabola = 0.0;
		double t = 0.0;
		if (!ParabolaVertexMax1D(data, t, f))
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
		double* outTX)
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
		const bool okY = ParabolaVertexMax1D(powY, tY, cy);
		const bool okX = ParabolaVertexMax1D(powX, tX, cx);
		if (yDetail)
			*yDetail = okY ? Peak1DValidateCode::Ok : cy;
		if (xDetail)
			*xDetail = okX ? Peak1DValidateCode::Ok : cx;
		if (!okY || !okX)
			return false;
		outRow = (int)std::lround(tY);
		outCol = (int)std::lround(tX);
		const int n = (int)powY.size();
		if (outRow < 0)
			outRow = 0;
		if (outRow >= n)
			outRow = n - 1;
		if (outCol < 0)
			outCol = 0;
		if (outCol >= n)
			outCol = n - 1;
		if (outTY)
			*outTY = tY;
		if (outTX)
			*outTX = tX;
		return true;
	}
}
