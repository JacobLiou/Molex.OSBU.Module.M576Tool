#pragma once
#include "stdafx.h"
#include "TemplateComm.h"
#include <memory>

#import "msxml4.tlb" no_mamespace
//#import "msxml4.dll"

#ifdef DYNAMICLINKLIBRARY_EXPORTS
#define DLLEXPORT_API __declspec(dllexport)
#else
#define DLLEXPORT_API __declspec(dllimport)
#endif

#define TTR_TESTMODE_MFG     "0"		//MFG模式
#define TTR_TESTMODE_DEBUG   "1"		//Debug模式
#define TTR_TESTMODE_RD      "2"		//RD模式

#define TTR_TESTTYPE_TESTDATA  "0"    //测试数据
#define TTR_TESTTYPE_EDTCURVE  "10"   //EDT需要原始曲线数据的测试数据

//

/****************************************************************************************************************
*错误码定义
*0--无错误
*1--申请数组长度不够
*2--未找到符合条件的数据
*3--解析模板出错
*4--保持数据出错
*5--无法创建DOMDocument对象，请检查是否安装了MS XML Parser 运行库!
*6--传入参数为空指针
*7--保存文件出错
*8--文件路径出错
*9--模板CRC校验出错
*10--未知错误
*11--保存数据失败：SN/工序/WO未保存
*12--保存数据失败：Passfail属性非法，只能为"P/F"
*13--Active值非法：只能为0/1
*14--MfgRecord里的Operator/Test_Station/Test_Type未保存
*15--MiscRecord里的软件信息未保存全
****************************************************************************************************************/

class DLLEXPORT_API  CATMSRecord
{
public:
	CATMSRecord();
	~CATMSRecord();
	//加载测试模板
	int LoadTemplate(char* templatePath);
	//保存测试数据
	int SaveTestData(char* path);

	/****************************************************************************************************************
	*函数描述：根据错误代码获取错误信息
	*[IN]errCode:	 错误代码
	*[IN]errMsgCount:存储错误代码数组长度
	*[OUT]errMsg：	 错误代码对应的描述
	****************************************************************************************************************/
	void GetErrMsg(int errCode, char *errMsg, int errMsgCount);
	/****************************************************************************************************************
	*函数描述：根据 TENV_ID.VALUE/OBJECT.NAME/PORT.VALUE/COND_ID.VALUE/PARA.NAME组合获取符合条件的测试参数
	*[IN]tenvKey：		TENV_ID.VALUE，即模板中节点TENV_ID下属性VALUE的值
	*[IN]objectKey:	OBJECT.NAME，即模板中节点OBJECT下属性NAME的值
	*[IN]portKey：		PORT.VALUE，即模板中节点PORT下属性VALUE的值
	*[IN]condKey:		COND_ID.VALUE，即模板中节点COND_ID下属性VALUE的值
	*[IN]paramName:		PARA.NAME,即模板中测试参数节点PARA下属性NAME值
	*[OUT]paramValue:	符合条件的参数的值
	*返回值：			错误码，0--正确  其他 出错，错误码见上述定义
	****************************************************************************************************************/
	int GetParamValue(char* tenvKey, char* objectKey, char* portKey, char* condKey, char* paramName, char* paramValue);

	/****************************************************************************************************************
	*函数描述：根据 TENV_ID.VALUE/OBJECT.NAME/PORT.VALUE/COND_ID.VALUE/PARA.NAME组合获取符合条件的测试参数
	*[IN]tenvKey：		TENV_ID.VALUE，即模板中节点TENV_ID下属性VALUE的值
	*[IN]objectKey:	OBJECT.NAME，即模板中节点OBJECT下属性NAME的值
	*[IN]portKey：		PORT.VALUE，即模板中节点PORT下属性VALUE的值
	*[IN]condKey:		COND_ID.VALUE，即模板中节点COND_ID下属性VALUE的值
	*[IN]paramName:		PARA.NAME,即模板中测试参数节点PARA下属性NAME值
	*[OUT]testParam:	符合条件的参数
	*返回值：			错误码，0--正确  其他 出错，错误码见上述定义
	****************************************************************************************************************/
	int GetParams(char* tenvKey, char* objectKey, char* portKey, char* condKey, char* paramName, CParamAttr& testParam);
	/****************************************************************************************************************
	 *该注释适用于以下所有的GetAllParamsCount和GetParams函数
	 *函数描述：根据 TENV_ID.VALUE/OBJECT.NAME/PORT.VALUE/COND_ID.VALUE组合获取符合条件的所有测试参数，调用GetParams前必须
			   获取符合条件参数数量，先申请好内存空间
	 *[IN]tenvKey：		TENV_ID.VALUE，即模板中节点TENV_ID下属性VALUE的值
	 *[IN]objectKey:	OBJECT.NAME，即模板中节点OBJECT下属性NAME的值
	 *[IN]portKey：		PORT.VALUE，即模板中节点PORT下属性VALUE的值
	 *[IN]condKey:		COND_ID.VALUE，即模板中节点COND_ID下属性VALUE的值
	 *[IN]count:		数组的长度
	 *[OUT]allParams:	符合条件的所有参数
	 *返回值：1）GetAllParamsCount 符合条件的参数数量
			 2）GetParams 错误码，0--正确  其他 出错，错误码见上述定义
	 ****************************************************************************************************************/
	int GetAllParamsCount(char* tenvKey, char* objectKey, char* portKey, char* condKey);
	int GetParams(char* tenvKey, char* objectKey, char* portKey, char* condKey, CParamAttr *allParams, int count);

