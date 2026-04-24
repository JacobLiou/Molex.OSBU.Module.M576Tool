#ifndef PM_CONTROL
#define PM_CONTROL __declspec(dllimport)

#define HP_8163A		0
#define OPLK_PM			1
#define PM_I830C		2
#define PM_N7745		3
#define PM_JH		    4

class PM_CONTROL CPMDll
{
public:
	CPMDll();
	virtual~CPMDll();

	DWORD GetUnit(long iChan, long* piUnit);
	DWORD GetAverageTime(long iChan, double* pdblAverageTime);
	DWORD GetWavelength(long iChan, double* pdblWL);
	DWORD OpenDevice(stDevType stType, LPCTSTR pszDeviceAddr,unsigned long *ComHandle);
	DWORD ReadPower(long iChan, double* pdbPowerValue);
	DWORD SetUnit(long iChan, long iPWUnit);
	DWORD SetWavelength(long iChan, double dblWL);
	DWORD SetAverageTime(long iChan, double dblPWAverageTime);
	DWORD SetRangeMode(long iChan, long iPWRangeMode);
	DWORD SetParameters(long iChan, BOOL bAutoRang, double dblWL, double dblAverageTime, double dblPwrRange);
	DWORD CloseDevice();
	DWORD GetRangeMode(long iChan, long* iPWMRangeMode);
	DWORD SetZero(long iChan);
	DWORD GetDeviceInfo(CHAR *pchInfo);
	DWORD SetHandle(stDevType stType, LPCTSTR pszDeviceAddr, unsigned long handle);

private:	
	void *m_pPm;
	void **m_pvoid;
};

#endif