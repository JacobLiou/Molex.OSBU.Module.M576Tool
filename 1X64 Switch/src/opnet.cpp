#include "stdafx.h"
#include  <stdio.h>
#include  <Winsock2.h>

#include  <time.h>
//#include <iostream.h> 


#include  "opnet.h"
#include  "opslot.h"

#define   RECV_BUF_LEN                1024
#define   SEND_BUF_LEN                1024

#define   TCP_DELAY_MS                50

#define  BUF_LEN                      128
#define  STR_LEN                      16


 
unsigned int   g_ip             = 0xAC108377;

int     socket_fd1 = 0;
int     socket_fd2 = 0;
int     socket_fd3 = 0;

int     g_socket_fd[16];
 
unsigned long  g_main_thread_id = 0;
unsigned long  g_net_thread_id  = 0;
unsigned long  g_net_thread_id1  = 0;
unsigned long  g_net_thread_id2  = 0;
unsigned long  g_net_thread_id3  = 0;
unsigned int   g_iconnetc_type  = 0;
//int  socket_fd = 0;

unsigned int   g_ipArray1 = 0xAC108377;
unsigned int   g_ipArray2 = 0xAC108377;
unsigned int   g_ipArray3 = 0xAC108377;

HANDLE  hThread1 = NULL; 
HANDLE  hThread2 = NULL; 
HANDLE  hThread3 = NULL; 

extern int  set_wl( unsigned char slot_id, unsigned char chan_id, unsigned int  wl )
{
	if  ( slot_id > (MAX_SLOT_NUM - 1) )
		slot_id = (MAX_SLOT_NUM -1);

	if  ( chan_id > (MAX_PM_CHAN_NUM - 1) )
		chan_id = (MAX_PM_CHAN_NUM -1);

	slot_array[slot_id].module.pm.chan[chan_id].set_wl_value = wl;
	slot_array[slot_id].module.pm.chan[chan_id].set_wl_flag  = 1;

	return  1;
}
extern int  set_wl1( unsigned char slot_id, unsigned char chan_id, unsigned int  wl )
{
	if  ( slot_id > (MAX_SLOT_NUM - 1) )
		slot_id = (MAX_SLOT_NUM -1);
	
	if  ( chan_id > (MAX_PM_CHAN_NUM - 1) )
		chan_id = (MAX_PM_CHAN_NUM -1);
	
	slot_array[slot_id].module.pm.chan[chan_id].set_wl_value1 = wl;
	slot_array[slot_id].module.pm.chan[chan_id].set_wl_flag  = 1;
	
	return  1;
}
extern int  set_wl2( unsigned char slot_id, unsigned char chan_id, unsigned int  wl )
{
	if  ( slot_id > (MAX_SLOT_NUM - 1) )
		slot_id = (MAX_SLOT_NUM -1);
	
	if  ( chan_id > (MAX_PM_CHAN_NUM - 1) )
		chan_id = (MAX_PM_CHAN_NUM -1);
	
	slot_array[slot_id].module.pm.chan[chan_id].set_wl_value2 = wl;
	slot_array[slot_id].module.pm.chan[chan_id].set_wl_flag  = 1;
	
	return  1;
}
extern int  set_wl3( unsigned char slot_id, unsigned char chan_id, unsigned int  wl )
{
	if  ( slot_id > (MAX_SLOT_NUM - 1) )
		slot_id = (MAX_SLOT_NUM -1);
	
	if  ( chan_id > (MAX_PM_CHAN_NUM - 1) )
		chan_id = (MAX_PM_CHAN_NUM -1);
	
	slot_array[slot_id].module.pm.chan[chan_id].set_wl_value3 = wl;
	slot_array[slot_id].module.pm.chan[chan_id].set_wl_flag  = 1;
	
	return  1;
}

extern   int  is_wl_set( unsigned char  slot_id, unsigned char  chan_id, unsigned int*  wl )
{
	if  ( slot_id > (MAX_SLOT_NUM - 1) )
		slot_id = (MAX_SLOT_NUM -1);

	if  ( chan_id > (MAX_PM_CHAN_NUM - 1) )
		chan_id = (MAX_PM_CHAN_NUM -1);

	if ( slot_array[slot_id].module.pm.chan[chan_id].set_wl_flag )
	{
	    *wl = slot_array[slot_id].module.pm.chan[chan_id].set_wl_value;
		return  1;
	}

	return  0;

}


