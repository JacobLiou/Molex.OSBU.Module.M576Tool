#include "StdAfx.h"

#include  <string.h>
#include  "opslot.h"


struct slots  slot_array[MAX_SLOT_NUM];

int  init_slot_all( void )
{
    int  i = 0;

    memset(slot_array, 0, sizeof(slot_array));
    for ( i = 0; i < MAX_SLOT_NUM; i++ )
    {
        slot_array[i].module_addr = UNKNOWN_ADDR;
        slot_array[i].module_id = UNKNOWN_MODULE;
        slot_array[i].com_id = UNKNOWN_COM;

        slot_array[i].status = SLOT_DISCONNECTED;
        slot_array[i].enable = DISABLE_SLOT;

    }

    return  1;
}


int  init_slot_auto_check_all( void )
{
    slot_array[0].module_addr = 0x40;
    slot_array[0].module_id = PM_MODULE;
    slot_array[0].com_id = I2C_COM;
    slot_array[0].enable = ENABLE_SLOT;

	slot_array[0].module.pm.chan[0].pm_value1 = -9999;
	slot_array[0].module.pm.chan[0].pm_value2 = -9999;
	slot_array[0].module.pm.chan[0].pm_value3 = -9999;
    slot_array[0].module.pm.chan[0].pm_unit = DBM_UNIT;
    slot_array[0].module.pm.chan[0].pm_value = 12321;
    slot_array[0].module.pm.chan[0].wl_unit = NM_UNIT;
    slot_array[0].module.pm.chan[0].wl_value = 15506832;
	slot_array[0].module.pm.chan[0].wl_value1 = 15506832;
	slot_array[0].module.pm.chan[0].wl_value2 = 15506832;
	slot_array[0].module.pm.chan[0].wl_value3 = 15506832;
    slot_array[0].module.pm.chan[0].avtime_value = 10000;
    slot_array[0].module.pm.chan[0].mode_value = 0x00;
    slot_array[0].module.pm.chan[0].range_value = RANGE_0;
    slot_array[0].module.pm.chan[0].set_wl_value = 15526832;
	slot_array[0].module.pm.chan[0].set_wl_value1 = 15526832;
	slot_array[0].module.pm.chan[0].set_wl_value2 = 15526832;
	slot_array[0].module.pm.chan[0].set_wl_value3 = 15526832;

    slot_array[0].module.pm.chan[0].set_avtime_value = 10000;
    slot_array[0].module.pm.chan[0].set_mode_value = 0x00;
    slot_array[0].module.pm.chan[0].set_range_value = RANGE_0;
    slot_array[0].module.pm.chan[0].data_flag = DATA_UNKNOWN;
    slot_array[0].module.pm.chan[0].set_zero_flag = 0;
    slot_array[0].module.pm.chan[0].set_setcal_flag = 0;
    slot_array[0].module.pm.chan[0].set_pm_flag = 0;
    slot_array[0].module.pm.chan[0].set_wl_flag = 0;
    slot_array[0].module.pm.chan[0].set_avtime_flag = 0;
    slot_array[0].module.pm.chan[0].set_mode_flag = 0;
    slot_array[0].module.pm.chan[0].set_range_flag = 0;
    slot_array[0].module.pm.chan[0].need_wl_flag = 1;
    slot_array[0].module.pm.chan[0].need_avtime_flag = 1;
    slot_array[0].module.pm.chan[0].need_mode_flag = 1;
    slot_array[0].module.pm.chan[0].need_range_flag = 1;
    slot_array[0].module.pm.chan[0].need_chaninfo0_flag = 1;
    slot_array[0].module.pm.chan[0].need_chaninfo1_flag = 1;

	slot_array[0].module.pm.chan[1].pm_value1 = -9999;
	slot_array[0].module.pm.chan[1].pm_value2 = -9999;
	slot_array[0].module.pm.chan[1].pm_value3 = -9999;
    slot_array[0].module.pm.chan[1].pm_unit = DBM_UNIT;
    slot_array[0].module.pm.chan[1].pm_value = 36232;
    slot_array[0].module.pm.chan[1].wl_unit = NM_UNIT;
    slot_array[0].module.pm.chan[1].wl_value = 15483796;
	slot_array[0].module.pm.chan[1].wl_value1 = 15483796;
	slot_array[0].module.pm.chan[1].wl_value2 = 15483796;
	slot_array[0].module.pm.chan[1].wl_value3 = 15483796;
    slot_array[0].module.pm.chan[1].avtime_value = 10000;
    slot_array[0].module.pm.chan[1].mode_value = 0x00;
    slot_array[0].module.pm.chan[1].range_value = RANGE_0;
    slot_array[0].module.pm.chan[1].set_wl_value = 15586832;
	slot_array[0].module.pm.chan[1].set_wl_value1 = 15586832;
	slot_array[0].module.pm.chan[1].set_wl_value2 = 15586832;
	slot_array[0].module.pm.chan[1].set_wl_value3 = 15586832;
    slot_array[0].module.pm.chan[1].set_avtime_value = 10000;
    slot_array[0].module.pm.chan[1].set_mode_value = 0x00;
;
    slot_array[0].module.pm.chan[1].set_range_value = RANGE_0;
    slot_array[0].module.pm.chan[1].data_flag = DATA_UNKNOWN;
    slot_array[0].module.pm.chan[1].set_zero_flag = 0;
    slot_array[0].module.pm.chan[1].set_setcal_flag = 0;
    slot_array[0].module.pm.chan[1].set_pm_flag = 0;
    slot_array[0].module.pm.chan[1].set_wl_flag = 0;
    slot_array[0].module.pm.chan[1].set_avtime_flag = 0;
    slot_array[0].module.pm.chan[1].set_mode_flag = 0;
    slot_array[0].module.pm.chan[1].set_range_flag = 0;
    slot_array[0].module.pm.chan[1].need_wl_flag = 1;
    slot_array[0].module.pm.chan[1].need_avtime_flag = 1;
    slot_array[0].module.pm.chan[1].need_mode_flag = 1;
    slot_array[0].module.pm.chan[1].need_range_flag = 1;
    slot_array[0].module.pm.chan[1].need_chaninfo0_flag = 1;
    slot_array[0].module.pm.chan[1].need_chaninfo1_flag = 1;

    slot_array[0].module.pm.pm_chan_num = 2;
    slot_array[0].module.pm.need_slotinfo0_flag = 1; 
    slot_array[0].module.pm.need_slotinfo1_flag = 1;

    slot_array[1].module_addr = 0x41;
    slot_array[1].module_id = PM_MODULE;
    slot_array[1].com_id = I2C_COM;
    slot_array[1].enable = ENABLE_SLOT;
 
	slot_array[1].module.pm.chan[0].pm_value1 = -9999;
	slot_array[1].module.pm.chan[0].pm_value2 = -9999;
	slot_array[1].module.pm.chan[0].pm_value3 = -9999;
    slot_array[1].module.pm.chan[0].pm_unit = DBM_UNIT;
    slot_array[1].module.pm.chan[0].pm_value = 2321;
    slot_array[1].module.pm.chan[0].wl_unit = NM_UNIT;
    slot_array[1].module.pm.chan[0].wl_value = 15517061;
	slot_array[1].module.pm.chan[0].wl_value1 = 15517061;
	slot_array[1].module.pm.chan[0].wl_value2 = 15517061;
	slot_array[1].module.pm.chan[0].wl_value3 = 15517061;
    slot_array[1].module.pm.chan[0].avtime_value = 10000;
    slot_array[1].module.pm.chan[0].mode_value = 0x00;
    slot_array[1].module.pm.chan[0].range_value = RANGE_0;
    slot_array[1].module.pm.chan[0].set_wl_value = 15503302;
	slot_array[1].module.pm.chan[0].set_wl_value1 = 15503302;
	slot_array[1].module.pm.chan[0].set_wl_value2 = 15503302;
	slot_array[1].module.pm.chan[0].set_wl_value3 = 15503302;
    slot_array[1].module.pm.chan[0].set_avtime_value = 10000;
    slot_array[1].module.pm.chan[0].set_mode_value = 0x00;
    slot_array[1].module.pm.chan[0].set_range_value = RANGE_0;
    slot_array[1].module.pm.chan[0].data_flag = DATA_UNKNOWN;
    slot_array[1].module.pm.chan[0].set_zero_flag = 0;
    slot_array[1].module.pm.chan[0].set_setcal_flag = 0;
    slot_array[1].module.pm.chan[0].set_pm_flag = 0;
    slot_array[1].module.pm.chan[0].set_wl_flag = 0;
    slot_array[1].module.pm.chan[0].set_avtime_flag = 0;
    slot_array[1].module.pm.chan[0].set_mode_flag = 0;
    slot_array[1].module.pm.chan[0].set_range_flag = 0;
    slot_array[1].module.pm.chan[0].need_wl_flag = 1;
    slot_array[1].module.pm.chan[0].need_avtime_flag = 1;
    slot_array[1].module.pm.chan[0].need_mode_flag = 1;
    slot_array[1].module.pm.chan[0].need_range_flag = 1;
    slot_array[1].module.pm.chan[0].need_chaninfo0_flag = 1;
    slot_array[1].module.pm.chan[0].need_chaninfo1_flag = 1;

	slot_array[1].module.pm.chan[1].pm_value1 = -9999;
	slot_array[1].module.pm.chan[1].pm_value2 = -9999;
	slot_array[1].module.pm.chan[1].pm_value3 = -9999;
    slot_array[1].module.pm.chan[1].pm_unit = DBM_UNIT;
    slot_array[1].module.pm.chan[1].pm_value = 28006;
    slot_array[1].module.pm.chan[1].wl_unit = NM_UNIT;
    slot_array[1].module.pm.chan[1].wl_value = 15520821;
	slot_array[1].module.pm.chan[1].wl_value1 = 15520821;
	slot_array[1].module.pm.chan[1].wl_value2 = 15520821;
	slot_array[1].module.pm.chan[1].wl_value3 = 15520821;
    slot_array[1].module.pm.chan[1].avtime_value = 10000;
    slot_array[1].module.pm.chan[1].mode_value = 0x00;
    slot_array[1].module.pm.chan[1].range_value = RANGE_0;
    slot_array[1].module.pm.chan[1].set_wl_value = 15512683;
	slot_array[1].module.pm.chan[1].set_wl_value1 = 15512683;
	slot_array[1].module.pm.chan[1].set_wl_value2 = 15512683;
	slot_array[1].module.pm.chan[1].set_wl_value3 = 15512683;
    slot_array[1].module.pm.chan[1].set_avtime_value = 10000;
    slot_array[1].module.pm.chan[1].set_mode_value = 0x00;
    slot_array[1].module.pm.chan[1].set_range_value = RANGE_0;
    slot_array[1].module.pm.chan[1].data_flag = DATA_UNKNOWN;
    slot_array[1].module.pm.chan[1].set_zero_flag = 0;
    slot_array[1].module.pm.chan[1].set_setcal_flag = 0;
    slot_array[1].module.pm.chan[1].set_pm_flag = 0;
    slot_array[1].module.pm.chan[1].set_wl_flag = 0;
    slot_array[1].module.pm.chan[1].set_avtime_flag = 0;
    slot_array[1].module.pm.chan[1].set_mode_flag = 0;
    slot_array[1].module.pm.chan[1].set_range_flag = 0;
    slot_array[1].module.pm.chan[1].need_wl_flag = 1;
    slot_array[1].module.pm.chan[1].need_avtime_flag = 1;
    slot_array[1].module.pm.chan[1].need_mode_flag = 1;
    slot_array[1].module.pm.chan[1].need_range_flag = 1;
    slot_array[1].module.pm.chan[1].need_chaninfo0_flag = 1;
    slot_array[1].module.pm.chan[1].need_chaninfo1_flag = 1;

    slot_array[1].module.pm.pm_chan_num = 2;
    slot_array[1].module.pm.need_slotinfo0_flag = 1; 
    slot_array[1].module.pm.need_slotinfo1_flag = 1;


    return  1;
}