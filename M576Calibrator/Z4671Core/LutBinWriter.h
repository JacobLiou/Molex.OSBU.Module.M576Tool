#pragma once

#include "Z4767StructDefine.h"
#include "ByteSwap.h"

/// Parameters for Z4671 MCS LUT bundle binary (matches CreateBinFileZ4671 layout).
struct SLutBinWriteParams
{
	CString strOutputPath;
	stLutSettingZ4671* pLut;
	CString strBundleSN;
	CString strBundlePN;
	CString strBundleTime;
	DWORD dwImageBaseAddress;
	SLutBinWriteParams()
		: pLut(NULL)
		, dwImageBaseAddress(0x65000)
	{
	}
};

class CLutBinWriter
{
public:
	/// Fills dwCRC32 in *pLut and writes bundle (header1, header2, image hdr x2, lut).
	static BOOL Write(const SLutBinWriteParams& params);

	/// Offset of stLutSettingZ4671 payload in file.
	static size_t LutPayloadOffset();

	/// Read existing BIN and load LUT body (headers skipped).
	static BOOL ReadLutFromFile(LPCTSTR szPath, stLutSettingZ4671& lut);

	/// Full on-disk bundle size (headers + LUT), same as Write() produces.
	static size_t FullBundleFileSize();

	/// Bytes of `stLutSettingZ4671` only: size the firmware allows over 0xC4 (not `FullBundleFileSize()`).
	static size_t LutDevicePayloadSize();
};
