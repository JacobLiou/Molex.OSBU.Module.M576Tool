#include "stdafx.h"
#include <cstdio>
#include "Mems1x64LutBinWriter.h"
#include "OpCRC32.h"
#include <algorithm>
#include <memory>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

namespace {

static void M576OneX64CopyAsciiToBytes(BYTE* dest, size_t destLen, const CString& s)
{
	CStringA sa(s);
	size_t n = (std::min)(destLen - 1, (size_t)sa.GetLength());
	memcpy(dest, (LPCSTR)sa, n);
	if (n < destLen)
		memset(dest + n, 0, destLen - n);
}

static void M576OneX64PutBe32(BYTE* p, DWORD v)
{
	p[0] = (BYTE)(v >> 24);
	p[1] = (BYTE)(v >> 16);
	p[2] = (BYTE)(v >> 8);
	p[3] = (BYTE)v;
}

static void M576OneX64PutBe16(BYTE* p, WORD v)
{
	p[0] = (BYTE)(v >> 8);
	p[1] = (BYTE)v;
}

// Same per-block tail CRC as CLutBinWriter/stLutSettingZ4671: feed first sizeof-4 bytes; invert; write last 4 bytes.
static void M576OneX64FillLutStyleBlockCrc(stM576OneX64MemsSwCoef& blk)
{
	const size_t cb = sizeof(stM576OneX64MemsSwCoef);
	std::unique_ptr<BYTE[]> pby(new BYTE[cb]);
	memcpy(pby.get(), &blk, cb);
	COpCRC32 crc;
	DWORD dwCRC32Value = 0;
	crc.InitCRC32();
	for (size_t i = 0; i + 4 < cb; ++i)
		dwCRC32Value = crc.GetThisCRC(pby[i]);
	dwCRC32Value = ~dwCRC32Value;
	blk.dwCRC32 = (unsigned int)dwCRC32Value;
}

// Flash bases for four MEMS switches (CalibConstants.h / 126S).
static DWORD M576OneX64SwitchFlashBase(int swIndex)
{
	static const DWORD kAddr[4] = { 0x0E000u, 0x0E800u, 0x0F000u, 0x0F800u };
	if (swIndex < 0 || swIndex > 3)
		return 0x0E000u;
	return kAddr[swIndex];
}

/// BUNDLEHEADER[0..151] per 126S CreateSwitchPointBin (remainder of [160] left zero).
static void M576OneX64FillLegacy126sBundleHeader(
	stM576OneX64MemsSwCoef& sw, int swIndex, const CString& bundleSnVer, const CString& bundleTime)
{
	ZeroMemory(sw.BUNDLEHEADER, sizeof(sw.BUNDLEHEADER));

	M576OneX64CopyAsciiToBytes(sw.BUNDLEHEADER + 0, 8, CString(_T("OPLINK")));
	M576OneX64CopyAsciiToBytes(sw.BUNDLEHEADER + 8, 8, CString(_T("SWITCH")));
	// 16..19 dwBundleHdrCRC32 = 0 (already zero)
	M576OneX64PutBe32(sw.BUNDLEHEADER + 20, 0x000008A0u); // dwBundleSize
	M576OneX64PutBe16(sw.BUNDLEHEADER + 24, 128); // wBundleHdrSize
	M576OneX64PutBe16(sw.BUNDLEHEADER + 26, 1); // wImageCount

	M576OneX64CopyAsciiToBytes(sw.BUNDLEHEADER + 32, 16, bundleSnVer);
	M576OneX64CopyAsciiToBytes(sw.BUNDLEHEADER + 48, 32, CString(_T("14538_1x64MemsSw")));
	M576OneX64CopyAsciiToBytes(sw.BUNDLEHEADER + 80, 32, CString(_T("SUPERSN")));
	M576OneX64CopyAsciiToBytes(sw.BUNDLEHEADER + 112, 16, bundleTime);

	sw.BUNDLEHEADER[128] = 0x49;
	sw.BUNDLEHEADER[129] = 0x42;
	sw.BUNDLEHEADER[130] = 0x46;
	sw.BUNDLEHEADER[131] = 0x48;
	sw.BUNDLEHEADER[132] = 0x85; // ImageType
	sw.BUNDLEHEADER[133] = 0x00; // Hitless
	sw.BUNDLEHEADER[134] = 0x01; // StorageID
	sw.BUNDLEHEADER[135] = 0x01; // ImageIndex
	// 136..139 dwImageVersion = 0
	SYSTEMTIME st = {};
	GetLocalTime(&st);
	sw.BUNDLEHEADER[140] = (BYTE)(st.wMonth);
	sw.BUNDLEHEADER[141] = (BYTE)(st.wDay);
	sw.BUNDLEHEADER[142] = (BYTE)(st.wHour);
	sw.BUNDLEHEADER[143] = (BYTE)(st.wMinute);

	M576OneX64PutBe32(sw.BUNDLEHEADER + 144, M576OneX64SwitchFlashBase(swIndex));
	// 148..151 dwImageCRC32 = 0
}

} // namespace

static_assert(sizeof(stM576OneX64MemsSwCoef) == 2048u, "1x64 single switch bin");
static_assert(M576_1X64_MEMS_FILE_PAYLOAD_BYTES == 8192u, "4*2048=8192");

BOOL CMems1x64LutBinWriter::WriteSingleSwitch(
	const stM576OneX64MemsSwCoef& sw,
	int swIndex,
	LPCTSTR outPath,
	const CString& bundleSnVer,
	const CString& bundleTime)
{
	if (outPath == NULL || swIndex < 0 || swIndex > 3)
		return FALSE;

	stM576OneX64MemsSwCoef blk = sw;
	// 126S: BUNDLEHEADER[32..47] uses product SN (CreateSwitchPointBin: m_strSN in bBundleVersion field).
	CString ver = bundleSnVer;
	if (ver.IsEmpty())
		ver = _T("SN000000");
	CString t = bundleTime;
	if (t.IsEmpty())
	{
		SYSTEMTIME st = {};
		GetLocalTime(&st);
		t.Format(_T("%04d.%02d.%02d.%02d.%02d"), st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute);
	}
	M576OneX64FillLegacy126sBundleHeader(blk, swIndex, ver, t);
	M576OneX64FillLutStyleBlockCrc(blk);

	FILE* fp = NULL;
	if (_tfopen_s(&fp, outPath, _T("wb")) != 0 || fp == NULL)
		return FALSE;
	const size_t n = fwrite(&blk, 1, sizeof(blk), fp);
	fclose(fp);
	return n == sizeof(blk);
}

BOOL CMems1x64LutBinWriter::ReadMemsFromFile(LPCTSTR szPath, stM576OneX64MemsSwCoef* pOutOne)
{
	if (pOutOne == NULL)
		return FALSE;
	FILE* fp = NULL;
	if (_tfopen_s(&fp, szPath, _T("rb")) != 0 || fp == NULL)
		return FALSE;
	if (fseek(fp, 0, SEEK_END) != 0)
	{
		fclose(fp);
		return FALSE;
	}
	const long flenLong = ftell(fp);
	if (flenLong < (long)sizeof(stM576OneX64MemsSwCoef))
	{
		fclose(fp);
		return FALSE;
	}
	if (fseek(fp, 0, SEEK_SET) != 0)
	{
		fclose(fp);
		return FALSE;
	}
	const size_t kOne = sizeof(stM576OneX64MemsSwCoef);
	const size_t n = fread(pOutOne, 1, kOne, fp);
	fclose(fp);
	return n == kOne;
}
