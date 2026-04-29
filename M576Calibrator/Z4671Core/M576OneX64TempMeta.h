#pragma once
// 1x64 stCalibDAC[0/1] sTemperature (shared by Z4671Core and CalibConstants.h includers on app include path).
/// 1x64 `stM576OneX64ChnDAC.sTemperature`: **0.01C** integer (126S legacy, e.g. FE3E = -450 => -4.50C). Not MCS `wTemperaturePoint` (0.1C).
// 1x64 温区元数据：与 MCS LUT 的 0.1C 标度不同；低温 0C、常温 30C 写 standard bin 时统一为 0 / 3000。

#ifndef M576_1X64_STEMP_CENTI_LOW
#define M576_1X64_STEMP_CENTI_LOW 0
#endif
#ifndef M576_1X64_STEMP_CENTI_ROOM
#define M576_1X64_STEMP_CENTI_ROOM 3000
#endif
