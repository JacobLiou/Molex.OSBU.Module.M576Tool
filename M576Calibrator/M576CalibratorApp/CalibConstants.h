#pragma once

/// Same as legacy Z4671 UI: low-temperature LUT slot index.
#ifndef IDX_TEMP_LOW
#define IDX_TEMP_LOW 0
#endif

/// RECAL 0: TLS source channel 1~8 (2x8 switch channel per PRD).
#ifndef M576_DEFAULT_TLS_SOURCE
#define M576_DEFAULT_TLS_SOURCE 1
#endif
/// RECAL 0: wavelength (nm), PRD 850~1650; 1310 calibrator default.
#ifndef M576_DEFAULT_WAVELENGTH_NM
#define M576_DEFAULT_WAVELENGTH_NM 1310.0
#endif
