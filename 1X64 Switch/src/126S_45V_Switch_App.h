// 126S_45V_Switch_App.h : main header file for the 126S_45V_SWITCH_APP application
//

#if !defined(AFX_126S_45V_SWITCH_APP_H__AAE9D1BA_2916_4A45_9841_7ECE2FA3DD66__INCLUDED_)
#define AFX_126S_45V_SWITCH_APP_H__AAE9D1BA_2916_4A45_9841_7ECE2FA3DD66__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CMy126S_45V_Switch_AppApp:
// See 126S_45V_Switch_App.cpp for the implementation of this class
//

/*
#define MAX_CHANNEL_NUM    36  // 6*6=36 channels
#define MAX_MIDPTR_NUM       25  // Under the 6*6 channels, the middle points will be 5*5=25
#define MAX_SWITCH_TARGET_NUM    MAX_CHANNEL_NUM + MAX_MIDPTR_NUM  // 61
//#define DAC_NUM       61  // DACֵ

enum IDX_SWITCH_TEMPT
{
    IDX_LOW_SWITCH = 0,
    IDX_ROOM_SWITCH,
    IDX_HIGH_SWITCH,
    TEMPT_SWITCH_NUM
};


typedef struct tagMidPointsMatrix
{
    BYTE bMidPointNum; //Middle point number should be decided by switch type  
    BYTE c_bMidChange[MAX_MIDPTR_NUM][MAX_MIDPTR_NUM]; 
    // The total size is 1+25*25=626 byte
}stMidPointsMatrix;

typedef struct tagAxisDAC
{
    BYTE sDACx[2];
    BYTE sDACy[2];
}stAxisDAC;


typedef struct tagChnDAC
{
    BYTE      wValid[2];
    BYTE     sTemperature[2];
    stAxisDAC stDAC[MAX_SWITCH_TARGET_NUM]; // Change SWITCH_TARGET_NUM to MAX_SWITCH_TARGET_NUM on 20161107, by yyh
    // The total size is 2+2+(2+2)*61=248 bytes
}stChnDAC;


typedef struct tagMemsSwCoef
{
    // Individual MemsSwitch calibration LUT structure
    BYTE BUNDLEHEADER[160];
    // Switch type max channel num, for example, 1x19 switch, this value should be 19.
    BYTE bSWTypeChanNum;

    // Middle points matrix. 626bytes

	BYTE bSacve;

    stMidPointsMatrix pstMidPointsMatrix;

    // DAC setting and Calibration temperature point for each switch, 248*3=744 bytes
    stChnDAC stCalibDAC[TEMPT_SWITCH_NUM];

    // Setting value is 1-base, 36 bytes
    BYTE  bSWChnMapping[MAX_CHANNEL_NUM]; 

    // The next target point for each channel. 36 bytes
    BYTE  bMidPoint[MAX_CHANNEL_NUM]; 

    // Select one of channels or middle points for dark setting points. 1-base. 1 bytes
    BYTE  bDarkPoint;

    // Channel number for each Switch. 1 bytes
    BYTE  bMaxChannelNUM;

    // Time delay for each DAC setting point. 2 bytes
    BYTE  wSwitchDelayUs[2];

    // IL of each channel. 2*36=72 bytes
    BYTE  wSwitchIL[MAX_CHANNEL_NUM][2];

    BYTE bReserved[524];

    BYTE dwCRC32[4]; // 4 bytes

    // 1+626+744+36+36+1+1+2+72+525+4=2048, 2K Bytes

}stMemsSwCoef;
*/

typedef struct tagstMfgInfo
{
	  BYTE BUNDLEHEADER[160];
      char pchSN[32];
      char pchPN[32];              
      WORD wMfgID;
      WORD wFwVer;
      WORD wPCBAVer;
      WORD wReserved0;
      DWORD dwMfgDate;
      DWORD dwCalDate;
      WORD wSwitchType;
      BYTE bReserved1[30];
      char pchGUIVer[16];
      char pchDevice1SN[16];
      char pchDevice2SN[16];
      char pchDevice3SN[16];
      char pchDevice4SN[16];
      BYTE bReserve[64];

	  BYTE bChMapping[4][32]; // Index is optical channel, content is MemosSwitch channel.
	  BYTE bReserve1[128];

	  BYTE bPortVsSwCh[65][4]; // Block + Normal channel
	  BYTE bReserve2[248];
      DWORD dwCRC32;
}stMfgInfo;

