// CommandForFUJ.cpp: implementation of the CCommandForFUJ class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "126s_45v_switch_app.h"
#include "CommandForFUJ.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

BYTE g_pbyMFGCmd[] = {0x4D,0x46,0x47,0x5F,0x43,0x4D,0x44,0x41};

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCommandForFUJ::CCommandForFUJ()
{

}

CCommandForFUJ::~CCommandForFUJ()
{

}

BOOL CCommandForFUJ::GetCmdInfo()
{
	//m_stCMDInfo¸³Öµ
	CString strFile;
	FILE    *fp=NULL;
	LPCTSTR lpStr=NULL;
	char    pchRead[256];
	CString strToken;
	int     nIndex=0;
	BYTE    byValue;
	GetCurrentDirectory(256,m_tszAppFolder);
	strFile.Format("%s\\config\\CMDInfo.csv", m_tszAppFolder);
	fp = fopen(strFile,"r");
	if (fp==NULL)
	{
		m_strError.Format("´ò¿ªÎÄ¼þ%sÊ§°Ü!",strFile);
		return FALSE;
	}
	ZeroMemory(pchRead,sizeof(pchRead));
	lpStr = fgets(pchRead,256,fp);
	while (1)
	{
		lpStr = fgets(pchRead,256,fp);
		if (lpStr==NULL)
		{
			break;
		}
		strToken = strtok(pchRead,",");
		strToken = strtok(NULL,",");
		sscanf(strToken,"%X",&byValue);
		m_stCMDInfo[nIndex].stCmd.byObjectID = byValue;
		strToken = strtok(NULL,",");
		ZeroMemory(m_stCMDInfo[nIndex].pchCmdName,1024);
		memcpy(m_stCMDInfo[nIndex].pchCmdName,strToken,strToken.GetLength());
		strToken = strtok(NULL,",");
		m_stCMDInfo[nIndex].stCmd.byInstance = atoi(strToken);
		strToken = strtok(NULL,",");
		m_stCMDInfo[nIndex].stCmd.byPara = atoi(strToken);
		strToken = strtok(NULL,",");
		if (strToken=="R")
		{
			m_stCMDInfo[nIndex].stCmd.byAccess = I2C_GET;
		}
		else
		{
			m_stCMDInfo[nIndex].stCmd.byAccess = I2C_SET;
		}
		
		strToken = strtok(NULL,",");
		m_stCMDInfo[nIndex].byDataSize = atoi(strToken);
		nIndex++;
		if (nIndex>=1024) 
		{
			break;
		}
	}
	m_nCmdCount = nIndex;
	fclose(fp);
	return TRUE;

}

