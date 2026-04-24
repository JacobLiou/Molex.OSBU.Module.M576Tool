// R152Set.cpp : implementation file
//

#include "stdafx.h"
#include "126s_45v_switch_app.h"
#include "R152Set.h"
#include "opnet.h"
#include "opslot.h"
#include "AFXSOCK.H"
#include "126sDataType.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern stPMCfg g_stPMCfg;
extern TCHAR g_tszAppFolder[MAX_PATH];
extern CString m_strNetConfigPath;

/////////////////////////////////////////////////////////////////////////////
// CR152Set dialog
int   CR152Set::read_ip_config( unsigned int*  ip, int  n)
{
	
	CString strFileName;
	CString strSection;
	CString strIP;
	CHAR chTemp[MAX_PATH];
	strFileName.Format("%s\\device-14538.ini", m_strNetConfigPath);

	ZeroMemory(chTemp,MAX_PATH);
	strSection.Format("PM%d",n);
	GetPrivateProfileString(strSection,"ADDRESS","0",chTemp,MAX_PATH,strFileName);

	*ip=(unsigned int)atoi(chTemp);
	
	return  TRUE;
}


int   CR152Set::write_ip_config( unsigned int*  ip, int n)
{
	CString strFileName;
	CString strSection;
	CString strIP;
	strFileName.Format("%s\\device-14538.ini", m_strNetConfigPath);

	strSection.Format("PM%d",n);
	strIP.Format("%u",*ip);
	WritePrivateProfileString(strSection,"ADDRESS",strIP,strFileName);
	
	return  TRUE;	
}

CR152Set::CR152Set(CWnd* pParent /*=NULL*/)
	: CDialog(CR152Set::IDD, pParent)
{
	//{{AFX_DATA_INIT(CR152Set)
	//}}AFX_DATA_INIT
	m_bLinkIP1 = FALSE;
	m_bLinkIP2 = FALSE;
	m_bLinkIP3 = FALSE;	
}


void CR152Set::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CR152Set)
	DDX_Control(pDX, IDC_IPADDRESS3, m_IPAddress3);
	DDX_Control(pDX, IDC_IPADDRESS2, m_IPAddress2);
	DDX_Control(pDX, IDC_IPADDRESS1, m_IPAddress1);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CR152Set, CDialog)
	//{{AFX_MSG_MAP(CR152Set)
	ON_BN_CLICKED(IDC_BUTTON_SaveIP1, OnBUTTONSaveIP1)
	ON_BN_CLICKED(IDC_BUTTON_SaveIP2, OnBUTTONSaveIP2)
	ON_BN_CLICKED(IDC_BUTTON_SaveIP3, OnBUTTONSaveIP3)
	ON_BN_CLICKED(IDC_BUTTON_CONNECTIP, OnButtonConnectip)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CR152Set message handlers

void CR152Set::OnBUTTONSaveIP1() 
{
	// TODO: Add your control notification handler code here
	
	DWORD   dwIP;
	int  ret  =0;
	UpdateData();
		
	m_IPAddress1.GetAddress(dwIP);
	g_ip = dwIP;
	
	ret = write_ip_config( &g_ip,0);

	m_IPAddress2.GetAddress(dwIP);
	g_ip = dwIP;
	
	ret = write_ip_config( &g_ip,1);

	m_IPAddress3.GetAddress(dwIP);
	g_ip = dwIP;
	
	ret = write_ip_config( &g_ip,2);

	if (ret < 0 )
		MessageBox( "不能保存IP地址!", "保存IP地址", MB_OK | MB_ICONWARNING );
	else
		MessageBox( "成功保存IP地址!\n", "保存IP地址", MB_OK );
	
	
    return  ;
}

void CR152Set::OnBUTTONSaveIP2() 
{
	// TODO: Add your control notification handler code here
	
	DWORD   dwIP;
	int  ret  =0;
	
	m_IPAddress2.GetAddress(dwIP);
	g_ip = dwIP;
	
	ret = write_ip_config( &g_ip,2);
	if (ret < 0 )
		MessageBox( "Cannot save the IP address!", "Save IP addresss", MB_OK | MB_ICONWARNING );
	else
		MessageBox( "Success to  save the IP address!\n\nIf the new IP address is validated, you should restart this application \n", "Save IP addresss", MB_OK );
	
	
    return  ;
}

void CR152Set::OnBUTTONSaveIP3() 
{
	// TODO: Add your control notification handler code here
	
	DWORD   dwIP;
	int  ret  =0;
	
	
	m_IPAddress3.GetAddress(dwIP);
	g_ip = dwIP;
	
	ret = write_ip_config( &g_ip,3);
	if (ret < 0 )
		MessageBox( "Cannot save the IP address!", "Save IP addresss", MB_OK | MB_ICONWARNING );
	else
		MessageBox( "Success to  save the IP address!\n\nIf the new IP address is validated, you should restart this application \n", "Save IP addresss", MB_OK );
	
	
    return  ;
}

// void CR152Set::OnBUTTONSaveIP4() 
// {
// 	// TODO: Add your control notification handler code here
// 	
// 	DWORD   dwIP;
// 	int  ret  =0;
// 	
// 	
// 	m_IPAddress1.GetAddress(dwIP);
// 	g_ip = dwIP;
// 	
// 	ret = write_ip_config( &g_ip,4);
// 	if (ret < 0 )
// 		MessageBox( "Cannot save the IP address!", "Save IP addresss", MB_OK | MB_ICONWARNING );
// 	else
// 		MessageBox( "Success to  save the IP address!\n\nIf the new IP address is validated, you should restart this application \n", "Save IP addresss", MB_OK );
// 	
// 	
//     return  ;
// }