enum IDX_SWITCH
{
    IDX_SWITCH_00 = 0,
    IDX_SWITCH_01,
    IDX_SWITCH_02,
    IDX_SWITCH_03, 
    SWITCH_NUM // There are four switches in 14538 1x64 switch module
};

enum IDX_SWITCH_TEMPT
{
    IDX_LOW_SWITCH = 0,
    IDX_ROOM_SWITCH,
    IDX_HIGH_SWITCH,
    IDX_RESERVE,
    TEMPT_SWITCH_NUM
};

#define MAX_CHANNEL_NUM          36  // 6*6=36 channels
#define MAX_MIDPTR_NUM           25  // In case of 6*6 channels, the middle points will be 5*5=25
#define MAX_SWITCH_TARGET_NUM    MAX_CHANNEL_NUM + MAX_MIDPTR_NUM // 61

typedef struct tagAxisDAC
{
    SHORT sDACx;
    SHORT sDACy;
}stAxisDAC;

typedef struct tagChnDAC
{
    WORD      wValid;
    SHORT     sTemperature;
    stAxisDAC stChnDAC[MAX_CHANNEL_NUM];
    stAxisDAC stMidDAC[MAX_MIDPTR_NUM];
    // The total size is 2+2+(2+2)*61=248 bytes
}stChnDAC;

// Middle points matrix structure, add on 20161107, by yyh
typedef struct tagMidPointsMatrix
{
    BYTE bMidPointNum;
    BYTE bReserved4;
    
    BYTE c_bMidChange[MAX_MIDPTR_NUM][MAX_MIDPTR_NUM];  // 25*25=625 bytes
    BYTE bReserved5;
    // The total size is 1+25*25=628 bytes
    
}stMidPointsMatrix;

typedef struct tagMemsSwCoef
{
	BYTE BUNDLEHEADER[160];
    // Individual MemsSwitch calibration LUT structure
    
    // Switch type max channel num, for example, 1x19 switch, this value should be 19.
    BYTE bSWTypeChanNum;
    BYTE bReserved0;
    // Select one of channels or middle points for dark setting points. 1-base. 1 bytes
    BYTE bDarkPoint;
    BYTE bReserved1;
    // Time delay after setting DAC. 2 bytes
    WORD  wSwitchDelayUs;
    BYTE bReserved2[2];
    // The middle point for each channel. 36 bytes
    BYTE  bMidPoint[MAX_CHANNEL_NUM]; 
    // IL of each channel. 2*36=72 bytes
    WORD  wSwitchIL[MAX_CHANNEL_NUM];
    BYTE bReserved3[44];
    // Middle points matrix. 628bytes
    stMidPointsMatrix pstMidPointsMatrix;
    BYTE bReserved6[236];
    
    // DAC setting and Calibration temperature point for each switch, 248*4=992 bytes
    stChnDAC stCalibDAC[TEMPT_SWITCH_NUM];
    
    BYTE bReserved7[28];
    
    DWORD dwCRC32; // 4 bytes
    
    // Total 2048 Bytes
    
}stMemsSwCoef;

typedef struct tagSwChInfo
{
	BYTE BUNDLEHEADER[160];
	BYTE bChMapping[SWITCH_NUM][32]; // Index is optical channel, content is MemosSwitch channel.
	BYTE bReserve[1024-4-32*SWITCH_NUM]; 
	DWORD dwCRC32;
}stSwChInfo;

#define MAX_SW_PORT_NUM          64

typedef struct tag64PortMap
{
  BYTE BUNDLEHEADER[160];
  BYTE bPortVsSwCh[MAX_SW_PORT_NUM + 1][SWITCH_NUM]; // Block + Normal channel
  BYTE bReserve[1024-4-(MAX_SW_PORT_NUM + 1)*SWITCH_NUM];
  DWORD dwCRC32;
}st64PortMap;

class CMy126S_45V_Switch_AppApp : public CWinApp
{
public:
	CMy126S_45V_Switch_AppApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMy126S_45V_Switch_AppApp)
	public:
	virtual BOOL InitInstance();
	afx_msg void OnRefreshDataMessage(WPARAM wPara, LPARAM lPara);
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CMy126S_45V_Switch_AppApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_126S_45V_SWITCH_APP_H__AAE9D1BA_2916_4A45_9841_7ECE2FA3DD66__INCLUDED_)
