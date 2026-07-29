// listDlg.h : header file
//

#if !defined(AFX_LISTDLG_H__3D1DCD47_1BF6_4E91_AD6E_90A9E9B29C10__INCLUDED_)
#define AFX_LISTDLG_H__3D1DCD47_1BF6_4E91_AD6E_90A9E9B29C10__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "OpCRC32.h"
#include "OpComm.h"
#include "ColorListBox.h"
#include "CommandSFPVOA.h"
/////////////////////////////////////////////////////////////////////////////
// CListDlg dialog
#define MAX_COUNT 1024
#define COLOR_BLACK			RGB(0, 0, 0)
#define COLOR_BLUE			RGB(0, 0, 255)
#define COLOR_GREEN			RGB(0, 255, 0)
#define COLOR_RED			RGB(255, 0, 0)
#define COLOR_YELLOW		RGB(255, 255, 0)
#define COLOR_LIGHT_BLUE	RGB(0, 255, 255)
#define COLOR_PINK			RGB(255, 0, 255)
#define COLOR_WHITE			RGB(255, 255, 255)
#define COLOR_ORINGE		RGB(255, 128, 0)


#define	MAX_VOA_CAL_DATA				(4096)
#define COMMAND_INDEX    0X00
#define MAX_PD_CAL_SAMPLE				700
#define MAX_PD_VER_SAMPLE				512
//#define EEROM_START_ADDRESS                0x0000 //EEROM起始地址
//#define EEROM_END_ADDRESS                  0x3FFF //EEROM终止地址
#define MAX_COUNT 1024

#define SLAVE_WRITE      0XAE
#define SLAVE_READ       0XAE
#define COMMAND_INDEX    0X00

#define EEROM_START_ADDRESS                0x0000 //EEROM起始地址
#define EEROM_ENDPASE_ADDRESS              0x0040 //EEROM起始地址
#define EEROM_END_ADDRESS                  0x3FFF //EEROM终止地址

#define EEROM_CUSTOMLABELINFO_START        0x0100 //客户Label信息起始地址
#define EEROM_CUSTOMLABELINFO_END          0x0237 //客户Label信息终止地址

#define EEROM_EEROMHEADER_START            0x1000 //EEROM头结构起始
#define EEROM_EEROMHEADER_END              0x15AC //EEROM头结构终止

#define EEROM_TABLE1_START                 0x110D //Table 1 起始地址 //P3
#define EEROM_TABLE1_DECNAME               0x1115 //Table 1 起始地址 //P3
#define EEROM_TABLE1_RESVERS               0x1129 //Table 1 预留起始地址 //P3
#define EEROM_TABLE1_RESVERE               (EEROM_TABLE1_RESVERS-1)+ 108//Table 1 预留结束地址 //P3
#define EEROM_TABLE1_THRS                  0x1195//上门限
#define EEROM_TABLE1_THRE                  (EEROM_TABLE1_THRS-1)+24//门限结束
#define EEROM_TABLE1_END                   0x122C //Table 1 终止地址

#define EEROM_TABLE2_START                 0x122D //Table 2 起始地址 //PD1
#define EEROM_TABLE2_DECNAME               0x1235 //Table 2 起始地址 //P1
#define EEROM_TABLE2_RESVERS               0x1249 //Table 2 预留起始地址 //P1
#define EEROM_TABLE2_RESVERE               (EEROM_TABLE2_RESVERS-1)+ 108//Table 2 预留结束地址 //P1
#define EEROM_TABLE2_THRS                  0x12B5//上门限
#define EEROM_TABLE2_THRE                  (EEROM_TABLE2_THRS-1)+24//门限结束
#define EEROM_TABLE2_END                   0x134C //Table 2 终止地址

#define EEROM_TABLE3_START                 0x134D //Table 3 起始地 //NULL
#define EEROM_TABLE3_DECNAME               0x1355 //Table 3 起始地址 //NULL
#define EEROM_TABLE3_RESVERS               0x1369 //Table 3 预留起始地址 //NULL
#define EEROM_TABLE3_RESVERE               (EEROM_TABLE3_RESVERS-1)+ 108//Table 3 预留结束地址 //NULL
#define EEROM_TABLE3_THRS                  0x13D5//上门限
#define EEROM_TABLE3_THRE                  (EEROM_TABLE3_THRS-1)+24//门限结束
#define EEROM_TABLE3_END                   0x1414 //Table 3 终止地址