BOOL CCommandForFUJ::SetCmdToSlave(BYTE byIDIndex, BYTE *pbyData)
{
	int  nIndex=0;
	BYTE byCmdLength;
	BYTE byCheckSum;
	BYTE byGetCmdLength;
	BYTE pbyGetCmd[MAX_BUFFER];
	int  i=0;
	int  nPara=0;
	CString strTemp="";
	CString strMsg="";

	m_stCMDInfo[byIDIndex].stCmd.byCmdLength = m_stCMDInfo[byIDIndex].byDataSize+4;
	byCmdLength = m_stCMDInfo[byIDIndex].stCmd.byCmdLength;
	PBYTE pbySetCmd = new BYTE[byCmdLength];
	m_pbySendData[nIndex++] = m_stCMDInfo[byIDIndex].stCmd.byMsgID;
	m_pbySendData[nIndex++] = m_stCMDInfo[byIDIndex].byDataSize; //m_stCMDInfo[byIDIndex].stCmd.byCmdLength;
	m_pbySendData[nIndex++] = m_stCMDInfo[byIDIndex].stCmd.byStatus;
    //m_pbySendData[nIndex++] = m_stCMDInfo[byIDIndex].stCmd.byAccess;
	//m_pbySendData[nIndex++] = m_stCMDInfo[byIDIndex].stCmd.byInstance;
	//m_pbySendData[nIndex++] = m_stCMDInfo[byIDIndex].stCmd.byObjectID;
	//m_pbySendData[nIndex++] = m_stCMDInfo[byIDIndex].stCmd.byPara;
	if (m_stCMDInfo[byIDIndex].stCmd.byAccess==I2C_SET)
	{
		/*if (CMD_SET_SW_DAC==byIDIndex||CMD_SW_SCAN_START==byIDIndex||
			CMD_SW_HITLESS_TEST == byIDIndex) 
		{
			for (nPara=0;nPara<8;nPara++)
			{
				m_pbySendData[nIndex++] = g_pbyMFGCmd[nPara]; //MFG
			}
		}*/
		for (nPara=0;nPara<m_stCMDInfo[byIDIndex].byDataSize;nPara++) 
		{
			m_pbySendData[nIndex++] = pbyData[nPara];
		}
	}
	//Cal CheckSum;
	GetCheckSum(m_pbySendData,nIndex,&byCheckSum);
	m_pbySendData[nIndex++] = byCheckSum;
	if(!WriteBuffer((char*)m_pbySendData,nIndex))
	{
		for (i=0;i<nIndex;i++)
		{
			strTemp.Format("%02X ",m_pbySendData[i]);
			strMsg = strMsg+strTemp;
		}
		m_strError = "·¢ËÍÐÅÏ¢´íÎó:" + strMsg;
		delete []pbySetCmd;
		pbySetCmd = NULL;
		return FALSE;
	}
	delete []pbySetCmd;
	pbySetCmd = NULL;
	
	Sleep(50);
	ZeroMemory(pbyGetCmd,sizeof(pbyGetCmd));
	if (!ReadBuffer((char*)pbyGetCmd,MAX_BUFFER,(DWORD*)&byGetCmdLength)) 
	{
		m_strError = "½ÓÊÕÐÅÏ¢´íÎó";
		return FALSE;
	}
	for (i=0;i<byGetCmdLength;i++)
	{
		strTemp.Format("%02X ",pbyGetCmd[i]);
		strMsg = strMsg+strTemp;
	}
	m_strError = strMsg;
	
	if (!CheckResponseCmd(pbyGetCmd,byGetCmdLength,byIDIndex)) 
	{
		return FALSE;
	}
//	*pbyData = 
	return TRUE;
}

BOOL CCommandForFUJ::GetCheckSum(BYTE *pbyCmd, int nLength, BYTE *pbyCheckSum)
{
//	DWORD dwCRC32=0;
	BYTE  byCRC=0;
	//m_CRC32.InitCRC32();
	for (int nIndex=0;nIndex<nLength;nIndex++)
	{
		byCRC += pbyCmd[nIndex];
	}
	byCRC = 0XFF - byCRC;
	*pbyCheckSum = byCRC;
	return TRUE;
}

BOOL CCommandForFUJ::CheckResponseCmd(PBYTE pbyReadData, int nDataLength,BYTE byIDIndex)
{
	//Check
	BYTE byGetCRC;
	int nIndex=0;
	for (nIndex=0;nIndex<3;nIndex++)
	{
		if (pbyReadData[nIndex]!=m_pbySendData[nIndex]) 
		{
			return FALSE;
		}
	}
	if (m_stCMDInfo[byIDIndex].stCmd.byAccess==I2C_GET)
	{
		for (nIndex=0;nIndex<m_stCMDInfo[byIDIndex].byDataSize;nIndex++)
		{
			m_pbyReadData[nIndex] = pbyReadData[3+nIndex];
		}
	}

	//check sum

	GetCheckSum(pbyReadData,nDataLength,&byGetCRC);
	if (byGetCRC!=pbyReadData[nDataLength-1])
	{
		m_strError = "CheckSUM´íÎó:" + m_strError;
		return FALSE;
	}
	return TRUE;
}

BOOL CCommandForFUJ::SetXYDAC(SHORT sXDAC, SHORT sYDAC)
{
	BYTE pbyData[4];
	pbyData[0] = (BYTE)(sXDAC>>8);
	pbyData[1] = (BYTE)(sXDAC);
	pbyData[2] = (BYTE)(sYDAC>>8);
	pbyData[3] = (BYTE)(sYDAC);
	if (!SetCmdToSlave(CMD_SET_SW_DAC,pbyData)) 
	{
		return FALSE;
	}
	return TRUE;

}

