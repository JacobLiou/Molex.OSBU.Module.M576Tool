// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the OPLKGPIB_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// OPLKGPIB_API functions as being imported from a DLL, wheras this DLL sees symbols
// defined with this macro as being exported.

//#pragma comment(lib, "..//library//VOAControl.lib")

#ifndef VOA_DLL
#define VOA_DLL __declspec(dllimport)

#define HP_8156A 0
#define OPLK_VOA 1

class VOA_DLL CVOADll
{
public:
	CVOADll();
	virtual ~CVOADll();
	// TODO: add your methods here.
	// stType.bType: 0 for HP_8156A; 1 for OPLK_VOA
	// *pszDeviceAddr: "GPIB,0,28,0" for GBIP; "COM,1,9600,8,0,1" for COM
	DWORD OpenDevice(stDevType stType, char* pszDeviceAddr, unsigned long *comHandle);
	DWORD CloseDevice();
	DWORD ResetDevice(void);
	DWORD GetDeviceInfo(stDevInfo *stInfo);
	DWORD SetWavelength(long lChIndex, double dblWL);
	DWORD GetWavelength(long lChIndex, double* pdblWL);
    DWORD SetAttenuation(long lChIndex, double dblAtten);
	DWORD GetAttenuation(long lChIndex, double* pdblAtten);
	DWORD SetEnableOutput(long lChIndex, BOOL bEnable);
	DWORD SetInputOffset(long lChIndex, double dblOffset);
	DWORD SetHandle(stDevType stType, HANDLE hHandle);
	DWORD GetStatus(DWORD *dwStatus);
/*	BOOL GetErrorMessage(DWORD dwErrorCode, char *pszErrorMsg);*/
private:
	void *m_pVOA;
	void **m_pvoid;
};
#endif