#define EEROM_TABLE4_START                 0x1415 //Table 4 起始地址//PD2
#define EEROM_TABLE4_DECNAME               0x141D //Table 4 起始地址 //P2
#define EEROM_TABLE4_RESVERS               0x1431 //Table 4 预留起始地址 //P2
#define EEROM_TABLE4_RESVERE               (EEROM_TABLE4_RESVERS-1)+ 108//Table 4 预留结束地址 //P2
#define EEROM_TABLE4_THRS                  0x149D//上门限
#define EEROM_TABLE4_THRE                  (EEROM_TABLE4_THRS-1)+24//门限结束
#define EEROM_TABLE4_END                   0x14DC //Table 4 终止地址

#define EEROM_TABLE5_START                 0x14DD //Table 5 起始地址//IL与表1一致
#define EEROM_TABLE5_DECNAME               0x14E5 //Table 5 起始地址 //IL
#define EEROM_TABLE5_RESVERS               0x14F9 //Table 5 预留起始地址 //IL
#define EEROM_TABLE5_RESVERE               (EEROM_TABLE5_RESVERS-1)+ 108//Table 5 预留结束地址 //IL
#define EEROM_TABLE5_THRS                  0x1565//上门限
#define EEROM_TABLE5_THRE                  (EEROM_TABLE5_THRS-1)+24//门限结束
#define EEROM_TABLE5_ILBASE				   0x157D //IL数据开始
#define EEROM_TABLE5_ILBASEADC  		   0x1595 //ADC数据开始
#define EEROM_TABLE5_END                   0x15AC //Table 5 终止地址



#define EEROM_TABLE6_START                 0x120D//Table 6 起始地址
#define EEROM_TABLE6_END                   0x154C //Table 6 终止地址

#define EEROM_TABLE7_START                 0x154D //Table 7 起始地址
#define EEROM_TABLE7_END                   0x1890 //Table 7 终止地址
//MFG地址
#define ELECLABLE_MOUDLE_TPYE	0x0100  //1、	光模块型号：如"ABCDEFG",长度20个字节，不足20个字节在后面补充空格；//SN
#define ELECLABLE_MOUDLE_CPN 	0x0114  //2、	光模块条码：如"45020396"，长度20个字节，不足20个字节在后面补充空格；//45020434
#define ELECLABLE_MOUDLE_SN		0x0128  //3、	光模块编码：如"11111111"， 长度20个字节，不足20个字节在后面补充空格；
#define ELECLABLE_MOUDLE_EDEC	0x013C  //4、	光模块英文描述：如"xxxxxxxxxx"，长度200个字节，不足200个字节在后面补充空格；
#define ELECLABLE_MOUDLE_TIME	0x0204  //5、	光模块生产日期：格式"YYYY-MM-DD",长度12个字节
#define ELECLABLE_MOUDLE_VERD	0x0210  //6、	光模块供应商名称：如"xxxxxx"，长度20个字节，不足20个字节在后面补充空格//OPLINK
#define ELECLABLE_MOUDLE_OUT	0x0224  //7、	光模块对外型号：如"xxxxxx"，长度20个字节，不足20个字节在后面补充空格//
//?PD??ADC?????????
#define EEROM_TABLE1_AD_ADRR      0x11DD  //PD1
#define EEROM_TABLE1_POWER_ADRR   0x11E1  

#define EEROM_TABLE2_AD_ADRR      0x12CD  //PD2
#define EEROM_TABLE2_POWER_ADRR   0x12D1

#define EEROM_TABLE3_AD_ADRR      0x13ED  //NULL
#define EEROM_TABLE3_POWER_ADRR   0x13F1