	int GetAllParamsCount(char* tenvKey, char* objectKey, char* portKey);
	int GetParams(char* tenvKey, char* objectKey, char* portKey, CParamAttr *allParams, int count);

	int GetAllParamsCountByPortName(char* tenvKey, char* objectKey, char* portName);
	int GetParamsByPortName(char* tenvKey, char* objectKey, char* portName, CParamAttr *allParams, int count);

	int GetAllParamsCount(char* tenvKey, char* objectKey);
	int GetParams(char* tenvKey, char* objectKey, CParamAttr *allParams, int count);

	int GetAllParamsCount(char* tenvKey);
	int GetParams(char* tenvKey, CParamAttr *allParams, int count);

	int GetAllParamsCount();
	int GetParams(CParamAttr *allParams, int count);


	//获取模板中COND_RECORD节点下所有Cond数量
	int GetAllCondsCount();
	/****************************************************************************************************************
	*函数描述：获取COND_RECORD节点下所有的COND_ID,即该模板所有条件ID，必须先调用GetAllCondsCount获取数量，，先申请好内存空间
	*[IN]count:		COND_RECORD节点下所有Cond数量
	*[OUT]allConds：	该模板所有条件ID，以及描述
	*返回值：		0--正确  其他 出错，错误码见上述定义
	****************************************************************************************************************/
	int GetAllConds(CCondAttr *allConds, int count);
	int GetCondSettingCounts(char* condKey);

	//TEST_RECORD节点下指定温度、object、port下的所有conditions的数量
	int GetTestRecCondsCount(char* tenvKey, char* objectKey, char* portKey);
	/****************************************************************************************************************
	*函数描述：获取TEST_RECORD节点下指定温度、object、port下的所有conditions
	*[IN]tenvKey：		TENV_ID.VALUE，即模板中节点TENV_ID下属性VALUE的值
	*[IN]objectKey:		OBJECT.NAME，即模板中节点OBJECT下属性NAME的值
	*[IN]portKey：		PORT.VALUE，即模板中节点PORT下属性VALUE的值
	*[IN]count:			指定温度、object、port下的所有conditions的数量
	*[OUT]conds：	指定温度、object、port下的所有conditions
	*返回值：		0--正确  其他 出错，错误码见上述定义
	****************************************************************************************************************/
	int GetTestRecConds(CCondAttr *conds, char* tenvKey, char* objectKey, char* portKey, int count);

	//TEST_RECORD节点下指定温度、object下的所有ports的数量
	int GetTestRecPortsCount(char* tenvKey, char* objectKey);
	/****************************************************************************************************************
	*函数描述：获取TEST_RECORD节点下指定温度、object下的所有ports
	*[IN]tenvKey：		TENV_ID.VALUE，即模板中节点TENV_ID下属性VALUE的值
	*[IN]objectKey:		OBJECT.NAME，即模板中节点OBJECT下属性NAME的值
	*[IN]count:			TEST_RECORD节点下指定温度、object下的所有ports的数量
	*[OUT]ports：	TEST_RECORD节点下指定温度、object下的所有ports
	*返回值：		0--正确  其他 出错，错误码见上述定义
	****************************************************************************************************************/
	int GetTestRecPort(CPortAttr *ports, char* tenvKey, char* objectKey, int count);