BOOL CR152Set::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	int  ret = 0;
	//初始化
	ret = read_ip_config( &g_ipArray1,0);
	if ( ret < 0 )
		g_ipArray1 = 0xAC108377;
	ret = read_ip_config( &g_ipArray2,1);
	if ( ret < 0 )
		g_ipArray2 = 0xAC108377;
	ret = read_ip_config( &g_ipArray3,2);
	if ( ret < 0 )
		g_ipArray3 = 0xAC108377;

	m_IPAddress1.SetAddress( g_ipArray1 );
	m_IPAddress2.SetAddress( g_ipArray2 );
	m_IPAddress3.SetAddress( g_ipArray3 );


	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
/*
double CR152Set::ReadR152Power(int nChannel)
{
	unsigned char slot_id;
	unsigned char chan_id;
    int n;
	double dblR152Power;

	n = nChannel%4;
	slot_id = n/2;
	chan_id = n%2;

	if (nChannel >=0 && nChannel <= 3)
	{
        //WaitForSingleObject(hThread1,1000);
		dblR152Power = slot_array[slot_id].module.pm.chan[chan_id].pm_value1/10000.0;
 		if ( slot_array[slot_id].module.pm.chan[chan_id].data_flag1 == DATA_OK )
 		{
 			return dblR152Power;
 		}
 		else
 		{
			MessageBox("Read PM ERROR!");
		}
		return dblR152Power;
	}
	
	if (nChannel >=4 && nChannel <= 7)
	{
		//WaitForSingleObject(hThread2,1000);
		dblR152Power = slot_array[slot_id].module.pm.chan[chan_id].pm_value2/10000.0;
		if ( slot_array[slot_id].module.pm.chan[chan_id].data_flag2 != DATA_OK )
		{
			MessageBox("Read PM ERROR!");
		}
		else
		{
           return dblR152Power;
		}
	}
	
	if (nChannel >=8 && nChannel <= 11)
	{
		//WaitForSingleObject(hThread3,1000);
		dblR152Power = slot_array[slot_id].module.pm.chan[chan_id].pm_value3/10000.0;
		if ( slot_array[slot_id].module.pm.chan[chan_id].data_flag3 != DATA_OK )
		{
			MessageBox("Read PM ERROR!");
		}
		else
		{
			return dblR152Power;
		}
	} 

	//return -999;

}
*/
void CreateSocket()
{
//======================
//Csocket
//======================
/*
	if (!AfxSocketInit())
   {
	   AfxMessageBox("error");	
   }
  CSocket SocketClient;
  BOOL bRet;
  CString strInfo;
  bRet = SocketClient.Create(8686);
  strInfo.Format("%d",bRet);
  //AfxMessageBox(strInfo);
	  
  if (bRet == FALSE)
  {
    AfxMessageBox("error");
  }
*/
//==================
//SOCKET
//==================
   //加载套接字库
	int  socket_fd = 0;
	WORD wVersion;
	WSADATA wsaData;
	int err;
	wVersion = MAKEWORD(1,1);
	err = WSAStartup(wVersion,&wsaData);
	if (err != 0)
	{
		return;
	}
	if (LOBYTE(wsaData.wVersion) != 1||
		HIBYTE(wsaData.wVersion) != 1)
	{
		WSACleanup();
		return;
	}
  SOCKET sockClient = socket(AF_INET,SOCK_DGRAM,0);
  socket_fd = sockClient;

}

void CR152Set::create_udp_client1()
{
	
    hThread1 = CreateThread( NULL, 0, (LPTHREAD_START_ROUTINE)route_udp_client1, NULL, 0, &g_net_thread_id1 );
	if ( hThread1 == NULL )
	{
		AfxMessageBox("create thread1 failed!");
	}
}

void CR152Set::create_udp_client2()
{
	
    hThread2 = CreateThread( NULL, 0, (LPTHREAD_START_ROUTINE)route_udp_client2, NULL, 0, &g_net_thread_id2 );
	if ( hThread2 == NULL )
	{
		AfxMessageBox("create thread2 failed!");
	}
}

void CR152Set::create_udp_client3()
{
	
    hThread3 = CreateThread( NULL, 0, (LPTHREAD_START_ROUTINE)route_udp_client3, NULL, 0, &g_net_thread_id3 );
	if ( hThread3 == NULL )
	{
		AfxMessageBox("create thread3 failed!");
	}
}

#define   RECV_BUF_LEN                1024
#define   SEND_BUF_LEN                1024

