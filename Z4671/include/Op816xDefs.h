#ifndef	_816X_DEFINITION_H_INCLUDED_
#define _816X_DEFINITION_H_INCLUDED_
 
typedef struct tagOp816XScanParam
{
	//	ScanParameter members are in nm units.
	//	Oplk816x will transfer them into m (by 1e-9)

	//	Use size to verify if parameter structure is of correct version
	DWORD	m_dwSize;
	//	Basic parameters
	double	m_dblStartWL, m_dblStopWL, m_dblStepSize;
	//	Advanced parameters
	double	m_dblTLSPower, m_dblPWMPower;
	DWORD	m_dwNumberOfScan;
	DWORD	m_dwChannelNumber;
	DWORD	m_dwChannelCfgHigh, m_dwChannelCfgLow;
	//	To be retrieved from VXIPnP driver
	DWORD	m_dwSampleCount;
} stOp816XScanParam, *POp816XScanParam;

typedef struct tagOp816XRawData
{
	double		*m_pdblWavelengthArray;
	PDWORD		m_pdwDataArrayAddr;
} stOp816XRawData, *POp816XRawData;

#endif