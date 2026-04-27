#pragma once
// 1x64 MEMS layout matches 126S_45V_Switch_App (stMemsSwCoef, per-2K flash) for 1X64 trans + write-bin.
// Four switches = four separate 2K files; device backup is 8KB contiguous MEM (4×2K). XMODEM burns one 2K per `fwdl` session.

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

// Cal/backup: four 2K stM576OneX64MemsSwCoef per 1x64 (see M576_1X64_MEMS_FILE_PAYLOAD_BYTES)
#ifndef M576_1X64_MEMS_FILE_PAYLOAD_BYTES
#define M576_1X64_MEMS_FILE_PAYLOAD_BYTES (4u * 2048u)
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
	// bReserved6[76]: padding for 2K block; #pragma pack(1) per 126S; st totals 2048B (sizeof / static_assert).
	unsigned char bReserved6[76];
	stM576OneX64ChnDAC stCalibDAC[M576_1X64_TEMPT_SWITCH_NUM];
	unsigned char bReserved7[28];
	unsigned int dwCRC32; // little-endian 4B in file
} stM576OneX64MemsSwCoef;

#pragma pack(pop)

#if defined(_MSC_VER)
static_assert(sizeof(stM576OneX64MemsSwCoef) == 2048u, "stMemsSwCoef must be 2048 bytes (126S).");
static_assert(M576_1X64_MEMS_FILE_PAYLOAD_BYTES == 8192u, "4*2048=8192");
#endif
