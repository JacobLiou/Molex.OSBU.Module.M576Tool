#include "stdafx.h"
#include "LutBinWriter.h"
#include "OpCRC32.h"
#include <algorithm>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

static void CopyAsciiToBytes(BYTE* dest, size_t destLen, const CString& s)
{
	CStringA sa(s);
	size_t n = (std::min)(destLen - 1, (size_t)sa.GetLength());
	memcpy(dest, (LPCSTR)sa, n);
	if (n < destLen)
		memset(dest + n, 0, destLen - n);
}

size_t CLutBinWriter::LutPayloadOffset()
{
	return sizeof(stLutBundleHeader1) + sizeof(stLutBundleHeader2) + sizeof(stImageHeader) * 2;
}

size_t CLutBinWriter::FullBundleFileSize()
{
	return sizeof(stLutBundleHeader1) + sizeof(stLutBundleHeader2) + sizeof(stImageHeader) * 2
		+ sizeof(stLutSettingZ4671);
}

size_t CLutBinWriter::LutDevicePayloadSize()
{
	return FullBundleFileSize() - LutPayloadOffset();
}

BOOL CLutBinWriter::ReadLutFromFile(LPCTSTR szPath, stLutSettingZ4671& lut)
{
	FILE* fp = NULL;
	if (_tfopen_s(&fp, szPath, _T("rb")) != 0 || fp == NULL)
		return FALSE;
	if (_fseeki64(fp, static_cast<__int64>(LutPayloadOffset()), SEEK_SET) != 0)
	{
		fclose(fp);
		return FALSE;
	}
	size_t n = fread(&lut, 1, sizeof(stLutSettingZ4671), fp);
	fclose(fp);
	return n == sizeof(stLutSettingZ4671);
}

BOOL CLutBinWriter::Write(const SLutBinWriteParams& params)
{
	if (params.pLut == NULL || params.strOutputPath.IsEmpty())
		return FALSE;

	stLutBundleHeader1 hdr1;
	stLutBundleHeader2 hdr2;
	stImageHeader img;
	ZeroMemory(&hdr1, sizeof(hdr1));
	ZeroMemory(&hdr2, sizeof(hdr2));
	ZeroMemory(&img, sizeof(img));

	CopyAsciiToBytes(hdr1.pBundleTag, sizeof(hdr1.pBundleTag), CString(_T("OPLINK")));
	CopyAsciiToBytes(hdr1.pProductType, sizeof(hdr1.pProductType), CString(_T("SWITCH")));

	hdr2.dwBundleSize = SwapDWORD(sizeof(stLutBundleHeader1) + sizeof(stLutBundleHeader2)
		+ sizeof(stImageHeader) * 2 + sizeof(stLutSettingZ4671));
	hdr2.wBundleHdrSize = SwapWORD(160);
	hdr2.wImageCount = SwapWORD(1);

	CopyAsciiToBytes(hdr2.pBundleVer, sizeof(hdr2.pBundleVer), CString(_T("1.0.0.0")));

	CString strPN = params.strBundlePN;
	if (strPN.IsEmpty())
		strPN = _T("OMSSMCSAMPZAB01");
	CopyAsciiToBytes(hdr2.pBundlePN, sizeof(hdr2.pBundlePN), strPN);

	CopyAsciiToBytes(hdr2.pBundleSN, sizeof(hdr2.pBundleSN), params.strBundleSN);

	SYSTEMTIME st = {};
	GetLocalTime(&st);
	CString strTime = params.strBundleTime;
	if (strTime.IsEmpty())
		strTime.Format(_T("%04d.%02d.%02d.%02d.%02d"), st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute);
	CopyAsciiToBytes(hdr2.pBundleTime, sizeof(hdr2.pBundleTime), strTime);

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
	img.dwImageSize = SwapDWORD(sizeof(stLutSettingZ4671));
	img.bySectionCount = 0x01;

	stLutSettingZ4671 lutCopy = *params.pLut;

	const size_t cbLut = sizeof(stLutSettingZ4671);
	std::unique_ptr<BYTE[]> pby(new BYTE[cbLut]);

	COpCRC32 crc;
	DWORD dwCRC32Value = 0;
	memcpy(pby.get(), &lutCopy, cbLut);
	crc.InitCRC32();
	for (size_t i = 0; i + 4 < cbLut; ++i)
		dwCRC32Value = crc.GetThisCRC(pby[i]);
	dwCRC32Value = ~dwCRC32Value;
	lutCopy.dwCRC32 = dwCRC32Value;

	crc.InitCRC32();
	dwCRC32Value = 0;
	memcpy(pby.get(), &lutCopy, cbLut);
	for (size_t i = 0; i < cbLut; ++i)
		dwCRC32Value = crc.GetThisCRC(pby[i]);
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
	fwrite(&lutCopy, 1, sizeof(lutCopy), fp);
	fclose(fp);

	*params.pLut = lutCopy;
	return TRUE;
}
