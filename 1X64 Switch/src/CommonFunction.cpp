//CommonFunction.cpp
//Created by xiaoyong wang
//Date: 2006-3-16 


#include "stdafx.h"
#include "CommonFunction.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


CCommonFunction::CCommonFunction()
{
	m_dwMaxMessageLine = 1000;
}

CCommonFunction::~CCommonFunction()
{

}


BYTE CCommonFunction::HighByte(WORD wVar)
{

	wVar &= 0xFF00;
	wVar = wVar >> 8;

	return (BYTE)wVar;
}

BYTE CCommonFunction::LowByte(WORD wVar)
{

	wVar &= 0x00FF;

	return (BYTE)wVar;
}

WORD CCommonFunction::HighWord(DWORD dwVar)
{

	dwVar &= 0xFFFF0000;
	dwVar = dwVar >> 16;

	return (WORD)dwVar;
}

WORD CCommonFunction::LowWord(DWORD dwVar)
{
	dwVar &= 0x0000FFFF;

	return (WORD)dwVar;
}

WORD CCommonFunction::MergeToWord(BYTE btHighByte, BYTE btLowByte)
{
	WORD		wFinalWord;

	wFinalWord = 0;

	wFinalWord = wFinalWord + btHighByte;
	wFinalWord = (wFinalWord << 8) + btLowByte;

	return wFinalWord;
}

WORD CCommonFunction::MergeToWord(CString strHighByte, CString strLowByte)
{
	WORD		wFinalWord;

	wFinalWord = 0;

	wFinalWord = wFinalWord + (BYTE)strtol(strHighByte,	NULL, 16);
	wFinalWord = (wFinalWord << 8) + (BYTE)strtol(strLowByte,	NULL, 16);

	return wFinalWord;
}

DWORD CCommonFunction::MergeToDoubleWord(BYTE btFirstByte, BYTE btSecondByte, BYTE btThirdByte, BYTE btFourthByte)
{
	DWORD		dwFinalWord;

	dwFinalWord = 0;

	dwFinalWord = dwFinalWord + btFirstByte;
	dwFinalWord = (dwFinalWord << 8) + btSecondByte;
	dwFinalWord = (dwFinalWord << 8) + btThirdByte;
	dwFinalWord = (dwFinalWord << 8) + btFourthByte;

	return dwFinalWord;
}

DWORD CCommonFunction::MergeToDoubleWord(CString strFirst, CString strSecond, CString strThird, CString strFourth)
{
	DWORD		dwFinalWord;

	dwFinalWord = 0;

	dwFinalWord = dwFinalWord + (BYTE)strtol(strFirst,	NULL, 16);
	dwFinalWord = (dwFinalWord << 8) + (BYTE)strtol(strFirst, NULL, 16);
	dwFinalWord = (dwFinalWord << 8) + (BYTE)strtol(strFirst, NULL, 16);
	dwFinalWord = (dwFinalWord << 8) + (BYTE)strtol(strFirst, NULL, 16);

	return dwFinalWord;
}

int CCommonFunction::Contain(char * pchStrBuffer, const double dblPowerValue)
{
	char		pchSeps[] = " ~^*";
	char		*token = NULL;
	int			iPos = 0;

	iPos = 0;
	token = strtok(pchStrBuffer, pchSeps);
	while (NULL != token)
	{
		// compare two value
		if (dblPowerValue == strtod(token, NULL))
			return iPos;
		token = strtok(NULL, pchSeps);
		iPos ++;
	}

	return -1;
}

// Get current time
CString CCommonFunction::GetCurrentTime()
{
	CString		strCurTime;
	int			iYear,
				iMonth,
				iDay,
				iHour,
				iMinute,
				iSecond;
	CTime		tmDateTime(CTime::GetCurrentTime());

	iYear	= tmDateTime.GetYear();
	iMonth	= tmDateTime.GetMonth();
	iDay	= tmDateTime.GetDay();
	iHour	= tmDateTime.GetHour();
	iMinute	= tmDateTime.GetMinute();
	iSecond	= tmDateTime.GetSecond();

	strCurTime.Format("%d-%d-%d %02d:%02d:%02d", iYear, iMonth, iDay, iHour, iMinute, iSecond);

	return strCurTime;
}

VOID CCommonFunction::SetMaxMessageLine(DWORD dwMaxMessageLine)
{
	m_dwMaxMessageLine = dwMaxMessageLine;
}

BOOL CCommonFunction::ConvertToHexString(CString strInputString,
										int iDataType,
										int iDataSize,
										CString &strHexString,
										int iAlignType,
										int iFilledCharInValue,
										BOOL bNeedSeparate,
										BOOL bGetASCII)
{
	// Define some var
	CString		strTemp;
	TCHAR		tchASCIIString[512],
				chSepChar = ' ';
	int			i = 0, iStrLen;
	BYTE		btYearHigh = 0, btYearLow = 0, btMonth = 0, btDay = 0;
	WORD		wValue = 0;
	long		lValue = 0;
	double		dblValue = 0;

	// Initialiation
	// Clear the strHexString
	strHexString.Empty();
	if (0 == iDataSize)
		return FALSE;
	switch (iDataType)
	{
		// Process byte type
	case DATA_TYPE_BYTE:
		strInputString.MakeLower();
		if (strInputString.Left(2) == "0x")
			lValue = strtol(strInputString , NULL, 16);
		else
			lValue = strtol(strInputString , NULL, 10);
		strHexString.Format("%02x", lValue);
		break;
		// Process checksum type
	case DATA_TYPE_CHECKSUM:
		// Process word type
	case DATA_TYPE_WORD:
		lValue = strtol(strInputString , NULL, 10);
		strHexString.Format("%04x", lValue);
		break;
		// Process dword type
	case DATA_TYPE_DWORD:
		lValue = strtol(strInputString , NULL, 10);
		strHexString.Format("%08x", lValue);
		break;
		// Process double type
	case DATA_TYPE_DOUBLE:
		dblValue = strtod(strInputString, NULL);
		dblValue = dblValue * DATA_SCALE;
		strHexString.Format("%x", dblValue);
		break;
		// Process number type
	case DATA_TYPE_NUMBER:
		// Process string type
	case DATA_TYPE_ASCII:
		for (i = 0;i < iDataSize;i ++)
			tchASCIIString[i] = iFilledCharInValue;
		tchASCIIString[i] = 0x00;

		iStrLen = strInputString.GetLength();
		if (iStrLen != 0)
		{
			if (DATA_RIGHT_ALIGN == iAlignType)
				sprintf(tchASCIIString + (iDataSize - iStrLen), "%s", strInputString);
			else
			{
				sprintf(tchASCIIString, "%s", strInputString);
				tchASCIIString[iStrLen] = iFilledCharInValue;
			}
		}

		for (i = 0;i < iDataSize;i ++)
		{
			if (bGetASCII)
			{
				if (!bNeedSeparate)
					strTemp.Format("%02x", (BYTE)tchASCIIString[i]);
				else
					strTemp.Format("%c%02x", chSepChar, (BYTE)tchASCIIString[i]);
			}
			else
			{
				if (!bNeedSeparate)
					strTemp.Format("%c", (BYTE)tchASCIIString[i]);
				else
					strTemp.Format("%c%c", chSepChar, (BYTE)tchASCIIString[i]);
			}
			strHexString = strHexString + strTemp;
		}
		if (!strHexString.IsEmpty())
			if (bNeedSeparate && strHexString[0] == ' ')
				strHexString.Delete(0);
		break;
		// Process date type
	case DATA_TYPE_DATE:
		// Get year
		if (strInputString.GetLength() >= 4)
		{
			wValue = (WORD)strtol(strInputString.Left(4), NULL, 10);
			btYearLow = (BYTE)wValue;
			btYearHigh = (BYTE)(wValue >> 8);
			strInputString.Delete(0, 4);
		}
		// Get month
		if (strInputString.GetLength() >= 2)
		{
			wValue = (WORD)strtol(strInputString.Left(2), NULL, 10);
			btMonth = (BYTE)wValue;
			strInputString.Delete(0, 2);
		}
		// Get day
		if (strInputString.GetLength() >= 2)
		{
			wValue = (WORD)strtol(strInputString.Left(2), NULL, 10);
			btDay = (BYTE)wValue;
		}
		strHexString.Format("%02x %02x %02x %02x",
							btYearHigh,
							btYearLow,
							btMonth,
							btDay);
		break;
		// Process version type
	case DATA_TYPE_VERSION:
		break;
	case DATA_TYPE_DWORD_COMBINED:
	case DATA_TYPE_FOUR_WORD_COMBINED:
	case DATA_TYPE_SIX_WORD_COMBINED:
	case DATA_TYPE_SIXTEEN_WORD_COMBINED:
		break;
	default:
		break;
	}

	return TRUE;
}

BOOL CCommonFunction::HexStrToEyeableStr(BYTE * pchByteArray,
										 int iDataType,
										 int iDataSize,
										 CString &strEyeableString)
{
	// Define some var
	CString		strTemp;
	int			i = 0;
	long		lValue = 0;
	double		dblValue = 0;

	// Initialiation
	// Clear the strEyeableString
	strEyeableString.Empty();
	if (iDataSize <= 0)
		return FALSE;

	switch (iDataType)
	{
		// Process string type
	case DATA_TYPE_ASCII:
		for (i = 0;i < iDataSize;i ++)
		{
			strTemp.Format("%c", pchByteArray[i]);
			strEyeableString += strTemp;
		}
		break;
		// Process byte type
	case DATA_TYPE_BYTE:
//		break;
		// Process word type
	case DATA_TYPE_WORD:
//		break;
		// Process checksum type
	case DATA_TYPE_CHECKSUM:
		// Process dword type
	case DATA_TYPE_DWORD:
		for (i = 0;i < iDataSize;i ++)
		{
			lValue = lValue << 8;
			lValue |= pchByteArray[i];
		}
		strEyeableString.Format("%d", lValue);
		break;
		// Process double type
	case DATA_TYPE_DOUBLE:
		for (i = 0;i < iDataSize;i ++)
		{
			lValue = lValue << 8;
			lValue |= pchByteArray[i];
		}
		dblValue = (double)lValue / DATA_SCALE;
		strEyeableString.Format("%.2f", dblValue);
		break;
		// Process number type
	case DATA_TYPE_NUMBER:
		for (i = 0;i < iDataSize;i ++)
		{
			strTemp.Format("%c", pchByteArray[i]);
			strEyeableString += strTemp;
		}
		break;
		// Process date type
	case DATA_TYPE_DATE:
		break;
		// Process version type
	case DATA_TYPE_VERSION:
		break;
		// Process special type: DATA_TYPE_SLOPE_INTERCEPT
	case DATA_TYPE_DWORD_COMBINED:
	case DATA_TYPE_FOUR_WORD_COMBINED:
	case DATA_TYPE_SIX_WORD_COMBINED:
	case DATA_TYPE_SIXTEEN_WORD_COMBINED:
		break;
	default:
		break;
	}
	return TRUE;
}

WORD CCommonFunction::GetCmdTypeWord(CString strCmdType)
{
	WORD		wCmdType;

	if (!strcmp("一般命令", strCmdType))
	{
		wCmdType = CMD_COMMON;
	}else if (!strcmp("MFG命令", strCmdType))
	{
		wCmdType = CMD_MFG;
	}else if (!strcmp("MFG信息命令", strCmdType))
	{
		wCmdType = CMD_MFG_INFO;
	}else if (!strcmp("出厂设置命令", strCmdType))
	{
		wCmdType = CMD_DELIVERY;
	}

	return wCmdType;
}

CString CCommonFunction::GetCmdTypeString(WORD wCmdType)
{
	CString			strResultString;
	CHAR			pCmdTypeString[16];

	ZeroMemory(pCmdTypeString, 16 * sizeof(CHAR));
	switch (wCmdType)
	{
	case CMD_COMMON:
		strcpy(pCmdTypeString, "一般命令");
		break;
	case CMD_MFG:
		strcpy(pCmdTypeString, "MFG命令");
		break;
	case CMD_MFG_INFO:
		strcpy(pCmdTypeString, "MFG信息命令");
		break;
	case CMD_DELIVERY:
		strcpy(pCmdTypeString, "出厂设置命令");
		break;
	default:
		break;
	}
	strResultString.Format("%s", pCmdTypeString);

	return strResultString;
}

BYTE CCommonFunction::GetUpgradeFileTypeByte(CString strFileType)
{
	BYTE	btFileType;

	if (!strcmp("NONE", strFileType))
	{
		btFileType = UFT_NONE;
	}else if (!strcmp("BOOT_LOADER", strFileType))
	{
		btFileType = UFT_BOOT_LOADER;
	}else if (!strcmp("FIRMWARE_CODE", strFileType))
	{
		btFileType = UFT_FIRMWARE_CODE;
	}else if (!strcmp("DAC_ASE", strFileType))
	{
		btFileType = UFT_DAC_ASE;
	}else if (!strcmp("TO_EEPROM", strFileType))
	{
		btFileType = UFT_TO_EEPROM;
	}

	return btFileType;
}

WORD CCommonFunction::GetParamAlterabilityWord(CString strParamAlterability)
{
	WORD		wParamAlterability;

	if (!strcmp("可变", strParamAlterability))
	{
		wParamAlterability = PARAM_ALTERABLE;
	}else if (!strcmp("固定", strParamAlterability))
	{
		wParamAlterability = PARAM_FIXED;
	}

	return wParamAlterability;
}

CString CCommonFunction::GetParamAlterabilityString(WORD wParamAlterability)
{
	CString			strResultString;
	CHAR			pParamAlterabilityString[16];

	ZeroMemory(pParamAlterabilityString, 16 * sizeof(CHAR));
	switch (wParamAlterability)
	{
	case PARAM_ALTERABLE:
		strcpy(pParamAlterabilityString, "可变");
		break;
	case PARAM_FIXED:
		strcpy(pParamAlterabilityString, "固定");
		break;
	default:
		break;
	}
	strResultString.Format("%s", pParamAlterabilityString);

	return strResultString;
}

WORD CCommonFunction::GetDataTypeWord(CString strDataType)
{
	WORD		wDataType;

	if (!strcmp("ASCII", strDataType))
	{
		wDataType = DATA_TYPE_ASCII;
	}else if (!strcmp("字节", strDataType))
	{
		wDataType = DATA_TYPE_BYTE;
	}else if (!strcmp("字", strDataType))
	{
		wDataType = DATA_TYPE_WORD;
	}else if (!strcmp("双字", strDataType))
	{
		wDataType = DATA_TYPE_DWORD;
	}else if (!strcmp("小数", strDataType))
	{
		wDataType = DATA_TYPE_DOUBLE;
	}else if (!strcmp("数字", strDataType))
	{
		wDataType = DATA_TYPE_NUMBER;
	}else if (!strcmp("日期", strDataType))
	{
		wDataType = DATA_TYPE_DATE;
	}else if (!strcmp("版本", strDataType))
	{
		wDataType = DATA_TYPE_VERSION;
	}else if (!strcmp("双字组合", strDataType))
	{
		wDataType = DATA_TYPE_DWORD_COMBINED;
	}else if (!strcmp("四字组合", strDataType))
	{
		wDataType = DATA_TYPE_FOUR_WORD_COMBINED;
	}else if (!strcmp("六字组合", strDataType))
	{
		wDataType = DATA_TYPE_SIX_WORD_COMBINED;
	}else if (!strcmp("十六双字", strDataType))
	{
		wDataType = DATA_TYPE_SIXTEEN_WORD_COMBINED;
	}else if (!strcmp("校验和", strDataType))
	{
		wDataType = DATA_TYPE_CHECKSUM;
	}

	return wDataType;
}

CString CCommonFunction::GetDataTypeString(WORD wDataType)
{
	CString			strResultString;
	CHAR			pDataTypeString[16];

	ZeroMemory(pDataTypeString, 16 * sizeof(CHAR));
	switch (wDataType)
	{
	case DATA_TYPE_ASCII:
		strcpy(pDataTypeString, "ASCII");
		break;
	case DATA_TYPE_BYTE:
		strcpy(pDataTypeString, "字节");
		break;
	case DATA_TYPE_WORD:
		strcpy(pDataTypeString, "字");
		break;
	case DATA_TYPE_DWORD:
		strcpy(pDataTypeString, "双字");
		break;
	case DATA_TYPE_DOUBLE:
		strcpy(pDataTypeString, "小数");
		break;
	case DATA_TYPE_NUMBER:
		strcpy(pDataTypeString, "数字");
		break;
	case DATA_TYPE_DATE:
		strcpy(pDataTypeString, "日期");
		break;
	case DATA_TYPE_VERSION:
		strcpy(pDataTypeString, "版本");
		break;
	case DATA_TYPE_DWORD_COMBINED:
		strcpy(pDataTypeString, "双字组合");
		break;
	case DATA_TYPE_FOUR_WORD_COMBINED:
		strcpy(pDataTypeString, "四字组合");
		break;
	case DATA_TYPE_SIX_WORD_COMBINED:
		strcpy(pDataTypeString, "六字组合");
		break;
	case DATA_TYPE_SIXTEEN_WORD_COMBINED:
		strcpy(pDataTypeString, "十六双字");
		break;
	case DATA_TYPE_CHECKSUM:
		strcpy(pDataTypeString, "校验和");
		break;
	default:
		break;
	}
	strResultString.Format("%s", pDataTypeString);

	return strResultString;
}

WORD CCommonFunction::GetAccessTypeWord(CString strAccessType)
{
	WORD		wAccessType;

	strAccessType.MakeLower();

	if (!strcmp("r", strAccessType))
	{
		wAccessType = READ_ONLY;
	}else if (!strcmp("w", strAccessType))
	{
		wAccessType = WRITE_ONLY;
	}else if (!strcmp("r/w", strAccessType))
	{
		wAccessType = WRITE_READ;
	}else if (!strcmp("r\\w", strAccessType))
	{
		wAccessType = WRITE_READ;
	}

	return wAccessType;
}

CString CCommonFunction::GetAccessTypeString(WORD wAccessType)
{
	CString			strResultString;
	CHAR			pAccessTypeString[10];

	ZeroMemory(pAccessTypeString, 8 * sizeof(CHAR));
	switch (wAccessType)
	{
	case READ_ONLY:
		strcpy(pAccessTypeString, "R");
		break;
	case WRITE_ONLY:
		strcpy(pAccessTypeString, "W");
		break;
	case WRITE_READ:
		strcpy(pAccessTypeString, "R/W");
		break;
	default:
		break;
	}
	strResultString.Format("%s", pAccessTypeString);

	return strResultString;
}

