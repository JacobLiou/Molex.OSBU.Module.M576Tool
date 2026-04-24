#pragma once
#include "stdafx.h"
#include <string>
#include <map>
#include <iostream>
#include <vector>
#include <memory>


//#define DLL_DEBUG

using namespace std;
#define COMMOM_LEN  256
#define DESC_LEN    256

class CCommAttr
{
public:
	char m_Name[COMMOM_LEN];
	char m_Value[COMMOM_LEN];
	char m_Desc[DESC_LEN];

	CCommAttr()
	{
		memset(m_Name, 0, COMMOM_LEN);
		memset(m_Value, 0, COMMOM_LEN);
		memset(m_Desc, 0, DESC_LEN);
	}

	~CCommAttr()
	{

	}

	CCommAttr& operator=(const CCommAttr& attr)
	{
		if (this != &attr)
		{
			memcpy_s(this->m_Name, COMMOM_LEN, attr.m_Name, COMMOM_LEN);
			memcpy_s(this->m_Value, COMMOM_LEN, attr.m_Value, COMMOM_LEN);
			memcpy_s(this->m_Desc, DESC_LEN, attr.m_Desc, DESC_LEN);
		}
		return *this;
	}
};

class CSettingAttr
{
public:
	char m_Name[COMMOM_LEN];
	char m_Value[COMMOM_LEN];
	char m_Desc[DESC_LEN];
	char m_Units[COMMOM_LEN];
	char m_Scale[COMMOM_LEN];
	CSettingAttr()
	{
		memset(m_Name, 0, COMMOM_LEN);
		memset(m_Value, 0, COMMOM_LEN);
		memset(m_Desc, 0, DESC_LEN);
		memset(m_Units, 0, COMMOM_LEN);
		memset(m_Scale, 0, COMMOM_LEN);
	}
	~CSettingAttr()
	{

	}

	CSettingAttr& operator=(const CSettingAttr& attr)
	{
		if (this != &attr)
		{
			memcpy_s(this->m_Name, COMMOM_LEN, attr.m_Name, COMMOM_LEN);
			memcpy_s(this->m_Value, COMMOM_LEN, attr.m_Value, COMMOM_LEN);
			memcpy_s(this->m_Desc, DESC_LEN, attr.m_Desc, DESC_LEN);
			memcpy_s(this->m_Units, COMMOM_LEN, attr.m_Units, COMMOM_LEN);
			memcpy_s(this->m_Scale, COMMOM_LEN, attr.m_Scale, COMMOM_LEN);
		}
		return *this;
	}
};

class CPortAttr
{
public:
	char m_Name[COMMOM_LEN];
	char m_Value[COMMOM_LEN];
	char m_Desc[DESC_LEN];
	char m_Active[COMMOM_LEN];
	char m_PassFail[COMMOM_LEN];

	CPortAttr()
	{
		memset(m_Name, 0, COMMOM_LEN);
		memset(m_Value, 0, COMMOM_LEN);
		memset(m_Desc, 0, DESC_LEN);
		memset(m_Active, 0, COMMOM_LEN);
		memset(m_PassFail, 0, COMMOM_LEN);
	}
	~CPortAttr()
	{

	}

	CPortAttr& operator=(const CPortAttr& attr)
	{
		if (this != &attr)
		{
			memcpy_s(this->m_Name, COMMOM_LEN, attr.m_Name, COMMOM_LEN);
			memcpy_s(this->m_Value, COMMOM_LEN, attr.m_Value, COMMOM_LEN);
			memcpy_s(this->m_Desc, DESC_LEN, attr.m_Desc, DESC_LEN);
			memcpy_s(this->m_Active, COMMOM_LEN, attr.m_Active, COMMOM_LEN);
			memcpy_s(this->m_PassFail, COMMOM_LEN, attr.m_PassFail, COMMOM_LEN);
		}
		return *this;
	}
};

class CObjectAttr
{
public:
	char m_Name[COMMOM_LEN];
	char m_Desc[DESC_LEN];
	char m_Instance[COMMOM_LEN];
	char m_Active[COMMOM_LEN];
	char m_PassFail[COMMOM_LEN];
	CObjectAttr()
	{
		memset(m_Name, 0, COMMOM_LEN);
		memset(m_Instance, 0, COMMOM_LEN);
		memset(m_Desc, 0, DESC_LEN);
		memset(m_Active, 0, COMMOM_LEN);
		memset(m_PassFail, 0, COMMOM_LEN);
	}
	~CObjectAttr()
	{

	}

