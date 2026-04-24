#ifndef  __OPLINK_NET_PROTOCOL_H__
#define  __OPLINK_NET_PROTOCOL_H__

#include "opconst.h"

#define  NET_RECBUF_LEN                              1024
#define  NET_SENDBUF_LEN                             1024



#ifdef __cplusplus 
extern "C" { 
#endif 


    extern  unsigned int   g_ip;
    extern  unsigned long  g_main_thread_id;
	extern  unsigned long  g_net_thread_id;
    extern  unsigned long  g_net_thread_id1;
	extern  unsigned long  g_net_thread_id2;
	extern  unsigned long  g_net_thread_id3;
    extern  unsigned int   g_iconnetc_type;
    extern  unsigned int   g_ipArray1;
	extern  unsigned int   g_ipArray2;
	extern  unsigned int   g_ipArray3;

	extern  HANDLE  hThread1;
    extern  HANDLE  hThread2;
	extern  HANDLE  hThread3;

	extern  int     socket_fd1;
	extern  int     socket_fd2;
	extern  int     socket_fd3;

	extern  int     g_socket_fd[16];

//	extern  int     socket_fd;

    int  set_wl( unsigned char slot_id, unsigned char chan_id, unsigned int  wl );
	int  set_wl1( unsigned char slot_id, unsigned char chan_id, unsigned int  wl );
	int  set_wl2( unsigned char slot_id, unsigned char chan_id, unsigned int  wl );
	int  set_wl3( unsigned char slot_id, unsigned char chan_id, unsigned int  wl );


    int  make_net_read_power_frame( unsigned char  slot_id, unsigned char  chan_id, 
                                unsigned char* sendbuf, unsigned int*  send_len );

    int  make_net_read_wl_frame( unsigned char  slot_id, unsigned char  chan_id, 
                                unsigned char* sendbuf, unsigned int*  send_len );

    int  make_net_set_wl_frame( unsigned char  slot_id, unsigned char  chan_id,  unsigned int  wl,
                                unsigned char* sendbuf, unsigned int*  send_len );


    int decode_pm_buf( unsigned char slot_id, unsigned char chan_id, unsigned char* recbuf, unsigned int rec_len );
    int decode_wl_buf( unsigned char slot_id, unsigned char chan_id, unsigned char* recbuf, unsigned int rec_len );

//    int CreateSocket(void);

    int  create_udp_client1( void );
	int  create_udp_client2( void );
	int  create_udp_client3( void );
    int  create_tcp_client( void );


#ifdef __cplusplus 
} 
#endif 

#endif   /*__OPLINK_NET_PROTOCOL_H__*/