BOOL CCommandForFUJ::GetXYDAC(int *pXDAC, int *pYDAC)
{
	short sValue;
	if (!SetCmdToSlave(CMD_GET_SW_DAC)) 
	{
		return FALSE;
	}
	sValue = m_pbyReadData[0]<<8;
	sValue += m_pbyReadData[1];
	*pXDAC = sValue;
	sValue = m_pbyReadData[2]<<8;
	sValue += m_pbyReadData[3];
	*pYDAC = sValue;
	return TRUE;

}

BOOL CCommandForFUJ::SetScanStart(int nStartX, int nEndX, int nStartY, int nEndY, int nStep)
{
	BYTE pbyData[10];
    pbyData[0] = (BYTE)(nStartX>>8);
	pbyData[1] = (BYTE)(nStartX);
	pbyData[2] = (BYTE)(nEndX>>8);
	pbyData[3] = (BYTE)(nEndX);
	pbyData[4] = (BYTE)(nStartY>>8);
	pbyData[5] = (BYTE)(nStartY);
	pbyData[6] = (BYTE)(nEndY>>8);
	pbyData[7] = (BYTE)(nEndY);
	pbyData[8] = (BYTE)(nStep>>8);
	pbyData[9] = (BYTE)(nStep);
	if (!SetCmdToSlave(CMD_SW_SCAN_START,pbyData)) 
	{
		return FALSE;
	}
	return TRUE;

}

BOOL CCommandForFUJ::HitlessScan(int nCH, int nTime)
{
	BYTE pbyData[5];
    pbyData[0] = (BYTE)(nCH);
	pbyData[1] = (BYTE)(nTime>>24);
	pbyData[2] = (BYTE)(nTime>>16);
	pbyData[3] = (BYTE)(nTime>>8);
	pbyData[4] = (BYTE)(nTime);
	if (!SetCmdToSlave(CMD_SW_HITLESS_TEST,pbyData)) 
	{
		return FALSE;
	}
	return TRUE;

}

BOOL CCommandForFUJ::SetPort(int nCH)
{
	BYTE pbyData[1];
    pbyData[0] = (BYTE)(nCH);
	if (!SetCmdToSlave(CMD_SET_CHANNEL,pbyData)) 
	{
		return FALSE;
	}
	return TRUE;

}

BOOL CCommandForFUJ::GetPort(int *pnCH)
{
	int nCH;
	if (!SetCmdToSlave(CMD_GET_CHANNEL)) 
	{
		return FALSE;
	}
	nCH = m_pbyReadData[0];
	*pnCH = nCH;
	return TRUE;
}

BOOL CCommandForFUJ::FWUpgrade(CString strBinFile)
{
	BYTE byGetData[32];
	FILE *fp=NULL;
	int  nCount;
	int  nReadCount=32;
	//1.FW Start
	if (!FWStart())
	{
		return FALSE;
	}
	//2.transfer data
	fp = fopen(strBinFile,"rb");
	if (fp==NULL)
	{
		return FALSE;
	}
	while (1)
	{
		nCount = fread(byGetData,1,nReadCount,fp);
		if (nCount==0)
		{
			break;
		}
		if (!UpgradeFWData(byGetData))
		{
			fclose(fp);
			return FALSE;
		}
		if (nCount!=nReadCount)
		{
			break;
		}
		if (nCount>nReadCount)
		{
			fclose(fp);
			return FALSE;
		}
	}
	//3.Arm FW.
	if (!ArmFWUpgrade())
	{
		return FALSE;
	}
	//4.RESET
	if (!SoftwareReset()) 
	{
		return FALSE;
	}
	return TRUE;

}

BOOL CCommandForFUJ::FWStart()
{
	BYTE pbyData[1];
    pbyData[0] = 0X01;
	if (!SetCmdToSlave(CMD_START_FW_UPGRADE,pbyData)) 
	{
		return FALSE;
	}
	return TRUE;
}


