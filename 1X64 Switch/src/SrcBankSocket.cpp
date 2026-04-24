// SrcBankSocket.cpp : implementation file
//

#include "stdafx.h"
#include "SrcBankSocket.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSrcBankSocket

CSrcBankSocket::CSrcBankSocket()
{
	m_nBytesTotal = 0;
	ZeroMemory(m_pszRespondMsg, 1024);

	m_nReceiveCount = 0;
	InitLogFile(TRUE, 0);
}

CSrcBankSocket::~CSrcBankSocket()
{
}


// Do not edit the following lines, which are needed by ClassWizard.
#if 0
BEGIN_MESSAGE_MAP(CSrcBankSocket, CSocket)
	//{{AFX_MSG_MAP(CSrcBankSocket)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
#endif	// 0

/////////////////////////////////////////////////////////////////////////////
// CSrcBankSocket member functions
void CSrcBankSocket::OnReceive(int nErrorCode) 
{
	//数据包开始符号为*号, 结束符号为#号
	CHAR	pszErrorMsg[1024];
	int		dwBytesReceived = 0;
	int		nSocketSN1 = 0, nSocketSN2 = 0;
	CString strNewRes = "", strMsg = "", strLog = "";
	CString strRecvMsg = "", strTmp = "";
	m_bFunctionOK = FALSE;
	try
	{
		if(m_nReceiveCount%100 == 0)
		{
			InitLogFile(FALSE, m_nReceiveCount/100);
		}

		m_nReceiveCount++;

		strTmp.Format("//请求数: %d", m_nReceiveCount);
		////m_cLogOnReceive.StatusOut(strTmp);

		ZeroMemory(pszErrorMsg, 1024);

		ZeroMemory(m_pszRecvTemp, 1024);
		dwBytesReceived = Receive(&m_pszRecvTemp, 1024);

		////m_cLogOnReceive.StatusOut(m_pszRecvTemp);

		if(dwBytesReceived == SOCKET_ERROR)
		{
			strMsg.Format("集成光源 Socket 接收数据失败: SOCKET_ERROR");
			////m_cLogOnReceive.StatusOut(strMsg);

			throw "";
		}

		if(dwBytesReceived == 0)
		{
			strLog.Format("集成光源 Socket 接收数据0字节");
			////m_cLogOnReceive.StatusOut(strLog);

			throw "";
		}

		//接收长度已超出存储空间
		if(m_nBytesTotal + dwBytesReceived > 1024)
		{
			strMsg.Format("集成光源 Socket 接收数据已超出1024字节: %d", dwBytesReceived);
			////m_cLogOnReceive.StatusOut(strMsg);

			ZeroMemory(m_pszRespondMsg, 1024);
			m_nBytesTotal = 0;

			throw "";
		}

		//接收到的第1部分数据没有*号, 为错误数据, 不处理
		if((m_nBytesTotal == 0) && !strstr(m_pszRecvTemp, "*"))
		{
			strLog.Format("集成光源服务器发送的返回信息没有*号, 为错误数据, 不处理");
			////m_cLogOnReceive.StatusOut(strLog);

			throw "";
		}

		//将本次接收数据与之前没有接收完的数据拼接
		memcpy(&m_pszRespondMsg[m_nBytesTotal], m_pszRecvTemp, dwBytesReceived);
		m_nBytesTotal += dwBytesReceived;

		strRecvMsg = m_pszRespondMsg;

		//最后一个字符为#号, 为一个完整的数据包, 处理返回信息
		if(strRecvMsg.Right(1) == "#")
		{
			if(!AnalysisMsg(strRecvMsg, &strNewRes, pszErrorMsg))	//消息包无法处理
			{
				//消息包无法处理则全部清空
				ZeroMemory(m_pszRespondMsg, 1024);
				m_nBytesTotal = 0;

				throw "";
			}

			/*
			//Socket格式正确
			if (strNewRes.Find("UpdateWL") != -1)
			{
				//SendMessage(hWnd, WM_UPDATEWLLIST, (DWORD)strNewRes.GetBuffer(strNewRes.GetLength()), NULL);
			}
			else
			{
				//SendMessage(hWnd, WM_SETWLFINISHED,(DWORD)strNewRes.GetBuffer(strNewRes.GetLength()), NULL);
			}
			*/
			if (strNewRes.Find("Error") != -1)
			{
				m_bFunctionOK = FALSE;
			}
			else
			{
				m_bFunctionOK = TRUE;
			}
			m_strError.Format("%s", &m_pszRespondMsg);

			//已处理完消息包中所有信息, 清空存储空间
			ZeroMemory(m_pszRespondMsg, 1024);
			m_nBytesTotal = 0;
		}
		else
		{
			if(strRecvMsg.Find("#") != -1)	//已经接收到*号和#号, 且最后1个字符不是#号, 可能最后1个消息包还没有接收完
			{
				//先处理前面已经接收完的消息包
				strTmp = strRecvMsg;
				strRecvMsg = strTmp.Left(strTmp.ReverseFind('#'));

				if(!AnalysisMsg(strRecvMsg, &strNewRes, pszErrorMsg))	//消息包无法处理
				{
					//消息包无法处理则全部清空
					ZeroMemory(m_pszRespondMsg, 1024);
					m_nBytesTotal = 0;
					throw "";
				}

				/*
				//Socket格式正确
				if (strNewRes.Find("UpdateWL") != -1)
				{
//					PostMessage(hWnd, WM_UPDATEWLLIST, (DWORD)strNewRes.GetBuffer(strNewRes.GetLength()), NULL);
					//SendMessage(hWnd, WM_UPDATEWLLIST, (DWORD)strNewRes.GetBuffer(strNewRes.GetLength()), NULL);
				}
				else
				{
//					PostMessage(hWnd, WM_SETWLFINISHED,(DWORD)strNewRes.GetBuffer(strNewRes.GetLength()), NULL);
					//SendMessage(hWnd, WM_SETWLFINISHED,(DWORD)strNewRes.GetBuffer(strNewRes.GetLength()), NULL);
				}
				*/
				m_strError.Format("%s", &m_pszRespondMsg);
				if (strNewRes.Find("Error") != -1)
				{
					m_bFunctionOK = FALSE;
				}
				else
				{
					m_bFunctionOK = TRUE;
				}

				//去掉前面已经处理的消息包
				strTmp.Delete(0, strRecvMsg.GetLength());

				//将余下部分复制到存储空间
				m_nBytesTotal -= strRecvMsg.GetLength();
				ZeroMemory(m_pszRespondMsg, 1024);
				memcpy(m_pszRespondMsg, strTmp, strTmp.GetLength());

				if(strTmp.Left(1) != "*")	//余下部分不是以*号开始
				{
					strLog.Format("集成光源服务器发送的返回信息格式有误, 结束符后有字符 %s", strTmp);
					////m_cLogOnReceive.StatusOut(strLog);

					if(strTmp.Find("*") != -1)	//余下部分有*号且没有#号
					{
						//去掉*号前面的错误信息
						strTmp.Delete(0, strTmp.Find("*"));

						//将*及后面的部分复制到存储空间
						m_nBytesTotal = strTmp.GetLength();
						memcpy(m_pszRespondMsg, strTmp, strTmp.GetLength());

						//继续接收消息包剩下的部分
					}
					else						//余下部分没有*号, 全部为错误信息
					{
						strLog.Format("集成光源服务器发送的返回信息余下部分没有*号, 全部为错误信息: %s", strTmp);
						////m_cLogOnReceive.StatusOut(strLog);

						//余下部分全部清空
						m_nBytesTotal = 0;
						ZeroMemory(m_pszRespondMsg, 1024);
					}
				}
				else
				{
					//余下部分以*号开始, 但没有#号, 继续接收消息包剩下的部分
				}
			}
			else
			{
				//以*号开始, 但没有#号, 继续接收消息包剩下的部分
			}
		}
	}
	catch(...)
	{
		if(strMsg != "")
		{
			MessageBox(NULL, strMsg, "集成光源 Socket 出错", MB_OK | MB_ICONERROR);
		}
	}

	CSocket::OnReceive(nErrorCode);
}