extern   int  clear_wl_set( unsigned char  slot_id, unsigned char  chan_id )
{
	if  ( slot_id > (MAX_SLOT_NUM - 1) )
		slot_id = (MAX_SLOT_NUM -1);

	if  ( chan_id > (MAX_PM_CHAN_NUM - 1) )
		chan_id = (MAX_PM_CHAN_NUM -1);

	slot_array[slot_id].module.pm.chan[chan_id].set_wl_flag  = 0;

	return  1;
}





extern int  make_net_read_power_frame( unsigned char  slot_id, unsigned char  chan_id, 
                                unsigned char* sendbuf, unsigned int*  send_len )
{
    unsigned char  buf[BUF_LEN];
    unsigned int   len = 0;
    unsigned char  str_buf[STR_LEN];
    unsigned int   str_len = 0;
    unsigned int   i = 0;
   

    memset( buf, 0, sizeof(buf) );
    len = 0;

    memset( str_buf, 0, sizeof(str_buf) );
    str_len = 0;   
    sprintf( (char *)str_buf, "SENS%d", slot_id );
    str_len = strlen((char *)str_buf);

    for ( i = 0; i < str_len; i++ )
        buf[len++] = str_buf[i];

    buf[len++] = ':';

    memset( str_buf, 0, sizeof(str_buf) );
    str_len = 0;
    if ( chan_id == 0xFF ) 
        sprintf( (char *)str_buf, "ALL" );
    else
        sprintf( (char *)str_buf, "CHAN%d", chan_id );
    str_len = strlen((char *)str_buf);

    for ( i = 0; i < str_len; i++ )
        buf[len++] = str_buf[i];

    buf[len++] = ':';

    buf[len++] = 'P';
    buf[len++] = 'O';
    buf[len++] = 'W';
    buf[len++] = ':';
    buf[len++] = 'R';
    buf[len++] = 'E';
    buf[len++] = 'A';
    buf[len++] = 'D';
    buf[len++] = '?';

    memcpy( sendbuf, buf, len );
    *send_len = len;

    return  len;
}



extern int  make_net_read_wl_frame( unsigned char  slot_id, unsigned char  chan_id, 
                                unsigned char* sendbuf, unsigned int*  send_len )
{
    unsigned char  buf[BUF_LEN];
    unsigned int   len = 0;
    unsigned char  str_buf[STR_LEN];
    unsigned int   str_len = 0;
    unsigned int   i = 0;
   

    memset( buf, 0, sizeof(buf) );
    len = 0;

    memset( str_buf, 0, sizeof(str_buf) );
    str_len = 0;   
    sprintf( (char *)str_buf, "SENS%d", slot_id );
    str_len = strlen((char *)str_buf);

    for ( i = 0; i < str_len; i++ )
        buf[len++] = str_buf[i];

    buf[len++] = ':';

    memset( str_buf, 0, sizeof(str_buf) );
    str_len = 0;
    if ( chan_id == 0xFF ) 
        sprintf( (char *)str_buf, "ALL" );
    else
        sprintf( (char *)str_buf, "CHAN%d", chan_id );
    str_len = strlen((char *)str_buf);

    for ( i = 0; i < str_len; i++ )
        buf[len++] = str_buf[i];

    buf[len++] = ':';

    buf[len++] = 'P';
    buf[len++] = 'O';
    buf[len++] = 'W';
    buf[len++] = ':';
    buf[len++] = 'W';
    buf[len++] = 'A';
    buf[len++] = 'V';
    buf[len++] = '?';

    memcpy( sendbuf, buf, len );
    *send_len = len;

    return  len;
}