void route_udp_client1( LPVOID  lpParam )
{
    int  iMode  = 1;
	int  socket_fd1 = 0;

    sockaddr_in   server_addr; //服务器地址
    unsigned short int   udp_port;
    unsigned int         addr_len;

             char        sendbuf[SEND_BUF_LEN];
             char        recbuf[RECV_BUF_LEN];
    unsigned int         send_len = 0;
    unsigned int         rec_len  = RECV_BUF_LEN;

	unsigned char        slot_id = 0;
	unsigned char        chan_id = 0;
	unsigned int         wl = 15500000;

	         time_t      first_time;
			 time_t      cur_time;
             int         ret = 0;

    static   int         counter = 0;
    CString strInfo;
	
	WORD wVersion;
	WSADATA wsaData;
	int err;
	wVersion = MAKEWORD(1,1);
	err = WSAStartup(wVersion,&wsaData);
	if (err != 0)
	{
		return;
	}
	if (LOBYTE(wsaData.wVersion) != 1||
		HIBYTE(wsaData.wVersion) != 1)
	{
		WSACleanup();
		return;
	}
	socket_fd1 = socket(AF_INET,SOCK_DGRAM,0);

    udp_port = 8686;    
  
    memset(&server_addr, 0, sizeof(sockaddr_in));
    server_addr.sin_family      = AF_INET;
    server_addr.sin_port        = htons(udp_port); 
    server_addr.sin_addr.s_addr = htonl(g_ipArray1);	

	iMode = 1;
    ioctlsocket(socket_fd1, FIONBIO,(u_long FAR*)&iMode);//控制套接口的模式，本函数可用于任一状态的任一个套接口
                         //允许或禁止套接口的非阻塞模式        
	
	while( 1 )
	{
		for ( slot_id = 0; slot_id < MAX_SLOT_NUM; slot_id++ )
		{
			for ( chan_id = 0; chan_id < MAX_PM_CHAN_NUM; chan_id++ )
			{
                ret = is_wl_set11( slot_id, chan_id, &wl );
		        if ( ret > 0 )
				{
					memset( sendbuf, 0, sizeof(sendbuf) );
                    make_net_set_wl_frame( slot_id, chan_id,  wl,(unsigned char *) sendbuf, &send_len );
			        clear_wl_set1( slot_id, chan_id );

					addr_len = sizeof( struct sockaddr );
                    sendto( socket_fd1, sendbuf, send_len, 0, (struct sockaddr *)(&server_addr), addr_len );

                    slot_array[slot_id].wl_counter = 0;

			        Sleep(50);
					break;
				}
			}

			memset( sendbuf, 0, sizeof(sendbuf) );

			if ( (slot_array[slot_id].wl_counter) < 10 )
			{
				if ( (slot_array[slot_id].next_read) > 0 )
                    make_net_read_wl_frame( slot_id, 0xFF, (unsigned char *)sendbuf, &send_len );
				else
                    make_net_read_power_frame( slot_id, 0xFF, (unsigned char *)sendbuf, &send_len );
			} else {
                make_net_read_power_frame( slot_id, 0xFF,(unsigned char *) sendbuf, &send_len );
				slot_array[slot_id].next_read = 0;
			}

            //UDP SEND:
		    if (send_len < 1)
			    goto  UDP_RECV;

		    time( &first_time );
            addr_len = sizeof( struct sockaddr );

            ret = sendto( socket_fd1, sendbuf, send_len, 0, (struct sockaddr *)(&server_addr), addr_len );
            if ( ret <= 0 )
			{
                slot_array[slot_id].status = SLOT_DISCONNECTED;
               // PostThreadMessage( g_main_thread_id, REFRESH_DATA_MESSAGE, 0, 0 );
			    Sleep(1);
                continue;
			}
            send_len = 0;
		
			//UDP RECV
            UDP_RECV:
		    Sleep(90);
            addr_len = sizeof(sockaddr );

			memset( recbuf, 0, sizeof(recbuf) );
            ret = recvfrom( socket_fd1, recbuf, RECV_BUF_LEN, 0, (sockaddr *)(&server_addr), (int *)&addr_len);
            if ( ret > 0 )
			{
			    //decode
				if ( slot_array[slot_id].next_read > 0)
				{
				    decode_wl_buf1( slot_id, chan_id, (unsigned char *)recbuf, rec_len );
					slot_array[slot_id].wl_counter++;
				} else {
				    decode_pm_buf11( slot_id, chan_id, (unsigned char *)recbuf, rec_len );
				}
			    send_len = 0;

			    if ( slot_array[slot_id].next_read > 0 )
				    slot_array[slot_id].next_read = 0;
			    else
				    slot_array[slot_id].next_read = 1;

                slot_array[slot_id].status = SLOT_CONNECTED;

                //PostThreadMessage( g_main_thread_id, REFRESH_DATA_MESSAGE, 0, 0 );

				counter++;
				if ( counter > 50 )
                    slot_array[slot_id].wl_counter = 0;

				continue;
			}

		    time( &cur_time );
		    if ( (cur_time - first_time) < 1)
		        goto  UDP_RECV;

			slot_array[slot_id].status = SLOT_DISCONNECTED;
            PostThreadMessage( g_main_thread_id, REFRESH_DATA_MESSAGE, 0, 0 );

		}

	}

}