BOOL CSrcBankSocket::AnalysisMsg(CString strRecvMsg, CString* strNewRes, char* pszErrorMsg)
{
	int		dwBytesReturned = 0;
	int		nResClientID1, nResClientID2;
	CString strSocketSN = "", strReq1 = "", strReq2 = "";
	CString strLog = "";

	*strNewRes = "";

	//取最后1个消息包, 最后1个*号到最后1个#号之间的信息
	strReq1 = strRecvMsg.Right(strRecvMsg.GetLength() - (strRecvMsg.ReverseFind('*') + 1));

	//最后1个消息包中的最后1个字符前有#号, 格式有误, 不处理
	if(strReq1.Find("#") < (strReq1.GetLength() - 1))
	{
		strLog.Format("集成光源服务器发送的返回信息格式有误, 开始符号后面有不止1个结束符号: %s", strReq1);
		////m_cLogOnReceive.StatusOut(strLog);

		return FALSE;
	}

	//去掉最后1个消息包
	strRecvMsg.Delete(strRecvMsg.ReverseFind('*'), strRecvMsg.GetLength() - (strRecvMsg.ReverseFind('*') + 1));

	//取倒数第2个消息包, 超过2个消息包时前面信息不处理, 最多处理2个消息包
	if(strRecvMsg.Find("*") != -1)
	{
		if(strRecvMsg.Right(1) == "#")
		{
			strReq2 = strRecvMsg.Right(strRecvMsg.GetLength() - (strRecvMsg.ReverseFind('*') + 1));

			//倒数第2个消息包中的最后1个字符前有#号, 格式有误, 处理最后1个消息包
			if(strReq2.Find("#") < (strReq2.GetLength() - 1))
			{
				strLog.Format("集成光源服务器发送的返回信息2格式有误, 开始符号后面有不止1个结束符号: %s", strReq2);
				////m_cLogOnReceive.StatusOut(strLog);

				strReq2 = "";
			}
		}
		else	//最后1个消息包与倒数第2个消息包之间若有其他字符则数据包格式错误, 处理最后第1个消息包
		{
			strLog.Format("集成光源服务器发送的返回信息格式有误, 2个消息包之间有字符 %c", strRecvMsg.Right(1));
			//m_cLogOnReceive.StatusOut(strLog);
		}
	}

	*strNewRes = strReq1;

	nResClientID1 = atoi(strReq1.Left(2));			//客户端ID
	if (int(m_dwSocketIndex) == nResClientID1)
	{
		if((strReq1.Find("UpdateWL") == -1) && strReq1.Find("Send Error") == -1)
		{
			strReq1.Delete(0, 2);
			strSocketSN = strReq1.Left(6);				//SocketSN

			if(atoi(strSocketSN) != m_nSocketSN)
			{
				strLog.Format("从集成光源服务器接收到的SocketSN有误\n\n返回信息: %s\n返回的SocketSN: %d, 客户端发送的SocketSN: %d", 
					strReq1, atoi(strSocketSN), m_nSocketSN);

				//m_cLogOnReceive.StatusOut(strLog);

				return FALSE;
			}
		}

		return TRUE;
	}
	else
	{
		strLog.Format("从集成光源服务器接收到的客户端ID号不正确\n\n返回信息: %s\n本机客户端ID号: %d\n从服务器接收到的客户端ID号: %d", 
			strReq1, m_dwSocketIndex, nResClientID1);
		//m_cLogOnReceive.StatusOut(strLog);
	}

	//有2个消息包
	if(strReq2 != "")
	{
		*strNewRes = strReq2;

		nResClientID2 = atoi(strReq2.Left(2));			//客户端ID
		if (int(m_dwSocketIndex) == nResClientID2)
		{
			if((strReq2.Find("UpdateWL") == -1) && strReq2.Find("Send Error") == -1)
			{
				strReq2.Delete(0, 2);
				strSocketSN = strReq2.Left(6);				//SocketSN

				if(atoi(strSocketSN) != m_nSocketSN)
				{
					strLog.Format("从集成光源服务器接收到的SocketSN有误\n\n返回信息: %s\n返回的SocketSN: %d, 客户端发送的SocketSN: %d", 
						strReq2, atoi(strSocketSN), m_nSocketSN);

					//m_cLogOnReceive.StatusOut(strLog);

					return FALSE;
				}
			}

			return TRUE;
		}
		else
		{
			strLog.Format("从集成光源服务器接收到的客户端ID号不正确\n\n返回信息: %s\n本机客户端ID号: %d\n从服务器接收到的客户端ID号: %d", 
				strReq2, m_dwSocketIndex, nResClientID2);
			//m_cLogOnReceive.StatusOut(strLog);
		}			
	}

	return TRUE;
}