	//TEST_RECORD节点下指定温度下的所有object的数量
	int GetTestRecObjectsCount(char* tenvKey);
	/****************************************************************************************************************
	*函数描述：获取TEST_RECORD节点下指定温度下的所有object
	*[IN]tenvKey：		TENV_ID.VALUE，即模板中节点TENV_ID下属性VALUE的值
	*[IN]count:			TEST_RECORD节点下指定温度下的所有object的数量
	*[OUT]objects：	TEST_RECORD节点下指定温度下的所有object
	*返回值：		0--正确  其他 出错，错误码见上述定义
	****************************************************************************************************************/
	int GetTestRecObject(CObjectAttr *objects, char* tenvKey, int count);

	/****************************************************************************************************************
	*函数描述：获取给定COND_ID.VALUE节点下所有的COND的设置，必须先调用GetCondSettingCounts获取数量，先申请好内存空间
	*[IN]condKey:			COND_ID.VALUE 条件ID值
	*[IN]count:				给定COND_ID.VALUE节点下所有的COND的设置数量
	*[OUT]allCondSettings：	给定COND_ID.VALUE节点下所有的COND的设置
	*返回值：		0--正确  其他 出错，错误码见上述定义
	****************************************************************************************************************/
	int GetCondSettingByKey(char* condKey, CSettingAttr* allCondSettings, int count);

	//获取模板中COND_RECORD节点下所有Port数量
	int GetAllPortsCount();
	/****************************************************************************************************************
	*函数描述：获取COND_RECORD节点下所有的PORT,必须先调用GetAllPortsCount获取数量，先申请好内存空间
	*[IN]count:		COND_RECORD节点下所有PORT数量
	*[OUT]allPorts：	该模板所有PORT，以及描述
	*返回值：		0--正确  其他 出错，错误码见上述定义
	****************************************************************************************************************/
	int GetAllPorts(CPortAttr *allPorts, int count);
	/****************************************************************************************************************
	*函数描述：根据PORT.VALUE获取该PORT的具体信息
	*[IN]portKey:PORT.VALUE
	*返回值：	 该PORT的具体信息
	****************************************************************************************************************/
	CPortAttr GetPortByKey(char* portKey);

	//获取模板中COND_RECORD节点下所有Object数量
	int GetAllObjectsCount();
	/****************************************************************************************************************
	*函数描述：获取COND_RECORD节点下所有的Object,必须先调用GetAllObjectsCount获取数量，先申请好内存空间
	*[IN]count:		COND_RECORD节点下所有Object数量
	*[OUT]allObjects：	该模板所有Object，以及描述
	*返回值：			0--正确  其他 出错，错误码见上述定义
	****************************************************************************************************************/
	int GetAllObjects(CObjectAttr *allObjects, int count);
	/****************************************************************************************************************
	*函数描述：根据OBJECT.NAME获取该PORT的具体信息
	*[IN]objectKey:OBJECT.NAME
	*返回值：	   该OBJECT的具体信息
	****************************************************************************************************************/
	CObjectAttr GetObjectByKey(string objectKey);

	//获取模板中COND_RECORD节点下所有TENV_ID数量
	int GetAllTenvsCount();
	/****************************************************************************************************************
	*函数描述：获取COND_RECORD节点下所有的TENV_ID,必须先调用GetAllTenvsCount获取数量，先申请好内存空间
	*[IN]count:		  COND_RECORD节点下所有TENV_ID数量
	*[OUT]allObjects：该模板所有TENV_ID，以及描述
	*返回值：		  0--正确  其他 出错，错误码见上述定义
	****************************************************************************************************************/
	int GetAllTenvs(CTenvAttr *allTenvs, int count);
	//获取给定的TENV_ID.VALUE节点下所有TENV设置数量
	int GetTenvSettingCount(char* tenvKey);
	/****************************************************************************************************************
	*函数描述：获取给定的TENV_ID.VALUE节点下所有TENV设置,必须先调用GetGetTenvSettingCount获取数量，先申请好内存空间
	*[IN]tenvKey:		   给定的TENV_ID.VALUE
	*[IN]count:			   给定的TENV_ID.VALUE节点下所有TENV设置数量
	*[OUT]allTenvSettings：给定的TENV_ID.VALUE节点下所有TENV设置
	*返回值：		       0--正确  其他 出错，错误码见上述定义
	****************************************************************************************************************/
	int GetTenvSettingByKey(char* tenvKey, CSettingAttr *allTenvSettings, int count);