	CObjectAttr& operator=(const CObjectAttr& attr)
	{
		if (this != &attr)
		{
			memcpy_s(this->m_Name, COMMOM_LEN, attr.m_Name, COMMOM_LEN);
			memcpy_s(this->m_Instance, COMMOM_LEN, attr.m_Instance, COMMOM_LEN);
			memcpy_s(this->m_Desc, DESC_LEN, attr.m_Desc, DESC_LEN);
			memcpy_s(this->m_Active, COMMOM_LEN, attr.m_Active, COMMOM_LEN);
			memcpy_s(this->m_PassFail, COMMOM_LEN, attr.m_PassFail, COMMOM_LEN);
		}
		return *this;
	}
};

class CTenvAttr
{
public:
	char m_Value[COMMOM_LEN];
	char m_Desc[DESC_LEN];
	char m_Active[COMMOM_LEN];
	char m_PassFail[COMMOM_LEN];
	CTenvAttr()
	{
		memset(m_Value, 0, COMMOM_LEN);
		memset(m_Desc, 0, DESC_LEN);
		memset(m_Active, 0, COMMOM_LEN);
		memset(m_PassFail, 0, COMMOM_LEN);
	}
	~CTenvAttr()
	{

	}

	CTenvAttr& operator=(const CTenvAttr& attr)
	{
		if (this != &attr)
		{
			memcpy_s(this->m_Value, COMMOM_LEN, attr.m_Value, COMMOM_LEN);
			memcpy_s(this->m_Desc, DESC_LEN, attr.m_Desc, DESC_LEN);
			memcpy_s(this->m_Active, COMMOM_LEN, attr.m_Active, COMMOM_LEN);
			memcpy_s(this->m_PassFail, COMMOM_LEN, attr.m_PassFail, COMMOM_LEN);
		}
		return *this;
	}
};

class CCondAttr
{
public:
	char m_Value[COMMOM_LEN];
	char m_Desc[DESC_LEN];
	char m_PassFail[COMMOM_LEN];
	CCondAttr()
	{
		memset(m_Value, 0, COMMOM_LEN);
		memset(m_Desc, 0, DESC_LEN);
		memset(m_PassFail, 0, COMMOM_LEN);
	}
	~CCondAttr()
	{

	}

	CCondAttr& operator=(const CCondAttr& attr)
	{
		if (this != &attr)
		{
			memcpy_s(this->m_Value, COMMOM_LEN, attr.m_Value, COMMOM_LEN);
			memcpy_s(this->m_Desc, DESC_LEN, attr.m_Desc, DESC_LEN);
			memcpy_s(this->m_PassFail, COMMOM_LEN, attr.m_PassFail, COMMOM_LEN);
		}
		return *this;
	}
};


class CParamAttr
{
public:
	char m_Name[COMMOM_LEN];
	char m_Value[COMMOM_LEN];
	char m_Desc[DESC_LEN];
	char m_Active[COMMOM_LEN];
	char m_PassFail[COMMOM_LEN];
	char m_Min[COMMOM_LEN];
	char m_Max[COMMOM_LEN];
	char m_FreeMin[COMMOM_LEN];
	char m_FreeMax[COMMOM_LEN];
	char m_Units[COMMOM_LEN];
	char m_FileName[COMMOM_LEN];
	char m_TestDate[COMMOM_LEN];
	char m_CondKey[COMMOM_LEN];
	char m_PortKey[COMMOM_LEN];
	char m_ObjectKey[COMMOM_LEN];
	char m_TenvKey[COMMOM_LEN];
	char m_Scale[COMMOM_LEN];
	CParamAttr()
	{
		memset(m_Name, 0, COMMOM_LEN);
		memset(m_Value, 0, COMMOM_LEN);
		memset(m_Desc, 0, DESC_LEN);
		memset(m_Active, 0, COMMOM_LEN);
		memset(m_PassFail, 0, COMMOM_LEN);
		memset(m_Min, 0, COMMOM_LEN);
		memset(m_Max, 0, COMMOM_LEN);
		memset(m_FreeMin, 0, COMMOM_LEN);
		memset(m_FreeMax, 0, COMMOM_LEN);
		memset(m_Units, 0, COMMOM_LEN);
		memset(m_FileName, 0, COMMOM_LEN);
		memset(m_TestDate, 0, COMMOM_LEN);
		memset(m_CondKey, 0, COMMOM_LEN);
		memset(m_PortKey, 0, COMMOM_LEN);
		memset(m_ObjectKey, 0, COMMOM_LEN);
		memset(m_TenvKey, 0, COMMOM_LEN);
		memset(m_Scale, 0, COMMOM_LEN);
	}
	~CParamAttr()
	{

	}