void route_udp_client2( LPVOID  lpParam )
{
    int  iMode  = 1;
	int  socket_fd2 = 0;

    sockaddr_in   server_addr; //服务器地址
    unsigned short int   udp_port;
    unsigned int         addr_len;

             char        sendbuf[SEND_BUF_LEN];
             char        recbuf[RECV_BUF_LEN];
    unsigned int         send_len = 0;
    unsigned int         rec_len  = RECV_BUF_LEN;

	unsigned char        slot_id = 0;
	unsigned char        chan_id = 0;
	unsigned int         wl = 15500000;

	         time_t      first_time;
			 time_t      cur_time;
             int         ret = 0;

    static   int         counter = 0;
    CString strInfo;
	
	WORD wVersion;
	WSADATA wsaData;
	int err;
	wVersion = MAKEWORD(1,1);
	err = WSAStartup(wVersion,&wsaData);
	if (err != 0)
	{
		return;
	}
	if (LOBYTE(wsaData.wVersion) != 1||
		HIBYTE(wsaData.wVersion) != 1)
	{
		WSACleanup();
		return;
	}
	socket_fd2 = socket(AF_INET,SOCK_DGRAM,0);

    udp_port = 8686;    
  
    memset(&server_addr, 0, sizeof(sockaddr_in));
    server_addr.sin_family      = AF_INET;
    server_addr.sin_port        = htons(udp_port); 
    server_addr.sin_addr.s_addr = htonl(g_ipArray2);	

	iMode = 1;
    ioctlsocket(socket_fd2, FIONBIO,(u_long FAR*)&iMode);//控制套接口的模式，本函数可用于任一状态的任一个套接口
                         //允许或禁止套接口的非阻塞模式        
	
	while( 1 )
	{
		for ( slot_id = 0; slot_id < MAX_SLOT_NUM; slot_id++ )
		{
			for ( chan_id = 0; chan_id < MAX_PM_CHAN_NUM; chan_id++ )
			{
                ret = is_wl_set12( slot_id, chan_id, &wl );
		        if ( ret > 0 )
				{
					memset( sendbuf, 0, sizeof(sendbuf) );
                    make_net_set_wl_frame( slot_id, chan_id,  wl,(unsigned char *) sendbuf, &send_len );
			        clear_wl_set1( slot_id, chan_id );

					addr_len = sizeof( struct sockaddr );
                    sendto( socket_fd2, sendbuf, send_len, 0, (struct sockaddr *)(&server_addr), addr_len );

                    slot_array[slot_id].wl_counter = 0;

			        Sleep(50);
					break;
				}
			}

			memset( sendbuf, 0, sizeof(sendbuf) );

			if ( (slot_array[slot_id].wl_counter) < 10 )
			{
				if ( (slot_array[slot_id].next_read) > 0 )
                    make_net_read_wl_frame( slot_id, 0xFF, (unsigned char *)sendbuf, &send_len );
				else
                    make_net_read_power_frame( slot_id, 0xFF, (unsigned char *)sendbuf, &send_len );
			} else {
                make_net_read_power_frame( slot_id, 0xFF,(unsigned char *) sendbuf, &send_len );
				slot_array[slot_id].next_read = 0;
			}

            //UDP SEND:
		    if (send_len < 1)
			    goto  UDP_RECV;

		    time( &first_time );
            addr_len = sizeof( struct sockaddr );;

            ret = sendto( socket_fd2, sendbuf, send_len, 0, (struct sockaddr *)(&server_addr), addr_len );
            if ( ret <= 0 )
			{
                slot_array[slot_id].status = SLOT_DISCONNECTED;
               // PostThreadMessage( g_main_thread_id, REFRESH_DATA_MESSAGE, 0, 0 );
			    Sleep(1);
                continue;
			}
            send_len = 0;
		
			//UDP RECV
            UDP_RECV:
		    Sleep(90);
            addr_len = sizeof(sockaddr );

			memset( recbuf, 0, sizeof(recbuf) );
            ret = recvfrom( socket_fd2, recbuf, RECV_BUF_LEN, 0, (sockaddr *)(&server_addr), (int *)&addr_len);
            if ( ret > 0 )
			{
			    //decode
				if ( slot_array[slot_id].next_read > 0)
				{
				    decode_wl_buf2( slot_id, chan_id, (unsigned char *)recbuf, rec_len );
					slot_array[slot_id].wl_counter++;
				} else {
				    decode_pm_buf12( slot_id, chan_id, (unsigned char *)recbuf, rec_len );
				}
			    send_len = 0;

			    if ( slot_array[slot_id].next_read > 0 )
				    slot_array[slot_id].next_read = 0;
			    else
				    slot_array[slot_id].next_read = 1;

                slot_array[slot_id].status = SLOT_CONNECTED;

                //PostThreadMessage( g_main_thread_id, REFRESH_DATA_MESSAGE, 0, 0 );

				counter++;
				if ( counter > 50 )
                    slot_array[slot_id].wl_counter = 0;

				continue;
			}

		    time( &cur_time );
		    if ( (cur_time - first_time) < 1)
		        goto  UDP_RECV;

			slot_array[slot_id].status = SLOT_DISCONNECTED;
            PostThreadMessage( g_main_thread_id, REFRESH_DATA_MESSAGE, 0, 0 );

		}

	}

}
void route_udp_client3( LPVOID  lpParam )
{
    int  iMode  = 1;
    int  socket_fd3 = 0;

    sockaddr_in   server_addr; //服务器地址
    unsigned short int   udp_port;
    unsigned int         addr_len;

             char        sendbuf[SEND_BUF_LEN];
             char        recbuf[RECV_BUF_LEN];
    unsigned int         send_len = 0;
    unsigned int         rec_len  = RECV_BUF_LEN;

	unsigned char        slot_id = 0;
	unsigned char        chan_id = 0;
	unsigned int         wl = 15500000;

	         time_t      first_time;
			 time_t      cur_time;
             int         ret = 0;

    static   int         counter = 0;
    CString strInfo;
	
	WORD wVersion;
	WSADATA wsaData;
	int err;
	wVersion = MAKEWORD(1,1);
	err = WSAStartup(wVersion,&wsaData);
	if (err != 0)
	{
		return;
	}
	if (LOBYTE(wsaData.wVersion) != 1||
		HIBYTE(wsaData.wVersion) != 1)
	{
		WSACleanup();
		return;
	}
	socket_fd3 = socket(AF_INET,SOCK_DGRAM,0);

    udp_port = 8686;    
  
    memset(&server_addr, 0, sizeof(sockaddr_in));
    server_addr.sin_family      = AF_INET;
    server_addr.sin_port        = htons(udp_port); 
    server_addr.sin_addr.s_addr = htonl(g_ipArray3);	

	iMode = 1;
    ioctlsocket(socket_fd3, FIONBIO,(u_long FAR*)&iMode);//控制套接口的模式，本函数可用于任一状态的任一个套接口
                         //允许或禁止套接口的非阻塞模式        
	
	while( 1 )
	{
		for ( slot_id = 0; slot_id < MAX_SLOT_NUM; slot_id++ )
		{
			for ( chan_id = 0; chan_id < MAX_PM_CHAN_NUM; chan_id++ )
			{
                ret = is_wl_set13( slot_id, chan_id, &wl );
		        if ( ret > 0 )
				{
					memset( sendbuf, 0, sizeof(sendbuf) );
                    make_net_set_wl_frame( slot_id, chan_id,  wl,(unsigned char *) sendbuf, &send_len );
			        clear_wl_set1( slot_id, chan_id );

					addr_len = sizeof( struct sockaddr );
                    sendto( socket_fd3, sendbuf, send_len, 0, (struct sockaddr *)(&server_addr), addr_len );

                    slot_array[slot_id].wl_counter = 0;

			        Sleep(50);
					break;
				}
			}

			memset( sendbuf, 0, sizeof(sendbuf) );

			if ( (slot_array[slot_id].wl_counter) < 10 )
			{
				if ( (slot_array[slot_id].next_read) > 0 )
                    make_net_read_wl_frame( slot_id, 0xFF, (unsigned char *)sendbuf, &send_len );
				else
                    make_net_read_power_frame( slot_id, 0xFF, (unsigned char *)sendbuf, &send_len );
			} else {
                make_net_read_power_frame( slot_id, 0xFF,(unsigned char *) sendbuf, &send_len );
				slot_array[slot_id].next_read = 0;
			}

            //UDP SEND:
		    if (send_len < 1)
			    goto  UDP_RECV;

		    time( &first_time );
            addr_len = sizeof( struct sockaddr );

            ret = sendto( socket_fd3, sendbuf, send_len, 0, (struct sockaddr *)(&server_addr), addr_len );
            if ( ret <= 0 )
			{
                slot_array[slot_id].status = SLOT_DISCONNECTED;
               // PostThreadMessage( g_main_thread_id, REFRESH_DATA_MESSAGE, 0, 0 );
			    Sleep(1);
                continue;
			}
            send_len = 0;
		
			//UDP RECV
            UDP_RECV:
		    Sleep(90);
            addr_len = sizeof(sockaddr );

			memset( recbuf, 0, sizeof(recbuf) );
            ret = recvfrom( socket_fd3, recbuf, RECV_BUF_LEN, 0, (sockaddr *)(&server_addr), (int *)&addr_len);
            if ( ret > 0 )
			{
			    //decode
				if ( slot_array[slot_id].next_read > 0)
				{
				    decode_wl_buf3( slot_id, chan_id, (unsigned char *)recbuf, rec_len );
					slot_array[slot_id].wl_counter++;
				} else {
				    decode_pm_buf13( slot_id, chan_id, (unsigned char *)recbuf, rec_len );
				}
			    send_len = 0;

			    if ( slot_array[slot_id].next_read > 0 )
				    slot_array[slot_id].next_read = 0;
			    else
				    slot_array[slot_id].next_read = 1;

                slot_array[slot_id].status = SLOT_CONNECTED;

                //PostThreadMessage( g_main_thread_id, REFRESH_DATA_MESSAGE, 0, 0 );

				counter++;
				if ( counter > 50 )
                    slot_array[slot_id].wl_counter = 0;

				continue;
			}

		    time( &cur_time );
		    if ( (cur_time - first_time) < 1)
		        goto  UDP_RECV;

			slot_array[slot_id].status = SLOT_DISCONNECTED;
            PostThreadMessage( g_main_thread_id, REFRESH_DATA_MESSAGE, 0, 0 );

		}

	}

}
int is_wl_set11(unsigned char slot_id, unsigned char chan_id, unsigned int *wl)
{
	if  ( slot_id > (MAX_SLOT_NUM - 1) )
		slot_id = (MAX_SLOT_NUM -1);
	
	if  ( chan_id > (MAX_PM_CHAN_NUM - 1) )
		chan_id = (MAX_PM_CHAN_NUM -1);
	
	if ( slot_array[slot_id].module.pm.chan[chan_id].set_wl_flag )
	{
		*wl = slot_array[slot_id].module.pm.chan[chan_id].set_wl_value1;
		return  1;
	}
   return 0;
}