CString CCommonFunction::ConvertToStandardLength(CString strInputStr, WORD wDataType)
{
	CString			strTargetStr;
	CHAR			pstrTargetStr[MAX_LENGTH];
	WORD			wValue;
	DWORD			dwValue;
	BYTE			btValue;

	ZeroMemory(pstrTargetStr, MAX_LENGTH * sizeof(CHAR));
	switch (wDataType)
	{
	case DATA_TYPE_ASCII:
		strcpy(pstrTargetStr, strInputStr);
		strTargetStr.Format("%s", pstrTargetStr);
		break;
	case DATA_TYPE_BYTE:
		btValue = (BYTE)strtol(strInputStr, NULL, 16);
		strTargetStr.Format("%02x", btValue);
		break;
	case DATA_TYPE_WORD:
		wValue = (WORD)strtol(strInputStr, NULL, 16);
		strTargetStr.Format("%04x", wValue);
		break;
	case DATA_TYPE_DWORD:
		dwValue = strtol(strInputStr, NULL, 16);
		strTargetStr.Format("%08x", dwValue);
		break;
	case DATA_TYPE_DOUBLE:
		strcpy(pstrTargetStr, strInputStr);
		strTargetStr.Format("%04s", pstrTargetStr);
		break;
	case DATA_TYPE_NUMBER:
		strcpy(pstrTargetStr, strInputStr);
		strTargetStr.Format("%s", pstrTargetStr);
		break;
	case DATA_TYPE_DATE:
		strcpy(pstrTargetStr, strInputStr);
		strTargetStr.Format("%s", pstrTargetStr);
		break;
	case DATA_TYPE_VERSION:
		strcpy(pstrTargetStr, strInputStr);
		strTargetStr.Format("%s", pstrTargetStr);
		break;
	case DATA_TYPE_DWORD_COMBINED:
	case DATA_TYPE_FOUR_WORD_COMBINED:
	case DATA_TYPE_SIX_WORD_COMBINED:
	case DATA_TYPE_SIXTEEN_WORD_COMBINED:
		strcpy(pstrTargetStr, strInputStr);
		strTargetStr.Format("%s", pstrTargetStr);
		break;
	default:
		break;
	}

	return strTargetStr;
}

CString CCommonFunction::ParseStatus(int iStatus)
{
	CString strReturnMsg("");

	switch (iStatus)
	{
	case DOWNLOAD_SUCCESS:
		strReturnMsg.Format("下载代码成功！");
		break;
	case FIRMWARE_BUSY:
		strReturnMsg.Format("Firmware忙！");
		break;
	case COMMUNICATION_FAILURE:
		strReturnMsg.Format("通讯失败！");
		break;
	case NOT_ENOUGH_FIRMWARE_DATA:
		strReturnMsg.Format("Firmware数据不足！");
		break;
	case FILE_SIZE_DOESNOT_MATCH:
		strReturnMsg.Format("文件大小不匹配！");
		break;
	case CRC32_DOESNOT_MATCH:
		strReturnMsg.Format("CRC32不匹配！");
		break;
	case FLASH_PROGRAMMING_ERROR:
		strReturnMsg.Format("写入Flash失败！");
		break;
	case EEPROM_PROGRAMMING_ERROR:
		strReturnMsg.Format("写入EEPROM失败！");
		break;
	case USER_ABORT_UPGRADE:
		strReturnMsg.Format("用户取消升级！");
		break;
	case CANNOT_OPEN_BIN_FILE:
		strReturnMsg.Format("不能打开Bin文件！");
		break;
	default:
//		strReturnMsg.Format("软件升级错误！");
		break;
	}

	return strReturnMsg;
}