extern int  make_net_set_wl_frame( unsigned char  slot_id, unsigned char  chan_id,  unsigned int  wl,
                                unsigned char* sendbuf, unsigned int*  send_len )
{
    unsigned char  buf[BUF_LEN];
    unsigned int   len = 0;
    unsigned char  str_buf[STR_LEN];
    unsigned int   str_len = 0;
    unsigned int   i = 0;
   

    memset( buf, 0, sizeof(buf) );
    len = 0;

    memset( str_buf, 0, sizeof(str_buf) );
    str_len = 0;   
    sprintf( (char *)str_buf, "SENS%d", slot_id );
    str_len = strlen((char *)str_buf);

    for ( i = 0; i < str_len; i++ )
        buf[len++] = str_buf[i];

    buf[len++] = ':';

    memset( str_buf, 0, sizeof(str_buf) );
    str_len = 0;

    sprintf( (char *)str_buf, "CHAN%d", chan_id );
    str_len = strlen((char *)str_buf);

    for ( i = 0; i < str_len; i++ )
        buf[len++] = str_buf[i];

    buf[len++] = ':';

    buf[len++] = 'P';
    buf[len++] = 'O';
    buf[len++] = 'W';
    buf[len++] = ':';
    buf[len++] = 'W';
    buf[len++] = 'A';
    buf[len++] = 'V';
    buf[len++] = ' ';


    memset( str_buf, 0, sizeof(str_buf) );
    str_len = 0;

    sprintf( (char *)str_buf, "%.4f", wl/10000.0 );
    str_len = strlen((char *)str_buf);

    for ( i = 0; i < str_len; i++ )
        buf[len++] = str_buf[i];


    memcpy( sendbuf, buf, len );
    *send_len = len;

    return  len;

}

    
extern int decode_pm_buf( unsigned char slot_id, unsigned char chan_id, unsigned char* recbuf, unsigned int rec_len )
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
		slot_array[slot_id].module.pm.chan[chan_id].pm_value = (int)(pm_value*10000);
	    slot_array[slot_id].module.pm.chan[chan_id].data_flag = DATA_OK;
		return  1;
	}


	memset( str, 0, sizeof(str) );
	strncpy( (char *)str,(char *) recbuf, pos);

	if ( ((str[0] > 0x2F)&&( str[0] < 0x3A)) ||  (str[0] == '-')&&(str[1] != '.')  )
	{
	    pm_value = atof((char *)str);
	    slot_array[slot_id].module.pm.chan[0].pm_value1 = (int)(pm_value*10000);
	    slot_array[slot_id].module.pm.chan[0].data_flag = DATA_OK;
	} else {
		if( str[0] == 'U')
	        slot_array[slot_id].module.pm.chan[0].data_flag = DATA_UNCAL;
		else
	        slot_array[slot_id].module.pm.chan[0].data_flag = DATA_UNKNOWN;
	}


	if ( ((recbuf[pos+1]>0x2F)&&(recbuf[pos+1]<0x3A)) ||  ((recbuf[pos+1] == '-')&&(recbuf[pos+2] != '.') ) )
	{
	    pm_value = atof((char *)&recbuf[pos+1]);
	    slot_array[slot_id].module.pm.chan[1].pm_value1 = (int)(pm_value*10000);
	    slot_array[slot_id].module.pm.chan[1].data_flag = DATA_OK;
	} else {
		if( str[0] == 'U')
	        slot_array[slot_id].module.pm.chan[1].data_flag = DATA_UNCAL;
		else
	        slot_array[slot_id].module.pm.chan[1].data_flag = DATA_UNKNOWN;
	}

	return  1;
}

//1
extern int decode_pm_buf1( unsigned char slot_id, unsigned char chan_id, unsigned char* recbuf, unsigned int rec_len )
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

//2
extern int decode_pm_buf2( unsigned char slot_id, unsigned char chan_id, unsigned char* recbuf, unsigned int rec_len )
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

//3
extern int decode_pm_buf3( unsigned char slot_id, unsigned char chan_id, unsigned char* recbuf, unsigned int rec_len )
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

extern int decode_wl_buf( unsigned char slot_id, unsigned char chan_id, unsigned char* recbuf, unsigned int rec_len )
{
	double          wl_value = 0;
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
		wl_value = atof((char *)recbuf);
		slot_array[slot_id].module.pm.chan[chan_id].wl_value = (int)(wl_value*10000);
	    slot_array[slot_id].module.pm.chan[chan_id].data_flag = DATA_OK;

		return  1;
	}
	memset( str, 0, sizeof(str) );
	strncpy( (char *)str, (char *)recbuf, pos);

	if ( ((str[0] > 0x2F)&&( str[0] < 0x3A)) ||  (str[0] == '-')&&(str[1] != '.')  )
	{
	    wl_value = atof((char *)str);
	    slot_array[slot_id].module.pm.chan[0].wl_value = (int)(wl_value*10000);
	    slot_array[slot_id].module.pm.chan[0].data_flag = DATA_OK;
	} else {
		if( str[0] == 'U')
	        slot_array[slot_id].module.pm.chan[0].data_flag = DATA_UNCAL;
		else
	        slot_array[slot_id].module.pm.chan[0].data_flag = DATA_UNKNOWN;
	}


	if ( ((recbuf[pos+1]>0x2F)&&(recbuf[pos+1]<0x3A)) ||  ((recbuf[pos+1] == '-')&&(recbuf[pos+2] != '.') ) )
	{
	    wl_value = atof((char *)&recbuf[pos+1]);
	    slot_array[slot_id].module.pm.chan[1].wl_value = (int)(wl_value*10000);
	    slot_array[slot_id].module.pm.chan[1].data_flag = DATA_OK;
	} else {
		if( str[0] == 'U')
	        slot_array[slot_id].module.pm.chan[1].data_flag = DATA_UNCAL;
		else
	        slot_array[slot_id].module.pm.chan[1].data_flag = DATA_UNKNOWN;
	}

	return  1;
}