int is_wl_set12(unsigned char slot_id, unsigned char chan_id, unsigned int *wl)
{
	if  ( slot_id > (MAX_SLOT_NUM - 1) )
		slot_id = (MAX_SLOT_NUM -1);
	
	if  ( chan_id > (MAX_PM_CHAN_NUM - 1) )
		chan_id = (MAX_PM_CHAN_NUM -1);
	
	if ( slot_array[slot_id].module.pm.chan[chan_id].set_wl_flag )
	{
		*wl = slot_array[slot_id].module.pm.chan[chan_id].set_wl_value2;
		return  1;
	}
	return 0;
}
int is_wl_set13(unsigned char slot_id, unsigned char chan_id, unsigned int *wl)
{
	if  ( slot_id > (MAX_SLOT_NUM - 1) )
		slot_id = (MAX_SLOT_NUM -1);
	
	if  ( chan_id > (MAX_PM_CHAN_NUM - 1) )
		chan_id = (MAX_PM_CHAN_NUM -1);
	
	if ( slot_array[slot_id].module.pm.chan[chan_id].set_wl_flag )
	{
		*wl = slot_array[slot_id].module.pm.chan[chan_id].set_wl_value3;
		return  1;
	}
	return 0;
}
int  clear_wl_set1( unsigned char  slot_id, unsigned char  chan_id )
{
	if  ( slot_id > (MAX_SLOT_NUM - 1) )
		slot_id = (MAX_SLOT_NUM -1);
	
	if  ( chan_id > (MAX_PM_CHAN_NUM - 1) )
		chan_id = (MAX_PM_CHAN_NUM -1);
	
	slot_array[slot_id].module.pm.chan[chan_id].set_wl_flag  = 0;
	
	return  1;
}