BOOL CCommandForFUJ::UpgradeFWData(BYTE *pbyData)
{
	if (!SetCmdToSlave(CMD_ARM_FW_UPGRADE,pbyData)) 
	{
		return FALSE;
	}
	return TRUE;

}

BOOL CCommandForFUJ::ArmFWUpgrade()
{
	BYTE pbyData[1];
    pbyData[0] = 0X01;
	if (!SetCmdToSlave(CMD_ARM_FW_UPGRADE,pbyData)) 
	{
		return FALSE;
	}
	return TRUE;

}

BOOL CCommandForFUJ::SoftwareReset()
{
	BYTE pbyData[1];
    pbyData[0] = 0X01;
	if (!SetCmdToSlave(CMD_SOFTWARE_RESET,pbyData)) 
	{
		return FALSE;
	}
	return TRUE;

}

BOOL CCommandForFUJ::GetProductInfo()
{
	//INFO
	CString strCmd;
	CString strToken;
	char    chSend[256];
	char    chRead[1024];
	char    chSep[] = ":\n\r";
	ZeroMemory(chRead,sizeof(chRead));
	ZeroMemory(chSend,sizeof(chSend));
	strCmd.Format("INFO\r");
	memcpy(chSend,strCmd,strCmd.GetLength());
	if (!WriteBuffer(chSend,strCmd.GetLength()+1)) 
	{
		m_strError = "·¢ËÍINFO´íÎó";
		return FALSE;
	}
	Sleep(50);
	if (!ReadBuffer(chRead,1024))
	{
		m_strError = "½ÓÊÕINFO´íÎó";
		return FALSE;
	}
	strToken = strtok(chRead,chSep);
	strToken = strtok(NULL,chSep);
	strToken.TrimLeft();
	strToken.TrimRight();
	if (strToken.IsEmpty())
	{
		return FALSE;
	}
	m_stModuleInfo.wID = atoi(strToken);
		
	strToken = strtok(NULL,chSep);
	strToken = strtok(NULL,chSep);
	strToken.TrimLeft();
	strToken.TrimRight();
	m_stModuleInfo.strHWVer = (strToken);
	
	strToken = strtok(NULL,chSep);
	strToken = strtok(NULL,chSep);
	strToken.TrimLeft();
	strToken.TrimRight();
	m_stModuleInfo.strFWVer = (strToken);
	
	strToken = strtok(NULL,chSep);
	strToken = strtok(NULL,chSep);
	strToken.TrimLeft();
	strToken.TrimRight();
	m_stModuleInfo.strSN = strToken;
	
	strToken = strtok(NULL,chSep);
	strToken = strtok(NULL,chSep);
	strToken.TrimLeft();
	strToken.TrimRight();
	m_stModuleInfo.strPN = strToken;
	
	strToken = strtok(NULL,chSep);
	strToken = strtok(NULL,chSep);
	strToken.TrimLeft();
	strToken.TrimRight();
	m_stModuleInfo.strMFGDate = strToken;
	strToken = strtok(NULL,chSep);
	strToken = strtok(NULL,chSep);
	strToken.TrimLeft();
	strToken.TrimRight();
	m_stModuleInfo.strCalDate = strToken;
	strToken = strtok(NULL,chSep);
	strToken = strtok(NULL,chSep);
	strToken.TrimLeft();
	strToken.TrimRight();
	m_stModuleInfo.strSwitchType = strToken;
	return TRUE;
}

