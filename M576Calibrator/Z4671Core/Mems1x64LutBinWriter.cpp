#include "stdafx.h"
#include <cstdio>
#include "Mems1x64LutBinWriter.h"
#include "OpCRC32.h"
#include "ByteSwap.h"
#include <algorithm>
#include <memory>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

static void M576OneX64CopyAsciiToBytes(BYTE* dest, size_t destLen, const CString& s)
{
	CStringA sa(s);
	size_t n = (std::min)(destLen - 1, (size_t)sa.GetLength());
	memcpy(dest, (LPCSTR)sa, n);
	if (n < destLen)
		memset(dest + n, 0, destLen - n);
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

static_assert(sizeof(stM576OneX64MemsSwCoef) * 4 == M576_1X64_MEMS_FILE_PAYLOAD_BYTES, "1x64 payload");

BOOL CMems1x64LutBinWriter::Write(SMems1x64BinWriteParams& params)
{
	if (params.pSw4 == NULL || params.strOutputPath.IsEmpty())
		return FALSE;

	stM576OneX64MemsSwCoef sw4[4];
	for (int i = 0; i < 4; ++i)
		sw4[i] = params.pSw4[i];

	for (int b = 0; b < 4; ++b)
		M576OneX64FillLutStyleBlockCrc(sw4[b]);

	const size_t kPayload = DevicePayloadSize();
	const size_t kCbOne = sizeof(stM576OneX64MemsSwCoef);
	if (kPayload != 4u * kCbOne)
		return FALSE;

	std::unique_ptr<BYTE[]> pby8(new BYTE[kPayload]);
	for (int i = 0; i < 4; ++i)
		memcpy(pby8.get() + i * kCbOne, &sw4[i], kCbOne);

	stLutBundleHeader1 hdr1;
	stLutBundleHeader2 hdr2;
	stImageHeader img;
	ZeroMemory(&hdr1, sizeof(hdr1));
	ZeroMemory(&hdr2, sizeof(hdr2));
	ZeroMemory(&img, sizeof(img));

	M576OneX64CopyAsciiToBytes(hdr1.pBundleTag, sizeof(hdr1.pBundleTag), CString(_T("OPLINK")));
	M576OneX64CopyAsciiToBytes(hdr1.pProductType, sizeof(hdr1.pProductType), CString(_T("SWITCH")));

	const DWORD bundleBody = (DWORD)(sizeof(stLutBundleHeader1) + sizeof(stLutBundleHeader2)
		+ sizeof(stImageHeader) * 2 + (DWORD)kPayload);
	hdr2.dwBundleSize = SwapDWORD(bundleBody);
	hdr2.wBundleHdrSize = SwapWORD(160);
	hdr2.wImageCount = SwapWORD(1);
	M576OneX64CopyAsciiToBytes(hdr2.pBundleVer, sizeof(hdr2.pBundleVer), CString(_T("1.0.0.0")));

	CString strPN = params.strBundlePN;
	if (strPN.IsEmpty())
		strPN = _T("OMSSMCSAMPZAB01");
	M576OneX64CopyAsciiToBytes(hdr2.pBundlePN, sizeof(hdr2.pBundlePN), strPN);
	M576OneX64CopyAsciiToBytes(hdr2.pBundleSN, sizeof(hdr2.pBundleSN), params.strBundleSN);

	SYSTEMTIME st = {};
	GetLocalTime(&st);
	CString strTime = params.strBundleTime;
	if (strTime.IsEmpty())
		strTime.Format(_T("%04d.%02d.%02d.%02d.%02d"), st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute);
	M576OneX64CopyAsciiToBytes(hdr2.pBundleTime, sizeof(hdr2.pBundleTime), strTime);

	img.pLutTag[0] = 0x49;
	img.pLutTag[1] = 0x42;
	img.pLutTag[2] = 0x46;
	img.pLutTag[3] = 0x48;
	img.byImageType = 0x88;
	img.byHitless = 0x00;
	img.byStorageID = 0x01;
	img.byImageIndex = 0x00;
	img.dwImageVersion = 0;
	img.pTimeStamp[0] = (BYTE)(st.wMonth);
	img.pTimeStamp[1] = (BYTE)(st.wDay);
	img.pTimeStamp[2] = (BYTE)(st.wHour);
	img.pTimeStamp[3] = (BYTE)(st.wMinute);
	img.dwBaseAddress = SwapDWORD(params.dwImageBaseAddress);
	img.dwImageSize = SwapDWORD((DWORD)kPayload);
	img.bySectionCount = 0x01;

	COpCRC32 crc;
	DWORD dwCRC32Value = 0;
	crc.InitCRC32();
	for (size_t i = 0; i < kPayload; ++i)
		dwCRC32Value = crc.GetThisCRC(pby8[i]);
	dwCRC32Value = ~dwCRC32Value;
	img.dwImageCRC32 = SwapDWORD(dwCRC32Value);

	PBYTE pbyHeader = new BYTE[sizeof(stLutBundleHeader2) + sizeof(stImageHeader)];
	memcpy(pbyHeader, &hdr2, sizeof(stLutBundleHeader2));
	memcpy(pbyHeader + sizeof(stLutBundleHeader2), &img, sizeof(stImageHeader));
	crc.InitCRC32();
	DWORD dwHdrCRC32 = 0;
	for (size_t i = 0; i < sizeof(stLutBundleHeader2) + sizeof(stImageHeader); ++i)
		dwHdrCRC32 = crc.GetThisCRC(pbyHeader[i]);
	dwHdrCRC32 = ~dwHdrCRC32;
	hdr1.dwBundleHdrCRC32 = SwapDWORD(dwHdrCRC32);
	delete[] pbyHeader;

	FILE* fp = NULL;
	if (_tfopen_s(&fp, params.strOutputPath, _T("wb")) != 0 || fp == NULL)
		return FALSE;
	fwrite(&hdr1, 1, sizeof(hdr1), fp);
	fwrite(&hdr2, 1, sizeof(hdr2), fp);
	fwrite(&img, 1, sizeof(img), fp);
	fwrite(&img, 1, sizeof(img), fp);
	fwrite(pby8.get(), 1, kPayload, fp);
	fclose(fp);
	for (int b = 0; b < 4; ++b)
		params.pSw4[b] = sw4[b];
	return TRUE;
}

BOOL CMems1x64LutBinWriter::ReadMemsFromFile(LPCTSTR szPath, stM576OneX64MemsSwCoef* pOut4)
{
	if (pOut4 == NULL)
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
	if (flenLong < 0)
	{
		fclose(fp);
		return FALSE;
	}
	const __int64 flen = flenLong;
	const size_t kPayload = DevicePayloadSize();
	const size_t kFull = FullBundleFileSize();
	const size_t kOne = sizeof(stM576OneX64MemsSwCoef);
	__int64 offPayload = 0;
	if (flen < (__int64)kOne * 4)
	{
		fclose(fp);
		return FALSE;
	}
	if (flen == (__int64)kFull)
		offPayload = (__int64)LutPayloadOffset(); // full Z4671 bundle: 8K MEMS after header
	else if (flen == (__int64)kPayload)
		offPayload = 0; // raw 8K only
	else if (flen > (__int64)kFull)
		offPayload = (__int64)LutPayloadOffset(); // file longer than nominal bundle; MEMS at same offset
	else
	{
		fclose(fp);
		return FALSE;
	}
	if (fseek(fp, (long)offPayload, SEEK_SET) != 0)
	{
		fclose(fp);
		return FALSE;
	}
	for (int i = 0; i < 4; ++i)
	{
		const size_t n = fread(pOut4 + i, 1, kOne, fp);
		if (n != kOne)
		{
			fclose(fp);
			return FALSE;
		}
	}
	fclose(fp);
	return TRUE;
}