int decode_pm_buf11( unsigned char slot_id, unsigned char chan_id, unsigned char* recbuf, unsigned int rec_len )
{
	double          pm_value = 0;
	int             flag     = 0;
	unsigned  int   i        = 0;
	unsigned  int   pos      = 0;
	
	unsigned char   str[32];
	unsigned char*  p ;
		
	p = recbuf;
	
	if (slot_id > (MAX_SLOT_NUM-1) )
		slot_id =  (MAX_SLOT_NUM -1);	
	
	for (i = 0; i < rec_len; i++ )
	{
		if ( *(p+i) == ',' )
		{
			pos = i;
			flag = 1;
			break;
		}
	}
		
	if ( flag == 0 )
	{
		if ( chan_id > ( MAX_PM_CHAN_NUM - 1 ))
			chan_id = MAX_PM_CHAN_NUM - 1;
		
	}
	
	if ( flag == 0 )
	{
		pm_value = atof((char *)recbuf);
		slot_array[slot_id].module.pm.chan[chan_id].pm_value1 = (int)(pm_value*10000);
		slot_array[slot_id].module.pm.chan[chan_id].data_flag1 = DATA_OK;
		return  1;
	}	
	
	memset( str, 0, sizeof(str) );
	strncpy( (char *)str, (char *)recbuf, pos);
	
	if ( ((str[0] > 0x2F)&&( str[0] < 0x3A)) ||  (str[0] == '-')&&(str[1] != '.')  )
	{
		pm_value = atof((char *)str);
		slot_array[slot_id].module.pm.chan[0].pm_value1 = (int)(pm_value*10000);
		slot_array[slot_id].module.pm.chan[0].data_flag1 = DATA_OK;
	} else {
		if( str[0] == 'U')
			slot_array[slot_id].module.pm.chan[0].data_flag1 = DATA_UNCAL;
		else
			slot_array[slot_id].module.pm.chan[0].data_flag1 = DATA_UNKNOWN;
	}
		
	if ( ((recbuf[pos+1]>0x2F)&&(recbuf[pos+1]<0x3A)) ||  ((recbuf[pos+1] == '-')&&(recbuf[pos+2] != '.') ) )
	{
		pm_value = atof((char *)&recbuf[pos+1]);
		slot_array[slot_id].module.pm.chan[1].pm_value1 = (int)(pm_value*10000);
		slot_array[slot_id].module.pm.chan[1].data_flag1 = DATA_OK;
	} else {
		if( str[0] == 'U')
			slot_array[slot_id].module.pm.chan[1].data_flag1 = DATA_UNCAL;
		else
			slot_array[slot_id].module.pm.chan[1].data_flag1 = DATA_UNKNOWN;
	}
	
	return  1;
}

int decode_pm_buf12( unsigned char slot_id, unsigned char chan_id, unsigned char* recbuf, unsigned int rec_len )
{
	double          pm_value = 0;
	int             flag     = 0;
	unsigned  int   i        = 0;
	unsigned  int   pos      = 0;
	
	unsigned char   str[32];
	unsigned char*  p ;
	
	p = recbuf;
	
	if (slot_id > (MAX_SLOT_NUM-1) )
		slot_id =  (MAX_SLOT_NUM -1);	
	
	for (i = 0; i < rec_len; i++ )
	{
		if ( *(p+i) == ',' )
		{
			pos = i;
			flag = 1;
			break;
		}
	}
	
	if ( flag == 0 )
	{
		if ( chan_id > ( MAX_PM_CHAN_NUM - 1 ))
			chan_id = MAX_PM_CHAN_NUM - 1;
		
	}
	
	if ( flag == 0 )
	{
		pm_value = atof((char *)recbuf);
		slot_array[slot_id].module.pm.chan[chan_id].pm_value2 = (int)(pm_value*10000);
		slot_array[slot_id].module.pm.chan[chan_id].data_flag2 = DATA_OK;
		return  1;
	}	
	
	memset( str, 0, sizeof(str) );
	strncpy( (char *)str, (char *)recbuf, pos);
	
	if ( ((str[0] > 0x2F)&&( str[0] < 0x3A)) ||  (str[0] == '-')&&(str[1] != '.')  )
	{
		pm_value = atof((char *)str);
		slot_array[slot_id].module.pm.chan[0].pm_value2 = (int)(pm_value*10000);
		slot_array[slot_id].module.pm.chan[0].data_flag2 = DATA_OK;
	} else {
		if( str[0] == 'U')
			slot_array[slot_id].module.pm.chan[0].data_flag2 = DATA_UNCAL;
		else
			slot_array[slot_id].module.pm.chan[0].data_flag2 = DATA_UNKNOWN;
	}
	
	if ( ((recbuf[pos+1]>0x2F)&&(recbuf[pos+1]<0x3A)) ||  ((recbuf[pos+1] == '-')&&(recbuf[pos+2] != '.') ) )
	{
		pm_value = atof((char *)&recbuf[pos+1]);
		slot_array[slot_id].module.pm.chan[1].pm_value2 = (int)(pm_value*10000);
		slot_array[slot_id].module.pm.chan[1].data_flag2 = DATA_OK;
	} else {
		if( str[0] == 'U')
			slot_array[slot_id].module.pm.chan[1].data_flag2 = DATA_UNCAL;
		else
			slot_array[slot_id].module.pm.chan[1].data_flag2 = DATA_UNKNOWN;
	}
	
	return  1;
}

