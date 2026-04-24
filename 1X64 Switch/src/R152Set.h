#if !defined(AFX_R152SET_H__2E1A92C7_C8F6_4E1F_BA4F_8C7EB191BB34__INCLUDED_)
#define AFX_R152SET_H__2E1A92C7_C8F6_4E1F_BA4F_8C7EB191BB34__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// R152Set.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CR152Set dialog
void  route_udp_client1( LPVOID  lpParam );
void  route_udp_client2( LPVOID  lpParam );
void  route_udp_client3( LPVOID  lpParam );
int   is_wl_set11( unsigned char  slot_id, unsigned char  chan_id, unsigned int*  wl );
int   is_wl_set12( unsigned char  slot_id, unsigned char  chan_id, unsigned int*  wl );
int   is_wl_set13( unsigned char  slot_id, unsigned char  chan_id, unsigned int*  wl );
int   clear_wl_set1( unsigned char  slot_id, unsigned char  chan_id );
int   decode_pm_buf11( unsigned char slot_id, unsigned char chan_id, unsigned char* recbuf, unsigned int rec_len );
int   decode_pm_buf12( unsigned char slot_id, unsigned char chan_id, unsigned char* recbuf, unsigned int rec_len );
int   decode_pm_buf13( unsigned char slot_id, unsigned char chan_id, unsigned char* recbuf, unsigned int rec_len );
int   decode_wl_buf1( unsigned char slot_id, unsigned char chan_id, unsigned char* recbuf, unsigned int rec_len );
int   decode_wl_buf2( unsigned char slot_id, unsigned char chan_id, unsigned char* recbuf, unsigned int rec_len );
int   decode_wl_buf3( unsigned char slot_id, unsigned char chan_id, unsigned char* recbuf, unsigned int rec_len );
void  CreateSocket();



class CR152Set : public CDialog
{
// Construction
public:
	BOOL m_bLinkIP1;
	BOOL m_bLinkIP2;
	BOOL m_bLinkIP3;
	void ConnectIP1();
	void ConnectIP2();
	void ConnectIP3();
	
	void create_udp_client1();
	void create_udp_client2();
	void create_udp_client3();

	CR152Set(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CR152Set)
	enum { IDD = IDD_DIALOG_R152_SET };
	CIPAddressCtrl	m_IPAddress3;
	CIPAddressCtrl	m_IPAddress2;
	CIPAddressCtrl	m_IPAddress1;
	//}}AFX_DATA
	int   read_ip_config( unsigned int*  ip, int n);
    int  write_ip_config( unsigned int*  ip, int n);
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CR152Set)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CR152Set)
	afx_msg void OnBUTTONSaveIP1();
	afx_msg void OnBUTTONSaveIP2();
	afx_msg void OnBUTTONSaveIP3();
	virtual BOOL OnInitDialog();
	afx_msg void OnButtonConnectip();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_R152SET_H__2E1A92C7_C8F6_4E1F_BA4F_8C7EB191BB34__INCLUDED_)
