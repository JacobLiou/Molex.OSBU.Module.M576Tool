#include "stdafx.h"
#include "PeakFinder2D.h"

namespace M576
{
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
}
