#ifndef  __OPLINK_CONST_HEADER__
#define  __OPLINK_CONST_HEADER__




//define  GUI STYLE
#define	 MAIN_FOUR_ROW_GUI	 	             0
#define	 MAIN_THREE_ROW_GUI	 	             1
#define	 MAIN_TWO_ROW_GUI	 	             2
#define	 MAIN_ONE_ROW_GUI	 	             3

#define	 MAIN_DETAIL_GUI	 	             4
#define	 MAIN_ZERO_GUI	 	                 5
#define	 MAIN_SETCAL_GUI	 	             6

#define	 MAIN_ERROR_GUI	 	                 7
#define	 MAIN_ABOUT_GUI	 	                 8




//define  HOLD or CONTINUE
#define	 CONTINUE_STATUS	 	             0
#define	 HOLD_STATUS	 	                 1


//define  GUI TRANSFORM DIRECTION
#define	 UP_GUI	 	                         0
#define	 DOWN_GUI	 	                     1


#define	 SEPARATOR_SPACE	 	             0

#define	 FACE_SPACE	 	                     6
#define	 FILL_SPACE	 	                     4

#define	 RIGHT_BAR_LEN	 			         80
#define	 RIGHT_BAR_HEIGHT	    	         LCD_HEIGHT
#define	 MENU_LEN	 			             RIGHT_BAR_LEN
#define	 MENU_HEIGHT	    	             80

#define	 TOP_BAR_LEN	                     (LCD_LEN - SEPARATOR_SPACE - RIGHT_BAR_LEN)
#define	 TOP_BAR_HEIGHT	 			         30

#define	 BOTTOM_BAR_LEN	                     TOP_BAR_LEN
#define	 BOTTOM_BAR_HEIGHT	 	             TOP_BAR_HEIGHT


#define  UNKNOWN_MODULE                      (-1)
#define  TLS_MODULE                          0
#define  PM_MODULE                           1
#define  SW_MODULE                           2


#define  UNKNOWN_CHAN                        (-1)
#define  FREQ_CHAN                           0
#define  PM_CHAN                             1


#define  MAX_SLOT_NUM                        2

#define  INVALID_SLOT_ID                     (-1)

#define  ENABLE_SLOT                         0
#define  DISABLE_SLOT                        1

#define  ENABLE_CHAN                         0
#define  DISABLE_CHAN                        1

#define  SLOT_DISCONNECTED                   0
#define  SLOT_CONNECTED                      1



//define mode
//#define  AUTO_MODE                           0x00
#define  MANUAL_MODE                         0x01


//define range
#define  RANGE_0                             0x00
#define  RANGE_1                             0x01
#define  RANGE_2                             0x02
#define  RANGE_3                             0x03



//define  text len
#define  MENU_TEXT_LEN                       32
#define  INPUT_TEXT_LEN                      256
#define  BUTTON_TEXT_LEN                     32

#define	 FONT_HZ_16			                 3



//define type id
#define  DISABLE_TYPE                        0
#define  POWER_TYPE                          1
#define  FREQ_TYPE                           2
#define  WL_TYPE                             3
#define  CHANNEL_TYPE                        4

//power unit define
#define  DBM_UNIT                            0x00
#define  DB_UNIT                             0x01
#define  MW_UNIT                             0x02
#define  UW_UNIT                             0x03
#define  NW_UNIT                             0x05


#define  NM_UNIT                             0


#define  PM_MW_UNIT                          0
#define  PM_W_UNIT                           1
#define  PM_DB_UNIT                          2
#define  PM_DBM_UNIT                         3


//define move or edit state
#define  MOVE_STATE                          0
#define  EDIT_STATE                          1


//define shuttle direction
#define  UP_DIRECTION                        0
#define  DOWN_DIRECTION                      1


//define communication flag
#define  COM_IDLE                            0
#define  COM_SENT                            1
#define  COM_RECV                            2


//define light speed
#define  LIGHT_SPEED                         299792458


//define timer
#define  RT_TIMER                            0x01
#define  AD_TIMER                            0x02
#define  CLEAR_TIMER                         0x03


//define clear information time second
#define  CLEAR_INFORMATION_TIME_SEC          5

//define clear remote ip time second
#define  CLEAR_REMOTE_IP_TIME_SEC            3


//define AD max num
#define  AD_MAX_NUM                          10


//define BACKLOG
#define  BACKLOG                             20
#define  MAX_EVENTS                          20


//define i2c address
#define  UNKNOWN_ADDR                        (-1)


//define pm max chan
#define  MAX_PM_CHAN_NUM                     2

//define tls max chan
#define  MAX_TLS_CHAN_NUM                    2

//define sw max chan
#define  MAX_SW_NUM                          16


//define com id
#define  UNKNOWN_COM                         0x00
#define  I2C_COM                             0x01
#define  SPI_COM                             0x02
#define  RS232_COM                           0x03


//define param id
#define  WL_PARAM                            0x00
#define  PM_PARAM                            0x01
#define  AVTIME_PARAM                        0x02
#define  MODE_PARAM                          0x03
#define  RANGE_PARAM                         0x04



//define wl limited
#define  MAX_WL                              17000000
#define  MIN_WL                              6000000



//define pos id
#define  POS_ID_0                            0x00
#define  POS_ID_1                            0x01
#define  POS_ID_2                            0x02
#define  POS_ID_3                            0x03







//define editstatus
#define  UNEDIT_STATUS                       0x00
#define  EDIT_STATUS                         0x01


//defin  key value
#define  SKEY_S1                             0x05
#define  SKEY_S2                             0x1C
#define  SKEY_S3                             0x1A
#define  SKEY_S4                             0x08
#define  SKEY_S5                             0x06
#define  SKEY_S6                             0x04

#define  SKEY_UP                             0x1B
#define  SKEY_DOWN                           0x09
#define  SKEY_ENTER                          0x07

#define  SHUTTLE_LEFT                        0x81
#define  SHUTTLE_RIGHT                       0x80
#define  SHUTTLE_ENTER                       0x82



//define power meter status
#define  STATE_SUCCESS                       0x20
#define  STATE_BUSY                          0x21
#define  STATE_INVALID_CMD                   0x22
#define  STATE_INVALID_PARAM                 0x23
#define  STATE_INVALID_DATA                  0x24
#define  STATE_EXE_FAIL                      0x25
#define  STATE_NOT_SUPPORT                   0x26
#define  STATE_NOT_ONLINE                    0x27
#define  STATE_UNCAL                         0x28
#define  STATE_OTHER_ERROR                   0x29

//define power meter timeout
#define  BUSY_TIMEOUT                        15



//define data display flag
#define  DATA_OK                             0x00
#define  DATA_UNKNOWN                        0x01
#define  DATA_UNCAL                          0x02
#define  DATA_ERROR                          0x03


//define version len
#define  MAX_VERSION_LEN                     16                      



//add for WIN32
#define  REFRESH_DATA_MESSAGE                ( WM_USER + 1 )
#define  REFRESH_GUI_MESSAGE                 ( WM_USER + 2 )




#endif   /*__OPLINK_CONST_HEADER__*/