	//获取CFG_RECORD节点下所有的SECTION数量
	int GetAllCfgSectionCount();
	/****************************************************************************************************************
	*函数描述：获取CFG_RECORD节点下所有的SECTION,必须先调用GetAllCfgSectionCount获取数量，先申请好内存空间
	*[IN]count:		  CFG_RECORD节点下所有的SECTION数量
	*[OUT]allObjects：该模板CFG_RECORD节点下所有的SECTION
	*返回值：		  0--正确  其他 出错，错误码见上述定义
	****************************************************************************************************************/
	int GetAllCfgSection(CCommAttr *allCfgSection, int count);
	//给定的CFG_RECORD.SECTION.NAME节点下所有设置数量
	int GetSectionSettingsCount(char* section);
	/****************************************************************************************************************
	*函数描述：获取给定的CFG_RECORD.SECTION.NAME节点下所有设置,必须先调用GetSectionSettingsCount获取数量，先申请好内存空间
	*[IN]section:	  给定的CFG_RECORD.SECTION.NAME
	*[IN]count:		  给定的CFG_RECORD.SECTION.NAME节点下所有设置数量
	*[OUT]allSetting：给定的CFG_RECORD.SECTION.NAME节点下所有设置
	*返回值：		  0--正确  其他 出错，错误码见上述定义
	****************************************************************************************************************/
	int GetGetSectionSettings(char* section, CSettingAttr *allSetting, int count);

	//获取MFG_RECORD节点下所有的设置数量
	int GetAllMfgRecordsCount();
	//获取MFG_RECORD节点下所有的设置
	int GetAllMfgRecords(CCommAttr *allMfgRecords, int count);

	int GetAllDocRecordsCount();
	int GetAllDocRecords(CCommAttr *allDocRecords, int count);

	//获取PROD_RECORD节点下所有的数量
	int GetAllProdRecordsCount();
	//获取PROD_RECORD节点下所有的信息
	int GetAllProdRecords(CCommAttr *allProdRecords, int count);
	//根据PROD_RECORD下节点名称，例如PROD_CATEGORY、PROD_FAMILY来获取对应的信息
	int GetPordRecordByKey(char* key, CCommAttr& prodAttr);

	/****************************************************************************************************************
	*函数描述：根据 TENV_ID.VALUE/OBJECT.NAME/PORT.VALUE/COND_ID.VALUE/参数名称PARA.NAME 填入对应测试数据
	*[IN]tenvKey：	TENV_ID.VALUE，即模板中节点TENV_ID下属性VALUE的值
	*[IN]objectKey:	OBJECT.NAME，即模板中节点OBJECT下属性NAME的值
	*[IN]portKey：	PORT.VALUE，即模板中节点PORT下属性VALUE的值
	*[IN]condKey:	COND_ID.VALUE，即模板中节点COND_ID下属性VALUE的值
	*[IN]paramName:	PARA.NAME，即具体测试参数节点PARA下属性NAME的值
	*[IN]paramValue:测试参数结果
	*[IN]passfail:	测试参数是否合格，合格"P",不合格"F"
	*[IN]file:		存放测试该参数的Rawdata文件路径，无Rawdata数据时为空
	*[IN]date:		测试该参数的时间
	*[IN]active:	该参数是否做合格判断，0--不判断，1--判断，-1--默认为下载模板时的状态
	*返回值：		0--正确  其他 出错，错误码见上述定义
	****************************************************************************************************************/
	int SetParam(char* tenvKey, char* objectKey, char* portKey, char* condKey, char* paramName, char* paramValue, char* passfail, char* file, char* date,int active=-1);

	/****************************************************************************************************************
	*函数描述：根据 TENV_ID.VALUE/OBJECT.NAME/PORT.VALUE/COND_ID.VALUE 填入该Condition整体是否合格结果
	*[IN]tenvKey：	TENV_ID.VALUE，即模板中节点TENV_ID下属性VALUE的值
	*[IN]objectKey:	OBJECT.NAME，即模板中节点OBJECT下属性NAME的值
	*[IN]portKey：	PORT.VALUE，即模板中节点PORT下属性VALUE的值
	*[IN]condKey:	COND_ID.VALUE，即模板中节点COND_ID下属性VALUE的值
	*[IN]passfail:	测试参数是否合格，合格"P",不合格"F"
	*返回值：		0--正确  其他 出错，错误码见上述定义
	****************************************************************************************************************/
	int SetCondPassfail(char* tenvKey, char* objectKey, char* portKey, char* condKey, char* passfail);