extern int decode_wl_buf1( unsigned char slot_id, unsigned char chan_id, unsigned char* recbuf, unsigned int rec_len )
{
	double          wl_value = 0;
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
		wl_value = atof((char *)recbuf);
		slot_array[slot_id].module.pm.chan[chan_id].wl_value1 = (int)(wl_value*10000);
		slot_array[slot_id].module.pm.chan[chan_id].data_flag1 = DATA_OK;
		
		return  1;
	}
	memset( str, 0, sizeof(str) );
	strncpy( (char *)str, (char *)recbuf, pos);
	
	if ( ((str[0] > 0x2F)&&( str[0] < 0x3A)) ||  (str[0] == '-')&&(str[1] != '.')  )
	{
		wl_value = atof((char *)str);
		slot_array[slot_id].module.pm.chan[0].wl_value1 = (int)(wl_value*10000);
		slot_array[slot_id].module.pm.chan[0].data_flag1 = DATA_OK;
	} else {
		if( str[0] == 'U')
			slot_array[slot_id].module.pm.chan[0].data_flag1 = DATA_UNCAL;
		else
			slot_array[slot_id].module.pm.chan[0].data_flag1 = DATA_UNKNOWN;
	}
	
	
	if ( ((recbuf[pos+1]>0x2F)&&(recbuf[pos+1]<0x3A)) ||  ((recbuf[pos+1] == '-')&&(recbuf[pos+2] != '.') ) )
	{
		wl_value = atof((char *)&recbuf[pos+1]);
		slot_array[slot_id].module.pm.chan[1].wl_value1 = (int)(wl_value*10000);
		slot_array[slot_id].module.pm.chan[1].data_flag1 = DATA_OK;
	} else {
		if( str[0] == 'U')
			slot_array[slot_id].module.pm.chan[1].data_flag1 = DATA_UNCAL;
		else
			slot_array[slot_id].module.pm.chan[1].data_flag1 = DATA_UNKNOWN;
	}
	
	return  1;
}
extern int decode_wl_buf2( unsigned char slot_id, unsigned char chan_id, unsigned char* recbuf, unsigned int rec_len )
{
	double          wl_value = 0;
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
		wl_value = atof((char *)recbuf);
		slot_array[slot_id].module.pm.chan[chan_id].wl_value2 = (int)(wl_value*10000);
		slot_array[slot_id].module.pm.chan[chan_id].data_flag2 = DATA_OK;
		
		return  1;
	}
	memset( str, 0, sizeof(str) );
	strncpy( (char *)str, (char *)recbuf, pos);
	
	if ( ((str[0] > 0x2F)&&( str[0] < 0x3A)) ||  (str[0] == '-')&&(str[1] != '.')  )
	{
		wl_value = atof((char *)str);
		slot_array[slot_id].module.pm.chan[0].wl_value2 = (int)(wl_value*10000);
		slot_array[slot_id].module.pm.chan[0].data_flag2 = DATA_OK;
	} else {
		if( str[0] == 'U')
			slot_array[slot_id].module.pm.chan[0].data_flag2 = DATA_UNCAL;
		else
			slot_array[slot_id].module.pm.chan[0].data_flag2 = DATA_UNKNOWN;
	}
	
	
	if ( ((recbuf[pos+1]>0x2F)&&(recbuf[pos+1]<0x3A)) ||  ((recbuf[pos+1] == '-')&&(recbuf[pos+2] != '.') ) )
	{
		wl_value = atof((char *)&recbuf[pos+1]);
		slot_array[slot_id].module.pm.chan[1].wl_value2 = (int)(wl_value*10000);
		slot_array[slot_id].module.pm.chan[1].data_flag2 = DATA_OK;
	} else {
		if( str[0] == 'U')
			slot_array[slot_id].module.pm.chan[1].data_flag2 = DATA_UNCAL;
		else
			slot_array[slot_id].module.pm.chan[1].data_flag2 = DATA_UNKNOWN;
	}
	
	return  1;
}
extern int decode_wl_buf3( unsigned char slot_id, unsigned char chan_id, unsigned char* recbuf, unsigned int rec_len )
{
	double          wl_value = 0;
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
		wl_value = atof((char *)recbuf);
		slot_array[slot_id].module.pm.chan[chan_id].wl_value3 = (int)(wl_value*10000);
		slot_array[slot_id].module.pm.chan[chan_id].data_flag3 = DATA_OK;
		
		return  1;
	}
	memset( str, 0, sizeof(str) );
	strncpy( (char *)str, (char *)recbuf, pos);
	
	if ( ((str[0] > 0x2F)&&( str[0] < 0x3A)) ||  (str[0] == '-')&&(str[1] != '.')  )
	{
		wl_value = atof((char *)str);
		slot_array[slot_id].module.pm.chan[0].wl_value3 = (int)(wl_value*10000);
		slot_array[slot_id].module.pm.chan[0].data_flag3 = DATA_OK;
	} else {
		if( str[0] == 'U')
			slot_array[slot_id].module.pm.chan[0].data_flag3 = DATA_UNCAL;
		else
			slot_array[slot_id].module.pm.chan[0].data_flag3 = DATA_UNKNOWN;
	}
	
	
	if ( ((recbuf[pos+1]>0x2F)&&(recbuf[pos+1]<0x3A)) ||  ((recbuf[pos+1] == '-')&&(recbuf[pos+2] != '.') ) )
	{
		wl_value = atof((char *)&recbuf[pos+1]);
		slot_array[slot_id].module.pm.chan[1].wl_value3 = (int)(wl_value*10000);
		slot_array[slot_id].module.pm.chan[1].data_flag3 = DATA_OK;
	} else {
		if( str[0] == 'U')
			slot_array[slot_id].module.pm.chan[1].data_flag3 = DATA_UNCAL;
		else
			slot_array[slot_id].module.pm.chan[1].data_flag3 = DATA_UNKNOWN;
	}
	
	return  1;
}

