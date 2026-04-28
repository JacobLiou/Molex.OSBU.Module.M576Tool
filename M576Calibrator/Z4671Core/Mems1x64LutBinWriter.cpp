#include "stdafx.h"
#include <cstdio>
#include "Mems1x64LutBinWriter.h"
#include "OpCRC32.h"
#include <algorithm>

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

/// Fixed-width ASCII: copy up to destLen bytes (no destLen-1 NUL guard); pad remainder with 0.
static void M576OneX64WriteAsciiFixed(BYTE* dest, size_t destLen, const CString& s)
{
	CStringA sa(s);
	size_t n = (std::min)(destLen, (size_t)sa.GetLength());
	if (n > 0)
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

// Flash bases for four MEMS switches (CalibConstants.h / 126S).
static DWORD M576OneX64SwitchFlashBase(int swIndex)
{
	static const DWORD kAddr[4] = { 0x0E000u, 0x0E800u, 0x0F000u, 0x0F800u };
	if (swIndex < 0 || swIndex > 3)
		return 0x0E000u;
	return kAddr[swIndex];
}

/// BUNDLEHEADER[0..159] per 126S CreateSwitchPointBin; CRC slots @16 and @148 remain 0 until WriteSingleSwitch CRC steps.
static void M576OneX64FillLegacy126sBundleHeader(
	stM576OneX64MemsSwCoef& sw,
	int swIndex,
	const CString& bundleSnVer,
	const CString& bundleTime,
	const SYSTEMTIME& stUtc)
{
	ZeroMemory(sw.BUNDLEHEADER, sizeof(sw.BUNDLEHEADER));

	M576OneX64CopyAsciiToBytes(sw.BUNDLEHEADER + 0, 8, CString(_T("OPLINK")));
	M576OneX64CopyAsciiToBytes(sw.BUNDLEHEADER + 8, 8, CString(_T("SWITCH")));
	// 16..19 dwBundleHdrCRC32 = 0 (filled in CRC#3)
	M576OneX64PutBe32(sw.BUNDLEHEADER + 20, 0x000008A0u); // dwBundleSize
	M576OneX64PutBe16(sw.BUNDLEHEADER + 24, 128); // wBundleHdrSize
	M576OneX64PutBe16(sw.BUNDLEHEADER + 26, 1); // wImageCount

	M576OneX64WriteAsciiFixed(sw.BUNDLEHEADER + 32, 16, bundleSnVer);
	M576OneX64WriteAsciiFixed(sw.BUNDLEHEADER + 48, 32, CString(_T("14538_1x64MemsSw")));
	M576OneX64WriteAsciiFixed(sw.BUNDLEHEADER + 80, 32, CString(_T("SUPERSN")));
	M576OneX64WriteAsciiFixed(sw.BUNDLEHEADER + 112, 16, bundleTime);

	sw.BUNDLEHEADER[128] = 0x49;
	sw.BUNDLEHEADER[129] = 0x42;
	sw.BUNDLEHEADER[130] = 0x46;
	sw.BUNDLEHEADER[131] = 0x48;
	sw.BUNDLEHEADER[132] = 0x85; // ImageType
	sw.BUNDLEHEADER[133] = 0x00; // Hitless
	sw.BUNDLEHEADER[134] = 0x01; // StorageID
	sw.BUNDLEHEADER[135] = 0x01; // ImageIndex
	// 136..139 dwImageVersion = 0
	sw.BUNDLEHEADER[140] = (BYTE)(stUtc.wMonth);
	sw.BUNDLEHEADER[141] = (BYTE)(stUtc.wDay);
	sw.BUNDLEHEADER[142] = (BYTE)(stUtc.wHour);
	sw.BUNDLEHEADER[143] = (BYTE)(stUtc.wMinute);

	M576OneX64PutBe32(sw.BUNDLEHEADER + 144, M576OneX64SwitchFlashBase(swIndex));
	// 148..151 dwImageCRC32 placeholder (CRC#2); must be 0 while CRC#1 runs
	M576OneX64PutBe32(sw.BUNDLEHEADER + 148, 0u);
	// 152..155 dwImageSize = 2048 (legacy: dwBinsize - 160)
	M576OneX64PutBe32(sw.BUNDLEHEADER + 152, 2048u);
	// 156..159 fixed magic
	sw.BUNDLEHEADER[156] = 0x01;
	sw.BUNDLEHEADER[157] = 0x00;
	sw.BUNDLEHEADER[158] = 0x00;
	sw.BUNDLEHEADER[159] = 0x00;
}

static void M576OneX64FillBodyTailIdentity(
	stM576OneX64MemsSwCoef& sw, const CString& snAscii, const SYSTEMTIME& stUtc)
{
	ZeroMemory(sw.bReserved7, sizeof(sw.bReserved7));
	M576OneX64WriteAsciiFixed(sw.bReserved7 + 0, 16, snAscii);
	CString tCompact;
	tCompact.Format(_T("%04d%02d%02d%02d%02d"),
		stUtc.wYear, stUtc.wMonth, stUtc.wDay, stUtc.wHour, stUtc.wMinute);
	M576OneX64WriteAsciiFixed(sw.bReserved7 + 16, 12, tCompact);
}

} // namespace

