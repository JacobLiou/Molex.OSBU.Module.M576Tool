// OpVOA.h: interface for the COpVOA class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OPVOA_H__7FCF455A_5827_4C72_AD49_D509BC551D14__INCLUDED_)
#define AFX_OPVOA_H__7FCF455A_5827_4C72_AD49_D509BC551D14__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "headerDef.h"
#include "Hp8156A.h"
#include "Command.h"



class DLLEXPORT COpVOA  
{
public:
	COpVOA();
	virtual ~COpVOA();
	//设置OPlink VOA类型
	void SetOpVOAType(opVoaType voatype);
	//设置衰减器波长
	bool SetAttenWL(double dbWLAtten);
	//设置衰减器输出状态
	bool SetOutputState(BOOL bOutputEnable);
	//获得衰减器值
	bool GetAttenuation(double &dblAtten);
	//设置衰减器值
	bool SetAttenuation(double dblAtten);
	//打开设备
	bool OpenDevice();
	//初始化VOA结构体
	void SetVOAInitial(pstDeviceSetInfo pVOAInfo);
private:
	opVoaType  m_voatype;
	pstDeviceSetInfo m_pDevInfo;
	COplkGPIBDevice  m_VOAGPIB;
	COpComm  m_VOASerial;
	CHp8156A * m_Hp8156A;
	CCommandAtten *m_opVoa;
};

#endif // !defined(AFX_OPVOA_H__7FCF455A_5827_4C72_AD49_D509BC551D14__INCLUDED_)
