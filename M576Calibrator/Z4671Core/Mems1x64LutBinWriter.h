#pragma once

#include "M576OneX64Coef.h"

/// Per-switch 2208 B bin (BUNDLEHEADER[160] + body[2048], stMemsSwCoef layout, 126S): no outer Z4671/MCS bundle wrapper.
class CMems1x64LutBinWriter
{
public:
	static constexpr size_t SingleSwitchFileBytes() { return sizeof(stM576OneX64MemsSwCoef); }
	static constexpr size_t FourSwitchPayloadBytes() { return (size_t)M576_1X64_MEMS_FILE_PAYLOAD_BYTES; }

	/// Fill BUNDLEHEADER[160] + bReserved7 identity per 126S `CreateSwitchPointBin`; CRC#1 [160..2203]->dwCRC32 LE,
	/// CRC#2 [160..2207]->hdr[148..151] BE32, CRC#3 hdr[20..127]->hdr[16..19] BE32; write sizeof(stM576OneX64MemsSwCoef) (2208 B).
	/// swIndex: 0..3 -> dwBaseAddress ADDR_SWITCH1_COEF + swIndex*0x800.
	static BOOL WriteSingleSwitch(
		const stM576OneX64MemsSwCoef& sw,
		int swIndex,
		LPCTSTR outPath,
		const CString& bundleSnVer,
		const CString& bundleTime);

	/// Read one switch file (raw file size must be >= sizeof(stM576OneX64MemsSwCoef); reads full struct).
	static BOOL ReadMemsFromFile(LPCTSTR szPath, stM576OneX64MemsSwCoef* pOutOne);
};