#define EEROM_TABLE4_AD_ADRR      0x14B5  //NULL
#define EEROM_TABLE4_POWER_ADRR   0x14B9
//整张表头结构开始与结束地址
#define EEROM_TABLE_HEADS_ADRR   0x1000
#define EEROM_TABLE_HEADE_ADRR   (EEROM_TABLE_HEADS_ADRR-1)+20
//大小
#define EEROM_TABLE_CAPAS_ADRR   0x1014
#define EEROM_TABLE_CAPAE_ADRR   (EEROM_TABLE_CAPAS_ADRR-1)+4
//预留0
#define EEROM_TABLE_REVE0S_ADRR   0x1018
#define EEROM_TABLE_REVE0E_ADRR   (EEROM_TABLE_REVE0S_ADRR-1)+36
//前60位按位相加校验和
#define EEROM_TABLE_CHSUMS_ADRR   0x103C
#define EEROM_TABLE_CHSUME_ADRR   (EEROM_TABLE_CHSUMS_ADRR-1)+4
//单板平台文件头,为了支持上下载,必须正确
#define EEROM_TABLE_FWDLS_ADRR   0x1040
#define EEROM_TABLE_FWDLE_ADRR   (EEROM_TABLE_FWDLS_ADRR-1)+16
//文件名称
#define EEROM_TABLE_FILENAMES_ADRR   0x1050
#define EEROM_TABLE_FILENAMEE_ADRR   (EEROM_TABLE_FILENAMES_ADRR-1)+20
//保留字节1
#define EEROM_TABLE_REVE1S_ADRR   0x1064
#define EEROM_TABLE_REVE1E_ADRR   (EEROM_TABLE_REVE1S_ADRR-1)+4
//文件类型
#define EEROM_TABLE_FILETYPES_ADRR   0x1068
#define EEROM_TABLE_FILETYPEE_ADRR   (EEROM_TABLE_FILETYPES_ADRR-1)+4
//文件长度
#define EEROM_TABLE_FILELENGS_ADRR   0x106C
#define EEROM_TABLE_FILELENGE_ADRR   (EEROM_TABLE_FILELENGS_ADRR-1)+4
//文件纯数据校验和，从地址0x10C0开始到文件结束0x15AC，以BYTE类型求和（备注1）
#define EEROM_TABLE_CHSUM0S_ADRR   0x1070
#define EEROM_TABLE_CHSUM0E_ADRR   (EEROM_TABLE_CHSUM0S_ADRR-1)+4
//生产时间
#define EEROM_TABLE_MFGDATES_ADRR   0x1074
#define EEROM_TABLE_MFGDATEE_ADRR   (EEROM_TABLE_MFGDATES_ADRR-1)+8

//加密标志
#define EEROM_TABLE_BCDS_ADRR   0x107C
#define EEROM_TABLE_BCDE_ADRR   (EEROM_TABLE_BCDS_ADRR-1)+2

//压缩标志
#define EEROM_TABLE_RARS_ADRR   0x107E
#define EEROM_TABLE_RARE_ADRR   (EEROM_TABLE_RARS_ADRR-1)+2

//保留2
#define EEROM_TABLE_REVE2S_ADRR   0x1080
#define EEROM_TABLE_REVE2E_ADRR   (EEROM_TABLE_REVE2S_ADRR-1)+60

//单板平台文件头校验和，填好了文件纯数据校验和后才能填这个校验和，从地址0x1040开始到0x10bb结束，以高字节序DWORD类型求和（备注2）
#define EEROM_TABLE_CHSUM2S_ADRR   0x10BC
#define EEROM_TABLE_CHSUM2E_ADRR   (EEROM_TABLE_CHSUM2S_ADRR-1)+4

//参数表从此开始，平台头文件校验和就是从这开始，文件长度也是从此开始计算，以下所有的都是大字节优先
#define EEROM_TABLE_PARAS_ADRR   0x10C0
#define EEROM_TABLE_PARAE_ADRR   (EEROM_TABLE_PARAS_ADRR-1)+32
//描述信息12字节，可根据模块需要填写
#define EEROM_TABLE_DECINFOS_ADRR   0x10E0
#define EEROM_TABLE_DECINFOE_ADRR   (EEROM_TABLE_DECINFOS_ADRR-1)+12
//主版本	副版本	参数表数
#define EEROM_TABLE_VERALLS_ADRR   0x10EC
#define EEROM_TABLE_VERALLE_ADRR   (EEROM_TABLE_VERALLS_ADRR-1)+2
//	表1标识（参数表ID）..表1项数(PIN管标定个数)	表1档位数	表2标识	表2项数	表2档位数	表3标识（参数表ID）	表3项数(PIN管标定个数)	表3档位数	表4标识（参数表ID）	表4项数(PIN管标定个数)	表4档位数	表5标识（参数表ID）	表5项数(光口插损个数)	表5档位数
#define EEROM_TABLE_LUTMARKS_ADRR   0x10EE
#define EEROM_TABLE_LUTMARKE_ADRR   (EEROM_TABLE_LUTMARKS_ADRR-1)+16
//保留3
#define EEROM_TABLE_REVE3S_ADRR   0x10FE
#define EEROM_TABLE_REVE3E_ADRR   (EEROM_TABLE_REVE3S_ADRR-1)+15
//LUT表CHECKSUM2计算起始地址和结束地址
#define EEROM_LUT_CHECKSUM2S_ADRR   0x1040
#define EEROM_LUT_CHECKSUM2E_ADRR   (EEROM_LUT_CHECKSUM2S_ADRR-1)+124
//LUT表CHECKSUM1计算起始地址和结束地址
#define EEROM_LUT_CHECKSUM1S_ADRR   0x10C0
#define EEROM_LUT_CHECKSUM1E_ADRR   (EEROM_LUT_CHECKSUM1S_ADRR-1)+1261
#define EEROM_TABLEALL_END_ADRR     0x15AC

