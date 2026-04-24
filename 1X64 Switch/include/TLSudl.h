
// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the TLSCONTROLDLL_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// TLSCONTROLDLL_API functions as being imported from a DLL, wheras this DLL sees symbols
// defined with this macro as being exported.
#ifndef TLS_UDL
#define TLS_UDL __declspec(dllimport)

#include "UDLReturnCode.h"


#define HP_816X 0
#define OPLK_TLS 1
#define SANTEC_TLS 2

// This class is exported from the TLSUDL.dll
class TLS_UDL CTLSDll 
{
public:
	CTLSDll();
	virtual~CTLSDll();
	// TODO: add your methods here.
	DWORD OpenDevice(stDevType stType, LPCTSTR pszDeviceAddr,unsigned long *ComHandle);
	DWORD SetWavelength(long lChIndex, double dblWL);
	DWORD SetSourcePower(long lChIndex, double dblPW);
	DWORD SetCoherenceControl(BOOL bEnable);
	DWORD SetSourceUnit(long lChIndex, long lUnit);
	DWORD SetParameters(long lChIndex, double dblPower, double dblWavelength);
	DWORD SetHandle(stDevType stType, LPCTSTR pszDeviceAddr, unsigned long handle);
	DWORD SetOutputEnable(BOOL bEnable);
	DWORD CloseDevice();

private:
	void *m_pTLS;
	void **m_pvoid;
};

#endif

