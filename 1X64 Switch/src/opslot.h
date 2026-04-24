#ifndef __OPLINK_SLOT_HEADER_H__
#define __OPLINK_SLOT_HEADER_H__

#include  "opconst.h"


#ifdef __cplusplus 
extern "C" { 
#endif 


struct  pm_chan {
    int             pm_value;
	//
	int             pm_value1;
	int             pm_value2;
	int             pm_value3;

    unsigned char   pm_unit;

    int             wl_value;
	int             wl_value1;
	int             wl_value2;
	int             wl_value3;
    unsigned char   wl_unit;

    int             avtime_value;
    int             mode_value;
    int             range_value;

    int             set_pm_value;
    int             ref_pm_value;
    int             set_wl_value;
	int             set_wl_value1;
	int             set_wl_value2;
	int             set_wl_value3;

    int             set_avtime_value;
    int             set_mode_value;
    int             set_range_value;

    unsigned char   data_flag;
	unsigned char   data_flag1;
	unsigned char   data_flag2;
	unsigned char   data_flag3;

    unsigned char   set_zero_flag;
    unsigned char   set_setcal_flag;

    unsigned char   set_pm_flag;
    unsigned char   set_wl_flag;
    unsigned char   set_avtime_flag;
    unsigned char   set_mode_flag;
    unsigned char   set_range_flag;

    unsigned char   need_pm_flag;
    unsigned char   need_wl_flag;
    unsigned char   need_avtime_flag;
    unsigned char   need_mode_flag;
    unsigned char   need_range_flag;

    unsigned char   need_chaninfo0_flag;
    unsigned char   need_chaninfo1_flag;

    unsigned char   chan_pn[MAX_VERSION_LEN];
    unsigned char   chan_sn[MAX_VERSION_LEN];
    unsigned char   chan_hw_version[MAX_VERSION_LEN];
    unsigned char   chan_calibration_date[MAX_VERSION_LEN];
    unsigned char   chan_made_date[MAX_VERSION_LEN];
};

struct  pm_module {
    struct pm_chan  chan[MAX_PM_CHAN_NUM];
    unsigned char   pm_chan_num;

    unsigned char   need_slotinfo0_flag;
    unsigned char   need_slotinfo1_flag;
};


struct  tls_chan {
    double          pm_value;
    unsigned char   pm_unit;
    int             wl_value;
    unsigned char   wl_unit;
    int             set_pm_value;
    int             ref_pm_value;
    int             set_wl_value;
    unsigned char   set_pm_flag;
    unsigned char   set_wl_flag;
    unsigned char   data_flag;
    int             avtime;
    int             mode;
    int             range;
};
struct  tls_module {
    struct tls_chan chan[MAX_PM_CHAN_NUM];
    unsigned char   tls_chan_num;
};


struct  sw_module {
    unsigned char   state[MAX_SW_NUM];
    unsigned char   sw_slot_id;
};



union  op_module {
    struct  pm_module   pm;
    struct  tls_module  tls;
    struct  sw_module   sw;
};




struct  slots {
             char     module_addr;
             char     module_id;
    unsigned char     com_id;

    unsigned char     status;
    unsigned char     enable;

    union  op_module  module;

    unsigned char     slot_pn[MAX_VERSION_LEN];
    unsigned char     slot_sn[MAX_VERSION_LEN];
    unsigned char     slot_sw_version[MAX_VERSION_LEN];
    unsigned char     slot_hw_version[MAX_VERSION_LEN];
    unsigned char     slot_made_date[MAX_VERSION_LEN];

	unsigned char     wl_counter;
	unsigned char     next_read;

};



struct  row_pm {
    unsigned char   slot_id;
    unsigned char   chan_id;
    unsigned char   int_bit;
    unsigned char   part_bit;
};








#define	 slot_is_disable(slot)               \
              ( ((slot_array[slot].enable)==DISABLE_SLOT)?1:0)
#define	 slot_set_able(slot)                 \
              ( (slot_array[slot].enable) = ENABLE_SLOT )

#define	 slot_is_disconnected(slot)          \
              ( ((slot_array[slot].status)==SLOT_DISCONNECTED)?1:0)
#define	 slot_set_connected(slot)            \
              ( (slot_array[slot].status) = SLOT_CONNECTED )

#define	 slot_enable(slot)             \
              (slot_array[slot].enable=1)



#define	 is_pm_slot(slot)               \
              ( ((slot_array[slot].module_id)==PM_MODULE)?1:0)
#define	 is_tls_slot(slot)               \
              ( ((slot_array[slot].module_id)==TLS_MODULE)?1:0)
 
#define	 slot_addr(slot)               \
              ( (slot_array[slot].module_addr) )




#define	 tls_is_set_wl_flag(slot, nchan)             \
              ( ((slot_array[slot].module.tls.chan[nchan].set_wl_flag)==1)?1:0)
#define	 tls_is_set_pm_flag(slot, nchan)             \
              ( ((slot_array[slot].module.tls.chan[nchan].set_pm_flag)==1)?1:0)

#define	 tls_get_set_pm_value(slot, nchan)             \
              (slot_array[slot].module.tls.chan[nchan].set_pm_value)
#define	 tls_get_set_wl_value(slot, nchan)             \
              (slot_array[slot].module.tls.chan[nchan].set_wl_value)
#define	 tls_clear_set_pm_flag(slot, nchan)             \
              (slot_array[slot].module.tls.chan[nchan].set_pm_flag = 0)
#define	 tls_clear_set_wl_flag(slot, nchan)             \
              (slot_array[slot].module.tls.chan[nchan].set_wl_flag = 0)

#define	 tls_set_pm_value(slot, nchan, value)             \
              ( slot_array[slot].module.tls.chan[nchan].pm_value = value);
#define	 tls_set_wl_value(slot, nchan, value)             \
              ( slot_array[slot].module.tls.chan[nchan].wl_value = value);



#define	 set_data_flag(slot, nchan, flag)             \
              (slot_array[slot].module.pm.chan[nchan].data_flag = flag)
#define	 pm_is_set_wl_flag(slot, nchan)             \
              ( ((slot_array[slot].module.pm.chan[nchan].set_wl_flag)==1)?1:0)
#define	 pm_is_set_pm_flag(slot, nchan)             \
              ( ((slot_array[slot].module.pm.chan[nchan].set_pm_flag)==1)?1:0)

#define	 pm_get_set_pm_value(slot, nchan)             \
              (slot_array[slot].module.pm.chan[nchan].set_pm_value)
#define	 pm_get_set_wl_value(slot, nchan)             \
              (slot_array[slot].module.pm.chan[nchan].set_wl_value)
#define	 pm_clear_set_pm_flag(slot, nchan)             \
              (slot_array[slot].module.pm.chan[nchan].set_pm_flag = 0)
#define	 pm_clear_set_wl_flag(slot, nchan)             \
              (slot_array[slot].module.pm.chan[nchan].set_wl_flag = 0)

#define	 pm_set_set_pm_value(slot, nchan, value)             \
              ( slot_array[slot].module.pm.chan[nchan].set_pm_value = value);
#define	 pm_set_set_wl_value(slot, nchan, value)             \
              ( slot_array[slot].module.pm.chan[nchan].set_wl_value = value);

#define	 pm_write_pm_value(slot, nchan, value)             \
              {  \
                  ( slot_array[slot].module.pm.chan[nchan].pm_value = value);     \
                  ( slot_array[slot].module.pm.chan[nchan].data_flag = DATA_OK);  \
              }
#define	 pm_write_wl_value(slot, nchan, value)             \
              {  \
                  ( slot_array[slot].module.pm.chan[nchan].wl_value = value);    \
                  ( slot_array[slot].module.pm.chan[nchan].data_flag = DATA_OK);  \
              }
#define	 pm_write_avtime_value(slot, nchan, value)             \
              {  \
                  ( slot_array[slot].module.pm.chan[nchan].avtime_value = value);    \
                  ( slot_array[slot].module.pm.chan[nchan].data_flag = DATA_OK);  \
              }
#define	 pm_write_mode_value(slot, nchan, value)             \
              {  \
                  ( slot_array[slot].module.pm.chan[nchan].mode_value = value);    \
                  ( slot_array[slot].module.pm.chan[nchan].data_flag = DATA_OK);  \
              }
#define	 pm_write_range_value(slot, nchan, value)             \
              {  \
                  ( slot_array[slot].module.pm.chan[nchan].range_value = value);    \
                  ( slot_array[slot].module.pm.chan[nchan].data_flag = DATA_OK);  \
              }
#define	 pm_get_pm_value(slot, nchan)             \
              (slot_array[slot].module.pm.chan[nchan].pm_value)
#define	 pm_get_wl_value(slot, nchan)             \
              (slot_array[slot].module.pm.chan[nchan].wl_value)



extern   struct  slots   slot_array[MAX_SLOT_NUM];



int  init_slot_all( void );
int  init_slot_auto_check_all( void );


#ifdef __cplusplus 
} 
#endif 


#endif  /*__OPLINK_SLOT_HEADER_H__*/
