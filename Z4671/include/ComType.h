// ComType.h: interface for the CComType class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_COMTYPE_H__88AF70E9_3BD2_4975_BF2C_08E2B7483985__INCLUDED_)
#define AFX_COMTYPE_H__88AF70E9_3BD2_4975_BF2C_08E2B7483985__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "..\\Inc\\OplkGPIB.h"
#include "..\\Inc\\OpComm.h"
class CComType  
{
public:
	CComType();
	virtual ~CComType();
	COplkGPIBDevice* getGPIBHandle();
	COpComm * getComHandle();
protected:
	COplkGPIBDevice * m_gpib;
	COpComm  *m_Com;
};

#endif // !defined(AFX_COMTYPE_H__88AF70E9_3BD2_4975_BF2C_08E2B7483985__INCLUDED_)