	/****************************************************************************************************************
	*函数描述：根据 TENV_ID.VALUE/OBJECT.NAME/PORT.VALUE 填入该PORT整体是否合格结果
	*[IN]tenvKey：	TENV_ID.VALUE，即模板中节点TENV_ID下属性VALUE的值
	*[IN]objectKey:	OBJECT.NAME，即模板中节点OBJECT下属性NAME的值
	*[IN]portKey：	PORT.VALUE，即模板中节点PORT下属性VALUE的值
	*[IN]passfail:	测试参数是否合格，合格"P",不合格"F"
	*返回值：		0--正确  其他 出错，错误码见上述定义
	****************************************************************************************************************/
	int SetPortPassfail(char* tenvKey, char* objectKey, char* portKey, char* passfail);

	/****************************************************************************************************************
	*函数描述：根据 TENV_ID.VALUE/OBJECT.NAME 填入该OBJECT整体是否合格结果
	*[IN]tenvKey：	TENV_ID.VALUE，即模板中节点TENV_ID下属性VALUE的值
	*[IN]objectKey:	OBJECT.NAME，即模板中节点OBJECT下属性NAME的值
	*[IN]passfail:	测试参数是否合格，合格"P",不合格"F"
	*返回值：		0--正确  其他 出错，错误码见上述定义
	****************************************************************************************************************/
	int SetObjectPassfail(char* tenvKey, char* objectKey, char* passfail);

	/****************************************************************************************************************
	*函数描述：根据 TENV_ID.VALUE 填入该TENV_ID整体是否合格结果
	*[IN]tenvKey：	TENV_ID.VALUE，即模板中节点TENV_ID下属性VALUE的值
	*[IN]passfail:	测试参数是否合格，合格"P",不合格"F"
	*返回值：		0--正确  其他 出错，错误码见上述定义
	****************************************************************************************************************/
	int SetTenvPassfail(char* tenvKey, char* passfail);

	/****************************************************************************************************************
	*函数描述：设置MEAS_RECORD.MFG_RECORD 下的内容
	*[IN]key：		MEAS_RECORD.MFG_RECORD.PARA 节点下NAME的值，例如Prod_SN、Prod_Process等
	*[IN]value:		MEAS_RECORD.MFG_RECORD.PARA 节点下VALUE的值，即SN号、process等
	*返回值：		0--正确  其他 出错，错误码见上述定义
	****************************************************************************************************************/
	int SetMfgRecordByKey(char* key, char* value);

	/****************************************************************************************************************
	*函数描述：设置提示信息、GFF目标曲线文件所在路径
	*[out]path：		提示信息、GFF目标曲线文件所在路径，通常使用默认值，特殊情况需要设置
	*返回值：		0--正确  其他 出错，错误码见上述定义
	****************************************************************************************************************/
	int SetNoticePath(char* path);

	/****************************************************************************************************************
	*函数描述：设置MISC_RECORD下的内容
	*[IN]objectValue：	要设置的MISC_RECORD.OBJECT_ID的Value值
	*[IN]key：			要设置的MISC_RECORD.OBJECT_ID.PARA的NAME值
	*[IN]value：			需要设置的MISC_RECORD.OBJECT_ID.PARA的VALUE值
	*返回值：		0--正确  其他 出错，错误码见上述定义
	****************************************************************************************************************/
	int SetMiscRecord(char *objectValue, char *key, char *value);

	/****************************************************************************************************************
	*函数描述：获取MISC_RECORD下的内容
	*[IN]objectValue：	要设置的MISC_RECORD.OBJECT_ID的Value值
	*[IN]key：			要设置的MISC_RECORD.OBJECT_ID.PARA的NAME值
	*[Out]value：		获取到的MISC_RECORD.OBJECT_ID.PARA的VALUE值
	*返回值：		0--正确  其他 出错，错误码见上述定义
	****************************************************************************************************************/
	int GetMiscRecord(char *objectValue, char *key, char *value);

	/****************************************************************************************************************
	*函数描述：获取DOCREV_RECORD下的内容
	*[IN]recName：	要设置的DOC_RECORD.NAME的Value值
	*[Out]value：	获取到的DOC_RECORD的VALUE值
	*返回值：		0--正确  其他 出错，错误码见上述定义
	****************************************************************************************************************/
	int GetDocRevRecord(char *recName, char *value);