extern   int  WINAPI  route_udp_client1( LPVOID  lpParam )
{
    int  iMode  = 1;
    int  socket_fd = 0;
    struct sockaddr_in   server_addr; //服务器地址
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
	char				 chBuf[100];
    
//	socket_fd =socket(AF_INET, SOCK_DGRAM, 0 );  //创建连接的SOCKET
    printf( "Can not init socket!\n");
    
	sprintf((char*)chBuf, "%d", socket_fd);
	MessageBox(GetActiveWindow(), chBuf, "", MB_OK);

    udp_port = 8686;    
  
    memset(&server_addr, 0, sizeof(struct sockaddr_in));
    server_addr.sin_family      = AF_INET;
    server_addr.sin_port        = htons(udp_port); 
    server_addr.sin_addr.s_addr = htonl(g_ipArray1);	


	iMode = 1;
    ioctlsocket(socket_fd, FIONBIO,(u_long FAR*)&iMode);//控制套接口的模式，本函数可用于任一状态的任一个套接口
                         //允许或禁止套接口的非阻塞模式        
	
	while( 1 )
	{
		for ( slot_id = 0; slot_id < MAX_SLOT_NUM; slot_id++ )
		{
			for ( chan_id = 0; chan_id < MAX_PM_CHAN_NUM; chan_id++ )
			{
                ret = is_wl_set( slot_id, chan_id, &wl );
		        if ( ret > 0 )
				{
					memset( sendbuf, 0, sizeof(sendbuf) );
                    make_net_set_wl_frame( slot_id, chan_id,  wl,(unsigned char *) sendbuf, &send_len );
			        clear_wl_set( slot_id, chan_id );

					addr_len = sizeof( struct sockaddr );
                    sendto( socket_fd, sendbuf, send_len, 0, (struct sockaddr *)(&server_addr), addr_len );

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

            ret = sendto( socket_fd, sendbuf, send_len, 0, (struct sockaddr *)(&server_addr), addr_len );
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
            addr_len = sizeof( struct sockaddr );

			memset( recbuf, 0, sizeof(recbuf) );
            ret = recvfrom( socket_fd, recbuf, RECV_BUF_LEN, 0, (struct sockaddr *)(&server_addr), (int *)&addr_len);
            if ( ret > 0 )
			{
			    //decode
				if ( slot_array[slot_id].next_read > 0)
				{
				    decode_wl_buf( slot_id, chan_id, (unsigned char *)recbuf, rec_len );
					slot_array[slot_id].wl_counter++;
				} else {
				    decode_pm_buf1( slot_id, chan_id, (unsigned char *)recbuf, rec_len );
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

    return 0; 

}

extern   int  WINAPI  route_udp_client2( LPVOID  lpParam )
{
    int  socket_fd = 0;
    int  iMode  = 1;

    struct sockaddr_in   server_addr; //服务器地址
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



    udp_port = 8686;    
  
    memset(&server_addr, 0, sizeof(struct sockaddr_in));
    server_addr.sin_family      = AF_INET;
    server_addr.sin_port        = htons(udp_port); 
    server_addr.sin_addr.s_addr = htonl(g_ipArray2);

    socket_fd =socket(AF_INET, SOCK_DGRAM, 0 );  //创建连接的SOCKET
    if ( socket_fd < 0 )
    {
        printf( "Can not init socket!\n");
        return  -1;
    }

    iMode = 1;
    ioctlsocket(socket_fd, FIONBIO, (u_long FAR*)&iMode);//控制套接口的模式，本函数可用于任一状态的任一个套接口
                         //允许或禁止套接口的非阻塞模式        

	while( 1 )
	{
		for ( slot_id = 0; slot_id < MAX_SLOT_NUM; slot_id++ )
		{
			for ( chan_id = 0; chan_id < MAX_PM_CHAN_NUM; chan_id++ )
			{
                ret = is_wl_set( slot_id, chan_id, &wl );
		        if ( ret > 0 )
				{
					memset( sendbuf, 0, sizeof(sendbuf) );
                    make_net_set_wl_frame( slot_id, chan_id,  wl, (unsigned char *)sendbuf, &send_len );
			        clear_wl_set( slot_id, chan_id );

					addr_len = sizeof( struct sockaddr );
                    sendto( socket_fd, sendbuf, send_len, 0, (struct sockaddr *)(&server_addr), addr_len );

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
                make_net_read_power_frame( slot_id, 0xFF, (unsigned char *)sendbuf, &send_len );
				slot_array[slot_id].next_read = 0;
			}

            //UDP SEND:
		    if (send_len < 1)
			    goto  UDP_RECV;

		    time( &first_time );
            addr_len = sizeof( struct sockaddr );

            ret = sendto( socket_fd, sendbuf, send_len, 0, (struct sockaddr *)(&server_addr), addr_len );
            if ( ret <= 0 )
			{
                slot_array[slot_id].status = SLOT_DISCONNECTED;
                PostThreadMessage( g_main_thread_id, REFRESH_DATA_MESSAGE, 0, 0 );
			    Sleep(1);
                continue;
			}
            send_len = 0;
		

			//UDP RECV
            UDP_RECV:
		    Sleep(90);
            addr_len = sizeof( struct sockaddr );

			memset( recbuf, 0, sizeof(recbuf) );
            ret = recvfrom( socket_fd, recbuf, RECV_BUF_LEN, 0, (struct sockaddr *)(&server_addr), (int *)&addr_len );
            if ( ret > 0 )
			{
			    //decode
				if ( slot_array[slot_id].next_read > 0)
				{
				    decode_wl_buf( slot_id, chan_id, (unsigned char *)recbuf, rec_len );
					slot_array[slot_id].wl_counter++;
				} else {
				    decode_pm_buf2( slot_id, chan_id, (unsigned char *)recbuf, rec_len );
				}
			    send_len = 0;

			    if ( slot_array[slot_id].next_read > 0 )
				    slot_array[slot_id].next_read = 0;
			    else
				    slot_array[slot_id].next_read = 1;


                slot_array[slot_id].status = SLOT_CONNECTED;

                PostThreadMessage( g_main_thread_id, REFRESH_DATA_MESSAGE, 0, 0 );

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

    return 0; 

}

extern   int  WINAPI  route_udp_client3( LPVOID  lpParam )
{
    int  socket_fd = 0;
    int  iMode  = 1;

    struct sockaddr_in   server_addr; //服务器地址
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



    udp_port = 8686;    
  
    memset(&server_addr, 0, sizeof(struct sockaddr_in));
    server_addr.sin_family      = AF_INET;
    server_addr.sin_port        = htons(udp_port); 
    server_addr.sin_addr.s_addr = htonl(g_ipArray3);

    socket_fd =socket(AF_INET, SOCK_DGRAM, 0 );  //创建连接的SOCKET
    if ( socket_fd < 0 )
    {
        printf( "Can not init socket!\n");
        return  -1;
    }

    iMode = 1;
    ioctlsocket(socket_fd, FIONBIO, (u_long FAR*)&iMode);//控制套接口的模式，本函数可用于任一状态的任一个套接口
                         //允许或禁止套接口的非阻塞模式        

	while( 1 )
	{
		for ( slot_id = 0; slot_id < MAX_SLOT_NUM; slot_id++ )
		{
			for ( chan_id = 0; chan_id < MAX_PM_CHAN_NUM; chan_id++ )
			{
                ret = is_wl_set( slot_id, chan_id, &wl );
		        if ( ret > 0 )
				{
					memset( sendbuf, 0, sizeof(sendbuf) );
                    make_net_set_wl_frame( slot_id, chan_id,  wl, (unsigned char*)sendbuf, &send_len );
			        clear_wl_set( slot_id, chan_id );

					addr_len = sizeof( struct sockaddr );
                    sendto( socket_fd, sendbuf, send_len, 0, (struct sockaddr *)(&server_addr), addr_len );

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

            ret = sendto( socket_fd, sendbuf, send_len, 0, (struct sockaddr *)(&server_addr), addr_len );
            if ( ret <= 0 )
			{
                slot_array[slot_id].status = SLOT_DISCONNECTED;
                PostThreadMessage( g_main_thread_id, REFRESH_DATA_MESSAGE, 0, 0 );
			    Sleep(1);
                continue;
			}
            send_len = 0;
		

			//UDP RECV
            UDP_RECV:
		    Sleep(90);
            addr_len = sizeof( struct sockaddr );

			memset( recbuf, 0, sizeof(recbuf) );
            ret = recvfrom( socket_fd, recbuf, RECV_BUF_LEN, 0, (struct sockaddr *)(&server_addr), (int *)&addr_len );
            if ( ret > 0 )
			{
			    //decode
				if ( slot_array[slot_id].next_read > 0)
				{
				    decode_wl_buf( slot_id, chan_id,(unsigned char *)recbuf, rec_len );
					slot_array[slot_id].wl_counter++;
				} else {
				    decode_pm_buf3( slot_id, chan_id,(unsigned char *)recbuf, rec_len );
				}
			    send_len = 0;

			    if ( slot_array[slot_id].next_read > 0 )
				    slot_array[slot_id].next_read = 0;
			    else
				    slot_array[slot_id].next_read = 1;


                slot_array[slot_id].status = SLOT_CONNECTED;

                PostThreadMessage( g_main_thread_id, REFRESH_DATA_MESSAGE, 0, 0 );

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

    return 0; 

}



extern int  WINAPI  route_tcp_client( LPVOID  lpParam )
{
    int  socket_fd = 0;
    int  iMode  = 1;

    struct sockaddr_in   server_addr;
    unsigned short int   tcp_port;
    unsigned int         addr_len;
	
             char        sendbuf[SEND_BUF_LEN];
             char        recbuf[RECV_BUF_LEN];
    unsigned int         send_len = 0;
    unsigned int         rec_len  = RECV_BUF_LEN;

	unsigned char        slot_id = 0;
	unsigned char        chan_id = 0;
	unsigned int         wl = 15500000;


             int         ret = 0;     
             int         i = 0;
			 int         err = 0;



SERVER_START:
    closesocket( socket_fd );
	Sleep(TCP_DELAY_MS*2);
	

    tcp_port = 8687;    
  
    memset(&server_addr, 0, sizeof(struct sockaddr_in));
    server_addr.sin_family      = AF_INET;
    server_addr.sin_port        = htons(tcp_port); 
    server_addr.sin_addr.s_addr = htonl(g_ip);

    socket_fd =socket(AF_INET, SOCK_STREAM, 0 );
    if ( socket_fd < 0 )
    {
        printf( "Can not init socket!\n");
        return  -1;
    }

	Sleep(TCP_DELAY_MS*2);

    iMode = 1;
    ioctlsocket(socket_fd, FIONBIO, (u_long FAR*)&iMode);




SERVER_CONNECT_DEAL:
	Sleep(TCP_DELAY_MS*2);
	addr_len = sizeof( struct sockaddr );
	ret = connect( socket_fd, (struct sockaddr *)&server_addr, addr_len );
	if ( ret < 0 )
	{
		printf("Connect is failed...\n");
		err = WSAGetLastError();
		if ( err == WSAEWOULDBLOCK )
			goto  SERVER_LOOP_START;

		Sleep(TCP_DELAY_MS);
		goto  SERVER_CONNECT_DEAL;
	}
	Sleep(TCP_DELAY_MS);



SERVER_LOOP_START:
	while( 1 )
	{
		//??
        ret = is_wl_set( slot_id, chan_id, &wl );
		if ( ret > 0 )
		{
            make_net_set_wl_frame( slot_id, chan_id,  wl, (unsigned char*)sendbuf, &send_len );
			clear_wl_set( slot_id, chan_id );
			goto  TCP_SEND;
		}

        make_net_read_power_frame( slot_id, chan_id, (unsigned char*)sendbuf, &send_len );


TCP_SEND:
		if (send_len < 1)
			goto  TCP_RECV;

		Sleep(TCP_DELAY_MS);
        ret = send( socket_fd, sendbuf, send_len, 0 );
        if ( ret < 0 )
		{
			send_len = 0;
			err = WSAGetLastError();
			if ( err == WSAENETDOWN || err == WSAENOTCONN || err == WSAENOTSOCK || err == WSAESHUTDOWN ||
				 err == WSAEHOSTUNREACH || err == WSAETIMEDOUT || err ==WSAECONNABORTED )
				goto   SERVER_START;
		}


TCP_RECV:
		Sleep(TCP_DELAY_MS);
        addr_len = sizeof( struct sockaddr );
		memset( recbuf, 0, sizeof( recbuf) );
        ret = recv( socket_fd, recbuf, RECV_BUF_LEN, 0 );
        if ( ret > 0 )
		{
		    decode_pm_buf( slot_id, chan_id, (unsigned char*)recbuf, rec_len );
			send_len = 0;

            PostThreadMessage( g_main_thread_id, REFRESH_DATA_MESSAGE, 0, 0 );

		} else if ( ret < 0 )
		{
			send_len = 0;
			err = WSAGetLastError();
		    if ( err == WSAEWOULDBLOCK )
			    goto  TCP_RECV;

			if ( err == WSAENETDOWN || err == WSAENOTCONN || err == WSAENOTSOCK || err == WSAESHUTDOWN || 
				 err == WSAEHOSTUNREACH || err == WSAETIMEDOUT || err ==WSAECONNABORTED )
				goto   SERVER_START;
		}
	}

    return 0; 

}


extern int  create_udp_client1( void )
{

// 	char chBuf[100];
// 	socket_fd =socket(AF_INET, SOCK_DGRAM, 0 ); 
// 	sprintf(chBuf, "%d", socket_fd);
// 	OutputDebugString(chBuf);
  	
    hThread1 = CreateThread( NULL, 0, (LPTHREAD_START_ROUTINE)route_udp_client1, NULL, 0, &g_net_thread_id1 );
	if ( hThread1 == NULL )
		return  -1;

	return  1;
}

extern int  create_udp_client2( void )
{	
	
     hThread2 = CreateThread( NULL, 0, (LPTHREAD_START_ROUTINE)route_udp_client2, NULL, 0, &g_net_thread_id2 );
	
	if ( hThread2 == NULL )
		return  -1;
	
	return  1;
}

extern int  create_udp_client3( void )
{
	
    hThread3 = CreateThread( NULL, 0, (LPTHREAD_START_ROUTINE)route_udp_client3, NULL, 0, &g_net_thread_id3 );
	
	if ( hThread3 == NULL )
		return  -1;
	
	return  1;
}

extern int  create_tcp_client( void )
{

	HANDLE  hThread = NULL; 

    hThread = CreateThread( NULL, 0, (LPTHREAD_START_ROUTINE)route_tcp_client, NULL, 0, &g_net_thread_id );

	if ( hThread == NULL )
		return  -1;

	return  1;
}
