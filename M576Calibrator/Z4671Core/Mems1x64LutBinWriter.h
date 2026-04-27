#pragma once

#include "M576OneX64Coef.h"

/// Per-switch 2K bin (stMemsSwCoef layout, 126S legacy): no outer Z4671/MCS bundle wrapper.
class CMems1x64LutBinWriter
{
public:
	static constexpr size_t SingleSwitchFileBytes() { return sizeof(stM576OneX64MemsSwCoef); }
	static constexpr size_t FourSwitchPayloadBytes() { return (size_t)M576_1X64_MEMS_FILE_PAYLOAD_BYTES; }

	/// Fill BUNDLEHEADER[160] per 126S `CreateSwitchPointBin`, recompute tail CRC, write exactly 2048 B.
	/// swIndex: 0..3 -> dwBaseAddress ADDR_SWITCH1_COEF + swIndex*0x800.
	static BOOL WriteSingleSwitch(
		const stM576OneX64MemsSwCoef& sw,
		int swIndex,
		LPCTSTR outPath,
		const CString& bundleSnVer,
		const CString& bundleTime);

	/// Read one 2K block (raw file size must be >= 2048; reads first 2048 bytes only).
	static BOOL ReadMemsFromFile(LPCTSTR szPath, stM576OneX64MemsSwCoef* pOutOne);
};
