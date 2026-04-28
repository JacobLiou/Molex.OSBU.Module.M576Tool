#pragma once
// 1x64 MEMS layout matches 126S_45V_Switch_App `tagMemsSwCoef`: file = BUNDLEHEADER[160] + body[2048] = 2208 B (dwBundleSize 0x8A0).
// Flash body only is 2048 B/switch (stride 0x800); four switches = 8192 B MEM read; host synth header then writes four bin files. XMODEM burns one file per `fwdl`.

#include "Z4767StructDefine.h"

// --- Keep in sync with 1X64 Switch / 126S firmware ---
#ifndef M576_1X64_MAX_CHANNEL_NUM
#define M576_1X64_MAX_CHANNEL_NUM 36u
#endif
#ifndef M576_1X64_MAX_MIDPTR_NUM
#define M576_1X64_MAX_MIDPTR_NUM 25u
#endif
#ifndef M576_1X64_TEMPT_SWITCH_NUM
#define M576_1X64_TEMPT_SWITCH_NUM 4u
#endif

// Cal/backup: four files × 2208 B each (4 × (160 + 2048)).
#ifndef M576_1X64_MEMS_FILE_PAYLOAD_BYTES
#define M576_1X64_MEMS_FILE_PAYLOAD_BYTES (4u * 2208u)
#endif

#include <cstddef>
#if defined(_MSC_VER)
#include <cstdint>
#endif

#pragma pack(push, 1)

typedef struct tagM576OneX64AxisDAC
{
	short sDACx;
	short sDACy;
} stM576OneX64AxisDAC;

typedef struct tagM576OneX64ChnDAC
{
	unsigned short wValid;
	short sTemperature;
	stM576OneX64AxisDAC stChnDAC[M576_1X64_MAX_CHANNEL_NUM];
	stM576OneX64AxisDAC stMidDAC[M576_1X64_MAX_MIDPTR_NUM];
} stM576OneX64ChnDAC;

typedef struct tagM576OneX64MidPointsMatrix
{
	unsigned char bMidPointNum;
	unsigned char bReserved4;
	unsigned char c_bMidChange[M576_1X64_MAX_MIDPTR_NUM][M576_1X64_MAX_MIDPTR_NUM];
	unsigned char bReserved5;
} stM576OneX64MidPointsMatrix;

typedef struct tagM576OneX64MemsSwCoef
{
	unsigned char BUNDLEHEADER[160];
	unsigned char bSWTypeChanNum;
	unsigned char bReserved0;
	unsigned char bDarkPoint;
	unsigned char bReserved1;
	unsigned short wSwitchDelayUs;
	unsigned char bReserved2[2];
	unsigned char bMidPoint[M576_1X64_MAX_CHANNEL_NUM];
	unsigned short wSwitchIL[M576_1X64_MAX_CHANNEL_NUM];
	unsigned char bReserved3[44];
	stM576OneX64MidPointsMatrix pstMidPointsMatrix;
	// bReserved6[236]: padding to match 126S `tagMemsSwCoef` body size 2048 B after BUNDLEHEADER.
	unsigned char bReserved6[236];
	stM576OneX64ChnDAC stCalibDAC[M576_1X64_TEMPT_SWITCH_NUM];
	unsigned char bReserved7[28];
	unsigned int dwCRC32; // little-endian 4B in file
} stM576OneX64MemsSwCoef;

#pragma pack(pop)

#if defined(_MSC_VER)
static_assert(sizeof(stM576OneX64MemsSwCoef) == 2208u, "BUNDLEHEADER[160]+body[2048]=2208 (126S tagMemsSwCoef file).");
static_assert(M576_1X64_MEMS_FILE_PAYLOAD_BYTES == 8832u, "4*2208=8832");
#endif