void CSrcBankSocket::OnClose(int nErrorCode) 
{
//	m_pParentWnd->SendMessage(WM_STOPWAITTING);

	Close();

	CAsyncSocket::OnClose(nErrorCode);
}

BOOL CSrcBankSocket::InitLogFile(BOOL bFirstOpen, int nFileIndex)
{
	CString		strFolder, strMsg, strLog;
	CFileFind	Find;
	BOOL		IsFind;
	CTime		tmNow = CTime::GetCurrentTime();
	
	return TRUE;  //Added by ris 2017-11-28
	//创建文件夹
	strFolder.Format("Log1");
	IsFind = (BOOL)Find.FindFile(strFolder);
	Find.Close();
	if(!IsFind)
	{
		::CreateDirectory(strFolder, NULL);
	}
	
	m_strLogFile.Format("%s\\OnReceive-%02d-%d", strFolder, tmNow.GetDay(), nFileIndex);

	IsFind = (BOOL)Find.FindFile(m_strLogFile);
	Find.Close();
	if(IsFind)
	{
//		if(bFirstOpen)
		{
			//m_cLogOnReceive.Init(m_strLogFile);

			strLog.Format("Log文件 %s 已存在", m_strLogFile);
			//m_cLogOnReceive.StatusOut(strLog);
		}

		return TRUE;
	}

	//m_cLogOnReceive.Init(m_strLogFile);

	strLog.Format("创建Log文件: %s", m_strLogFile);
	//m_cLogOnReceive.StatusOut(strLog);

	IsFind = (BOOL)Find.FindFile(m_strLogFile);
	Find.Close();
	if(!IsFind)
	{
		strMsg.Format("创建Log文件失败!\n\n文件名: %s", m_strLogFile);

		if(bFirstOpen)
		{
			MessageBox(NULL, strMsg, "创建Log文件", MB_OK|MB_ICONERROR);
		}

		return FALSE;
	}

	return TRUE;
}
