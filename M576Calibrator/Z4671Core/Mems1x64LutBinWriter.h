#pragma once

#include "LutBinWriter.h"
#include "M576OneX64Coef.h"

struct SMems1x64BinWriteParams
{
	CString strOutputPath;
	/// Pre-fill pSw4[0..3] with four stM576OneX64MemsSwCoef; matches trans 1x64 8K payload (4x2K).
	stM576OneX64MemsSwCoef* pSw4;
	CString strBundleSN;
	CString strBundlePN;
	CString strBundleTime;
	DWORD dwImageBaseAddress;
	SMems1x64BinWriteParams()
		: pSw4(NULL)
		, dwImageBaseAddress(0x0E000u)
	{
	}
};

class CMems1x64LutBinWriter
{
public:
	static size_t LutPayloadOffset() { return CLutBinWriter::LutPayloadOffset(); }
	static size_t DevicePayloadSize() { return (size_t)M576_1X64_MEMS_FILE_PAYLOAD_BYTES; }
	static size_t FullBundleFileSize() { return LutPayloadOffset() + DevicePayloadSize(); }
	/// On success, params.pSw4[0..3] are updated with per-block tail CRCs (and full-image CRC in bundle).
	static BOOL Write(SMems1x64BinWriteParams& params);
	static BOOL ReadMemsFromFile(LPCTSTR szPath, stM576OneX64MemsSwCoef* pOut4);
};