static_assert(sizeof(stM576OneX64MemsSwCoef) == 2208u, "1x64 single-switch file BUNDLEHEADER+body");
static_assert(M576_1X64_MEMS_FILE_PAYLOAD_BYTES == 8832u, "4*2208=8832");

BOOL CMems1x64LutBinWriter::WriteSingleSwitch(
	const stM576OneX64MemsSwCoef& sw,
	int swIndex,
	LPCTSTR outPath,
	const CString& bundleSnVer,
	const CString& bundleTime)
{
	if (outPath == NULL || swIndex < 0 || swIndex > 3)
		return FALSE;

	SYSTEMTIME stUtc = {};
	GetSystemTime(&stUtc);

	CString snVer = bundleSnVer;
	if (snVer.IsEmpty())
		snVer = _T("SN000000");
	CString tDot = bundleTime;
	if (tDot.IsEmpty())
		tDot.Format(_T("%04d.%02d.%02d.%02d.%02d"),
			stUtc.wYear, stUtc.wMonth, stUtc.wDay, stUtc.wHour, stUtc.wMinute);

	stM576OneX64MemsSwCoef blk = sw;

	M576OneX64FillLegacy126sBundleHeader(blk, swIndex, snVer, tDot, stUtc);
	M576OneX64FillBodyTailIdentity(blk, snVer, stUtc);

	// CRC#1: body [160..2203] -> dwCRC32 LE @ offsetof(dwCRC32)
	{
		const BYTE* p = reinterpret_cast<const BYTE*>(&blk);
		COpCRC32 crc;
		crc.InitCRC32();
		DWORD v = 0;
		for (size_t i = 160; i < 2204; ++i)
			v = crc.GetThisCRC(p[i]);
		v = ~v;
		blk.dwCRC32 = (unsigned int)v;
	}

	// CRC#2: body [160..2207] -> BUNDLEHEADER[148..151] BE32
	{
		const BYTE* p = reinterpret_cast<const BYTE*>(&blk);
		COpCRC32 crc;
		crc.InitCRC32();
		DWORD v = 0;
		for (size_t i = 160; i < 2208; ++i)
			v = crc.GetThisCRC(p[i]);
		v = ~v;
		M576OneX64PutBe32(blk.BUNDLEHEADER + 148, v);
	}

	// CRC#3: BUNDLEHEADER [20..127] -> BUNDLEHEADER[16..19] BE32
	{
		const BYTE* p = reinterpret_cast<const BYTE*>(&blk);
		COpCRC32 crc;
		crc.InitCRC32();
		DWORD v = 0;
		for (size_t i = 20; i < 128; ++i)
			v = crc.GetThisCRC(p[i]);
		v = ~v;
		M576OneX64PutBe32(blk.BUNDLEHEADER + 16, v);
	}

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