BOOL CCommandForFUJ::GetTesterCmdInfo()
{
	//m_stCMDInfo¸³Öµ
	CString strFile;
	FILE    *fp=NULL;
	LPCTSTR lpStr=NULL;
	char    pchRead[256];
	CString strToken;
	int     nIndex=0;
	GetCurrentDirectory(256,m_tszAppFolder);
	strFile.Format("%s\\config\\CmdInfoTester.csv", m_tszAppFolder);
	fp = fopen(strFile,"r");
	if (fp==NULL)
	{
		m_strError.Format("´ò¿ªÎÄ¼þ%sÊ§°Ü!",strFile);
		return FALSE;
	}
	ZeroMemory(pchRead,sizeof(pchRead));
	lpStr = fgets(pchRead,256,fp);
	while (1)
	{
		lpStr = fgets(pchRead,256,fp);
		if (lpStr==NULL)
		{
			break;
		}
		strToken = strtok(pchRead,",");
		strToken = strtok(NULL,",");
		m_stTesterCmd[nIndex].strCmd = strToken;
		strToken = strtok(NULL,",");
		m_stTesterCmd[nIndex].bRead = atoi(strToken);
		strToken = strtok(NULL,",");
		m_stTesterCmd[nIndex].byParaCount = atoi(strToken);
		strToken = strtok(NULL,",");
		m_stTesterCmd[nIndex].nGetDataSize = atoi(strToken);
		strToken = strtok(NULL,",");
		ZeroMemory(m_stTesterCmd[nIndex].pchCmdName,1024);
		memcpy(m_stTesterCmd[nIndex].pchCmdName,strToken,strToken.GetLength());
		nIndex++;
		if (nIndex>=256) 
		{
			break;
		}
	}
	m_nTesterCmdCount = nIndex;
	fclose(fp);
	return TRUE;
}

BOOL CCommandForFUJ::SendTesterCmd(int nCmdType, int *pnData)
{
	CString strSend;
	CString strTemp;
	char    chSend[256];
	int     i;
	strSend = m_stTesterCmd[nCmdType].strCmd;
	if (m_stTesterCmd[nCmdType].byParaCount!=0)
	{		
		for (i=0;i<m_stTesterCmd[nCmdType].byParaCount;i++)
		{
			strTemp.Format(" %d",pnData[i]);
			strSend += strTemp;
		}
	}
	strSend = strSend + "\n";
	
	ZeroMemory(chSend,sizeof(chSend));
	ZeroMemory(m_pReadData,sizeof(m_pReadData));
	memcpy(chSend,strSend,strSend.GetLength());
	
	if (!WriteBuffer(chSend,strSend.GetLength())) 
	{
		m_strError.Format("·¢ËÍ%s´íÎó",strSend);
		return FALSE;
	}
	Sleep(50);
	if (!ReadBuffer(m_pReadData,1024))
	{
		m_strError.Format("½ÓÊÕ%s´íÎó",strSend);
		return FALSE;
	}
	return TRUE;
}

BOOL CCommandForFUJ::SetWorkMode(int nMode)
{
	/*
	char chSend[256];
	CString strSend;
	if (!WriteBuffer(chSend,strSend.GetLength())) 
	{
		m_strError.Format("·¢ËÍ%s´íÎó",strSend);
		return FALSE;
	}
	Sleep(50);
	if (!ReadBuffer(m_pReadData,1024))
	{
		m_strError.Format("½ÓÊÕ%s´íÎó",strSend);
		return FALSE;
	}
	*/
	return TRUE;
}

BOOL CCommandForFUJ::GetWorkMode(int *pMode)
{
	return TRUE;
}

BOOL CCommandForFUJ::SendProductScanZone(int nSWIndex, int nMinX, int nMaxX, int nMinY, int nMaxY, int nStep)
{
	CString strCommand;
	int     nIndex=0;
	char    chCommand[256];
	ZeroMemory(chCommand,sizeof(chCommand));
	//·¢ËÍÉ¨ÃèÃüÁî
	strCommand.Format("SCAN %d %d %d %d %d %d\r",nSWIndex,nMinX,nMaxX,nMinY,nMaxY,nStep);
	//LogInfo(strCommand);
	memcpy(chCommand,strCommand,strCommand.GetLength());
	if (!WriteBuffer(chCommand,strCommand.GetLength()))
	{
		m_strError.Format("·¢ËÍÉ¨ÃèÃüÁîÊ§°Ü£¡");
		return FALSE;
	}
	Sleep(100);
	return TRUE;
}