int decode_pm_buf13( unsigned char slot_id, unsigned char chan_id, unsigned char* recbuf, unsigned int rec_len )
{
	double          pm_value = 0;
	int             flag     = 0;
	unsigned  int   i        = 0;
	unsigned  int   pos      = 0;
	
	unsigned char   str[32];
	unsigned char*  p ;
	
	p = recbuf;
	
	if (slot_id > (MAX_SLOT_NUM-1) )
		slot_id =  (MAX_SLOT_NUM -1);	
	
	for (i = 0; i < rec_len; i++ )
	{
		if ( *(p+i) == ',' )
		{
			pos = i;
			flag = 1;
			break;
		}
	}
	
	if ( flag == 0 )
	{
		if ( chan_id > ( MAX_PM_CHAN_NUM - 1 ))
			chan_id = MAX_PM_CHAN_NUM - 1;
		
	}
	
	if ( flag == 0 )
	{
		pm_value = atof((char *)recbuf);
		slot_array[slot_id].module.pm.chan[chan_id].pm_value3 = (int)(pm_value*10000);
		slot_array[slot_id].module.pm.chan[chan_id].data_flag3 = DATA_OK;
		return  1;
	}	
	
	memset( str, 0, sizeof(str) );
	strncpy( (char *)str, (char *)recbuf, pos);
	
	if ( ((str[0] > 0x2F)&&( str[0] < 0x3A)) ||  (str[0] == '-')&&(str[1] != '.')  )
	{
		pm_value = atof((char *)str);
		slot_array[slot_id].module.pm.chan[0].pm_value3 = (int)(pm_value*10000);
		slot_array[slot_id].module.pm.chan[0].data_flag3 = DATA_OK;
	} else {
		if( str[0] == 'U')
			slot_array[slot_id].module.pm.chan[0].data_flag3 = DATA_UNCAL;
		else
			slot_array[slot_id].module.pm.chan[0].data_flag3 = DATA_UNKNOWN;
	}
	
	if ( ((recbuf[pos+1]>0x2F)&&(recbuf[pos+1]<0x3A)) ||  ((recbuf[pos+1] == '-')&&(recbuf[pos+2] != '.') ) )
	{
		pm_value = atof((char *)&recbuf[pos+1]);
		slot_array[slot_id].module.pm.chan[1].pm_value3 = (int)(pm_value*10000);
		slot_array[slot_id].module.pm.chan[1].data_flag3 = DATA_OK;
	} else {
		if( str[0] == 'U')
			slot_array[slot_id].module.pm.chan[1].data_flag3 = DATA_UNCAL;
		else
			slot_array[slot_id].module.pm.chan[1].data_flag3 = DATA_UNKNOWN;
	}
	
	return  1;
}

void CR152Set::OnButtonConnectip() 
{
	// TODO: Add your control notification handler code here
	
	if(g_stPMCfg.iConnect[0]==CONNECT_NET)
	{
		ConnectIP1();
		if (m_bLinkIP1 == FALSE )
		{
			MessageBox("IP1未连接成功！");
		}
		else 
		{
			g_stPMCfg.bPMOpen[0]=TRUE;
			MessageBox("已成功连接IP1！");
		}
	}

	if(g_stPMCfg.iConnect[1]==CONNECT_NET)
	{
		ConnectIP2();
		if (m_bLinkIP2 == FALSE )
		{
			MessageBox("IP2未连接成功！");
		}
		else 
		{
			g_stPMCfg.bPMOpen[1]=TRUE;
			MessageBox("已成功连接IP2！");
		}
	}

	if(g_stPMCfg.iConnect[2]==CONNECT_NET)
	{
		ConnectIP3();
		if (m_bLinkIP3 == FALSE )
		{
			MessageBox("IP3未连接成功！");
		}
		else 
		{
			g_stPMCfg.bPMOpen[2]=TRUE;
			MessageBox("已成功连接IP3！");
		}
	}
}