	CParamAttr& operator=(const CParamAttr& attr)
	{
		if (this != &attr)
		{
			memcpy_s(this->m_Name, sizeof(CParamAttr), &attr, sizeof(CParamAttr));
		}
		return *this;
	}
};


//map如果想第三个参数返回true需要经过两次比较，如果第一次返回true那么会把左右参数对调再判断一次，
//这一次则要返回false，才能无序排列,但是Find无法使用
template<class T>
struct DisableCompare : public std::binary_function<T, T, bool>
{
	bool operator()(T lhs, T rhs)const
	{
		static bool disblecompare = false;
		if (lhs == rhs)
		{
			return false;
		}
		if (disblecompare)
		{
			disblecompare = false;
			return false;
		}
		else
		{
			disblecompare = true;
			return true;
		}
	}
};


#ifndef DLL_DEBUG

class CCondSetting
{
public:
	vector<shared_ptr<CSettingAttr>> m_CondParams;
	CCondAttr m_CondAttr;
};


class CCfgRecord
{
public:
	//SECTION NAME = "PROD_CFG" 将PROD_CFG作为key
	vector<shared_ptr<CSettingAttr>> m_SectionSettings;
	CCommAttr m_Section;
};

class CTenvSetting
{
public:
	CTenvAttr m_TenID;
	vector<shared_ptr<CSettingAttr>> m_TenvSettings;
};

class CMiscParam
{
public:
	CCommAttr m_MiscObject;
	vector<shared_ptr<pair<string, shared_ptr<CCommAttr>>>> m_ObjectParams;

};


class CCondRecord
{
public:
	vector<shared_ptr<pair<string, shared_ptr<CTenvSetting>>>> m_Tenvs;
	vector<shared_ptr<pair<string, shared_ptr<CObjectAttr>>>> m_Objects;
	vector<shared_ptr<pair<string, shared_ptr<CPortAttr>>>> m_Ports;
	vector<shared_ptr<pair<string, shared_ptr<CCondSetting>>>> m_Conds;
	
};
class CCondParam
{
public:
	CCondAttr m_CondAttr;
	vector<shared_ptr<pair<string, shared_ptr<CParamAttr>>>> m_Params;
};

class CPortParam
{
public:
	CPortAttr m_PortAttr;
	vector<shared_ptr<pair<string, shared_ptr<CCondParam>>>> m_CondParam;
};

class CObjectParam
{
public:
	CObjectAttr m_ObjectAttr;
	vector<shared_ptr<pair<string, shared_ptr<CPortParam>>>> m_PortParam;
};

class CTenvParam
{
public:
	CTenvAttr m_Tenv;
	vector<shared_ptr<pair<string, shared_ptr<CObjectParam>>>> m_ObjectParam;
};


class CMeasRecord
{
public:
	//key是下一节点Name的值
	vector<shared_ptr<CCommAttr>> m_DocrevRecord;
	vector<shared_ptr<pair<string, shared_ptr<CCommAttr>>>> m_MfgRecord;
	vector<shared_ptr<pair<string, shared_ptr<CCfgRecord>>>> m_CfgRecords;
	//vector<CCfgRecord> m_CfgRecords;
	//CCfgRecord m_CfgRecord;
	vector<shared_ptr<pair<string, shared_ptr<CTenvParam>>>> m_TenvParams;
	CCondRecord m_CondRecord;
	//vector<CCommAttr> m_TestRecord;
	//rjf test
	vector <shared_ptr<pair<string, shared_ptr<CCommAttr>>>> m_RecipeRecord;
	//map<string, shared_ptr<CCommAttr>, DisableCompare<string>> m_RecipeRecord;
	//map<string, shared_ptr<CMiscParam>, DisableCompare<string>> m_MiscRecord;
	vector<shared_ptr<pair<string, shared_ptr<CMiscParam>>>> m_MiscRecord;
	char m_CRC32[COMMOM_LEN];
	CMeasRecord::CMeasRecord()
	{
		memset(m_CRC32, 0, COMMOM_LEN);
	}

	CMeasRecord::~CMeasRecord()
	{
	}
	
};
#endif // !DLL_DEBUG