double CCommandForFUJ::GetModuleTemp()
{
	double dblTemp = 0;
	CString strTemp;
	char chData[10];
	char chReData[20];
	
	ZeroMemory(chData,sizeof(char)*10);
	ZeroMemory(chReData,sizeof(char)*20);
	
	strTemp.Format("GTMP\r");
	memcpy(chData,strTemp,strTemp.GetLength());
    if(!WriteBuffer(chData,strTemp.GetLength()))
    {
		m_strError.Format("·¢ËÍÃüÁîÊ§°Ü£¡");
		return -99;
	}
	Sleep(50);
	if (!ReadBuffer(chReData,20))
	{
		m_strError.Format("¶ÁÈ¡´íÎó£¬½ÓÊÕ´íÎó£¡");
		return -99;
	}
    dblTemp = atof(chReData);	
	dblTemp = dblTemp/100.0;
	return dblTemp;

}

int CCommandForFUJ::GetScanState(BYTE btSWIndex)
{
	int nState = 0;
	CString strTemp;
	char chData[10];
	char chReData[20];
	
	ZeroMemory(chData,sizeof(char)*10);
	ZeroMemory(chReData,sizeof(char)*20);
	
	strTemp.Format("SCAN %d\r",btSWIndex);
	memcpy(chData,strTemp,strTemp.GetLength());
    if(!WriteBuffer(chData,strTemp.GetLength()))
    {
		m_strError.Format("·¢ËÍÃüÁîÊ§°Ü£¡");
		return -1;
	}
	Sleep(50);
	if (!ReadBuffer(chReData,20))
	{
		m_strError.Format("¶ÁÈ¡´íÎó£¬½ÓÊÕ´íÎó£¡");
		return -1;
	}
    nState = atoi(chReData);	
	return nState;

}

BOOL CCommandForFUJ::SetTesterXYDAC(int nSWIndex,int x, int y)
{
	CString strCommand;
	int     nIndex=0;
	char    chCommand[256];
	ZeroMemory(chCommand,sizeof(chCommand));
	strCommand.Format("SETXY %d %d %d\r",nSWIndex,x,y);
	memcpy(chCommand,strCommand,strCommand.GetLength());
	if (!WriteBuffer(chCommand,strCommand.GetLength()))
	{
		m_strError.Format("·¢ËÍÉ¨ÃèÃüÁîÊ§°Ü£¡");
		return FALSE;
	}
	Sleep(100);
	return TRUE;
}

BOOL CCommandForFUJ::SetSWChannel(int nSWIndex,int nCHIndex)
{
	CString strCommand;
	int     nIndex=0;
	char	chReData[40];
	char    chCommand[256];
	ZeroMemory(chCommand,sizeof(chCommand));
	ZeroMemory(chReData,sizeof(char)*40);
	strCommand.Format("SWCH %d %d\r",nSWIndex,nCHIndex);
	memcpy(chCommand,strCommand,strCommand.GetLength());
	if (!WriteBuffer(chCommand,strCommand.GetLength()))
	{
		m_strError.Format("·¢ËÍÉ¨ÃèÃüÁîÊ§°Ü£¡");
		return FALSE;
	}
	Sleep(100);
	if (!ReadBuffer(chReData,40))
	{
		m_strError.Format("¿ª¹ØÇÐ»»´íÎó£¬½ÓÊÕ´íÎó£¡");
		return FALSE;
	}
	return TRUE;
}

BOOL CCommandForFUJ::GetMemoryData(DWORD dwAddress,char *pchData)
{
	int nState = 0;
	CString strTemp;
	char chData[32];
	char chReData[32];
	
	ZeroMemory(chData,sizeof(char)*32);
	ZeroMemory(chReData,sizeof(char)*32);
	
	strTemp.Format("MEM %d\r",dwAddress);
	memcpy(chData,strTemp,strTemp.GetLength());
    if(!WriteBuffer(chData,strTemp.GetLength()))
    {
		m_strError.Format("·¢ËÍÉ¨ÃèÃüÁîÊ§°Ü£¡");
		return -99;
	}
	Sleep(50);
	if (!ReadBuffer(chReData,32))
	{
		m_strError.Format("½ÓÊÕ´íÎó£¡");
		return -99;
	}
    memcpy(pchData,chReData,32);	
	return nState;

}