	/****************************************************************************************************************
	*函数描述：获取提示信息，最大长度为2048
	*[in]len:		pNotice数组长度
	*[out]pNotice：	提示信息
	*返回值：		0--正确  其他 出错，错误码见上述定义
	****************************************************************************************************************/
	int GetNotice(char* pNotice,int len);

	/****************************************************************************************************************
	*函数描述：获取GFF目标曲线，最大长度为2048
	*[in]len:		pTarget数组长度
	*[out]pTarget：	GFF目标曲线
	*返回值：		0--正确  其他 出错，错误码见上述定义
	****************************************************************************************************************/
	int GetGFFTargetCurve(char* pTarget, int len);
	//清除测试结果
	int ClearTestData();

	/****************************************************************************************************************
	*函数描述：根据PORT.Name获取该PORT的具体信息
	*[IN]portName:PORT.Name
	*返回值：	 该PORT的具体信息
	****************************************************************************************************************/
	CPortAttr GetPortByName(char* portName);

	/****************************************************************************************************************
	*函数描述：根据具体的设置，找到对应的环境信息
	*[IN]vKeys: 设置的关键字
	*[IN]vSets: 设置的具体值
	*返回值：	 该TENV_ID，以及描述
	****************************************************************************************************************/
	CTenvAttr GetTenvBySetting(const vector<string> vKeys, const vector<string> vSets);

	/****************************************************************************************************************
	*函数描述：根据具体的设置，找到对应的条件信息
	*[IN]vKeys: 设置的关键字
	*[IN]vSets: 设置的具体值
	*返回值：	 该ConditionID，以及描述
	****************************************************************************************************************/
	CCondAttr GetConditonBySetting(const vector<string> vKeys, const vector<string> vSets);

	/****************************************************************************************************************
	*函数描述：设置Misc里的软件信息
	*[IN]pSoftName: 软件名称
	*[IN]pSoftVer: 软件版本
	*[IN]pReleaseTime: 软件发布时间
	*[IN]pAuthor: 软件负责人
	*返回值：	 0--正确  其他 出错，错误码见上述定义
	****************************************************************************************************************/
	int SetMiscSoftwareInfo(char *pSoftName, char *pSoftVer, char *pReleaseTime, char *pAuthor);

	/****************************************************************************************************************
	*函数描述：设置 TENV_ID.VALUE为pTenvID下的所有测试项Active的值
	*[IN]pTenvID：		TENV_ID.VALUE，即模板中节点TENV_ID下属性VALUE的值
	*[IN]active:	    想要设置Active的值，0/1
	*返回值：			错误码，0--正确  其他 出错，错误码见上述定义
	****************************************************************************************************************/
	int SetAllActive(char *pTenvID, int active);

	/****************************************************************************************************************
	*函数描述：设置 TENV_ID.VALUE为pTenvID，OBJECT.NAME为pObjectID下的所有测试项Active的值
	*[IN]pTenvID：		TENV_ID.VALUE，即模板中节点TENV_ID下属性VALUE的值
	*[IN]pObjectID:		OBJECT.NAME，即模板中节点OBJECT下属性NAME的值
	*[IN]active:	    想要设置Active的值，0/1
	*返回值：			错误码，0--正确  其他 出错，错误码见上述定义
	****************************************************************************************************************/
	int SetAllActive(char *pTenvID,char *pObjectID, int active);


	/****************************************************************************************************************
	*函数描述：设置 TENV_ID.VALUE为pTenvID，OBJECT.NAME为pObjectID，PORT.VALUE为pPortID下的所有测试项Active的值
	*[IN]pTenvID：		TENV_ID.VALUE，即模板中节点TENV_ID下属性VALUE的值
	*[IN]pObjectID:		OBJECT.NAME，即模板中节点OBJECT下属性NAME的值
	*[IN]portKey：		PORT.VALUE，即模板中节点PORT下属性VALUE的值
	*[IN]active:	    想要设置Active的值，0/1
	*返回值：			错误码，0--正确  其他 出错，错误码见上述定义
	****************************************************************************************************************/
	int SetAllActive(char *pTenvID, char *pObjectID, char *pPortID,int active);
	//
private:
	void Release();
	char *m_pNoticePath;
	char *m_pTemplatePath;
	char *m_pGFFTarget;
	char *m_pNotice;
	char m_ErrMsg[1024];
	SYSTEMTIME m_OpenTime;
	CMeasRecord *m_pMeasRecord;
	map<string, shared_ptr<CCommAttr>> *m_pProdRecord;
	
	
};