//*****************************************************************************************
// Function Name: YieldToPeers
// Function : Pump the message from the message queue,
// but not remove the message now.
// input: none
// output: none
// return type: void
//*****************************************************************************************
VOID CCommonFunction::YieldToPeers()
{
	MSG	msg;
	while(::PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
	{
		if(!AfxGetThread()->PumpMessage())
			break;
	}
}

#if 0
// ** Note: not use this function
//x[]是x坐标数组，y[]是y坐标数组，n是数据点个数，a[]拟合结果，即多项式的系数数组；   
//m为拟合阶数，将M设成2就是直线
void CCommonFunction::iapcir(double x[], double y[], int n, double a[], int m, double dt[])
{
	int   i,j,k;   
	double   z,p,c,g,q,d1,d2,s[20],t[20],b[20];
	
	for   (i=0;   i<=m-1;   i++)   a[i]=0.0;   
	if   (m>n)   m=n;   
	if   (m>20)   m=20;   
	z=0.0;   
	for   (i=0;   i<=n-1;   i++)   z=z+x[i]/(1.0*n);   
	b[0]=1.0;   d1=1.0*n;   p=0.0;   c=0.0;   
	for   (i=0;   i<=n-1;   i++)   
	  {   p=p+(x[i]-z);   c=c+y[i];}   
	c=c/d1;   p=p/d1;   
	a[0]=c*b[0];   
	if   (m>1)   
      {   t[1]=1.0;   t[0]=-p;   
          d2=0.0;   c=0.0;   g=0.0;   
          for   (i=0;   i<=n-1;   i++)   
              {   q=x[i]-z-p;   d2=d2+q*q;   
                  c=c+y[i]*q;   
                  g=g+(x[i]-z)*q*q;   
              }   
          c=c/d2;   p=g/d2;   q=d2/d1;   
          d1=d2;   
          a[1]=c*t[1];   a[0]=c*t[0]+a[0];   
      }   
  for   (j=2;   j<=m-1;   j++)   
      {   s[j]=t[j-1];   
          s[j-1]=-p*t[j-1]+t[j-2];   
          if   (j>=3)   
              for   (k=j-2;   k>=1;   k--)   
                  s[k]=-p*t[k]+t[k-1]-q*b[k];   
          s[0]=-p*t[0]-q*b[0];   
          d2=0.0;   c=0.0;   g=0.0;   
          for   (i=0;   i<=n-1;   i++)   
              {   q=s[j];   
                  for   (k=j-1;   k>=0;   k--)   
                      q=q*(x[i]-z)+s[k];   
                  d2=d2+q*q;   c=c+y[i]*q;   
                  g=g+(x[i]-z)*q*q;   
              }   
          c=c/d2;   p=g/d2;   q=d2/d1;   
          d1=d2;   
          a[j]=c*s[j];   t[j]=s[j];   
          for   (k=j-1;   k>=0;   k--)   
              {   a[k]=c*s[k]+a[k];   
                  b[k]=t[k];   t[k]=s[k];   
              }   
      }   
  dt[0]=0.0;   dt[1]=0.0;   dt[2]=0.0;   
  for   (i=0;   i<=n-1;   i++)   
      {   q=a[m-1];   
          for   (k=m-2;   k>=0;   k--)   
              q=a[k]+q*(x[i]-z);   
          p=q-y[i];   
          if   (fabs(p)>dt[2])   dt[2]=fabs(p);   
          dt[0]=dt[0]+p*p;   
          dt[1]=dt[1]+fabs(p);   
      }   
  return;   
} 

// ** Note: not use this function

void CCommonFunction::CalculateCurveParameter(CDoubleArray * X, CDoubleArray * Y, CDoubleArray * a, int M, int N)
{
//X,Y   --     X,Y两轴的坐标   
//M       --     结果变量组数   
//N       --     采样数目   
//a       --     结果参数   

int   i,   j,   k;   
double   Z,   D1,   D2,   C,   P,   G,   Q;   
CDoubleArray   B,   T,   S;   
B.SetSize(N);   
T.SetSize(N);   
S.SetSize(N);   
if(M   >   N)   M   =   N;   

for(i   =   0;   i   <   M;   i++)   (*a)[i]=0;   
Z   =   0;   
B[0]   =   1;   
D1   =   N;   
P   =   0;   
C   =   0;   

for(i   =   0;   i   <   N;   i++)   
{   
P   =   P   +   (*X)[i]   -   Z;   
C   =   C   +   (*Y)[i];   
}   

C   =   C   /   D1;   
P   =   P   /   D1;   
(*a)[0]   =   C   *   B[0];   

if(M>1)   
{   
T[1]   =   1;   
T[0]   =   -P;   
D2   =   0;   
C   =   0;   
G   =   0;   
for(i   =   0;   i   <   N;   i++)   
{   
Q   =   (*X)[i]   -   Z   -   P;   
D2   =   D2   +   Q   *   Q;   
C   =   (*Y)[i]   *   Q   +   C;   
G   =   ((*X)[i]-Z)   *   Q   *   Q   +   G;   
}   

C   =   C   /   D2;   
P   =   G   /   D2;   
Q   =   D2   /   D1;   
D1   =   D2;   
(*a)[1]   =   C   *   T[1];   
(*a)[0]   =   C*   T[0]   +   (*a)[0];   
}   

for(j   =   2;   j   <   M;   j++)   
{   
S[j]   =   T[j-1];   
S[j-1]   =   -P   *   T[j-1]   +   T[j-2];   
if(j   >=   3)   
{   
for(k   =   j   -   2;   k   >=   1;   k--)   
S[k]   =   -P   *   T[k]   +   T[k-1]   -   Q   *   B[k];   
}   

S[0]   =   -P   *   T[0]   -   Q   *   B[0];   
D2   =   0;   
C   =   0;   
G   =   0;   

for(i   =   0;   i   <   N;   i++)   
{   
Q   =   S[j];   
for(k   =   j   -   1;   k   >=   0;   k--)   
Q   =   Q   *((*X)[i]-Z)   +   S[k];   
D2   =D2   +   Q   *   Q;   
C   =   (*Y)[i]   *   Q   +   C;   
G   =   ((*X)[i]   -   Z)   *   Q   *   Q   +   G;   
}   

C   =C   /   D2;   
P   =G   /   D2;   
Q   =D2   /   D1;   
D1   =D2;   
(*a)[j]   =   C   *   S[j];   
T[j]   =   S[j];   

for(k   =   j   -   1;   k   >=   0;   k--)   
{   
(*a)[k]   =   C   *   S[k]   +   (*a)[k];   
B[k]   =   T[k];   
T[k]   =   S[k];   
}   
}   
}

// ** Note: not use this function
BOOL CCommonFunction::CalculateCurveParameter(CDoubleArray *X,CDoubleArray *Y,long M,long N,CDoubleArray *A)
{
 //X,Y --  X,Y两轴的坐标
 //M   --  结果变量组数
 //N   --  采样数目
 //A   --  结果参数

 reGISter long i,j,k;
 double Z,D1,D2,C,P,G,Q;
 CDoubleArray B,T,S;
 B.SetSize(N);
 T.SetSize(N);
 S.SetSize(N);
 if(M>N)M=N;
 for(i=0;i<M;i++)
  (*A)[i]=0;
 Z=0;
 B[0]=1;
 D1=N;
 P=0;
 C=0;
 for(i=0;i<N;i++)
 {
  P=P+(*X)[i]-Z;
  C=C+(*Y)[i];
 }
 C=C/D1;
 P=P/D1;
 (*A)[0]=C*B[0];
 if(M>1)
 {
  T[1]=1;
  T[0]=-P;
  D2=0;
  C=0;
  G=0;
  for(i=0;i<N;i++)
  {
   Q=(*X)[i]-Z-P;
   D2=D2+Q*Q;
   C=(*Y)[i]*Q+C;
   G=((*X)[i]-Z)*Q*Q+G;
  }
  C=C/D2;
  P=G/D2;
  Q=D2/D1;
  D1=D2;
  (*A)[1]=C*T[1];
  (*A)[0]=C*T[0]+(*A)[0];
 }
 for(j=2;j<M;j++)
 {
  S[j]=T[j-1];
  S[j-1]=-P*T[j-1]+T[j-2];
  if(j>=3)
  {
   for(k=j-2;k>=1;k--)
    S[k]=-P*T[k]+T[k-1]-Q*B[k];
  }
  S[0]=-P*T[0]-Q*B[0];
  D2=0;
  C=0;
  G=0;
  for(i=0;i<N;i++)
  {
   Q=S[j];
   for(k=j-1;k>=0;k--)
    Q=Q*((*X)[i]-Z)+S[k];
   D2=D2+Q*Q;
   C=(*Y)[i]*Q+C;
   G=((*X)[i]-Z)*Q*Q+G;
  }
  C=C/D2;
  P=G/D2;
  Q=D2/D1;
  D1=D2;
  (*A)[j]=C*S[j];
  T[j]=S[j];
  for(k=j-1;k>=0;k--)
  {
   (*A)[k]=C*S[k]+(*A)[k];
   B[k]=T[k];
   T[k]=S[k];
  }
 }
 return TRUE;
}

// ** Note: not use this function
void CCommonFunction::pcir(double X[], double Y[], double A[], int N, int M, double &DT1, double &DT2, double &DT3)
{
	// X[] Y[] 使需要拟合的点，A[]时拟合曲线的系数，
	double S[21],T[21],B[21];
	double Z,D1,P,C,D2,G,Q,DT;
	int i,j,jk;
    for(i=1;i<=M;i++)//	DO 5 i=1,M
	{
		A[i]=0.0;
	}

	if (M>=N) M=N;
	if (M>=20) M=20;
	Z=0.0;
/*	for(i=1;i<=N;i++)//DO 10 i=1,N
	{
		Z=Z+X[i]/N;
	}//10	Z=Z+X(i)/N*/
	B[1]=1.0;
	D1=N;
	P=0.0;
	C=0.0;
	for(i=1;i<=N;i++)
	{
		P=P+(X[i]-Z);
		C=C+Y[i];
	}
	C=C/D1;
	P=P/D1;
	A[1]=C*B[1];
	if(M>1)
	{
	  T[2]=1.0;
	  T[1]=-P;
	  D2=0.0;
	  C=0.0;
	  G=0.0;
	  for(i=1;i<=N;i++)
	  {
		Q=X[i]-Z-P;
	    D2=D2+Q*Q;
	    C=Y[i]*Q+C;
	    G=(X[i]-Z)*Q*Q+G;
	  }

	  C=C/D2;
	  P=G/D2;
	  Q=D2/D1;
	  D1=D2;
	  A[2]=C*T[2];
	  A[1]=C*T[1]+A[1];
	}
	for(j=3;j<=M;j++)
	{
	  S[j]=T[j-1];
	  S[j-1]=-P*T[j-1]+T[j-2];
	  if(j>=4)
		  for(jk=j-2;jk>=2;jk--)
            S[jk]=-P*T[jk]+T[jk-1]-Q*B[jk];
	  S[1]=-P*T[1]-Q*B[1];
	  D2=0.0;
	  C=0.0;
	  G=0.0;
	  for(i=1;i<=N;i++)
	  {
	    Q=S[j];
		for(jk=j-1;jk>=1;jk--)
		{    
			Q=Q*(X[i]-Z)+S[jk];
		}
	    D2=D2+Q*Q;
	    C=Y[i]*Q+C;
	    G=(X[i]-Z)*Q*Q+G;
	  }
	  C=C/D2;
	  P=G/D2;
	  Q=D2/D1;
	  D1=D2;
	  A[j]=C*S[j];
	  T[j]=S[j];
	  for(jk=j-1;jk>=1;jk--)
	  {
	    A[jk]=C*S[jk]+A[jk];
	    B[jk]=T[jk];
	    T[jk]=S[jk];
	  }
	}
	DT1=0.0;
	DT2=0.0;
	DT3=0.0;
	
	for(i=1;i<=N;i++)
	{
	  Q=A[M];
	  for(jk=M-1;jk>=1;jk--)
	  {
		  Q=Q*(X[i]-Z)+A[jk];
	  }
	  DT=Q-Y[i];
	  if (fabs(DT)>DT3) DT3=fabs(DT);
	  DT1=DT1+DT*DT;
	  DT2=DT2+fabs(DT);
	}
}
#endif

VOID CCommonFunction::ShowInfo(CListBox& pListBox, LPCTSTR tszlogMsg, BOOL bAppend)
{
//	int			dx=0;
//	CDC	*		pDC;
	DWORD		dwLineIndex;
//	CSize		sz;

//	pDC			= pListBox.GetDC();
	dwLineIndex = pListBox.GetCount();

	// if the lines is more than the value user set
	// clear the message here
	if(dwLineIndex >= m_dwMaxMessageLine)
	{
		pListBox.ResetContent();
		dwLineIndex = 0;
	}

	if (bAppend)
	{
		pListBox.AddString(tszlogMsg);
		// Scroll the list message to the bottom
		pListBox.SetTopIndex(dwLineIndex);
	}
	else
		pListBox.InsertString(0, tszlogMsg);
#if 0
	for (DWORD i = 0; i < dwLineIndex + 1; i++)
	{
		pListBox.GetText(i, strList);
		sz = pDC->GetTextExtent(strList);
		
		if (sz.cx > dx)
			dx = sz.cx;
	}
#endif
	// Set the horizontal extent so every character of all strings
	// can be scrolled to.
//	pListBox.SetHorizontalExtent(dx + 10);
//	pListBox.ReleaseDC(pDC);
}

VOID CCommonFunction::ShowInfo(CListBox *pListBox, LPCTSTR tszlogMsg, BOOL bAppend)
{
	DWORD		dwLineIndex;

	dwLineIndex = ((CListBox *)pListBox)->GetCount();

	// if the lines is more than the value user set
	// clear the message here
	if(dwLineIndex >= m_dwMaxMessageLine)
	{
		((CListBox *)pListBox)->ResetContent();
		dwLineIndex = 0;
	}

	if (bAppend)
	{
		(CListBox *)pListBox->AddString(tszlogMsg);
		// Scroll the list message to the bottom
		(CListBox *)pListBox->SetTopIndex(dwLineIndex);
	}
	else
		(CListBox *)pListBox->InsertString(0, tszlogMsg);
}

VOID CCommonFunction::LogInfo(CString strFileName, LPCTSTR tszlogMsg, BOOL bAppend)
{
	FILE		*pLogFile = NULL;
	CString		strTemp;
	TCHAR		tszAppFolder[MAX_PATH];

	if ("" == strFileName)
	{
		// Get the current directory here
		GetCurrentDirectory(MAX_PATH, tszAppFolder);
		strTemp.Format("%s\\TestInformation.txt", tszAppFolder);
		strFileName = strTemp;
	}
	if(bAppend)
		pLogFile = fopen(strFileName, "at");
	else
		pLogFile = fopen(strFileName, "wt");
	
	if(!pLogFile)
		return;

	fprintf(pLogFile, "%s\n", tszlogMsg);
	fclose(pLogFile);
}

VOID CCommonFunction::FindMaxMinValue(  double * pdblSourceArray,
										int iArrayLength,
										double * pdblMaxValue,
										double * pdblMinValue)
{
	int		i;

	* pdblMaxValue = 0;
	* pdblMinValue = 100000;

	if (pdblSourceArray	== NULL ||
		iArrayLength	== 0)
		return;

	for (i = 0; i < iArrayLength; i ++)
	{
		if(fabs(pdblSourceArray[i]) > fabs(* pdblMaxValue))
		{
			* pdblMaxValue = pdblSourceArray[i];
		}
		else if(fabs(pdblSourceArray[i]) <= fabs(* pdblMinValue))
		{
			* pdblMinValue = pdblSourceArray[i];
		}
	}
}

VOID CCommonFunction::HexStrToHexArray( CString strHexString,
										int iDataType,
										int iSendCmdLength,
										BYTE *pchSendMsg,
										BYTE btFilledCharInValue)
{
	int			iCounter, iStrLen;
	CString		strLeftTwoChar;
	long		lValue;
	BYTE		btValue;

	strHexString.Remove(' ');
	switch (iDataType)
	{
	case DATA_TYPE_BYTE:
		btValue = (BYTE)strtol(strHexString, NULL, 16);
		pchSendMsg[0] = btValue;
		break;
		// Process word type
	case DATA_TYPE_WORD:
//		break;
		// Process dword type
	case DATA_TYPE_DWORD:
//		break;
		// Process checksum type
	case DATA_TYPE_CHECKSUM:
		lValue = strtol(strHexString, NULL, 16);
		for (iCounter = iSendCmdLength - 1;iCounter >= 0;iCounter --)
		{
			pchSendMsg[iCounter] = lValue & 0x000000FF;
			lValue = lValue >> 8;
		}
		break;
		// Process double type
	case DATA_TYPE_DOUBLE:
		break;
	case DATA_TYPE_ASCII:
	case DATA_TYPE_NUMBER:
	case DATA_TYPE_DATE:
		iStrLen = strHexString.GetLength() / 2;
		for (iCounter = 0;iCounter < iSendCmdLength; iCounter ++)
		{
			if (iCounter >= iStrLen)
				strLeftTwoChar.Format("%02x", btFilledCharInValue);
			else
			{
				strLeftTwoChar = strHexString.Left(2);
				strHexString.Delete(0, 2);
			}
			btValue = (BYTE)strtol(strLeftTwoChar, NULL, 16);
			pchSendMsg[iCounter] = btValue;
		}
//		pchSendMsg[iCounter] = 0x00;
		break;
	default:
		break;
	}
}

VOID CCommonFunction::HexArrayToHexStr( BYTE *pchCmdArray,
										int iCmdLength,
										CString & strHexString)
{
	int			i;
	CString		strTemp;
	char		chSepChar = ' ';

	strHexString.Empty();

	for (i = 0;i < iCmdLength;i ++)
	{
		strTemp.Format("%c%02x", chSepChar, pchCmdArray[i]);
		strHexString = strHexString + strTemp;
	}
	if (!strHexString.IsEmpty())
		if (strHexString[0] == ' ')
			strHexString.Delete(0);
}

VOID CCommonFunction::UpdateChNumber(CComboBox& pComboBox, int iChNumber, int * piChIndex, int iStartChID)
{
	int			iCounter = 0;
	char		pChar[8];

	pComboBox.ResetContent();

	for (iCounter = iStartChID; iCounter <= (int)iChNumber; iCounter ++)
	{
		pComboBox.AddString(itoa(iCounter, pChar, 10));
	}

	* piChIndex < iStartChID ? * piChIndex = iStartChID - 1 : * piChIndex = * piChIndex;
	* piChIndex >= (int)(iChNumber) ? * piChIndex = iChNumber - iStartChID : * piChIndex = * piChIndex;
}

CString CCommonFunction::IntArrayToStr(int * pintArray, int iValidLength, char chDelimiter)
{
	CString		strTargetStr, strTemp;

	for (int iCounter = 0;iCounter < iValidLength;iCounter ++)
	{
		strTemp.Format("%02d", pintArray[iCounter]);
		strTargetStr = strTargetStr + strTemp + chDelimiter;
	}
	strTargetStr.TrimRight();

	return strTargetStr;
}

VOID CCommonFunction::StrToIntArray(int * pintArray, CString &strInputString, int &iValidLength, DWORD dwArraySize)
{
	char		pchSeps[] = " ,\t\n";
	char		*token = NULL;
//	char		*pchStrBuffer = NULL;
	char		pchStrBuffer[32];
	int			iCounter;

	iValidLength = 0;
	strcpy(pchStrBuffer, strInputString);
//	pchStrBuffer = strInputString.GetBuffer(strInputString.GetLength());
	token = strtok(pchStrBuffer, pchSeps);

	for (iCounter = 0;iCounter < (int)dwArraySize;iCounter ++)
	{
		if (token == NULL)
			break;
		pintArray[iCounter] = strtol(token, NULL, 10);
		token = strtok(NULL, pchSeps);
		iValidLength ++;
	}
	// Initialize other element in the array
	for (iCounter = iCounter;iCounter < (int)dwArraySize;iCounter ++)
	{
		pintArray[iCounter] = -1;
	}
}

/*****************************************************************
** 函数名:GetPath
** 输 入: 无
** 输 出: CString strPath
**        strPath非空, 表示用户选择的文件夹路径
**        strPath为空, 表示用户点击了“取消”键，取消选择
** 功能描述:显示“选择文件夹”对话框，让用户选择文件夹
****************************************************************/
CString CCommonFunction::GetSelectedPath(HWND hWnd)
{
	CString strPath = "";
	BROWSEINFO bInfo;

	ZeroMemory(&bInfo, sizeof(bInfo));
	bInfo.hwndOwner = hWnd;
	bInfo.lpszTitle = _T("请选择路径: ");
	bInfo.ulFlags = BIF_RETURNONLYFSDIRS;    

	LPITEMIDLIST lpDlist;					//用来保存返回信息的IDList
	lpDlist = SHBrowseForFolder(&bInfo) ;	//显示选择对话框
	if(lpDlist != NULL)						//用户按了确定按钮
	{
		TCHAR chPath[255];					//用来存储路径的字符串
		SHGetPathFromIDList(lpDlist, chPath);//把项目标识列表转化成字符串
		strPath = chPath;					//将TCHAR类型的字符串转换为CString类型的字符串
	}
	return strPath;
}

double CCommonFunction::FrequencyOffsetToWLOffset(double dblFrequencyOffset)
{
	double	dblRefFreq, dblRefWL, dblNewWL, dblWLOffset;

	dblRefFreq = 196000;
	dblRefWL = LIGHT / dblRefFreq;
	dblNewWL = LIGHT / (dblRefFreq - dblFrequencyOffset);

	dblWLOffset = dblNewWL - dblRefWL;

	return dblWLOffset;
}

double CCommonFunction::WLOffsetToFrequencyOffset(double dblWLOffset)
{
	double	dblRefFreq, dblRefWL, dblNewFreq, dblFrequencyOffset;

	dblRefFreq = 196000;
	dblRefWL = LIGHT / dblRefFreq;
	dblNewFreq = LIGHT / (dblRefWL + dblWLOffset);

	dblFrequencyOffset = dblRefFreq - dblNewFreq;

	return dblFrequencyOffset;
}

BOOL CCommonFunction::OpenCommandFile(BOOL bNeedOpenDialog,
									  CListCtrl& pctrlList,
									  CString& strCommandFileFullName,
									  int iCmdTypeForFilter,
									  int iStartCol,
									  BOOL bPopMsg,
									  stListCtrlCol _stListCtrlCol)
{
	CString			strTemp;
	CFileDialog		dlgFile(TRUE);
	int				nRet;

	if (bNeedOpenDialog)
	{
		// Open the dialog to select a data file
		dlgFile.m_ofn.lpstrFilter	  = "CSV File (*.csv)\x0*.csv\x0"
										"All File (*.*)\x0*.*";
		dlgFile.m_ofn.lpstrTitle	  = "打开文件";

		nRet = dlgFile.DoModal();
		if (nRet == 1)
		{
			strCommandFileFullName = dlgFile.GetPathName();
			pctrlList.GetParent()->UpdateData(FALSE);
		}
		else
			return FALSE;
	}

	if (!ShowCommandInfoToListCtrl( pctrlList,
									strCommandFileFullName,
									iCmdTypeForFilter,
									iStartCol,
									bPopMsg,
									_stListCtrlCol))
		return FALSE;
	else
		return TRUE;
}

BOOL CCommonFunction::SaveListBoxContent(CString& strCommandFileFullName,
										 CString& strFileHead,
										 CListBox& pctrlListBox)
{
	CString		strRowData;
	int			iRow;

	LogInfo(strCommandFileFullName, strFileHead, FALSE);
	for (iRow = 0;iRow < pctrlListBox.GetCount();iRow ++)
	{
		//Get command information from the list control
		pctrlListBox.GetText(iRow, strRowData);
		LogInfo(strCommandFileFullName, strRowData, TRUE);
	}

	return TRUE;
}

BOOL CCommonFunction::SaveListCtrlContent( CString& strCommandFileFullName,
										   CString& strFileHead,
										   CListCtrl& pctrlList,
										   int iColumnCount)
{
	CString		strRowData;
	int			iRow;

	LogInfo(strCommandFileFullName, strFileHead, FALSE);
	for (iRow = 0;iRow < pctrlList.GetItemCount();iRow ++)
	{
		//Get command information from the list control
		strRowData = GetOneLineFromListCtrl(pctrlList, iColumnCount, iRow, 0);
		if (!strRowData.IsEmpty())
			LogInfo(strCommandFileFullName, strRowData, TRUE);
	}

	return TRUE;
}

BOOL CCommonFunction::SaveListCtrlContent( CString& strCommandFileFullName,
										   CString& strFileHead,
										   CListCtrl& pctrlList,
										   int iColumnCount,
										   int iDataTypeColID,
										   int * piColForGet)
{
	CString		strRowData;
	int			iRow,
				iDataType = 0;

	LogInfo(strCommandFileFullName, strFileHead, FALSE);
	for (iRow = 0;iRow < pctrlList.GetItemCount();iRow ++)
	{
		iDataType = GetDataTypeWord(pctrlList.GetItemText(iRow, iDataTypeColID));
		//Get command information from the list control
		strRowData = GetOneLineFromListCtrl(pctrlList, iColumnCount, iRow, iDataType, piColForGet);
		if (!strRowData.IsEmpty())
			LogInfo(strCommandFileFullName, strRowData, TRUE);
	}

	return TRUE;
}

BOOL CCommonFunction::SaveListCtrlContent( CString& strCommandFileFullName,
										   CString& strFileHead,
										   CListCtrl& pctrlList,
										   int iColumnCount,
										   stListCtrlCol& _stListCtrlCol)
{
	CString		strRowData;
	int			iRow;

	LogInfo(strCommandFileFullName, strFileHead, FALSE);
	for (iRow = 0;iRow < pctrlList.GetItemCount();iRow ++)
	{
		//Get command information from the list control
		strRowData = GetOneLineFromListCtrl(pctrlList, iColumnCount, iRow, _stListCtrlCol);
		LogInfo(strCommandFileFullName, strRowData, TRUE);
	}

	return TRUE;
}

CString CCommonFunction::GetOneLineFromListCtrl(CListCtrl& pctrlList,
												int iColumnCount,
												int iRow,
												int iStartCol)
{
	CString		strFieldValue, strRowData;
	int			iCol = 0, i = 0;

	for (iCol = iStartCol;iCol < iColumnCount;iCol ++)
	{
		strFieldValue = pctrlList.GetItemText(iRow, iCol);
		if (strFieldValue.IsEmpty())
			strFieldValue = "NULL";
		strRowData = strRowData + "," + strFieldValue;
	}
	if (strRowData.Left(1) == ",")
		strRowData.Delete(0);

	return strRowData;
}

CString CCommonFunction::GetOneLineFromListCtrl(CListCtrl& pctrlList,
												int iColumnCount,
												int iRow,
												int iDataType,
												int * piColForGet)
{
	CString		strFieldValue, strRowData, strTemp;
	int			iCol = 0, i = 0, j = 0;
	char	*	pChar;
	WORD		wValue;
	BYTE		btYearHigh, btYearLow, btMonth, btDay;

	for (iCol = 0;iCol < iColumnCount;iCol ++)
	{
		if (-1 == piColForGet[iCol])
			continue;
		strFieldValue = pctrlList.GetItemText(iRow, iCol);
		if (strFieldValue.IsEmpty())
		{
			strFieldValue = "NULL";
			return "NULL";
		}

		switch (iDataType)
		{
		case DATA_TYPE_ASCII:
			pChar = strFieldValue.GetBuffer(MAX_LENGTH);
			for (j = 0;j < strFieldValue.GetLength();j ++)
			{
				strTemp = strTemp + "," + pChar[j];
			}
			if (strTemp.Left(1) == ",")
				strTemp.Delete(0);
			strFieldValue = strTemp;
			break;
		case DATA_TYPE_DATE:
			// Get year
			wValue = (WORD)strtol(strFieldValue.Left(4), NULL, 10);
			btYearLow = (BYTE)wValue;
			btYearHigh = (BYTE)(wValue >> 8);
			strFieldValue.Delete(0, 4);
			// Get month
			wValue = (WORD)strtol(strFieldValue.Left(2), NULL, 10);
			btMonth = (BYTE)wValue;
			strFieldValue.Delete(0, 2);
			// Get day
			wValue = (WORD)strtol(strFieldValue.Left(2), NULL, 10);
			btDay = (BYTE)wValue;
			strFieldValue.Format("0x%02x,0x%02x,0x%02x,0x%02x",
								btYearHigh,
								btYearLow,
								btMonth,
								btDay);
			break;
		default:
			break;
		}
		strRowData = strRowData + "," + strFieldValue;
	}
	if (strRowData.Left(1) == ",")
		strRowData.Delete(0);

	return strRowData;
}

CString CCommonFunction::GetOneLineFromListCtrl(CListCtrl& pctrlList,
												int iColumnCount,
												int iRow,
												stListCtrlCol& _stListCtrlCol)
{
	CString		strFieldValue, strRowData;
	int			iCol = 0, i = 0;

	for (iCol = 0;iCol < iColumnCount;iCol ++)
	{
		strFieldValue = pctrlList.GetItemText(iRow, iCol);
		if (strFieldValue.IsEmpty())
			strFieldValue = "NULL";
		if (iCol == _stListCtrlCol.iStartCol)
		{
			for (i = 0;i < _stListCtrlCol.iCount;i ++)
			{
				strRowData = strRowData + "," + _stListCtrlCol.chInsertionValue;
			}
		}
		strRowData = strRowData + "," + strFieldValue;
	}
	if (strRowData.Left(1) == ",")
		strRowData.Delete(0);

	return strRowData;
}

BOOL CCommonFunction::ShowCommandInfoToListCtrl(CListCtrl& pctrlList,
												CString strCommandFileFullName,
												int iCmdTypeForFilter,
												int iStartCol,
												BOOL bPopMsg,
												stListCtrlCol _stListCtrlCol)
{
	char		pchLineBuf[2 * MAX_LENGTH];
	LPTSTR		pStr;
	CStdioFile	fileCmdInfo;
	CString		strMsg;
	int			i;

	i = 0;
	pctrlList.DeleteAllItems();
	// Open CSV file
	if (!(fileCmdInfo.Open(strCommandFileFullName, CFile::modeRead | CFile::typeText, NULL)))
	{
		//Cannot open command file
		strMsg.Format("Cannot open the file:\n%s", strCommandFileFullName);
		if (bPopMsg)
			MessageBox(pctrlList.GetParent()->m_hWnd, strMsg, "数据导入", MB_OK);

		return FALSE;
	}
	else
	{
		// Read data from CSV file
		pStr  = fileCmdInfo.ReadString(pchLineBuf, 2 * MAX_LENGTH);
		i = 0;
		// Begin to process the first command data line
		while (NULL != pStr)
		{
			// If the first character is ";" or CR('\r', 0x0D) or LF('\n', 0x0A), then continue
			if (pchLineBuf[0] == ';' || pchLineBuf[0] == '\r' || pchLineBuf[0] == '\n')
			{
				pStr  = fileCmdInfo.ReadString(pchLineBuf, 2 * MAX_LENGTH);
				continue;
			}
			if (ShowOneLineToListCtrl(pchLineBuf, pctrlList, iCmdTypeForFilter, iStartCol, i, TRUE, _stListCtrlCol))
				i ++;

			pStr  = fileCmdInfo.ReadString(pchLineBuf, 2 * MAX_LENGTH);
		}
		fileCmdInfo.Close();
		// If the data file is null
		if (0 == i)
		{
			strMsg.Format("文件格式错误或文件为空，未能导入数据!");
			if (bPopMsg)
				MessageBox(pctrlList.GetParent()->m_hWnd, strMsg, "数据导入", MB_OK);
			return FALSE;
		}
		else
		{
			strMsg.Format("导入成功!");
			return TRUE;
		}
	}
}

BOOL CCommonFunction::ShowOneLineToListCtrl(char* pchLineBuf,
											CListCtrl& pctrlList,
											int iStartCol,
											int iItem,
											BOOL bNeedInsertItem,
											stListCtrlCol _stListCtrlCol)
{
	char		pchSeps[] = ",\t\n";
	char		*token = NULL;
	CString		strTemp;
	int			i = 0, j = iStartCol - 1;

	token = strtok(pchLineBuf, pchSeps);

	if (bNeedInsertItem)
	{
		// Insert one line
		pctrlList.InsertItem(iItem, "", 1);
	}

	while (NULL != token)
	{
		if ((++j - iStartCol == _stListCtrlCol.iStartCol) && (_stListCtrlCol.iCount != 0))
			continue;

		strTemp.Format("%s", token);
		if (!strcmp(token, "NULL"))
			strTemp.Empty();
		pctrlList.SetItemText(iItem, j, strTemp);

		token = strtok(NULL, pchSeps);
	}

	return TRUE;
}

BOOL CCommonFunction::ShowOneLineToListCtrl(char* pchLineBuf,
											CListCtrl& pctrlList,
											int iCmdTypeForFilter,
											int iStartCol,
											int iItem,
											BOOL bNeedInsertItem,
											stListCtrlCol _stListCtrlCol)
{
	char		pchSeps[] = ",\t\n";
	char		*token = NULL;
	CString		strTemp;
	int			i = 0, j = iStartCol - 1;

	if (CMD_ALL != iCmdTypeForFilter)
	{
		char		pTempBuf[2 * MAX_LENGTH];
		BYTE		btCurCmdType = 0;

		memcpy(pTempBuf, pchLineBuf, 2 * MAX_LENGTH);
		token = strtok(pTempBuf, pchSeps);
		for (i = 0;i < 13;i ++)
			token = strtok(NULL, pchSeps);
		// Is Common command?
		if (token != NULL)
			btCurCmdType |= !strcmp(token, "√") ? CMD_COMMON : 0;
		// Is MFG command?
		token = strtok(NULL, pchSeps);
		if (token != NULL)
			btCurCmdType |= !strcmp(token, "√") ? CMD_MFG : 0;
		// Is MFG info command?
		token = strtok(NULL, pchSeps);
		if (token != NULL)
			btCurCmdType |= !strcmp(token, "√") ? CMD_MFG_INFO : 0;
		// Is delivery command?
		token = strtok(NULL, pchSeps);
		if (token != NULL)
			btCurCmdType |= !strcmp(token, "√") ? CMD_DELIVERY : 0;

		if (!(btCurCmdType & iCmdTypeForFilter))
			return FALSE;
	}

	token = strtok(pchLineBuf, pchSeps);

	if (bNeedInsertItem)
	{
		// Insert one line
		pctrlList.InsertItem(iItem, "", 1);
	}

	while (NULL != token)
	{
		if ((++j - iStartCol == _stListCtrlCol.iStartCol) && (_stListCtrlCol.iCount != 0))
			continue;

		strTemp.Format("%s", token);
		if (!strcmp(token, "NULL"))
			strTemp.Empty();
		pctrlList.SetItemText(iItem, j, strTemp);

		token = strtok(NULL, pchSeps);
	}

	return TRUE;
}

VOID CCommonFunction::SetListCtrlColumn(CListCtrl& pListCtrl, LVCOLUMN * pColumn, int iColumnCount, int iStartInsCol)
{
	int	iCounter;

	for (iCounter = iStartInsCol;iCounter < iColumnCount;iCounter ++)
	{
		pListCtrl.InsertColumn(iCounter, &pColumn[iCounter]);
	}
}

void CCommonFunction::DeleteAllColumns(CListCtrl& pctrlList,
										int iColumnCount)
{
	int			i = 0;

	for (i = iColumnCount - 1;i >= 0;i --)
		pctrlList.DeleteColumn(i);
}

void CCommonFunction::ClearItemData(CListCtrl& pctrlList,
									int iItem,
									int iColumnCount,
									int iStartCol)
{
	int			j = 0;

	for (j = iStartCol;j < iColumnCount;j ++)
		pctrlList.SetItemText(iItem, j, "");
}

BOOL CCommonFunction::DeleteMultiItem(CListCtrl& pctrlList)
{
	//1.Define some variables
	WORD		wSelectedItemCount;
	POSITION	posSelectedItem;
	int			iSelectedItemIndex;
	BYTE		btDeletedFlag;

	//2. Do some initialization
	iSelectedItemIndex = 0;
	btDeletedFlag = 0;
	posSelectedItem = pctrlList.GetFirstSelectedItemPosition();

	if (!posSelectedItem)
		return FALSE;
	//3. Delete all the selected items
	while (posSelectedItem)
	{
		wSelectedItemCount = pctrlList.GetSelectedCount();
		posSelectedItem = pctrlList.GetFirstSelectedItemPosition();
		iSelectedItemIndex = pctrlList.GetNextSelectedItem(posSelectedItem);

		// Delete the selected item
		pctrlList.DeleteItem(iSelectedItemIndex);
		btDeletedFlag = 1;
	}
	pctrlList.SetItemState(iSelectedItemIndex, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
	pctrlList.SetSelectionMark(iSelectedItemIndex);

	return btDeletedFlag;
}

BOOL CCommonFunction::MoveItemUpOrDown(CListCtrl& pctrlList,
									   int iColumnCount,
									   BOOL bIsMoveDown)
{
	CString		strCurrentItem, strTargetItem;
	int			iSelectedItemIndex, iTargetItemIndex;
	stListCtrlCol	_stListCtrlCol = {0, 0, '-'};

	// Get selected item
	iSelectedItemIndex = pctrlList.GetSelectionMark();
	if (iSelectedItemIndex == -1)
		return FALSE;
	if (!bIsMoveDown && iSelectedItemIndex <= 0)
		return FALSE;
	else if (bIsMoveDown && iSelectedItemIndex == pctrlList.GetItemCount() - 1)
		return FALSE;
	if (!bIsMoveDown)
		iTargetItemIndex = iSelectedItemIndex - 1;
	else
		iTargetItemIndex = iSelectedItemIndex + 1;
	// Get current item
	strCurrentItem = GetOneLineFromListCtrl(pctrlList, iColumnCount, iSelectedItemIndex, _stListCtrlCol);
	// Save target item
	strTargetItem = GetOneLineFromListCtrl(pctrlList, iColumnCount, iTargetItemIndex, _stListCtrlCol);

	// Clear the content of the target item
	ClearItemData(pctrlList, iTargetItemIndex, iColumnCount);
	// Set target item
	ShowOneLineToListCtrl(strCurrentItem.GetBuffer(strCurrentItem.GetLength()), pctrlList, CMD_ALL, 0, iTargetItemIndex, FALSE, _stListCtrlCol);

	// Clear the content of the old item
	ClearItemData(pctrlList, iSelectedItemIndex, iColumnCount);
	// Update old item
	ShowOneLineToListCtrl(strTargetItem.GetBuffer(strTargetItem.GetLength()), pctrlList, CMD_ALL, 0, iSelectedItemIndex, FALSE, _stListCtrlCol);
	// Set to selected mode
	pctrlList.SetItemState(iSelectedItemIndex, 0, LVIS_SELECTED);
	pctrlList.SetItemState(iTargetItemIndex, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
	pctrlList.SetSelectionMark(iTargetItemIndex);
	pctrlList.EnsureVisible(iTargetItemIndex, FALSE);

	return TRUE;
}

VOID CCommonFunction::ComputeCycleTimesInOneUS( long lSampleTimes,
												double& dblRefDelay,
												long& lCycleTimesInOneUS,
												double& dblExecuteTimeOfOneCycle)
{
	LARGE_INTEGER	liFirst, liSecond, liFreq;
	LONGLONG		lRefDelay = 0;
	double			dblCycleTimesInOneUS = 0;
	int				i = 0;

	try
	{
		// 1. Get clock's frequency of CPU
		QueryPerformanceFrequency(&liFreq);

		// 2. Find the reference delay of the systemic function
		QueryPerformanceCounter(&liFirst);
		// Null sentence ...
		QueryPerformanceCounter(&liSecond);
		lRefDelay = liSecond.QuadPart - liFirst.QuadPart;
		dblRefDelay = lRefDelay * 1000000.0 / liFreq.QuadPart;

		// 3. Compute the delay time after execute <lSampleTimes> times Null-Cycles of "for"
		// Start time
		QueryPerformanceCounter(&liFirst);
		for (;i < lSampleTimes;i ++)
		{
			;
		}
		// Stop time
		QueryPerformanceCounter(&liSecond);
		dblCycleTimesInOneUS = (liSecond.QuadPart - liFirst.QuadPart - lRefDelay) * 1000000.0 / liFreq.QuadPart;

		// 4. Compute result
		// Compute the Cycle Times In One us
		lCycleTimesInOneUS = (long)floor(lSampleTimes / dblCycleTimesInOneUS + 0.5);
		// Compute the execute time(in us) of one cycle
		dblExecuteTimeOfOneCycle = dblCycleTimesInOneUS / lSampleTimes;
	}
	catch (TCHAR *ptszErrorMsg)
	{
		// Not process the execution
		ptszErrorMsg;

		return;
	}
}

CString CCommonFunction::GetTemperatureIdentifier(int iTestingTemperType,
												  int iLanguageType)
{
	CString		strTemperIdentifier;

	switch(iTestingTemperType)
	{
	case TEMPERATURE_LOW:
		strTemperIdentifier = (iLanguageType == LANGUAGE_ENGLISH ? "Low" : "低温");
		break;
	case TEMPERATURE_ROOM:
		strTemperIdentifier = (iLanguageType == LANGUAGE_ENGLISH ? "Room" : "常温");
		break;
	case TEMPERATURE_HIGH:
		strTemperIdentifier = (iLanguageType == LANGUAGE_ENGLISH ? "High" : "高温");
		break;
	default:
		break;
	}

	return strTemperIdentifier;
}

CString CCommonFunction::GetTemperatureIdentifier(double dblCurTemper,
												  int iLanguageType)
{
	CString		strTemperIdentifier;

	if (dblCurTemper < 10.0)
		strTemperIdentifier = (iLanguageType == LANGUAGE_ENGLISH ? "Low" : "低温");
	else if (dblCurTemper < 40.0)
		strTemperIdentifier = (iLanguageType == LANGUAGE_ENGLISH ? "Room" : "常温");
	else
		strTemperIdentifier = (iLanguageType == LANGUAGE_ENGLISH ? "High" : "高温");

	return strTemperIdentifier;
}

WORD CCommonFunction::ComputeComplementCode(WORD wOriginalValue)
{
	WORD wSignBit = 0, wNumericalValue = 0, wTargetValue = 0;

	wSignBit = wOriginalValue & 0x8000;
	wNumericalValue = wOriginalValue & 0x7FFF;

	// The Original Value is positive
	if (wSignBit == 0)
	{
		wTargetValue = wOriginalValue;
	}
	// The Original Value is negative
	else
	{
		wTargetValue = wSignBit | (~wNumericalValue + 1);
	}

	return wTargetValue;
}

double CCommonFunction::LinearFit(CMyPoint pointArray[], int iPointNum, double * pdblSlope, double * pdblIntercept)
{
	double	av_x,av_y;
	double	l_xx,l_yy,l_xy;
	int		i = 0;

	av_x = 0;
	av_y = 0;
	l_xx = 0;
	l_yy = 0;
	l_xy = 0;

	for(i=0; i < iPointNum; i++)
	{
		av_x += pointArray[i].dblX / iPointNum;
		av_y += pointArray[i].dblY / iPointNum;
	}

	for(i = 0;i < iPointNum; i++)
	{
		l_xx += (pointArray[i].dblX - av_x) * (pointArray[i].dblX - av_x);
		l_yy += (pointArray[i].dblY - av_y) * (pointArray[i].dblY - av_y);
		l_xy += (pointArray[i].dblX - av_x) * (pointArray[i].dblY - av_y);
	}

//	cout<<"this   line   can   be   fitted   by   y=ax+b."<<endl;
//	cout<<"a="<<l_xy/l_xx<<endl;
//	cout<<"b="<<av_y-l_xy*av_x/l_xx<<endl;
	* pdblSlope = l_xy / l_xx;
	* pdblIntercept = av_y - l_xy * av_x / l_xx;

	return double(l_xy / sqrt(l_xx * l_yy));
}

double CCommonFunction::ComputeLinearity(double * pdblFirstArray,
									   double * pdblSecondArray,
									   int iPointNum,
									   double * pdblSlope,
									   double * pdblIntercept)
{
	int			i = 0;
	double		r = 0;
	CMyPoint	point[MAX_PD_CALIB_SAMPLE] = {0, 0};

	for (i = 0;i < iPointNum;i ++)
	{
		point[i].SetX(pdblFirstArray[i]);
		point[i].SetY(pdblSecondArray[i]);
	}

#if 0
	// For test
	CMyPoint(6 ,2),
	CMyPoint(5 ,3),
	CMyPoint(11,9),
	CMyPoint(7 ,1),
	CMyPoint(5 ,8),
	CMyPoint(4 ,7),
	CMyPoint(4 ,5)
#endif

	return LinearFit(point, iPointNum, pdblSlope, pdblIntercept);
//	cout<<"line coefficient r="<<r<<endl;
}

// For BYTE array
WORD CCommonFunction::ComputeChecksum(BYTE * pchDataArray, int iSendCmdLength, BOOL bNeedReverse)
{
	WORD	wChecksum = 0;
	int		iCounter = 0;

	for (iCounter = 0;iCounter < iSendCmdLength;iCounter ++)
	{
		wChecksum += pchDataArray[iCounter];
	}

	if (bNeedReverse)
		return ~wChecksum;
	else
		return wChecksum;
}

// For WORD array
WORD CCommonFunction::ComputeChecksum(WORD * pwDataArray, int iSendCmdLength, BOOL bNeedReverse)
{
	WORD	wChecksum = 0;
	int		iCounter = 0;

	for (iCounter = 0;iCounter < iSendCmdLength;iCounter ++)
	{
		wChecksum += pwDataArray[iCounter];
	}

	if (bNeedReverse)
		return ~wChecksum;
	else
		return wChecksum;
}

VOID CCommonFunction::SetCmdInfoStruct( pstConfigInfo _pstConfigInfo,
										pstProtocolConfig _pstProtocolConfig,
										pstCommandInfo _pstCommandInfo,
										char * pchCmdInfo,
										int iSelectedItemIndex,
										BOOL bHasInnerData,
										BOOL bIsRead)
{
//	char	*	pchLineBuf = NULL;
	char		pchSeps[] = ",\t\n";
	char		*token = NULL;
	CString		strTemp,
				strHexString;

	// Initialization
//	pchLineBuf = strCmdInfo.GetBuffer(MAX_LENGTH);
	ZeroMemory(_pstCommandInfo->pchInnerString, 2 * MAX_LENGTH);

	if (-1 != iSelectedItemIndex)
	{
		ZeroMemory(_pstCommandInfo->pchCMDValue, 2 * MAX_LENGTH);
		ZeroMemory(_pstCommandInfo->pchCMDValueParam, MAX_DATA_PARAM_COUNT * MAX_LENGTH / 4);
	}
	ZeroMemory(_pstCommandInfo->pchFeedbackMsg, 4 * MAX_LENGTH);
	ZeroMemory(_pstCommandInfo->pchSendMsg, 2 * MAX_LENGTH);

	_pstCommandInfo->iFeedbackCmdLength = 4 * MAX_LENGTH;

	// Column 0, Get command code
	token = strtok(pchCmdInfo, pchSeps);
	if (token != NULL && strcmp(token, "NULL") && strcmp(token, "-"))
		strcpy((char *)_pstCommandInfo->pchCmdCode, (char *)token);

	// Column 1, Get CommandName
	token = strtok(NULL, pchSeps);
	if (token != NULL && strcmp(token, "NULL") && strcmp(token, "-"))
		strcpy((char *)_pstCommandInfo->pchCmdName, (char *)token);

	// Column 2, Get CMDValue
	token = strtok(NULL, pchSeps);
	if (token != NULL && strcmp(token, "NULL") && strcmp(token, "-"))
	{
		strcpy((char *)_pstCommandInfo->pchCMDValue, (char *)token);
	}

	// Column 3, Get inner string
	if (bHasInnerData)
		token = strtok(NULL, pchSeps);
	// ** Note: the InnerString is assigned later according to the sending data

	// Column 4, Get IsCmdValueFixed
	token = strtok(NULL, pchSeps);
	if (token != NULL && strcmp(token, "NULL") && strcmp(token, "-"))
		_pstCommandInfo->bIsCmdValueFixed = !strcmp(token, "√") ? 1 : 0;

	// Column 5, Get FilledCharInValue
	token = strtok(NULL, pchSeps);
	if (token != NULL && strcmp(token, "NULL") && strcmp(token, "-"))
		_pstCommandInfo->btFilledCharInValue = (BYTE)strtol(token, NULL, 16);

	// Column 6, Get Align Way string
	token = strtok(NULL, pchSeps);
	if (token != NULL && strcmp(token, "NULL") && strcmp(token, "-"))
		strcpy((char *)_pstCommandInfo->pchAlignWay, (char *)token);

	// Column 7, Get Command code type
	token = strtok(NULL, pchSeps);
	if (token != NULL && strcmp(token, "NULL") && strcmp(token, "-"))
		_pstCommandInfo->iCmdCodeType = GetDataTypeWord((char *)token);

	// Column 8, Get DataType
	token = strtok(NULL, pchSeps);
	if (token != NULL && strcmp(token, "NULL") && strcmp(token, "-"))
		_pstCommandInfo->iDataType = GetDataTypeWord((char *)token);

	// Column 9, Get DataLength
	token = strtok(NULL, pchSeps);
	if (token != NULL && strcmp(token, "NULL") && strcmp(token, "-"))
		_pstCommandInfo->iDataLength = atoi((char *)token);

	// Column 10, Get bIsDataLenFixed
	token = strtok(NULL, pchSeps);
	if (token != NULL && strcmp(token, "NULL") && strcmp(token, "-"))
		_pstCommandInfo->bIsDataLenFixed = !strcmp(token, "√") ? 1 : 0;

	// Column 11, Get AccessType
	token = strtok(NULL, pchSeps);
	if (token != NULL && strcmp(token, "NULL") && strcmp(token, "-"))
		_pstCommandInfo->iAccessType = GetAccessTypeWord((char *)token);

	// Column 12, Get Data Amplification
	token = strtok(NULL, pchSeps);
	if (token != NULL && strcmp(token, "NULL") && strcmp(token, "-"))
		_pstCommandInfo->iDataAmplification = atoi((char *)token);

	// Column 13, Get DataUnit
	token = strtok(NULL, pchSeps);
	if (token != NULL && strcmp(token, "NULL") && strcmp(token, "-"))
		strcpy((char *)_pstCommandInfo->pchDataUnit, (char *)token);

//----------------------------------------------------------------------
	// Column 14, Get bIsCommonCmd
	token = strtok(NULL, pchSeps);
	if (token != NULL && strcmp(token, "NULL") && strcmp(token, "-"))
		_pstCommandInfo->bIsCommonCmd = !strcmp(token, "√") ? 1 : 0;
	// Column 15, Get bIsMfgInfoCmd
	token = strtok(NULL, pchSeps);
	if (token != NULL && strcmp(token, "NULL") && strcmp(token, "-"))
		_pstCommandInfo->bIsMfgCmd = !strcmp(token, "√") ? 1 : 0;
	// Column 16, Get bIsCommonCmd
	token = strtok(NULL, pchSeps);
	if (token != NULL && strcmp(token, "NULL") && strcmp(token, "-"))
		_pstCommandInfo->bIsMfgInfoCmd = !strcmp(token, "√") ? 1 : 0;
	// Column 17, Get bIsFactorySettingCmd
	token = strtok(NULL, pchSeps);
	if (token != NULL && strcmp(token, "NULL") && strcmp(token, "-"))
		_pstCommandInfo->bIsFactorySettingCmd = !strcmp(token, "√") ? 1 : 0;

	// Column 18, Get CmdSendWay string
	token = strtok(NULL, pchSeps);
	if (token != NULL && strcmp(token, "NULL") && strcmp(token, "-"))
		strcpy((char *)_pstCommandInfo->pchCmdSendWay, (char *)token);

	// Column 19, Get send protocol type string
	token = strtok(NULL, pchSeps);
	if (token != NULL && strcmp(token, "NULL") && strcmp(token, "-"))
		strcpy((char *)_pstCommandInfo->pchSendProtType, (char *)token);

	// Column 20, Get feedback protocol type string
	token = strtok(NULL, pchSeps);
	if (token != NULL && strcmp(token, "NULL") && strcmp(token, "-"))
		strcpy((char *)_pstCommandInfo->pchFeedbackProtType, (char *)token);

	// Column 21, Get bHasFeedbackMsg
	token = strtok(NULL, pchSeps);
	if (token != NULL && strcmp(token, "NULL") && strcmp(token, "-"))
		_pstCommandInfo->bHasFeedbackMsg = !strcmp(token, "√") ? TRUE : FALSE;

	// Column 22, Get FDBK Data Param Name List
	token = strtok(NULL, pchSeps);
	if (token != NULL && strcmp(token, "NULL") && strcmp(token, "-"))
		strcpy((char *)_pstCommandInfo->pchFDBKDataParamNameList, (char *)token);
	else
		ZeroMemory(_pstCommandInfo->pchFDBKDataParamNameList, MAX_LENGTH);

	// Column 23, Get Description
	token = strtok(NULL, pchSeps);
	if (token != NULL && strcmp(token, "NULL") && strcmp(token, "-"))
		strcpy((char *)_pstCommandInfo->pchDescription, (char *)token);
	else
		ZeroMemory(_pstCommandInfo->pchDescription, MAX_LENGTH);

	// Get Parameter from CMD value
	if (_pstCommandInfo->pchCMDValue != NULL)
		GetCMDValueParam(_pstCommandInfo, MAX_DATA_PARAM_COUNT);
	// Create formatted command string
//--------------------------------------------
	stProtocolConfig	_stProtocolConfig;
	CString				strProtConfigFileName;

	ZeroMemory(&_stProtocolConfig, sizeof(stProtocolConfig));
	// If the protcol type is default, use the configed protcol type,
	// otherwise, read file to get the specified protocol's config information
	if (!strcmp("默认", (char *)_pstCommandInfo->pchSendProtType))
		memcpy(&_stProtocolConfig, _pstProtocolConfig, sizeof(stProtocolConfig));
	else
	{
		if (bIsRead)
			strProtConfigFileName.Format("%s\\Command\\Protocol\\%s_Read.csv",
										_pstConfigInfo->_pstApplicationPara->pchWorkingDirectory,
										_pstCommandInfo->pchSendProtType);
		else
			strProtConfigFileName.Format("%s\\Command\\Protocol\\%s_Write.csv",
										_pstConfigInfo->_pstApplicationPara->pchWorkingDirectory,
										_pstCommandInfo->pchSendProtType);
		GetProtConfigFromFile(strProtConfigFileName, &_stProtocolConfig);
	}
	// Fill in the command string template
	FillCmdStrTemplate(&_stProtocolConfig, _pstCommandInfo);

	// Copy to Sending Command string
	memcpy(_pstCommandInfo->pchSendMsg, _stProtocolConfig.pchCmdStringTemplate, 2 * MAX_LENGTH);
	_pstCommandInfo->iSendCmdLength = _stProtocolConfig.iCmdLength;
//	_pstCommandInfo->iDataLength = _stProtocolConfig.iCmdLength;
//--------------------------------------------

	HexArrayToHexStr(_pstCommandInfo->pchSendMsg,
					_stProtocolConfig.iCmdLength,
					strHexString);
	strcpy((char *)_pstCommandInfo->pchInnerString, strHexString);

	// Additional information
	_pstCommandInfo->iRowIndex	 = iSelectedItemIndex;
}

VOID CCommonFunction::SetCmdInfoStruct( pstConfigInfo _pstConfigInfo,
										pstProtocolConfig _pstProtocolConfig,
										pstCommandInfo _pstCommandInfo,
										int iProtType)
{
	CString				strTemp,
						strHexString;
	stProtocolConfig	_stProtocolConfig;
	CString				strProtConfigFileName;

	// Initialization
	ZeroMemory(&_stProtocolConfig, sizeof(stProtocolConfig));

	// If the protcol type is default, use the configed protcol type,
	// otherwise, read file to get the specified protocol's config information
	if (!strcmp("默认", (char *)_pstCommandInfo->pchFeedbackProtType))
		memcpy(&_stProtocolConfig, _pstProtocolConfig, sizeof(stProtocolConfig));
	else
	{
		switch (iProtType)
		{
		case PROT_TYPE_READ:
			strProtConfigFileName.Format("%s\\Command\\Protocol\\%s_Read.csv",
										_pstConfigInfo->_pstApplicationPara->pchWorkingDirectory,
										_pstCommandInfo->pchSendProtType);
			break;
		case PROT_TYPE_WRITE:
			strProtConfigFileName.Format("%s\\Command\\Protocol\\%s_Write.csv",
										_pstConfigInfo->_pstApplicationPara->pchWorkingDirectory,
										_pstCommandInfo->pchSendProtType);
			break;
		case PROT_TYPE_FEEDBACK:
			strProtConfigFileName.Format("%s\\Command\\Protocol\\%s_Feedback.csv",
										_pstConfigInfo->_pstApplicationPara->pchWorkingDirectory,
										_pstCommandInfo->pchFeedbackProtType);
			break;
		default:
			break;
		}
		GetProtConfigFromFile(strProtConfigFileName, &_stProtocolConfig);
	}
	// Get Parameter from Feedback value
	GetFDBKValueParamName(_pstCommandInfo,
						MAX_FDBK_DATA_PARAM_COUNT,
						&_stProtocolConfig.iFDBKDataCount,
						&_pstCommandInfo->iKeyValuePos);
	// Find and Fill Single Protocol Parameter in the feedback message
	FillProtParam(&_stProtocolConfig, _pstCommandInfo);
}

VOID CCommonFunction::GetCMDValueParam(pstCommandInfo _pstCommandInfo,
										int iMaxDataParamCount)
{
	char	*	pchLineBuf = NULL;
	char		pchSeps[] = " ,\t\n";
	char		*token = NULL;
	int			i = 0;
	CString		strCMDValue;

	strCMDValue.Format("%s", _pstCommandInfo->pchCMDValue);
	pchLineBuf = strCMDValue.GetBuffer(strCMDValue.GetLength());
	token = strtok(pchLineBuf, pchSeps);
	for (i = 0;i < iMaxDataParamCount;i ++)
	{
		if (token == NULL)
			break;
		strcpy((char *)_pstCommandInfo->pchCMDValueParam[i], (char *)token);
		token = strtok(NULL, pchSeps);
	}
}

VOID CCommonFunction::GetFDBKValueParamName(pstCommandInfo _pstCommandInfo,
											int iMaxFDBKDataParamCount,
											int * piFDBKDataCount,
											int * piKeyValuePos)
{
	char	*	pchLineBuf = NULL;
	char		pchSeps[] = " ,\t\n";
	char		*token = NULL;
	int			i = 0;
	CString		strFDBKValueParamName;

	* piFDBKDataCount = 0;

	strFDBKValueParamName.Format("%s", _pstCommandInfo->pchFDBKDataParamNameList);
	pchLineBuf = strFDBKValueParamName.GetBuffer(strFDBKValueParamName.GetLength());
	token = strtok(pchLineBuf, pchSeps);
	for (i = 0;i < iMaxFDBKDataParamCount;i ++)
	{
		if (token == NULL)
			break;
		if (token[0] == '*')
		{
			* piKeyValuePos = i;
			strcpy((char *)_pstCommandInfo->pchFDBKDataParamName[i], (char *)(token + 1));
		}
		else
			strcpy((char *)_pstCommandInfo->pchFDBKDataParamName[i], (char *)token);

		(* piFDBKDataCount) ++;
		token = strtok(NULL, pchSeps);
	}
}

VOID CCommonFunction::FillCmdStrTemplate(pstProtocolConfig _pstProtocolConfig,
										 pstCommandInfo _pstCommandInfo)
{
	int				i = 0,
					iPosInArray = 0,
					iCmdLength = 0,
					iCheckSumIndex = -1,
					iLastProtParamItemLen = 0;
	WORD			wValue = 0;
	CString			strTemp;
	pstProtParamItem _pstProtParamItem = NULL;

	for (i = 0;i < _pstProtocolConfig->iProtParamCount;i ++)
	{
		_pstProtParamItem = NULL;
		// *** data stream: command info -> protocol param
		if (!CmdInfoToProtParam(i, _pstProtocolConfig, _pstProtParamItem, _pstCommandInfo))
		{
			iPosInArray -= iLastProtParamItemLen;
			continue;
		}

		if (_pstProtParamItem == NULL)
			continue;

		if (_pstProtParamItem->iItemLength < 0)
			continue;

		_pstProtParamItem->iPosInCmdString = iPosInArray;
		iPosInArray += _pstProtParamItem->iItemLength;

		iLastProtParamItemLen = _pstProtParamItem->iItemLength;

		// Fill each protocol parameter
		// *** data stream: protocol param -> CmdStringTemplate
		ProtParamToCmdStrTemplate(_pstProtocolConfig, _pstProtParamItem, _pstCommandInfo);

		if (_pstProtParamItem->bIsCheckSumSaveField)
			iCheckSumIndex = i;
		else
		{
			// Compute check sum
			if (_pstProtParamItem->bIsCheckSumAddField)
				wValue += ComputeChecksum(_pstProtocolConfig->pchCmdStringTemplate + _pstProtParamItem->iPosInCmdString,
										_pstProtParamItem->iItemLength,
										FALSE);
		}
	}
	_pstProtocolConfig->iCmdLength = iPosInArray;

	// Compute and fill in the checksum value
	if ((_pstProtocolConfig->_pstProtParamItem[iCheckSumIndex].iItemLength >= 0) &&
		(!_pstProtocolConfig->_pstProtParamItem[iCheckSumIndex].bIsParamValueFixed))
	{
		wValue %= 256;
		strTemp.Format("%x", (BYTE)wValue);
		strcpy((char *)_pstProtocolConfig->_pstProtParamItem[iCheckSumIndex].pchParamItemValue, strTemp);

		ProtParamToCmdStrTemplate(_pstProtocolConfig, &_pstProtocolConfig->_pstProtParamItem[iCheckSumIndex], _pstCommandInfo);
	}
}

VOID CCommonFunction::FillProtParam(pstProtocolConfig _pstProtocolConfig,
									pstCommandInfo _pstCommandInfo)
{
	int				i = 0,
					iPosInArray = 0,
					iCmdLength = 0;
	WORD			wValue = 0;
	CString			strTemp, strEyeableString;
	pstProtParamItem _pstProtParamItem = NULL;

	for (i = 0;i < _pstProtocolConfig->iProtParamCount;i ++)
	{
		_pstProtParamItem = NULL;

		// Fill each protocol parameter
		// *** data stream: feedback message -> protocol param
		FeedbackMsgToCmdInfo(i, _pstProtocolConfig, _pstProtParamItem, _pstCommandInfo);
//		FeedbackMsgToProtParam(i, _pstProtocolConfig, _pstProtParamItem, _pstCommandInfo);
		if (_pstProtParamItem == NULL)
			continue;

		if (_pstProtParamItem->iItemLength < 0)
			continue;

		iPosInArray += _pstProtParamItem->iItemLength;

		HexStrToEyeableStr(_pstCommandInfo->pchFeedbackMsg + iPosInArray,
						_pstProtParamItem->iItemType,
						_pstProtParamItem->iItemLength,
						strEyeableString);
		strcpy((char *)_pstProtParamItem->pchParamItemValue, strEyeableString);
		// Get value from feedback data
		if (_pstProtParamItem->bIsDataField)
		{
			strcpy((char *)_pstProtParamItem->pchParamItemName, (char *)_pstCommandInfo->pchFDBKDataParamName[_pstProtParamItem->iPosInCmdValue]);
			strcpy((char *)_pstCommandInfo->pchFDBKDataParamValue[_pstProtParamItem->iPosInCmdValue], (char *)_pstProtParamItem->pchParamItemValue);
		}
	}
	_pstProtocolConfig->iCmdLength = iPosInArray;
}

VOID CCommonFunction::ProtParamToCmdStrTemplate(pstProtocolConfig _pstProtocolConfig,
												pstProtParamItem _pstProtParamItem,
												pstCommandInfo _pstCommandInfo)
{
	CString		strHexString;
	int			iFilledCharInValue = 0, iAlignType = DATA_LEFT_ALIGN;

	iAlignType = !strcmp((char *)_pstCommandInfo->pchAlignWay, "右对齐") ? DATA_RIGHT_ALIGN : DATA_LEFT_ALIGN;

//	if (_pstProtocolConfig->_stProtParam._stMsgEndID.iItemLength == -1)
		iFilledCharInValue = _pstCommandInfo->btFilledCharInValue;
//	else
//		iFilledCharInValue = 0x00;
//For debug----------------------------------------------------
	strHexString.Format("%s", _pstProtParamItem->pchParamItemValue);
	int iLen = strHexString.GetLength();
//------------------------------------------------------------
	ConvertToHexString((char *)_pstProtParamItem->pchParamItemValue,
						_pstProtParamItem->iItemType,
						_pstProtParamItem->iItemLength,
						strHexString,
						iAlignType,
						iFilledCharInValue,
						FALSE,
						TRUE);
	HexStrToHexArray(strHexString,
					_pstProtParamItem->iItemType,
					_pstProtParamItem->iItemLength,
					_pstProtocolConfig->pchCmdStringTemplate + _pstProtParamItem->iPosInCmdString,
					iFilledCharInValue);
}

BOOL CCommonFunction::CmdInfoToProtParam(int iParamSN,
										 pstProtocolConfig _pstProtocolConfig,
										 pstProtParamItem &_pstProtParamItem,
										 pstCommandInfo _pstCommandInfo)
{
	CString		strTemp;

	if (iParamSN == _pstProtocolConfig->_pstProtParamItem[iParamSN].iSN)
	{
		_pstProtParamItem = &_pstProtocolConfig->_pstProtParamItem[iParamSN];
	}

	if (!strcmp("CmdCode", (char *)_pstProtParamItem->pchParamItemID))
	{
		if (-1 != _pstProtParamItem->iPosInCmdValue)
		{
			// Assign protocol parameter value
			if (!AssignProtParamValue(_pstCommandInfo, _pstProtParamItem))
				return FALSE;
		}
		else
		{
			strTemp.Format("%s", _pstCommandInfo->pchCmdCode);
			strcpy((char *)_pstProtParamItem->pchParamItemValue, strTemp);
			if (_pstProtParamItem->iItemLength == 0)
			{
				if (strTemp.IsEmpty())
					return FALSE;
				else
					_pstProtParamItem->iItemLength = strTemp.GetLength();
			}
		}

		return TRUE;
	}
	else if (!strcmp("WorkMode", (char *)_pstProtParamItem->pchParamItemID))
	{
		if (-1 != _pstProtParamItem->iPosInCmdValue)
		{
			// Assign protocol parameter value
			if (!AssignProtParamValue(_pstCommandInfo, _pstProtParamItem))
				return FALSE;
		}
		else
		{
			strTemp.Format("%s", _pstCommandInfo->pchWorkMode);
			strcpy((char *)_pstProtParamItem->pchParamItemValue, strTemp);
			if (_pstProtParamItem->iItemLength == 0)
			{
				if (strTemp.IsEmpty())
					return FALSE;
				else
					_pstProtParamItem->iItemLength = strTemp.GetLength();
			}
		}

		return TRUE;
	}
	else if (!strcmp("E2Addr", (char *)_pstProtParamItem->pchParamItemID))
	{
		if (-1 != _pstProtParamItem->iPosInCmdValue)
		{
			// Assign protocol parameter value
			if (!AssignProtParamValue(_pstCommandInfo, _pstProtParamItem))
				return FALSE;
		}
		else
		{
			strTemp.Format("%d", _pstCommandInfo->dwE2Addr);
			strcpy((char *)_pstProtParamItem->pchParamItemValue, strTemp);
			if (_pstProtParamItem->iItemLength == 0)
			{
				if (strTemp.IsEmpty())
					return FALSE;
				else
					_pstProtParamItem->iItemLength = strTemp.GetLength();
			}
		}

		return TRUE;
	}
	else if (!strcmp("ChannelID", (char *)_pstProtParamItem->pchParamItemID))
	{
		if (-1 != _pstProtParamItem->iPosInCmdValue)
		{
			// Assign protocol parameter value
			if (!AssignProtParamValue(_pstCommandInfo, _pstProtParamItem))
				return FALSE;
		}
		else
		{
			strTemp.Format("%d", _pstCommandInfo->dwChannelID);
			strcpy((char *)_pstProtParamItem->pchParamItemValue, strTemp);
			if (_pstProtParamItem->iItemLength == 0)
			{
				if (strTemp.IsEmpty())
					return FALSE;
				else
					_pstProtParamItem->iItemLength = strTemp.GetLength();
			}
		}

		return TRUE;
	}
	else if (!strcmp("SendValue", (char *)_pstProtParamItem->pchParamItemID))
	{
		if (_pstProtParamItem->iItemLength == 0)
		{
			if (_pstCommandInfo->bIsDataLenFixed)
			{
				_pstProtParamItem->iItemLength = _pstCommandInfo->iDataLength;
			}
			else
			{
				strTemp.Format("%s", _pstCommandInfo->pchCMDValueParam[_pstProtParamItem->iPosInCmdValue - 1]);
				if (strTemp.IsEmpty())
					return FALSE;
				else
					_pstProtParamItem->iItemLength = strTemp.GetLength();
			}
		}

		if (-1 != _pstProtParamItem->iPosInCmdValue)
		{
			int iAlignType = 
				!strcmp((char *)_pstCommandInfo->pchAlignWay, "右对齐") ? DATA_RIGHT_ALIGN : DATA_LEFT_ALIGN;
			// Regularize the parameter in CMD value according to: DataLength, AlignWay and FilledCharInValue
			ConvertToHexString((char *)_pstCommandInfo->pchCMDValueParam[_pstProtParamItem->iPosInCmdValue - 1],
								_pstProtParamItem->iItemType,
								_pstProtParamItem->iItemLength,
								strTemp,
								iAlignType,
								_pstCommandInfo->btFilledCharInValue,
								FALSE,
								FALSE);
			strcpy((char *)_pstCommandInfo->pchCMDValueParam[_pstProtParamItem->iPosInCmdValue - 1], strTemp);
			strcpy((char *)_pstProtParamItem->pchParamItemValue, strTemp);
		}

		return TRUE;
	}
	else if (!strcmp("FeedbackValue", (char *)_pstProtParamItem->pchParamItemID))
	{
		if (-1 != _pstProtParamItem->iPosInCmdValue)
			strcpy((char *)_pstProtParamItem->pchParamItemValue,
				(char *)_pstCommandInfo->pchCMDValueParam[_pstProtParamItem->iPosInCmdValue - 1]);
		if (_pstProtParamItem->iItemLength == 0)
		{
			strTemp.Format("%s", _pstCommandInfo->pchCMDValueParam[_pstProtParamItem->iPosInCmdValue - 1]);
			_pstProtParamItem->iItemLength = strTemp.GetLength();
		}

		return TRUE;
	}
#if 0
	else if (!strcmp("AccessType", (char *)_pstProtParamItem->pchParamItemID))
	{
		_pstProtParamItem = &_pstProtocolConfig->_stProtParam._stAccessType;
		return;
	}
	else if (!strcmp("Status", (char *)_pstProtParamItem->pchParamItemID))
	{
		_pstProtParamItem = &_pstProtocolConfig->_stProtParam._stStatus;
		return;
	}
	else if (!strcmp("Checksum", (char *)_pstProtParamItem->pchParamItemID))
	{
		_pstProtParamItem = &_pstProtocolConfig->_stProtParam._stChecksum;
		return;
	}
	else if (!strcmp("Reserve1", (char *)_pstProtParamItem->pchParamItemID))
	{
		_pstProtParamItem = &_pstProtocolConfig->_stProtParam._stReserve1;
		return;
	}
	else if (!strcmp("Reserve2", (char *)_pstProtParamItem->pchParamItemID))
	{
		_pstProtParamItem = &_pstProtocolConfig->_stProtParam._stReserve2;
		return;
	}
	else if (!strcmp("Separator1", (char *)_pstProtParamItem->pchParamItemID))
	{
		_pstProtParamItem = &_pstProtocolConfig->_stProtParam._stSeparator1;
		return;
	}
	else if (!strcmp("Separator2", (char *)_pstProtParamItem->pchParamItemID))
	{
		_pstProtParamItem = &_pstProtocolConfig->_stProtParam._stSeparator2;
		return;
	}
	else if (!strcmp("Separator3", (char *)_pstProtParamItem->pchParamItemID))
	{
		_pstProtParamItem = &_pstProtocolConfig->_stProtParam._stSeparator3;
		return;
	}
	else if (!strcmp("Separator4", (char *)_pstProtParamItem->pchParamItemID))
	{
		_pstProtParamItem = &_pstProtocolConfig->_stProtParam._stSeparator4;
		return;
	}
	else if (!strcmp("MsgEndID", (char *)_pstProtParamItem->pchParamItemID))
	{
		_pstProtParamItem = &_pstProtocolConfig->_stProtParam._stMsgEndID;
		return;
	}
#endif

	return TRUE;
}

#if 0
// *** data stream: feedback message -> protocol param
VOID CCommonFunction::FeedbackMsgToProtParam(int iParamSN,
											pstProtocolConfig _pstProtocolConfig,
											pstProtParamItem _pstProtParamItem,
											pstCommandInfo _pstCommandInfo)
{
	CString		strHexString;
	int			iFilledCharInValue = 0, iAlignType = DATA_LEFT_ALIGN;

	iAlignType = !strcmp((char *)_pstCommandInfo->pchAlignWay, "右对齐") ? DATA_RIGHT_ALIGN : DATA_LEFT_ALIGN;

	if (_pstProtocolConfig->_stProtParam._stMsgEndID.iItemLength == -1)
		iFilledCharInValue = _pstCommandInfo->btFilledCharInValue;
	else
		iFilledCharInValue = 0x00;
//For debug----------------------------------------------------
	strHexString.Format("%s", _pstProtParamItem->pchParamItemValue);
	int iLen = strHexString.GetLength();
//------------------------------------------------------------
	ConvertToHexString((char *)_pstProtParamItem->pchParamItemValue,
						_pstProtParamItem->iItemType,
						_pstProtParamItem->iItemLength,
						strHexString,
						iAlignType,
						iFilledCharInValue,
						FALSE,
						TRUE);
	HexStrToHexArray(strHexString,
					_pstProtParamItem->iItemType,
					_pstProtParamItem->iItemLength,
					_pstProtocolConfig->pchCmdStringTemplate + _pstProtParamItem->iPosInCmdString,
					iFilledCharInValue);
}
#endif

VOID CCommonFunction::FeedbackMsgToCmdInfo(int iParamSN,
										  pstProtocolConfig _pstProtocolConfig,
										  pstProtParamItem &_pstProtParamItem,
										  pstCommandInfo _pstCommandInfo)
{
	CString		strTemp;

	if (iParamSN == _pstProtocolConfig->_pstProtParamItem[iParamSN].iSN)
	{
		_pstProtParamItem = &_pstProtocolConfig->_pstProtParamItem[iParamSN];
	}

	if (!strcmp("FeedbackValue", (char *)_pstProtParamItem->pchParamItemID))
	{
		if (_pstProtParamItem->iItemLength == 0)
		{
			if (_pstCommandInfo->bIsDataLenFixed)
			{
				_pstProtParamItem->iItemLength = _pstCommandInfo->iDataLength;
			}
			else
			{
				strTemp.Format("%s", _pstCommandInfo->pchFeedbackMsg);
				_pstProtParamItem->iItemLength = strTemp.GetLength();
			}
		}

		return;
	}
}

BOOL CCommonFunction::AssignProtParamValue(pstCommandInfo _pstCommandInfo,
											pstProtParamItem _pstProtParamItem)
{
	CString		strTemp;
	int			iAlignType;

	if (_pstProtParamItem->iItemLength == 0)
	{
		strTemp.Format("%s", _pstCommandInfo->pchCMDValueParam[_pstProtParamItem->iPosInCmdValue - 1]);
		if (strTemp.IsEmpty())
			return FALSE;
		else
			_pstProtParamItem->iItemLength = strTemp.GetLength();
	}
	iAlignType = 
		!strcmp((char *)_pstCommandInfo->pchAlignWay, "右对齐") ? DATA_RIGHT_ALIGN : DATA_LEFT_ALIGN;
	// Regularize the parameter in CMD value according to: DataLength, AlignWay and FilledCharInValue
	ConvertToHexString((char *)_pstCommandInfo->pchCMDValueParam[_pstProtParamItem->iPosInCmdValue - 1],
						_pstProtParamItem->iItemType,
						_pstProtParamItem->iItemLength,
						strTemp,
						iAlignType,
						_pstCommandInfo->btFilledCharInValue,
						FALSE,
						FALSE);
	strcpy((char *)_pstCommandInfo->pchCMDValueParam[_pstProtParamItem->iPosInCmdValue - 1], strTemp);
	strcpy((char *)_pstProtParamItem->pchParamItemValue, strTemp);

	return TRUE;
}

void CCommonFunction::GetProtConfigFromFile(CString strConfigFileName, pstProtocolConfig _pstProtocolConfig)
{
	LPTSTR				pStr = NULL;
	char				pchLineBuf[2 * MAX_LENGTH];
	CStdioFile			stdioFile;
	stProtParamItem		_stProtParamItem;
	int					iProtParamCount = 0, iPosInArray = 0;

	ZeroMemory(&_stProtParamItem, sizeof(stProtParamItem));
	InitProtConfig(_pstProtocolConfig, MAX_PROT_PARAM_COUNT);

	// Open the configuration file
	if (!(stdioFile.Open(strConfigFileName, CFile::modeRead | CFile::typeText, NULL)))
		return;
	pStr = stdioFile.ReadString(pchLineBuf, 2 * MAX_LENGTH);
	while (pStr != NULL)
	{
		// If the first character is ";" or CR('\r', 0x0D) or LF('\n', 0x0A), then continue
		if (pchLineBuf[0] == ';' || pchLineBuf[0] == '\r' || pchLineBuf[0] == '\n')
		{
			pStr = stdioFile.ReadString(pchLineBuf, 2 * MAX_LENGTH);
			continue;
		}
		// parse the data, assign the struct: stProtocolConfig
		SetProtParamItemStruct(pchLineBuf, &_stProtParamItem, &iPosInArray);
		SetProtConfigStruct(_pstProtocolConfig, &_stProtParamItem, iProtParamCount);

		pStr = stdioFile.ReadString(pchLineBuf, 2 * MAX_LENGTH);

		iProtParamCount ++;
	}
	_pstProtocolConfig->iProtParamCount = iProtParamCount;

	stdioFile.Close();
}

void CCommonFunction::InitProtConfig(pstProtocolConfig _pstProtocolConfig, int iMaxProtParamCount)
{
	for (int i = 0;i < iMaxProtParamCount;i ++)
	{
		_pstProtocolConfig->_pstProtParamItem[i].iItemLength = -1;
		_pstProtocolConfig->_pstProtParamItem[i].iSN = -1;
	}
}

void CCommonFunction::SetProtParamItemStruct(char* pchLineBuf,
										pstProtParamItem _pstProtParamItem,
										int * piPosInArray)
{
	char				pchSeps[] = ",\t\n";
	char				*token = NULL;

	token = strtok(pchLineBuf, pchSeps);
	// 1. Assign serial number
	if (NULL != token)
	{
		_pstProtParamItem->iSN = strtol(token, NULL, 10);
	}
	// 2. Assign ParamItemID
	token = strtok(NULL, pchSeps);
	if (NULL != token)
	{
		strcpy((char *)_pstProtParamItem->pchParamItemID, (char *)token);
	}
	// 3. Assign ParamItemName
	token = strtok(NULL, pchSeps);
	if (NULL != token)
	{
		strcpy((char *)_pstProtParamItem->pchParamItemName, (char *)token);
	}
	// 4. Assign ParamItemValue
	token = strtok(NULL, pchSeps);
	if (NULL != token)
	{
		strcpy((char *)_pstProtParamItem->pchParamItemValue, (char *)token);
	}
	// 5. Assign DataType
	token = strtok(NULL, pchSeps);
	if (NULL != token)
	{
		_pstProtParamItem->iItemType = GetDataTypeWord(token);
	}
	// 6. Assign DataLength
	token = strtok(NULL, pchSeps);
	if (NULL != token)
	{
		_pstProtParamItem->iItemLength = strtol(token, NULL, 10);
	}
	// 7. Assign bIsParamValueFixed
	token = strtok(NULL, pchSeps);
	if (NULL != token)
		_pstProtParamItem->bIsParamValueFixed = !strcmp(token, "√") ? 1 : 0;
	// 8. Assign bIsDataField
	token = strtok(NULL, pchSeps);
	if (NULL != token)
		_pstProtParamItem->bIsDataField = !strcmp(token, "√") ? 1 : 0;
	// 9. Assign bIsCheckSumAddField
	token = strtok(NULL, pchSeps);
	if (NULL != token)
		_pstProtParamItem->bIsCheckSumAddField = !strcmp(token, "√") ? 1 : 0;
	// 10. Assign bIsParamValueFixed
	token = strtok(NULL, pchSeps);
	if (NULL != token)
		_pstProtParamItem->bIsCheckSumSaveField = !strcmp(token, "√") ? 1 : 0;
	// 11. Assign iPosInCmdValue
	token = strtok(NULL, pchSeps);
	if (NULL != token)
		_pstProtParamItem->iPosInCmdValue = !strcmp(token, "无") ? -1 : strtol(token, NULL, 10);
	// 12. Assign iPosInCmdString
	_pstProtParamItem->iPosInCmdString = * piPosInArray;
	* piPosInArray += _pstProtParamItem->iItemLength;
}

void CCommonFunction::SetProtConfigStruct(pstProtocolConfig _pstProtocolConfig,
										  pstProtParamItem _pstProtParamItem,
										  int iParamIndex)
{
	memcpy(&_pstProtocolConfig->_pstProtParamItem[iParamIndex], _pstProtParamItem, sizeof(stProtParamItem));

	// 21. Assign pchCmdStringTemplate
	CString		strInnerString;

	ConvertToHexString((char *)_pstProtParamItem->pchParamItemValue,
						_pstProtParamItem->iItemType,
						_pstProtParamItem->iItemLength,
						strInnerString,
						DATA_LEFT_ALIGN,
						0x00,
						FALSE,
						TRUE);
	HexStrToHexArray(strInnerString,
					_pstProtParamItem->iItemType,
					_pstProtParamItem->iItemLength,
					_pstProtocolConfig->pchCmdStringTemplate + _pstProtParamItem->iPosInCmdString,
					0x00);
}

void CCommonFunction::GetGainConfigFromFile(CString strConfigFileName, pstAllGainConfig _pstAllGainConfig)
{
	LPTSTR				pStr = NULL;
	char				pchLineBuf[2 * MAX_LENGTH];
	CStdioFile			stdioFile;
//	stProtParamItem		_stProtParamItem;
	int					iGainCount = 0, iPosInArray = 0;

//	ZeroMemory(&_stProtParamItem, sizeof(stProtParamItem));
//	InitProtConfig(_pstProtocolConfig, MAX_PROT_PARAM_COUNT);

	// Open the configuration file
	if (!(stdioFile.Open(strConfigFileName, CFile::modeRead | CFile::typeText, NULL)))
		return;
	pStr = stdioFile.ReadString(pchLineBuf, 2 * MAX_LENGTH);
	while (pStr != NULL)
	{
		// If the first character is ";" or CR('\r', 0x0D) or LF('\n', 0x0A), then continue
		if (pchLineBuf[0] == ';' || pchLineBuf[0] == '\r' || pchLineBuf[0] == '\n')
		{
			pStr = stdioFile.ReadString(pchLineBuf, 2 * MAX_LENGTH);
			continue;
		}
		// parse the data, assign the struct: stProtocolConfig
		SetSingleGainConfigStruct(pchLineBuf, &_pstAllGainConfig->_pstSingleGainConfigInfo[iGainCount]);

		pStr = stdioFile.ReadString(pchLineBuf, 2 * MAX_LENGTH);

		iGainCount ++;
	}
	_pstAllGainConfig->iGainCount = iGainCount;

	stdioFile.Close();
}

void CCommonFunction::SetSingleGainConfigStruct(char* pchLineBuf,
												pstSingleGainConfigInfo _pstSingleGainConfigInfo)
{
	char		pchSeps[] = ",\t\n";
	char		*token = NULL;

	token = strtok(pchLineBuf, pchSeps);
	// 1. Assign gain value
	if (NULL != token)
	{
		_pstSingleGainConfigInfo->dblGainValue = strtod(token, NULL);
	}
	// 2. Assign bIsValid
	token = strtok(NULL, pchSeps);
	if (NULL != token)
	{
		_pstSingleGainConfigInfo->bIsValid = !strcmp(token, "√") ? TRUE : FALSE;
	}
	// 3. Assign pchValidInputPowerSeq
	token = strtok(NULL, pchSeps);
	if (NULL != token)
	{
		strcpy((char *)_pstSingleGainConfigInfo->pchValidInputPowerSeq, (char *)token);
	}
	// 4. Assign dblGainError
	token = strtok(NULL, pchSeps);
	if (NULL != token)
	{
		_pstSingleGainConfigInfo->dblGainError = strtod(token, NULL);
	}
	// 5. Assign dblNFError
	token = strtok(NULL, pchSeps);
	if (NULL != token)
	{
		_pstSingleGainConfigInfo->dblNFError = strtod(token, NULL);
	}
}

// old version for int array, array count not greater than 4.
VOID CCommonFunction::SaveData( HWND hWnd,
								CString strFileFullName,
								CString strColumnName,
								int * piFirstArray,
								int * piSecondArray,
								int * piThirdArray,
								int * piForthArray,
								int iArraySize,
								int iDataArrayCount,
								BOOL bAppend)
{
	FILE	*	pDataFile = NULL;
	int			iCounter;

	try
	{
		if (NULL == piFirstArray && iDataArrayCount >= 1)
			throw "第一个数组为空!";

		if (NULL == piSecondArray && iDataArrayCount >= 2)
			throw "第二个数组为空!";

		if (NULL == piThirdArray && iDataArrayCount >= 3)
			throw "第三个数组为空!";

		if (NULL == piForthArray && iDataArrayCount >= 4)
			throw "第四个数组为空!";

		if(bAppend)
			pDataFile = fopen(strFileFullName, "at");
		else
			pDataFile = fopen(strFileFullName, "wt");

		if(!pDataFile)
			return;

		// Write column name
		if (!strColumnName.IsEmpty())
			fprintf(pDataFile, "%s\n", strColumnName);

		// Write data
		for (iCounter = 0;iCounter < iArraySize;iCounter ++)
		{
			switch (iDataArrayCount)
			{
			case 0:
				break;
			case 1:
				fprintf(pDataFile, "%d\n", piFirstArray[iCounter]);
				break;
			case 2:
				fprintf(pDataFile, "%d,%d\n", piFirstArray[iCounter], piSecondArray[iCounter]);
				break;
			case 3:
				fprintf(pDataFile, "%d,%d,%d\n", piFirstArray[iCounter], piSecondArray[iCounter], piThirdArray[iCounter]);
				break;
			case 4:
				fprintf(pDataFile, "%d,%d,%d,%d\n", piFirstArray[iCounter], piSecondArray[iCounter], piThirdArray[iCounter], piForthArray[iCounter]);
				break;
			default :
				break;
			}
		}

		fclose(pDataFile);
	}
	catch(TCHAR* ptszErrorMsg)
	{
		fclose(pDataFile);

		MessageBox(hWnd, ptszErrorMsg, "Error", MB_OK | MB_ICONERROR);
	}
	catch(...)
	{
		fclose(pDataFile);

		MessageBox(hWnd, "Other exception occured!","FATAL",MB_OK|MB_ICONERROR);
	}
}

// old version for double array, array count not greater than 4.
VOID CCommonFunction::SaveData( HWND hWnd,
								CString strFileFullName,
								CString strColumnName,
								double * pdblFirstArray,
								double * pdblSecondArray,
								double * pdblThirdArray,
								double * pdblForthArray,
								int iArraySize,
								int iDataArrayCount,
								BOOL bAppend)
{
	FILE	*	pDataFile = NULL;
	int			iCounter;

	try
	{
		if (NULL == pdblFirstArray && iDataArrayCount >= 1)
			throw "第一个数组为空!";

		if (NULL == pdblSecondArray && iDataArrayCount >= 2)
			throw "第二个数组为空!";

		if (NULL == pdblThirdArray && iDataArrayCount >= 3)
			throw "第三个数组为空!";

		if (NULL == pdblForthArray && iDataArrayCount >= 4)
			throw "第四个数组为空!";

		if(bAppend)
			pDataFile = fopen(strFileFullName, "at");
		else
			pDataFile = fopen(strFileFullName, "wt");

		if(!pDataFile)
			return;

		// Write column name
		if (!strColumnName.IsEmpty())
			fprintf(pDataFile, "%s\n", strColumnName);

		// Write data
		for (iCounter = 0;iCounter < iArraySize;iCounter ++)
		{
			switch (iDataArrayCount)
			{
			case 0:
				break;
			case 1:
				fprintf(pDataFile, "%.8f\n", pdblFirstArray[iCounter]);
				break;
			case 2:
				fprintf(pDataFile, "%.8f,%.8f\n", pdblFirstArray[iCounter], pdblSecondArray[iCounter]);
				break;
			case 3:
				fprintf(pDataFile, "%.8f,%.8f,%.8f\n", pdblFirstArray[iCounter], pdblSecondArray[iCounter], pdblThirdArray[iCounter]);
				break;
			case 4:
				fprintf(pDataFile, "%.8f,%.8f,%.8f,%.8f\n", pdblFirstArray[iCounter], pdblSecondArray[iCounter], pdblThirdArray[iCounter], pdblForthArray[iCounter]);
				break;
			default :
				break;
			}
		}

		fclose(pDataFile);
	}
	catch(TCHAR* ptszErrorMsg)
	{
		fclose(pDataFile);

		MessageBox(hWnd, ptszErrorMsg, "Error", MB_OK | MB_ICONERROR);
	}
	catch(...)
	{
		fclose(pDataFile);

		MessageBox(hWnd, "Other exception occured!","FATAL",MB_OK|MB_ICONERROR);
	}
}

// For any double array
VOID CCommonFunction::SaveData( HWND hWnd,
								CString strFileFullName,
								CString strColumnName,
								DWORD * pdwArrayAddr,
								int iArraySize,
								int iDataArrayCount,
								BOOL bAppend)
{
	FILE	*	pDataFile = NULL;
	int			iSampleId = 0, iArrayId = 0;
	char		pchMsg[MAX_MSG_BUFFER] = {0x00};
	double	*	pdblArray = NULL;

	try
	{
		for (iArrayId = 0;iArrayId < iDataArrayCount;iArrayId ++)
		{
			if (NULL == pdwArrayAddr[iArrayId] && iDataArrayCount >= iArrayId + 1)
			{
				sprintf(pchMsg, "第%d个数组为空!", iArrayId + 1);
				throw pchMsg;
			}
		}

		if(bAppend)
			pDataFile = fopen(strFileFullName, "at");
		else
			pDataFile = fopen(strFileFullName, "wt");

		if(!pDataFile)
			return;

		// Write column name
		if (!strColumnName.IsEmpty())
			fprintf(pDataFile, "%s\n", strColumnName);

		// Write data to file
		for (iSampleId = 0;iSampleId < iArraySize;iSampleId ++)
		{
			for (iArrayId = 0;iArrayId < iDataArrayCount - 1;iArrayId ++)
			{
				pdblArray = (double *)pdwArrayAddr[iArrayId];
				fprintf(pDataFile, "%.8f,", pdblArray[iSampleId]);
			}
			// write the last item of current row
			pdblArray = (double *)pdwArrayAddr[iArrayId];
			fprintf(pDataFile, "%.8f", pdblArray[iSampleId]);
//			fseek(pDataFile, -1, SEEK_CUR);

			fprintf(pDataFile, "\n");
		}

		fclose(pDataFile);
	}
	catch(TCHAR* ptszErrorMsg)
	{
		fclose(pDataFile);

		MessageBox(hWnd, ptszErrorMsg, "Error", MB_OK | MB_ICONERROR);
	}
	catch(...)
	{
		fclose(pDataFile);

		MessageBox(hWnd, "Other exception occured!","FATAL",MB_OK|MB_ICONERROR);
	}
}

// For two-dimensions int array
VOID CCommonFunction::SaveIntData(HWND hWnd,
								CString strFileFullName,
								CString strColumnName,
								int piData[][MAX_VOA_NUMBER],
								int iRowCount,
								int iColCount,
								BOOL bAppend)
{
	FILE	*	pDataFile = NULL;
	int			iCounter;

	try
	{
		if(bAppend)
			pDataFile = fopen(strFileFullName, "at");
		else
			pDataFile = fopen(strFileFullName, "wt");

		if(!pDataFile)
			return;

		// Write column name
		if (!strColumnName.IsEmpty())
			fprintf(pDataFile, "%s\n", strColumnName);

		// Write data
		switch (iColCount)
		{
		case 2:
			for (iCounter = 0;iCounter < iRowCount;iCounter ++)
			{
				fprintf(pDataFile, "%d, %d\n",
						piData[iCounter][0],
						piData[iCounter][1]);
			}
			break;
		case 4:
			for (iCounter = 0;iCounter < iRowCount;iCounter ++)
			{
				fprintf(pDataFile, "%d, %d, %d, %d\n",
						piData[iCounter][0],
						piData[iCounter][1],
						piData[iCounter][2],
						piData[iCounter][3]);
			}
			break;
		case 6:
			for (iCounter = 0;iCounter < iRowCount;iCounter ++)
			{
				fprintf(pDataFile, "%d, %d, %d, %d, %d, %d\n",
						piData[iCounter][0],
						piData[iCounter][1],
						piData[iCounter][2],
						piData[iCounter][3],
						piData[iCounter][4],
						piData[iCounter][5]);
			}
			break;
		case 8:
			for (iCounter = 0;iCounter < iRowCount;iCounter ++)
			{
				fprintf(pDataFile, "%d, %d, %d, %d, %d, %d, %d, %d\n",
						piData[iCounter][0],
						piData[iCounter][1],
						piData[iCounter][2],
						piData[iCounter][3],
						piData[iCounter][4],
						piData[iCounter][5],
						piData[iCounter][6],
						piData[iCounter][7]);
			}
			break;
		case 9:
			for (iCounter = 0;iCounter < iRowCount;iCounter ++)
			{
				fprintf(pDataFile, "%d, %d, %d, %d, %d, %d, %d, %d, %d\n",
						piData[iCounter][0],
						piData[iCounter][1],
						piData[iCounter][2],
						piData[iCounter][3],
						piData[iCounter][4],
						piData[iCounter][5],
						piData[iCounter][6],
						piData[iCounter][7],
						piData[iCounter][8]);
			}
			break;
		default:
			break;
		}

		fclose(pDataFile);
	}
	catch(TCHAR* ptszErrorMsg)
	{
		fclose(pDataFile);

		MessageBox(hWnd, ptszErrorMsg, "Error", MB_OK | MB_ICONERROR);
	}
	catch(...)
	{
		fclose(pDataFile);

		MessageBox(hWnd, "Other exception occured!","FATAL",MB_OK|MB_ICONERROR);
	}
}

VOID CCommonFunction::SaveMatrix(HWND hWnd,
								CString strFileFullName,
								pstMatrix _pstMatrix,
								double **pdblMatrixData,
								BOOL bAppend)
{
	FILE	*	pDataFile = NULL;
	int			iRowId = 0, iColId = 0;
	double	*	pdblArray = NULL;
	CString		strTemp;

	try
	{
		if(bAppend)
			pDataFile = fopen(strFileFullName, "at");
		else
			pDataFile = fopen(strFileFullName, "wt");

		if(!pDataFile)
			return;

		// Write column name
		strTemp.Format("%s", _pstMatrix->pchColName);
		if (!strTemp.IsEmpty())
			fprintf(pDataFile, "%s\n", _pstMatrix->pchColName);

		// Write data to file
		for (iRowId = 0;iRowId < _pstMatrix->iRowCount;iRowId ++)
		{
			if (NULL != _pstMatrix->pdblFirstColData)
				fprintf(pDataFile, "%.2f%s,",
									_pstMatrix->pdblFirstColData[iRowId],
									_pstMatrix->pchRowUnit);
			for (iColId = 0;iColId < _pstMatrix->iColCount - 1;iColId ++)
			{
				fprintf(pDataFile, "%.8f,", pdblMatrixData[iRowId][iColId]);
			}
			// write the last item of current row
			fprintf(pDataFile, "%.8f", pdblMatrixData[iRowId][iColId]);
			fprintf(pDataFile, "\n");
		}

		fclose(pDataFile);
	}
	catch(TCHAR* ptszErrorMsg)
	{
		fclose(pDataFile);

		MessageBox(hWnd, ptszErrorMsg, "Error", MB_OK | MB_ICONERROR);
	}
	catch(...)
	{
		fclose(pDataFile);

		MessageBox(hWnd, "Other exception occured!","FATAL",MB_OK|MB_ICONERROR);
	}
}

BOOL CCommonFunction::AllocateResource(double **pArrayAddress,
									   DWORD dwArraySize,
									   BYTE btDefaultValue)
{
	int			i = 0;

	try
	{
		// allocate resource
		for (i = 0;i < (int)dwArraySize;i ++)
		{
			pArrayAddress[i] = (double *)VirtualAlloc(NULL,
													MAX_MATRIX_COL_COUNT * sizeof(double),
													MEM_RESERVE | MEM_COMMIT,
													PAGE_READWRITE);
			if (NULL == pArrayAddress[i])
				throw "";
			memset(pArrayAddress[i], btDefaultValue, dwArraySize * sizeof(double));
		}
	}
	catch(TCHAR* ptszErrorMsg)
	{
		ptszErrorMsg;
		// free resource
		FreeResource(pArrayAddress, dwArraySize);
		return FALSE;
	}

	return TRUE;
}

VOID CCommonFunction::FreeResource(double **pArrayAddress,
								   DWORD dwArraySize)
{
	int		i = 0;

	for (i = 0;i < (int)dwArraySize;i ++)
	{
		if (NULL != pArrayAddress[i])
		{
			VirtualFree(pArrayAddress[i], MAX_MATRIX_COL_COUNT * sizeof(double), MEM_RELEASE);
			pArrayAddress[i] = NULL;
		}
	}
}

VOID CCommonFunction::RegularizeArray(double * pdblFirstArray,
									  double * pdblSecondArray,
									  double * pdblThirdArray,
									  double * pdblForthArray,
									  int iArraySize,
									  int iRegularPointQuantity,
									  double dblStep,
									  int iArrayType,
									  double dblStartValue)
{
	int		i = 0, iNearestPos = 0, iLessEqualGreatFlag = 0;
	double	dblRegularAtten = 0,
			dblRegularDAC = 0,
			dblSlope = 0,
			dblIntercept = 0;

	switch (iArrayType)
	{
	case ARRAY_TYPE_ASCENDIND:
		for (i = 0;i < iRegularPointQuantity;i ++)
		{
			dblRegularDAC = dblStartValue + i * dblStep;
			iNearestPos = FindNearestPosInArray(pdblFirstArray,
												iArraySize,
												dblRegularDAC,
												&iLessEqualGreatFlag,
												iArrayType);

			if (iLessEqualGreatFlag == 0)
			{
				pdblThirdArray[i] = dblRegularDAC;
				pdblForthArray[i] = pdblSecondArray[iNearestPos];
			}
			// pdblFirstArray[iNearestPos] is less than dblRegularDAC
			else if (iLessEqualGreatFlag > 0)
			{
				if (iNearestPos == 0)
					ComputeLinearity(pdblFirstArray + iNearestPos, pdblSecondArray + iNearestPos, 2, &dblSlope, &dblIntercept);
				else
					ComputeLinearity(pdblFirstArray + iNearestPos - 1, pdblSecondArray + iNearestPos - 1, 2, &dblSlope, &dblIntercept);
				pdblThirdArray[i] = dblRegularDAC;
				pdblForthArray[i] = dblSlope * dblRegularDAC + dblIntercept;
			}
			// pdblFirstArray[iNearestPos] is greater than dblRegularDAC
			else if (iLessEqualGreatFlag < 0)
			{
				if (iNearestPos == iArraySize - 1)
					ComputeLinearity(pdblFirstArray + iNearestPos - 1, pdblSecondArray + iNearestPos - 1, 2, &dblSlope, &dblIntercept);
				else
					ComputeLinearity(pdblFirstArray + iNearestPos, pdblSecondArray + iNearestPos, 2, &dblSlope, &dblIntercept);
				pdblThirdArray[i] = dblRegularDAC;
				pdblForthArray[i] = dblSlope * dblRegularDAC + dblIntercept;
			}
		}
		break;
	case ARRAY_TYPE_DESCENDIND:
		for (i = iRegularPointQuantity - 1;i >= 0;i --)
		{
			dblRegularAtten = dblStartValue + i * dblStep;
			iNearestPos = FindNearestPosInArray(pdblFirstArray,
												iArraySize,
												dblRegularAtten,
												&iLessEqualGreatFlag,
												iArrayType);

			if (iLessEqualGreatFlag == 0)
			{
				pdblThirdArray[i] = dblRegularAtten;
				pdblForthArray[i] = pdblSecondArray[iNearestPos];
			}
			// pdblFirstArray[iNearestPos] is less than dblRegularAtten
			else if (iLessEqualGreatFlag > 0)
			{
				if (iNearestPos == iArraySize - 1)
					ComputeLinearity(pdblFirstArray + iNearestPos - 1, pdblSecondArray + iNearestPos - 1, 2, &dblSlope, &dblIntercept);
				else
					ComputeLinearity(pdblFirstArray + iNearestPos, pdblSecondArray + iNearestPos, 2, &dblSlope, &dblIntercept);
				pdblThirdArray[i] = dblRegularAtten;
				pdblForthArray[i] = dblSlope * dblRegularAtten + dblIntercept;
			}
			// pdblFirstArray[iNearestPos] is greater than dblRegularAtten
			else if (iLessEqualGreatFlag < 0)
			{
				if (iNearestPos == 0)
					ComputeLinearity(pdblFirstArray + iNearestPos, pdblSecondArray + iNearestPos, 2, &dblSlope, &dblIntercept);
				else
					ComputeLinearity(pdblFirstArray + iNearestPos - 1, pdblSecondArray + iNearestPos - 1, 2, &dblSlope, &dblIntercept);
				pdblThirdArray[i] = dblRegularAtten;
				pdblForthArray[i] = dblSlope * dblRegularAtten + dblIntercept;
			}
		}
		break;
	case ARRAY_TYPE_OUT_OF_ORDER:
		break;
	default:
		break;
	}
}

BOOL CCommonFunction::FindValidDataIndex(const double * pdblTargetArray,
										const int iArraySize,
										int iArrayType,
										const double dblMinValidData,
										const double dblMaxValidData,
										int * piValidDataStartID,
										int * piValidDataEndID)
{
	int		iLessEqualGreatFlag = 0;

	* piValidDataStartID = FindNearestPosInArray(pdblTargetArray,
										iArraySize,
										dblMinValidData,
										&iLessEqualGreatFlag,
										iArrayType);
	* piValidDataEndID = FindNearestPosInArray(pdblTargetArray,
										iArraySize,
										dblMaxValidData,
										&iLessEqualGreatFlag,
										iArrayType);

	return TRUE;
}

BOOL CCommonFunction::ReadMappingFile(CString strMappingFileName,
									  double * pdblFirstArray,
									  double * pdblSecondArray,
									  double * pdblThirdArray,
									  double * pdblForthArray,
									  int * piArraySize,
									  char * pchrrorMsg)
{
	char		pchLineBuf[2 * MAX_LENGTH];
	LPTSTR		pStr;
	CStdioFile	fileCSVMappingFile;
	char		pchMsg[MAX_MSG_BUFFER] = {0x00};

	try
	{
		// Open CSV file
		if (!(fileCSVMappingFile.Open(strMappingFileName, CFile::modeRead | CFile::typeText, NULL)))
		{
			//Cannot open CSV Mapping File
			sprintf(pchMsg, "Cannot open the file:\n%s", strMappingFileName);

			throw pchMsg;
		}

		* piArraySize = 0;
		// Read data from CSV file
		pStr = fileCSVMappingFile.ReadString(pchLineBuf, 2 * MAX_LENGTH);
		// Begin to process the first line
		while (NULL != pStr)
		{
			// If the first character is ";" or CR('\r', 0x0D) or LF('\n', 0x0A), then continue
			if (pchLineBuf[0] == ';' || pchLineBuf[0] == '\r' || pchLineBuf[0] == '\n')
			{
				pStr = fileCSVMappingFile.ReadString(pchLineBuf, 2 * MAX_LENGTH);
				continue;
			}
			ParseOneLineData(pchLineBuf, pdblFirstArray, pdblSecondArray, pdblThirdArray, pdblForthArray, * piArraySize);

			pStr = fileCSVMappingFile.ReadString(pchLineBuf, 2 * MAX_LENGTH);

			(* piArraySize)++;
		}
		// If the data file is null
		if (0 == * piArraySize)
		{
			sprintf(pchMsg, "文件格式错误或文件为空，未能获取数据!", strMappingFileName);

			throw pchMsg;
		}
		else
		{
			fileCSVMappingFile.Close();

			sprintf(pchMsg, "获取成功!", strMappingFileName);
			pchrrorMsg = pchMsg;
			return TRUE;
		}
	}
	catch(TCHAR* ptszErrorMsg)
	{
		fileCSVMappingFile.Close();

		pchrrorMsg = ptszErrorMsg;
		return FALSE;
	}
}

BOOL CCommonFunction::ParseOneLineData(char* pchLineBuf,
										double * pdblFirstArray,
										double * pdblSecondArray,
										double * pdblThirdArray,
										double * pdblForthArray,
										int iCurArrayIndex)
{
	char		pchSeps[] = ",\t\n";
	char		*token = NULL;
	CString		strTemp;
	int			i = 0;

	token = strtok(pchLineBuf, pchSeps);

	if (NULL != token && NULL != pdblFirstArray)
	{
		pdblFirstArray[iCurArrayIndex] = strtod(token, NULL);
	}

	token = strtok(NULL, pchSeps);
	if (NULL != token && NULL != pdblSecondArray)
	{
		pdblSecondArray[iCurArrayIndex] = strtod(token, NULL);
	}

	token = strtok(NULL, pchSeps);
	if (NULL != token && NULL != pdblThirdArray)
	{
		pdblThirdArray[iCurArrayIndex] = strtod(token, NULL);
	}

	token = strtok(NULL, pchSeps);
	if (NULL != token && NULL != pdblForthArray)
	{
		pdblForthArray[iCurArrayIndex] = strtod(token, NULL);
	}

	return TRUE;
}

int CCommonFunction::FindNearestPosInArray(const double * pdblTargetArray,
										   int iArraySize,
										   double dblMatchValue,
										   int * piLessEqualGreatFlag,
										   int iArrayType)
{
	int			i = 0,
				iRetVal = -1;
	double		dblMinValue = 10000,
				dblDifference = 0.0;

	switch (iArrayType)
	{
	case ARRAY_TYPE_ASCENDIND:
		for (i = 0;i < iArraySize;i ++)
		{
			if (pdblTargetArray[i] == dblMatchValue)
			{
				* piLessEqualGreatFlag = 0;
				return i;
			}
			else if (pdblTargetArray[i] > dblMatchValue)
			{
				if (i == 0)
				{
					* piLessEqualGreatFlag = 1;
					return i;
				}
				if (fabs(pdblTargetArray[i] - dblMatchValue) > fabs(pdblTargetArray[i - 1] - dblMatchValue))
				{
					* piLessEqualGreatFlag = -1;
					return i - 1;
				}
				else
				{
					* piLessEqualGreatFlag = 1;
					return i;
				}
			}
		}
		* piLessEqualGreatFlag = -1;

		return i - 1;
		break;
	case ARRAY_TYPE_DESCENDIND:
		for (i = 0;i < iArraySize;i ++)
		{
			if (pdblTargetArray[i] == dblMatchValue)
			{
				* piLessEqualGreatFlag = 0;
				return i;
			}
			else if (pdblTargetArray[i] < dblMatchValue)
			{
				if (i == 0)
				{
					* piLessEqualGreatFlag = 1;
					return i;
				}
				if (fabs(pdblTargetArray[i] - dblMatchValue) > fabs(pdblTargetArray[i - 1] - dblMatchValue))
				{
					* piLessEqualGreatFlag = 1;
					return i - 1;
				}
				else
				{
					* piLessEqualGreatFlag = -1;
					return i;
				}
			}
		}
		* piLessEqualGreatFlag = 1;

		return i - 1;
		break;
	case ARRAY_TYPE_OUT_OF_ORDER:
		for (i = 0;i < iArraySize;i ++)
		{
			dblDifference = pdblTargetArray[i] - dblMatchValue;
			if (dblDifference == 0)
			{
				* piLessEqualGreatFlag = 0;
				return i;
			}
			else
			{
				if (fabs(dblDifference) < dblMinValue)
				{
					dblMinValue = dblDifference;
					if (dblDifference > 0)
						* piLessEqualGreatFlag = 1;
					else
						* piLessEqualGreatFlag = -1;
					iRetVal = i;
				}
			}
		}
		break;
	default:
		break;
	}

	return iRetVal;
}

double CCommonFunction::FindMostFitValue(double * pdblFirstArray,
										double * pdblSecondArray,
										int iArraySize,
										int iArrayType,
										double dblMatchValue,
										BOOL bOnlyFindNearestValue)
{
	int		iLessEqualGreatFlag = 0,
			iNearestPos = -1;
	double	dblSlope = 0,
			dblIntercept = 0;

	iNearestPos = FindNearestPosInArray(pdblFirstArray,
										iArraySize,
										dblMatchValue,
										&iLessEqualGreatFlag,
										iArrayType);
	if (iNearestPos == -1)
		return -10.0;

	if (bOnlyFindNearestValue || iLessEqualGreatFlag == 0)
		return pdblSecondArray[iNearestPos];
	else
	{
		// do linearly fit
		if (iLessEqualGreatFlag > 0)
			ComputeLinearity(pdblFirstArray + iNearestPos - 1, pdblSecondArray + iNearestPos - 1, 2, &dblSlope, &dblIntercept);
		else
			ComputeLinearity(pdblFirstArray + iNearestPos, pdblSecondArray + iNearestPos, 2, &dblSlope, &dblIntercept);
		return dblMatchValue * dblSlope + dblIntercept;
	}
}

double CCommonFunction::ComputeCurrentChannelWL(pstModulePara _pstModulePara, int iChannelID)
{
	if (_pstModulePara->dblEndFreqWB >= _pstModulePara->dblStartFreqWB)
		return LIGHT / (_pstModulePara->dblStartFreqWB + (iChannelID * 100));
	else
		return LIGHT / (_pstModulePara->dblStartFreqWB - (iChannelID * 100));
}

VOID CCommonFunction::GetFileExtName(CString strFileFullName, CString & strFileExtName, CString & strFileName)
{
	int			iStrLen = 0, iDotPos = -1, iSolidusPos = -1;

	iStrLen = strFileFullName.GetLength();
	iDotPos = strFileFullName.ReverseFind('.');
	strFileExtName = strFileFullName.Right(iStrLen - iDotPos - 1);
	strFileFullName.Delete(iDotPos, iStrLen - iDotPos);

	iStrLen = iDotPos;
	iSolidusPos = strFileFullName.ReverseFind('\\');
	strFileName = strFileFullName.Right(iStrLen - iSolidusPos - 1);
}

VOID CCommonFunction::SplitString(CString strSourceString,
								int iTargetStrPos,
								LPCSTR lpcstrTargetStr)
{
	char		pchSeps[] = " ,:\t\n";
	char		*token = NULL;
	CString		strTemp;
	int			i = 0;

	if (iTargetStrPos == 0)
		return;

	token = strtok(strSourceString.GetBuffer(MAX_PATH), pchSeps);
	if (iTargetStrPos == 1)
	{
		strcpy((char *)lpcstrTargetStr, (char *)token);
		return;
	}
	for (i = 1;i < iTargetStrPos;i ++)
	{
		if (NULL == token)
			break;
		token = strtok(NULL, pchSeps);
	}

	if (NULL != token)
		strcpy((char *)lpcstrTargetStr, (char *)token);
}

CString CCommonFunction::GetFilePath(CString strFileFullName)
{
	int		iPosition;
	CString strFilePath;

	iPosition = strFileFullName.ReverseFind('\\');
	strFilePath = strFileFullName.Left(iPosition);

	return strFilePath;
}

BOOL CCommonFunction::GetKeyValueFromFile(const CString strProtParamFullName,
										  CMapStringToString &mapProtParam,
										  CListBox & ctrlboxAvaliParam)
{
	char		pchSeps[] = ",\t\n";
	char		*token1 = NULL, *token2 = NULL;
	LPTSTR		pStr = NULL;
	char		pchLineBuf[2 * MAX_LENGTH];
	CStdioFile	stdioFile;

	// 1. Open file
	if (!(stdioFile.Open(strProtParamFullName, CFile::modeRead | CFile::typeText, NULL)))
		return FALSE;
	// 2. Read lines
	pStr = stdioFile.ReadString(pchLineBuf, 2 * MAX_LENGTH);
	while (pStr != NULL)
	{
		// If the first character is ";" or CR('\r', 0x0D) or LF('\n', 0x0A), then continue
		if (pchLineBuf[0] == ';' || pchLineBuf[0] == '\r' || pchLineBuf[0] == '\n')
		{
			pStr = stdioFile.ReadString(pchLineBuf, 2 * MAX_LENGTH);
			continue;
		}
		token1 = strtok(pchLineBuf, pchSeps);
		token2 = strtok(NULL, pchSeps);
		// 3. Assign the map
		mapProtParam.SetAt(token2, token1);
		pStr = stdioFile.ReadString(pchLineBuf, 2 * MAX_LENGTH);
	}
	stdioFile.Close();

	return TRUE;
}

BOOL CCommonFunction::GetStringFromFile(const CString strProtConfigFullName,
										CListBox * pboxCtrlAvailProt,
										CComboBox * pcomboboxProtocolType)
{
	LPTSTR		pStr = NULL;
	char		pchLineBuf[2 * MAX_LENGTH];
	CStdioFile	stdioFile;
	CString		strLine;

	if (pboxCtrlAvailProt != NULL)
		((CListBox *)pboxCtrlAvailProt)->ResetContent();
	if (pcomboboxProtocolType != NULL)
		((CComboBox *)pcomboboxProtocolType)->ResetContent();
	// 1. Open file
	if (!(stdioFile.Open(strProtConfigFullName, CFile::modeRead | CFile::typeText, NULL)))
		return FALSE;
	// 2. Read lines
	pStr = stdioFile.ReadString(pchLineBuf, 2 * MAX_LENGTH);
	while (pStr != NULL)
	{
		// If the first character is ";" or CR('\r', 0x0D) or LF('\n', 0x0A), then continue
		if (pchLineBuf[0] == ';' || pchLineBuf[0] == '\r' || pchLineBuf[0] == '\n')
		{
			pStr = stdioFile.ReadString(pchLineBuf, 2 * MAX_LENGTH);
			continue;
		}
		strLine.Format("%s", pchLineBuf);
		strLine.TrimLeft();
		strLine.TrimRight();
		// 3. Assign the map
		if (pboxCtrlAvailProt != NULL)
			((CListBox *)pboxCtrlAvailProt)->AddString(strLine);
		if (pcomboboxProtocolType != NULL)
			((CComboBox *)pcomboboxProtocolType)->AddString(strLine);
		// Read next lines
		pStr = stdioFile.ReadString(pchLineBuf, 2 * MAX_LENGTH);
	}
	stdioFile.Close();

	return TRUE;
}

int CCommonFunction::GetArrayIndex(DWORD * pdwArray, DWORD dwArraySize, int iArrayType, double dblMatchValue)
{
	int		iTargetArrayIndex = -1;
	DWORD	dwCounter = 0,
			dwMatchValue = 0;

	double	dblTemp;
	double	dblMinDiff;

	dwMatchValue = (DWORD)floor(dblMatchValue + 0.5);
	// find the match value in target array
	switch (iArrayType)
	{
	case ARRAY_TYPE_ASCENDIND:
		for (dwCounter = 0; dwCounter < dwArraySize;dwCounter ++)
		{
			if(*(pdwArray + dwCounter) > dwMatchValue)
			{
				iTargetArrayIndex = (int)(dwCounter - 1);
				break;
			}
		}
		break;
	case ARRAY_TYPE_DESCENDIND:
		for (dwCounter = 0; dwCounter < dwArraySize;dwCounter ++)
		{
			if(*(pdwArray + dwCounter) < dwMatchValue)
			{
				iTargetArrayIndex = (int)(dwCounter - 1);
				break;
			}
		}
		break;
	case ARRAY_TYPE_OUT_OF_ORDER:
		dblMinDiff = (int)*pdwArray;
		for (dwCounter = 0; dwCounter < dwArraySize;dwCounter ++)
		{
			if(*(pdwArray + dwCounter) == dwMatchValue)
			{
				iTargetArrayIndex = (int)dwCounter;
				break;
			}
			else
			{
				dblTemp = abs(*(pdwArray + dwCounter) - dwMatchValue);
				if (dblTemp < dblMinDiff)
				{
					dblMinDiff = dblTemp;
					iTargetArrayIndex = (int)dwCounter;
				}
			}
		}
		break;
	default:
		break;
	}

	return iTargetArrayIndex;
}

BOOL CCommonFunction::ReadWritePDSlopeIntercept(BOOL bIsRead,
												CString strConfigFileName,
												pstPDSlopeIntercept _pstPDSlopeIntercept,
												int iTemperType,
												int iPDChIndex)
{
	CString		strValue,
				strSection,
				strKeyName,
				strTemperID;
	WORD		wBufferSize = MAX_MSG_BUFFER;
	int			iTemperID = 0;

	strSection.Format("PD%02d", iPDChIndex + 1);
	strTemperID = GetTemperatureIdentifier(iTemperType);
	if(!bIsRead)
	{
//****************************************************************//
		// Write PDSlopeIntercept
		// Save temperature
		strKeyName.Format("%sTemperature", strTemperID);
		strValue.Format("%.3f", _pstPDSlopeIntercept->pdblCalibTemperature[iTemperType]);
		WritePrivateProfileString(strSection, strKeyName, strValue, strConfigFileName);
		// Save slope
		strKeyName.Format("%sSlope", strTemperID);
		strValue.Format("%.3f", _pstPDSlopeIntercept->pdblSlope[iTemperType]);
		WritePrivateProfileString(strSection, strKeyName, strValue, strConfigFileName);
		// Save intercept
		strKeyName.Format("%sIntercept", strTemperID);
		strValue.Format("%.3f", _pstPDSlopeIntercept->pdblIntercept[iTemperType]);
		WritePrivateProfileString(strSection, strKeyName, strValue, strConfigFileName);
		// Save calib time
		strKeyName.Format("%sCalibTime", strTemperID);
		strValue.Format("%.3f", _pstPDSlopeIntercept->pchCalibTime[iTemperType]);
		WritePrivateProfileString(strSection, strKeyName, strValue, strConfigFileName);
	}
	else
	{
//****************************************************************//
		// Read PDSlopeIntercept
		for (iTemperID = 0;iTemperID < MAX_TEMPERATURE_TYPE;iTemperID ++)
		{
			strTemperID = GetTemperatureIdentifier(iTemperID);
			// Read temperature
			strKeyName.Format("%sTemperature", strTemperID);
			GetPrivateProfileString(strSection, strKeyName, "", strValue.GetBuffer(wBufferSize), wBufferSize, strConfigFileName);
			_pstPDSlopeIntercept->pdblCalibTemperature[iTemperID] = atof(strValue);
			// Read slope
			strKeyName.Format("%sSlope", strTemperID);
			GetPrivateProfileString(strSection, strKeyName, "", strValue.GetBuffer(wBufferSize), wBufferSize, strConfigFileName);
			_pstPDSlopeIntercept->pdblSlope[iTemperID] = atof(strValue);
			// Read intercept
			strKeyName.Format("%sIntercept", strTemperID);
			GetPrivateProfileString(strSection, strKeyName, "", strValue.GetBuffer(wBufferSize), wBufferSize, strConfigFileName);
			_pstPDSlopeIntercept->pdblIntercept[iTemperID] = atof(strValue);
			// Read calib time
			strKeyName.Format("%sCalibTime", strTemperID);
			GetPrivateProfileString(strSection, strKeyName, "", strValue.GetBuffer(wBufferSize), wBufferSize, strConfigFileName);
			strcpy((char *)_pstPDSlopeIntercept->pchCalibTime[iTemperID], strValue);
		}
	}

	return TRUE;
}

BOOL CCommonFunction::ReadWritePDVerifyError(BOOL bIsRead,
											CString strConfigFileName,
											pstPDVerifyError _pstPDVerifyError,
											int iTemperType,
											int iPDChIndex)
{
	CString		strValue,
				strSection,
				strKeyName,
				strTemperID;
	WORD		wBufferSize = MAX_MSG_BUFFER;
	int			iTemperID = 0;

	strSection.Format("PD%02d", iPDChIndex + 1);
	strTemperID = GetTemperatureIdentifier(iTemperType);
	if(!bIsRead)
	{
//****************************************************************//
		// Write PD Verify Error
		// Save temperature
		strKeyName.Format("%sTemperature", strTemperID);
		strValue.Format("%.3f", _pstPDVerifyError->pdblVerifyTemperature[iTemperType]);
		WritePrivateProfileString(strSection, strKeyName, strValue, strConfigFileName);
		// Save Verify Error
		strKeyName.Format("%sVerifyError", strTemperID);
		strValue.Format("%.3f", _pstPDVerifyError->pdblVerifyError[iTemperType]);
		WritePrivateProfileString(strSection, strKeyName, strValue, strConfigFileName);
		// Save verify time
		strKeyName.Format("%sVerifyTime", strTemperID);
		strValue.Format("%s", _pstPDVerifyError->pchVerifyTime[iTemperType]);
		WritePrivateProfileString(strSection, strKeyName, strValue, strConfigFileName);
	}
	else
	{
//****************************************************************//
		// Read PD Verify Error
		for (iTemperID = 0;iTemperID < MAX_TEMPERATURE_TYPE;iTemperID ++)
		{
			strTemperID = GetTemperatureIdentifier(iTemperID);
			// Read temperature
			strKeyName.Format("%sTemperature", strTemperID);
			GetPrivateProfileString(strSection, strKeyName, "", strValue.GetBuffer(wBufferSize), wBufferSize, strConfigFileName);
			_pstPDVerifyError->pdblVerifyTemperature[iTemperID] = atof(strValue);
			// Read Verify Error
			strKeyName.Format("%sVerifyError", strTemperID);
			GetPrivateProfileString(strSection, strKeyName, "", strValue.GetBuffer(wBufferSize), wBufferSize, strConfigFileName);
			_pstPDVerifyError->pdblVerifyError[iTemperID] = atof(strValue);
			// Read verify time
			strKeyName.Format("%sVerifyTime", strTemperID);
			GetPrivateProfileString(strSection, strKeyName, "", strValue.GetBuffer(wBufferSize), wBufferSize, strConfigFileName);
			strcpy((char *)_pstPDVerifyError->pchVerifyTime[iTemperID], strValue);
		}
	}

	return TRUE;
}

BOOL CCommonFunction::ReadWriteVOAMinILDAC( BOOL bIsRead,
											CString strConfigFileName,
											pstSingleVOAMinILDAC _pstSingleVOAMinILDAC,
											pstVOAPara _pstVOAPara,
											int iVOAChIndex)
{
	CString		strValue,
				strSection,
				strKeyName,
				strTemperID;
	WORD		wBufferSize = MAX_MSG_BUFFER;
	int			iTemperID = 0;

	strTemperID = GetTemperatureIdentifier(_pstVOAPara->iCurCalibTemperType);
	if(!bIsRead)
	{
//****************************************************************//
		strSection.Format("VOA%02d", _pstVOAPara->iVOAChIndex + 1);
		// Write SingleVOAMinILDAC
		// Save temperature
		strKeyName.Format("%sTemperature", strTemperID);
		strValue.Format("%.2f", _pstSingleVOAMinILDAC->pdblCalibTemperature[_pstVOAPara->iCurCalibTemperType]);
		WritePrivateProfileString(strSection, strKeyName, strValue, strConfigFileName);
		// Save MinIL
		strKeyName.Format("%sMinIL", strTemperID);
		strValue.Format("%.8f", _pstSingleVOAMinILDAC->pdblMinIL[_pstVOAPara->iCurCalibTemperType]);
		WritePrivateProfileString(strSection, strKeyName, strValue, strConfigFileName);
		// Save intercept
		strKeyName.Format("%sDACAtMinIL", strTemperID);
		strValue.Format("%.2f", _pstSingleVOAMinILDAC->pdblDACAtMinIL[_pstVOAPara->iCurCalibTemperType]);
		WritePrivateProfileString(strSection, strKeyName, strValue, strConfigFileName);
		// Save calib time
		strKeyName.Format("%sCalibTime", strTemperID);
		strValue.Format("%.2f", _pstSingleVOAMinILDAC->pchCalibTime[_pstVOAPara->iCurCalibTemperType]);
		WritePrivateProfileString(strSection, strKeyName, strValue, strConfigFileName);
	}
	else
	{
//****************************************************************//
		strSection.Format("VOA%02d", iVOAChIndex + 1);
		// Read SingleVOAMinILDAC
		for (iTemperID = 0;iTemperID < MAX_TEMPERATURE_TYPE;iTemperID ++)
		{
			strTemperID = GetTemperatureIdentifier(iTemperID);
			// Read temperature
			strKeyName.Format("%sTemperature", strTemperID);
			GetPrivateProfileString(strSection, strKeyName, "", strValue.GetBuffer(wBufferSize), wBufferSize, strConfigFileName);
			_pstSingleVOAMinILDAC->pdblCalibTemperature[iTemperID] = atof(strValue);
			// Read MinIL
			strKeyName.Format("%sMinIL", strTemperID);
			GetPrivateProfileString(strSection, strKeyName, "", strValue.GetBuffer(wBufferSize), wBufferSize, strConfigFileName);
			_pstSingleVOAMinILDAC->pdblMinIL[iTemperID] = atof(strValue);
			// Read DACAtMinIL
			strKeyName.Format("%sDACAtMinIL", strTemperID);
			GetPrivateProfileString(strSection, strKeyName, "", strValue.GetBuffer(wBufferSize), wBufferSize, strConfigFileName);
			_pstSingleVOAMinILDAC->pdblDACAtMinIL[iTemperID] = atof(strValue);
			// Read calib time
			strKeyName.Format("%sCalibTime", strTemperID);
			GetPrivateProfileString(strSection, strKeyName, "", strValue.GetBuffer(wBufferSize), wBufferSize, strConfigFileName);
			strcpy((char *)_pstSingleVOAMinILDAC->pchCalibTime[iTemperID], strValue);
		}
	}

	return TRUE;
}

BOOL CCommonFunction::ReadWriteVOAVerifyError( BOOL bIsRead,
											CString strConfigFileName,
											pstVOAVerifyError _pstVOAVerifyError,
											pstVOAPara _pstVOAPara,
											int iVOAChIndex)
{
	CString		strValue,
				strSection,
				strKeyName,
				strTemperID;
	WORD		wBufferSize = MAX_MSG_BUFFER;
	int			iTemperID = 0;

	strTemperID = GetTemperatureIdentifier(_pstVOAPara->iCurCalibTemperType);
	if(!bIsRead)
	{
//****************************************************************//
		strSection.Format("VOA%02d", _pstVOAPara->iVOAChIndex);
		// Write SingleVOAMinILDAC
		// Save temperature
		strKeyName.Format("%sTemperature", strTemperID);
		strValue.Format("%.2f", _pstVOAVerifyError->pdblVerifyTemperature[_pstVOAPara->iCurCalibTemperType]);
		WritePrivateProfileString(strSection, strKeyName, strValue, strConfigFileName);
		// Save AttenModeError
		strKeyName.Format("%sAttenModeError", strTemperID);
		strValue.Format("%.8f", _pstVOAVerifyError->pdblAttenModeError[_pstVOAPara->iCurCalibTemperType]);
		WritePrivateProfileString(strSection, strKeyName, strValue, strConfigFileName);
		// Save PowerModeError
		strKeyName.Format("%sPowerModeError", strTemperID);
		strValue.Format("%.2f", _pstVOAVerifyError->pdblPowerModeError[_pstVOAPara->iCurCalibTemperType]);
		WritePrivateProfileString(strSection, strKeyName, strValue, strConfigFileName);
		// Save verify time
		strKeyName.Format("%sVerifyTime", strTemperID);
		strValue.Format("%.2f", _pstVOAVerifyError->pchVerifyTime[_pstVOAPara->iCurCalibTemperType]);
		WritePrivateProfileString(strSection, strKeyName, strValue, strConfigFileName);
	}
	else
	{
//****************************************************************//
		strSection.Format("VOA%02d", iVOAChIndex);
		// Read SingleVOAMinILDAC
		for (iTemperID = 0;iTemperID < MAX_TEMPERATURE_TYPE;iTemperID ++)
		{
			strTemperID = GetTemperatureIdentifier(iTemperID);
			// Read temperature
			strKeyName.Format("%sTemperature", strTemperID);
			GetPrivateProfileString(strSection, strKeyName, "", strValue.GetBuffer(wBufferSize), wBufferSize, strConfigFileName);
			_pstVOAVerifyError->pdblVerifyTemperature[iTemperID] = atof(strValue);
			// Read AttenModeError
			strKeyName.Format("%sAttenModeError", strTemperID);
			GetPrivateProfileString(strSection, strKeyName, "", strValue.GetBuffer(wBufferSize), wBufferSize, strConfigFileName);
			_pstVOAVerifyError->pdblAttenModeError[iTemperID] = atof(strValue);
			// Read PowerModeError
			strKeyName.Format("%sPowerModeError", strTemperID);
			GetPrivateProfileString(strSection, strKeyName, "", strValue.GetBuffer(wBufferSize), wBufferSize, strConfigFileName);
			_pstVOAVerifyError->pdblPowerModeError[iTemperID] = atof(strValue);
			// Read verify time
			strKeyName.Format("%sVerifyTime", strTemperID);
			GetPrivateProfileString(strSection, strKeyName, "", strValue.GetBuffer(wBufferSize), wBufferSize, strConfigFileName);
			strcpy((char *)_pstVOAVerifyError->pchVerifyTime[iTemperID], strValue);
		}
	}

	return TRUE;
}

CString CCommonFunction::GetComboBoxString(CComboBox& pctrlComboBox, int iSelItem)
{
	CString		strSelItem;

	pctrlComboBox.GetLBText(iSelItem, strSelItem);

	return strSelItem;
}

VOID CCommonFunction::SetComboBoxSel(CComboBox& pctrlComboBox, CString strFindStr, int& piCurSel)
{
	piCurSel = pctrlComboBox.FindString(-1, strFindStr);
}

BOOL CCommonFunction::ComputeAverage(double * pdblTargetArray,
									int iArraySize,
									double * pdblAverage)
{
	double	dblTotalValue = 0;
	int		i = 0;

	for (i = 0;i < iArraySize;i ++)
	{
		dblTotalValue += pdblTargetArray[i];
	}
	*pdblAverage = dblTotalValue / iArraySize;

	return TRUE;
}

BOOL CCommonFunction::WritePrivateProfileInt(LPCTSTR lpAppName, LPCTSTR lpKeyName, int iValue, LPCTSTR lpFileName)
{
	CString		strValue("");

	strValue.Format("%d", iValue);

	return WritePrivateProfileString(lpAppName, lpKeyName, strValue, lpFileName);
}