void CR152Set::ConnectIP1()
{
	WORD	wVersionRequested;
	WSADATA wsaData;
	int		err;
	CString	strTemp;
	SOCKADDR_IN addrSrv;
	BYTE	bResult;
	DWORD	dwAddress;
	
	UpdateData();
	
	if(m_bLinkIP1 == FALSE)
	{	
		wVersionRequested = MAKEWORD( 1, 1 );
		
		err = WSAStartup( wVersionRequested, &wsaData );
		if ( err != 0 ) {
			return;
		}
		
		if ( LOBYTE( wsaData.wVersion ) != 1 ||
			HIBYTE( wsaData.wVersion ) != 1 ) {
			WSACleanup();
			return;
		}
		socket_fd1 =socket(AF_INET,SOCK_DGRAM,0);
		
		m_IPAddress1.GetAddress(dwAddress);
		strTemp.Format("%d.%d.%d.%d", (BYTE)(dwAddress >> 24), (BYTE)(dwAddress >> 16), (BYTE)(dwAddress >> 8),(BYTE)(dwAddress));
		addrSrv.sin_addr.S_un.S_addr = inet_addr(strTemp);
		addrSrv.sin_family=AF_INET;
		addrSrv.sin_port=htons(8686);
		
		int iMode = 1;
	//	ioctlsocket(socket_fd1, FIONBIO,(u_long FAR*)&iMode);//控制套接口的模式，本函数可用于任一状态的任一个套接口
		//允许或禁止套接口的非阻塞模式  
		
		bResult = connect(socket_fd1,(SOCKADDR*)&addrSrv,sizeof(SOCKADDR));
		if (bResult == 0)
		{
			m_bLinkIP1 = TRUE;
		}
		else
		{
			m_bLinkIP1 = FALSE;
		}
		
		
	}
	else
	{
		closesocket(socket_fd1);
		WSACleanup();
		
		
		m_bLinkIP1 = FALSE;
	}
}

void CR152Set::ConnectIP2()
{
	WORD	wVersionRequested;
	WSADATA wsaData;
	int		err;
	CString	strTemp;
	SOCKADDR_IN addrSrv;
	BYTE	bResult;
	DWORD	dwAddress;
	UpdateData();
	
	if(m_bLinkIP2 == FALSE)
	{	
		wVersionRequested = MAKEWORD( 1, 1 );
		
		err = WSAStartup( wVersionRequested, &wsaData );
		if ( err != 0 ) {
			return;
		}
		
		if ( LOBYTE( wsaData.wVersion ) != 1 ||
			HIBYTE( wsaData.wVersion ) != 1 ) {
			WSACleanup( );
			return;
		}
		socket_fd2 =socket(AF_INET,SOCK_DGRAM,0);
		
		m_IPAddress2.GetAddress(dwAddress);
		strTemp.Format("%d.%d.%d.%d", (BYTE)(dwAddress >> 24), (BYTE)(dwAddress >> 16), (BYTE)(dwAddress >> 8),(BYTE)(dwAddress));
		addrSrv.sin_addr.S_un.S_addr = inet_addr(strTemp);
		addrSrv.sin_family=AF_INET;
		addrSrv.sin_port=htons(8686);
		
		int iMode = 1;
	//	ioctlsocket(socket_fd2, FIONBIO,(u_long FAR*)&iMode);//控制套接口的模式，本函数可用于任一状态的任一个套接口
		//允许或禁止套接口的非阻塞模式 
		
		bResult = connect(socket_fd2,(SOCKADDR*)&addrSrv,sizeof(SOCKADDR));
		
		if (bResult == 0)
		{
			m_bLinkIP2 = TRUE;
		}
	}
	else
	{
		closesocket(socket_fd2);
		WSACleanup();
		
		m_bLinkIP2 = FALSE;
	}	
}

void CR152Set::ConnectIP3()
{
	WORD	wVersionRequested;
	WSADATA wsaData;
	int		err;
	CString	strTemp;
	SOCKADDR_IN addrSrv;
	BYTE	bResult;
	DWORD	dwAddress;
	UpdateData();
	
	if(m_bLinkIP3 == FALSE)
	{	
		wVersionRequested = MAKEWORD( 1, 1 );
		
		err = WSAStartup( wVersionRequested, &wsaData );
		if ( err != 0 ) {
			return;
		}
		
		if ( LOBYTE( wsaData.wVersion ) != 1 ||
			HIBYTE( wsaData.wVersion ) != 1 ) {
			WSACleanup( );
			return;
		}
		socket_fd3 =socket(AF_INET,SOCK_DGRAM,0);
		
		m_IPAddress3.GetAddress(dwAddress);
		strTemp.Format("%d.%d.%d.%d", (BYTE)(dwAddress >> 24), (BYTE)(dwAddress >> 16), (BYTE)(dwAddress >> 8),(BYTE)(dwAddress));
		addrSrv.sin_addr.S_un.S_addr = inet_addr(strTemp);
		addrSrv.sin_family=AF_INET;
		addrSrv.sin_port=htons(8686);
		
		int iMode = 1;
	//	ioctlsocket(socket_fd3, FIONBIO,(u_long FAR*)&iMode);//控制套接口的模式，本函数可用于任一状态的任一个套接口
		//允许或禁止套接口的非阻塞模式 
		
		bResult = connect(socket_fd3,(SOCKADDR*)&addrSrv,sizeof(SOCKADDR));
		
		if (bResult == 0)
		{
			m_bLinkIP3 = TRUE;
		}
	}
	else
	{
		closesocket(socket_fd3);
		WSACleanup();
		
		m_bLinkIP3 = FALSE;
	}	
}