//#define EEROM_TABLE8_START                 0x1C75//Table 8 起始地址
//#define EEROM_TABLE8_END                   0x1FB4 //Table 8 终止地址
//
//#define EEROM_TABLE9_START                 0x1FB5 //Table 9 起始地址
//#define EEROM_TABLE9_END                   0x22F4 //Table 9 终止地址
//
//#define EEROM_TABLE10_START                 0x22F5 //Table 10 起始地址
//#define EEROM_TABLE10_END                   0x2634 //Table 10 终止地址
//
//#define EEROM_TABLE11_START                 0x2635 //Table 11 起始地址
//#define EEROM_TABLE11_END                   0x2974 //Table 11 终止地址

#define EEROM_SINGLEBORDFILEHEADER_START   0x1040 //单板平台文件头起始
#define EEROM_SINGLEBORDFILEHEADER_END     0x10BF //单板平台文件头终止

#define EEROM_PARMSTARTINFO_START          0x10C0 //参数表头起始
#define EEROM_PARMSTARTINFO_END            0x110C //参数表头终止

#define EEROM_LUT1_INPD_START              0x110D //Input PD Lut起始
#define EEROM_LUT1_INPD_END                0x123C //Input PD Lut终止
#define EEROM_LUT2_OUTPD_START             0x123D //Output PD Lut起始
#define EEROM_LUT2_OUTPD_END               0x136C //Output PD Lut终止
#define EEROM_LUT3_REPD_START              0x136D //RE PD Lut起始
#define EEROM_LUT3_REPD_END                0x149C //RE PD Lut终止
#define EEROM_LUT4_ROPD_START              0x149D //RO PD Lut起始
#define EEROM_LUT4_ROPD_END                0x15CC //RO PD Lut终止


typedef struct tagLutPoint
{
	int      nIndex;
	DWORD    dwPower;
	DWORD    dwADC; 
}stLutPoint,*pLutPoint;
typedef struct tagVOALut
{
	int nPointCount;
	stLutPoint stLut[200];
}stVOALut,*pVOALut;

typedef struct tagLutPDPoint
{
	int      nIndex;
	int      nPower;
	DWORD    dwADC;
}stLutPDPoint, *pLutPDPoint;

typedef struct tagPDCalLut
{
	int nLutCount;
	stLutPDPoint stLutData[20];
}stPDCalLut,*pPDCalLut;
class CListDlg : public CDialog
{
// Construction
public:
	CString m_strConfigFilePath;
	BOOL ReadRefMappFinal(DWORD i);
	BOOL SaveRefMappFile(DWORD dWCONUT);
	UINT32 CalCrc32(BYTE data[], UINT32 size);
	BOOL  PDGetCRC(COpComm *m_OpCom1 ,DWORD *dwCRC);
	DWORD SwapDWORD(DWORD dwInData);
	BOOL Control1X64Switch(COpComm *m_OpCom1,int index, int channel);
	BOOL Control1X8Switch(COpComm *m_OpCom1,int channel);
	BOOL PDPrepareScan(COpComm *m_OpCom1 ,DWORD dwContPoint);
	BOOL FinalTestGetPower(COpComm *m_OpCom1,PBYTE pbReadData, DWORD dwContPoint);
	BOOL SweepTLSMon(DWORD dwContPoint);
	BOOL ControlDev1X64Switch( int channel);
	BOOL Control1X64Switch(int channel);
	BOOL SetSWDUTtestCMD(CString *strTemp,BOOL bExitCMD =TRUE);//1 进入产品默认，0进行测试板
	BOOL SetSWPORT(int nCH,BOOL bHL);
	BOOL GetRS232PIN(int nBlock,int nPINIndex,double *InPower);
	BOOL GetRS232PDADC(int nBlock,int nPDIndex,PWORD pADCValue);
	BOOL Set1X2SwitchForASCII(int nStatus);
	BOOL ReadVOALutFromEEROMHandLutData(int nVOAIndex, pVOALut pstVOALutData);
	BOOL SetEEROMWritePinFroASCII(int nStatus);
	BOOL ReadEEROMData(WORD wStartAddress, PBYTE pbData, DWORD dwReadLength, DWORD* pdwFeedbackLength);
	BOOL RxDataFromI2C(BYTE byIndex, WORD wAddress, DWORD dwReadLength, BYTE *pbySetValue, PDWORD pdwSetLength);
	void AsiicToHex(BYTE *str, BYTE *hex, int cnt, int *nGetCount);
	BOOL TxDataToI2C(BYTE byIndex, WORD wAddress, BYTE *pbySetValue, DWORD dwSetLength);
	BOOL SetVOAActualDACForASCII(int nVOAIndex, WORD wDACValue);
	void LogInfoSave(CString strLogInfo, int nLength);
	//PD
	BOOL GetPDADCForASCII(int nPDIndex, PWORD pADCValue);
	BOOL CalcuPDReportPowerByADC(DWORD dwADC, double &pdbReportPower);
	BOOL ReadPDCalLutFromEEROM(int nPDIndex, pPDCalLut pData);
	BOOL CalcuVOAADCbyAtten(double dbAtten, PWORD pwADC);
	BOOL LinearFitArray(double *pdblX, double *pdblY, int nCount, double &k, double &c);
	double Sum(double *pdblArray, int nCount);

	stVOALut        m_stVOALut;
	stPDCalLut      m_stPDLut;
	///chunliang add
	BOOL OpZHDevice(); 
	void YieldToPeers();
public:
	BOOL m_bOpenTestDevice;
	BOOL m_bOpenCOM;
	CString m_strErrorMsg;
	int	nCount;
	BOOL bAB;
	BYTE bPNtpye;
	int nReadPD;
	CStringArray	m_strMsgDataArray[300]; 
	CStringArray	m_strDataArray; 
	void PreTest();
	void CalCrc32data(); 
	CCommandSFPVOA  m_CmdSFPVOA;
	COpCRC32 CRC32;
	COpComm m_OpCom;
	COpComm m_1X64OpCom;
	BOOL SetSwitchForASCII(int nSwtype,int nindex,int nChannal);
	BOOL ReadPM(double *dbPower);
	BOOL ReadTLSPM(double *dbPower);
	void LogInfo(LPCTSTR tszlogMsg,DWORD dwRGB=0, BOOL bAppend=0);
	CString LoadIniDataString(LPCTSTR strSection, LPCTSTR szID);
public:
	CListDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CListDlg)
	enum { IDD = IDD_LIST_DIALOG };
	CComboBox	m_MCS_CH2;
	CComboBox	m_MCS_CH1;
	CComboBox	m_MCS_2;
	CComboBox	m_1X8;
	CComboBox	m_MCS_1;
	CComboBox	m_1X2;
	CComboBox	m_1X64_2;
	CComboBox	m_1X64_1;
	CComboBox	m_SWch;
	CColorListBox	m_ctrMsgList;
	int		m_nCom;
	int		m_1x64Com;
	double	m_dbSetAtten;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CListDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation

protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CListDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnButton1();
	afx_msg void OnButton2();
	afx_msg void OnButton3();
	afx_msg void OnButton4();
	afx_msg void OnButton5();
	afx_msg void OnButton6();
	afx_msg void OnRadio1();
	afx_msg void OnRadio2();
	afx_msg void OnButton7();
	afx_msg void OnButton8();
	afx_msg void OnRadio3();
	afx_msg void OnRadio4();
	afx_msg void OnRadio5();
	afx_msg void OnButton9();
	afx_msg void OnButton10();
	afx_msg void OnBUTTON1x8();
	afx_msg void OnBUTTON1x2();
	afx_msg void OnBUTTON1x64();
	afx_msg void OnBUTTON1x11();
	afx_msg void OnBUTTON1x12();
	afx_msg void OnBUTTON1x13();
	afx_msg void OnButton11();
	afx_msg void OnButton12();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LISTDLG_H__3D1DCD47_1BF6_4E91_AD6E_90A9E9B29C10__INCLUDED_)
