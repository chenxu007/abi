/* bht_L0_genneral.c - Layer 1 implement in windows*/

/*
 * Copyright (c) 2017-2023 Bin Hong Tech, Inc.
 *
 * The right to copy, distribute, modify or otherwise make use
 * of this software may be licensed only pursuant to the terms
 * of an applicable BinHong-tech license agreement.
 */

/*
modification history
--------------------
01a,17may18,cx_  add file
*/
//#ifdef __cplusplus
//extern "C" {
//#endif

#include <bht_L0.h>
#include <bht_L1.h>
#include <bht_L1_a429.h>
#include <assert.h>

#define BHT_A429_DEVICE_MAX      16
#define BHT_A429_CHANNEL_MAX     16

typedef struct
{
    bht_L0_u32 chan_num;
    bht_L1_a429_chan_comm_param_t comm_param;
    bht_L1_a429_rx_chan_gather_param_t gather_param;
//    bht_L0_u32 filter_mode;     /* 0 - Black list, 1 - White list*/
}a429_rx_chan_param_t;

typedef struct
{
    bht_L0_u32 chan_num;
    bht_L0_u32 loop_enable;         /* 0 - disable, 1 - enable */
    bht_L1_a429_slope_e slope;
    bht_L1_a429_chan_comm_param_t comm_param;
    bht_L1_a429_tx_chan_inject_param_t inject_param;
}a429_tx_chan_param_t;

typedef struct
{
    bht_L0_u32 board_num;
    a429_tx_chan_param_t tx_chan_param[BHT_A429_CHANNEL_MAX];
    a429_rx_chan_param_t rx_chan_param[BHT_A429_CHANNEL_MAX];
}a429_device_param_t;

static a429_device_param_t a429_device_param[BHT_A429_DEVICE_MAX];

extern bht_L0_u32 bht_L1_device_softreset(bht_L0_u32);

static bht_L0_u32 
a429_chan_cfg_reg_generate(bht_L1_chan_type_e chan_type, 
        bht_L1_a429_chan_comm_param_t *comm_param,
        bht_L1_a429_rx_chan_gather_param_t *gather_param,
        bht_L1_a429_tx_chan_inject_param_t *inject_param)
{
    bht_L0_u32 value = 0;

    assert(NULL != comm_param);
    
    value = value | (chan_type << 31) | (comm_param->work_mode << 29);

    if(BHT_L1_CHAN_TYPE_TX == chan_type)
    {
        value |= (comm_param->par << 25);

        assert(NULL != inject_param);

        value = value | (inject_param->tb_bits << 28) | \
                (inject_param->tb_gap << 26) | \
                (inject_param->tb_par_en << 25);
    }
    else
    {
        value |= (comm_param->par << 14);

        assert(NULL != gather_param);

        value = value | (gather_param->gather_enable << 16) | \
                (gather_param->recv_mode << 15);
    }

    return value;
}
/* a429 general*/

static bht_L0_u32 
bht_L1_a429_mib_clearall(bht_L0_u32 dev_id)
{
    bht_L0_u32 result = BHT_SUCCESS;
    bht_L0_u32 value;

    value = 0xFFFFFFFF;
    if(BHT_SUCCESS != (result = bht_L0_write_mem32(dev_id, BHT_A429_MIBS_CLR, &value, 1)))
        return result;

    bht_L0_msleep(1);
    
    value = 0;
    result = bht_L0_write_mem32(dev_id, BHT_A429_MIBS_CLR, &value, 1);

    return result;
}

/*
 * @param chan_num 1 - 16
 */
static bht_L0_u32 
bht_L1_a429_mib_clear(bht_L0_u32 dev_id, 
        bht_L0_u32 chan_num, 
        bht_L1_chan_type_e chan_type)
{
    bht_L0_u32 result = BHT_SUCCESS;
    bht_L0_u32 value;

    if((chan_num > 16) | (chan_num < 1))
        return BHT_ERR_INVALID_CHANNEL_NUM;

    if(BHT_L1_CHAN_TYPE_RX == chan_type)
        chan_num += 16;
    
    value = 0x00000001 << (chan_num - 1);
    
    if(BHT_SUCCESS != (result = bht_L0_write_mem32(dev_id, BHT_A429_MIBS_CLR, &value, 1)))
        return result;

    bht_L0_msleep(1);
    
    value = 0; //TODO 确认其他通道写0会不会对统计造成影响
    result = bht_L0_write_mem32(dev_id, BHT_A429_MIBS_CLR, &value, 1);

    //TODO generate mib clear event

    return result;
}

/*
 * @param chan_num 1 - 16
 */
static bht_L0_u32 
bht_L1_a429_mib_get(bht_L0_u32 dev_id, 
        bht_L0_u32 chan_num, 
        bht_L1_chan_type_e chan_type, 
        bht_L1_a429_mib_data_t *mib_data)

{
#define BHT_A429_MIB_BASE_ADDR    BHT_A429_NUM_WORD_CHANNEL0
#define BHT_A429_MIB_CHANNEL_STEP   0x00000010
    bht_L0_u32 offset, result = BHT_SUCCESS;

    if((chan_num > 16) | (chan_num < 1))
        return BHT_ERR_INVALID_CHANNEL_NUM;

    if(BHT_L1_CHAN_TYPE_RX == chan_type)
        chan_num += 16;
    
    offset = BHT_A429_MIB_BASE_ADDR + (chan_num - 1) * BHT_A429_MIB_CHANNEL_STEP;
    
    result = bht_L0_read_mem32(dev_id, offset, (bht_L0_u32 *)mib_data, 2);
    
    return result;
}

bht_L0_u32 
bht_L1_a429_default_init(bht_L0_u32 dev_id)
{
    bht_L0_u32 board_num, chan_num;
    bht_L0_u32 result = BHT_SUCCESS;    
    
    /* reset a429 device */
    if(BHT_SUCCESS != (result = bht_L1_device_softreset(dev_id)))
        return result;

    /* clear MIB */
    result = bht_L1_a429_mib_clearall(dev_id);

    /* record loop and slope register default stat */
    for(board_num = 0; board_num < BHT_A429_DEVICE_MAX; board_num++)
    {
        for(chan_num = 0; chan_num < BHT_A429_CHANNEL_MAX; chan_num++)
        {
            /* send channel, slope default is 1.5us */
            a429_device_param[board_num].tx_chan_param[chan_num].slope = BHT_L1_A429_SLOPE_1_5_US;
            /* default not enable */
            a429_device_param[board_num].tx_chan_param[chan_num].loop_enable = BHT_L1_OPT_DISABLE;
        }
    }

    return result;
}

static bht_L0_u32 
bht_L1_a429_chan_comm_param(bht_L0_u32 dev_id, 
        bht_L0_u32 chan_num,
        bht_L1_chan_type_e chan_type,
        bht_L1_a429_chan_comm_param_t *comm_param, 
        bht_L1_param_opt_e param_opt)
{
    bht_L0_u32 value;
    bht_L0_u32 result = BHT_SUCCESS;
    bht_L0_u32 is_changed = BHT_L0_FALSE;
    bht_L0_u32 board_num = (dev_id & 0x000F0000) >> 16;
    const a429_device_param_t *device_param = NULL;
    bht_L1_a429_chan_comm_param_t * old_comm_param = NULL;
    bht_L1_a429_rx_chan_gather_param_t *gather_param = NULL;
    bht_L1_a429_tx_chan_inject_param_t *inject_param = NULL;
    

    if((chan_num > 16) | (chan_num < 1))
        return BHT_ERR_INVALID_CHANNEL_NUM;

//    if(board_num >= 16)
//        return BHT_ERR_INVALID_BOARD_NUM;
    
    //read
    device_param = &a429_device_param[board_num];
    if(BHT_L1_CHAN_TYPE_RX == chan_type)
    {
        old_comm_param = &device_param->rx_chan_param[chan_num - 1].comm_param;
        gather_param = &device_param->rx_chan_param[chan_num - 1].gather_param;
    }
    else
    {
        old_comm_param = &device_param->tx_chan_param[chan_num - 1].comm_param;
        inject_param = &device_param->tx_chan_param[chan_num - 1].inject_param;
    }

    if(BHT_L1_PARAM_OPT_GET == param_opt)
    {
        memcpy(comm_param, old_comm_param, sizeof(bht_L1_a429_chan_comm_param_t));
        return result;
    }
    
    //compare
    if(0 != memcmp(old_comm_param, comm_param, sizeof(bht_L1_a429_chan_comm_param_t)))
        is_changed = BHT_L0_TRUE;

    if(BHT_L0_FALSE == is_changed)
        return result;

    //param set
    value = 1;
    if(BHT_SUCCESS != (result = bht_L0_write_mem32(dev_id, BHT_A429_CFG_ENABLE, &value, 1)))
        return result;

	value = (chan_type == BHT_L1_CHAN_TYPE_TX) ? (chan_num - 1) : (16 + chan_num - 1);
	printf("%s ,channel choose value = 0x%08x\n", __FUNCTION__, value);
    if(BHT_SUCCESS != (result = bht_L0_write_mem32(dev_id, BHT_A429_CHOOSE_CHANNEL_NUM, &value, 1)))
        return result;

    value = a429_chan_cfg_reg_generate(chan_type, comm_param, gather_param, inject_param);
	printf("%s ,channel cfg value = 0x%08x\n", __FUNCTION__, value);
    if(BHT_SUCCESS != (result = bht_L0_write_mem32(dev_id, BHT_A429_CHANNEL_CFG, &value, 1)))
        return result;

    if(old_comm_param->baud != comm_param->baud)
    {
        value = 100 * 1000000 / comm_param->baud;
        if(BHT_SUCCESS != (result = bht_L0_write_mem32(dev_id, BHT_A429_BAUD_RATE_SET, &value, 1)))
            return result;
    }
    value = 0;
    if(BHT_SUCCESS != (result = bht_L0_write_mem32(dev_id, BHT_A429_CFG_ENABLE, &value, 1)))
        return result;
    
    //TODO generate chan common param change event

    memcpy(old_comm_param, comm_param, sizeof(bht_L1_a429_chan_comm_param_t));

    return result;
}

bht_L0_u32 bht_L1_a429_tx_chan_comm_param(bht_L0_u32 dev_id, 
        bht_L0_u32 chan_num,
        bht_L1_a429_chan_comm_param_t *comm_param, 
        bht_L1_param_opt_e param_opt)
{
    return bht_L1_a429_chan_comm_param(dev_id, chan_num, BHT_L1_CHAN_TYPE_TX, comm_param, param_opt);
}

/* a429 tx channel */
bht_L0_u32 
bht_L1_a429_tx_chan_inject_param(bht_L0_u32 dev_id, 
        bht_L0_u32 chan_num, 
        bht_L1_a429_tx_chan_inject_param_t *inject_param, 
        bht_L1_param_opt_e param_opt)
{
    bht_L0_u32 value;
    bht_L0_u32 result = BHT_SUCCESS;
    bht_L0_u32 is_changed = BHT_L0_FALSE;
    bht_L0_u32 board_num = (dev_id & 0x000F0000) >> 16;
    const a429_device_param_t *device_param = NULL;
    bht_L1_a429_chan_comm_param_t * comm_param = NULL;
    bht_L1_a429_tx_chan_inject_param_t *old_inject_param = NULL;

    if((chan_num > 16) | (chan_num < 1))
        return BHT_ERR_INVALID_CHANNEL_NUM;

//    if((board_num > 16) | (board_num < 1))
//        return BHT_ERR_INVALID_BOARD_NUM;
    
    //read
    device_param = &a429_device_param[board_num];
    comm_param = &device_param->tx_chan_param[chan_num - 1].comm_param;
    old_inject_param = &device_param->tx_chan_param[chan_num - 1].inject_param;

    if(BHT_L1_PARAM_OPT_GET == param_opt)
    {
        memcpy(inject_param, old_inject_param, sizeof(bht_L1_a429_tx_chan_inject_param_t));
        return result;
    }
    
    //compare
    if(0 != memcmp(old_inject_param, inject_param, sizeof(bht_L1_a429_tx_chan_inject_param_t)))
        is_changed = BHT_L0_TRUE;

    if(BHT_L0_FALSE == is_changed)
        return result;

    //param set
    value = 1;
    if(BHT_SUCCESS != (result = bht_L0_write_mem32(dev_id, BHT_A429_CFG_ENABLE, &value, 1)))
        return result;
    
    value = 0x01 << (chan_num - 1);
    if(BHT_SUCCESS != (result = bht_L0_write_mem32(dev_id, BHT_A429_CHOOSE_CHANNEL_NUM, &value, 1)))
        return result;

    value = a429_chan_cfg_reg_generate(BHT_L1_CHAN_TYPE_TX, comm_param, NULL, inject_param);
    if(BHT_SUCCESS != (result = bht_L0_write_mem32(dev_id, BHT_A429_CHANNEL_CFG, &value, 1)))
        return result;
    
    value = 0;
    if(BHT_SUCCESS != (result = bht_L0_write_mem32(dev_id, BHT_A429_CFG_ENABLE, &value, 1)))
        return result;
    
    //TODO generate chan common param change event

    memcpy(old_inject_param, inject_param, sizeof(bht_L1_a429_tx_chan_inject_param_t));
    
    return result;
}
bht_L0_u32 
bht_L1_a429_tx_chan_loop(bht_L0_u32 dev_id, 
        bht_L0_u32 chan_num, 
        bht_L0_u32 opt)
{
    bht_L0_u32 value = 0, idx;
    bht_L0_u32 result = BHT_SUCCESS;
    bht_L0_u32 board_num = (dev_id & 0x000F0000) >> 16;
    const a429_device_param_t *device_param = NULL;
	a429_tx_chan_param_t * a429_tx_chan_param = NULL;
    
    if((chan_num > 16) | (chan_num < 1))
        return BHT_ERR_INVALID_CHANNEL_NUM;

//    if(board_num >= 16)
//        return BHT_ERR_INVALID_BOARD_NUM;

    if((BHT_L1_OPT_DISABLE != opt) && (BHT_L1_OPT_ENABLE != opt))
        return BHT_ERR_BAD_INPUT;

    device_param = &a429_device_param[board_num];

    if(device_param->tx_chan_param[chan_num - 1].loop_enable == opt)
        return result;

    for(idx = 0; idx < BHT_A429_CHANNEL_MAX; idx++)
    {
        if(device_param->tx_chan_param[idx].loop_enable == BHT_L1_OPT_ENABLE)
            value |= (0x01 << idx);
    }
    
    if(BHT_L1_OPT_ENABLE == opt)
        value |= (0x01 << (chan_num - 1));
    else
        value &= (~(0x01 << (chan_num - 1)));

    printf("%s value = 0x%08x\n", __FUNCTION__, value);
    result = bht_L0_write_mem32(dev_id, BHT_A429_LOOP_EN, &value, 1);

    //TODO generate chan common param change event

    a429_tx_chan_param = &device_param->tx_chan_param[chan_num - 1];
    a429_tx_chan_param->loop_enable = opt;
    
    return result;
}

bht_L0_u32 
bht_L1_a429_tx_chan_slope_cfg(bht_L0_u32 dev_id, 
        bht_L0_u32 chan_num, 
        bht_L1_a429_slope_e slope)
{
    bht_L0_u32 value = 0, idx;
    bht_L0_u32 result = BHT_SUCCESS;
    bht_L0_u32 board_num = (dev_id & 0x000F0000) >> 16;
    const a429_device_param_t *device_param = NULL;
	a429_tx_chan_param_t * a429_tx_chan_param = NULL;

    
    if((chan_num > 16) | (chan_num < 1))
        return BHT_ERR_INVALID_CHANNEL_NUM;

//    if((board_num > 16) | (board_num < 1))
//        return BHT_ERR_INVALID_BOARD_NUM;

    device_param = &a429_device_param[board_num];

    if(device_param->tx_chan_param[chan_num].slope == slope)
        return result;

    for(idx = 0; idx < BHT_A429_CHANNEL_MAX; idx++)
    {
        if(device_param->tx_chan_param[idx].slope == BHT_L1_A429_SLOPE_1_5_US)
            value |= (0x01 << idx);
    }
    
    if(BHT_L1_A429_SLOPE_1_5_US == slope)
        value |= (0x01 << chan_num);
    else
        value &= (~(0x01 << chan_num));

    result = bht_L0_write_mem32(dev_id, BHT_A429_SLOPE_CTRL, &value, 1);

    //TODO generate chan common param change event

    a429_tx_chan_param = &device_param->tx_chan_param[chan_num];
    a429_tx_chan_param->slope = slope;
    
    return result;
}

bht_L0_u32 
bht_L1_a429_tx_chan_mib_clear(bht_L0_u32 dev_id, 
        bht_L0_u32 chan_num)
{
    return bht_L1_a429_mib_clear(dev_id, chan_num, BHT_L1_CHAN_TYPE_TX);
}

bht_L0_u32 
bht_L1_a429_tx_chan_mib_get(bht_L0_u32 dev_id, 
        bht_L0_u32 chan_num, 
        bht_L1_a429_mib_data_t *mib_data)
{
    return bht_L1_a429_mib_get(dev_id, chan_num, BHT_L1_CHAN_TYPE_TX, mib_data);
}

bht_L0_u32
bht_L1_a429_tx_chan_send(bht_L0_u32 dev_id, 
        bht_L0_u32 chan_num,  
        bht_L0_u32 data)
{
    bht_L0_u32 value, idx;
    bht_L0_u32 result = BHT_SUCCESS;
    bht_L0_u32 is_changed = BHT_L0_FALSE;

    if((chan_num > 16) | (chan_num < 1))
        return BHT_ERR_INVALID_CHANNEL_NUM;
    
    /* choose channel */
    value = 0x0F & (chan_num - 1);
    if(BHT_SUCCESS != (result = bht_L0_write_mem32(dev_id, BHT_A429_CHANNEL_NUM_SEND, &value, 1)))
        return result;

    /* check channel status stat: full or not full */
    if(BHT_SUCCESS != (result = bht_L0_read_mem32(dev_id, BHT_A429_STATUS_CHANNEL_SEND, &value, 1)))
        return result;
    if(value & BIT0)

	{
	    printf("channel status stat : 0x%08x\n", value);
        return BHT_ERR_BUFFER_FULL;
	}

    /* write send data */
    result = bht_L0_write_mem32(dev_id, BHT_A429_WORD_WR_CHANNEL, &data, 1);

    return result;
}

bht_L0_u32 bht_L1_a429_rx_chan_comm_param(bht_L0_u32 dev_id, 
        bht_L0_u32 chan_num,
        bht_L1_a429_chan_comm_param_t *comm_param, 
        bht_L1_param_opt_e param_opt)
{
    return bht_L1_a429_chan_comm_param(dev_id, chan_num, BHT_L1_CHAN_TYPE_RX, comm_param, param_opt);
}

/* a429 rx channel */
bht_L0_u32 
bht_L1_a429_rx_chan_gather_param(bht_L0_u32 dev_id,
        bht_L0_u32 chan_num,
        bht_L1_a429_rx_chan_gather_param_t *gather_param,
        bht_L1_param_opt_e param_opt)
{
    bht_L0_u32 value, idx;
    bht_L0_u32 result = BHT_SUCCESS;
    bht_L0_u32 is_changed = BHT_L0_FALSE;
    bht_L0_u32 board_num = (dev_id & 0x000F0000) >> 16;
    const a429_device_param_t *device_param = NULL;
    bht_L1_a429_chan_comm_param_t * comm_param = NULL;
    bht_L1_a429_rx_chan_gather_param_t *old_gather_param = NULL;

    /* input param check */
    if((chan_num > 16) | (chan_num < 1))
        return BHT_ERR_INVALID_CHANNEL_NUM;

//    if((board_num > 16) | (board_num < 1))
//        return BHT_ERR_INVALID_BOARD_NUM;

    if(gather_param->threshold_count > 1024)
        return BHT_ERR_BAD_INPUT;
    
    //read
    device_param = &a429_device_param[board_num];
    comm_param = &device_param->rx_chan_param[chan_num - 1].comm_param;
    old_gather_param = &device_param->rx_chan_param[chan_num - 1].gather_param;

    if(BHT_L1_PARAM_OPT_GET == param_opt)
    {
        memcpy(gather_param, old_gather_param, sizeof(bht_L1_a429_rx_chan_gather_param_t));
        return result;
    }
    
    //compare
    if(0 != memcmp(old_gather_param, gather_param, sizeof(bht_L1_a429_rx_chan_gather_param_t)))
        is_changed = BHT_L0_TRUE;

    if(BHT_L0_FALSE == is_changed)
        return result;

    //param set
    value = 1;
    if(BHT_SUCCESS != (result = bht_L0_write_mem32(dev_id, BHT_A429_CFG_ENABLE, &value, 1)))
        return result;
    
    value = BHT_A429_CHANNEL_MAX + chan_num - 1;
	printf("%s ,channel choose value = 0x%08x\n", __FUNCTION__, value);
    if(BHT_SUCCESS != (result = bht_L0_write_mem32(dev_id, BHT_A429_CHOOSE_CHANNEL_NUM, &value, 1)))
        return result;

    value = a429_chan_cfg_reg_generate(BHT_L1_CHAN_TYPE_RX, comm_param, gather_param, NULL);
	printf("%s ,channel cfg value = 0x%08x\n", __FUNCTION__, value);
    if(BHT_SUCCESS != (result = bht_L0_write_mem32(dev_id, BHT_A429_CHANNEL_CFG, &value, 1)))
        return result;
    
    value = gather_param->threshold_count << 16;
	
    value |= gather_param->threshold_time;
	printf("%s ,threshold value = 0x%08x\n", __FUNCTION__, value);
    if(BHT_SUCCESS != (result = bht_L0_write_mem32(dev_id, BHT_A429_INT_THRESHOLD, &value, 1)))
        return result;
    
    value = 0;
    if(BHT_SUCCESS != (result = bht_L0_write_mem32(dev_id, BHT_A429_CFG_ENABLE, &value, 1)))
        return result;
    
    //TODO generate chan common param change event

    memcpy(old_gather_param, gather_param, sizeof(bht_L1_a429_rx_chan_gather_param_t));
    
    return result;
}

bht_L0_u32 
bht_L1_a429_rx_chan_filter_cfg(bht_L0_u32 dev_id, 
        bht_L0_u32 chan_num, 
        bht_L1_a429_rx_chan_filter_t *filter)
{
    bht_L0_u32 value = 0, idx;
    bht_L0_u32 result = BHT_SUCCESS;
    bht_L0_u32 board_num = (dev_id & 0x000F0000) >> 16;
    
    if((chan_num > 16) | (chan_num < 1))
        return BHT_ERR_INVALID_CHANNEL_NUM;

//    if((board_num > 16) | (board_num < 1))
//        return BHT_ERR_INVALID_BOARD_NUM;

    value = 1;
    if(BHT_SUCCESS != (result = bht_L0_write_mem32(dev_id, BHT_A429_CFG_ENABLE, &value, 1)))
        return result;

    value = 16 + chan_num - 1;
    if(BHT_SUCCESS != (result = bht_L0_write_mem32(dev_id, BHT_A429_CHOOSE_CHANNEL_NUM, &value, 1)))
        return result;

    value = (filter->filter_mode << 12) | (filter->flt.reg16 & 0x0fff);
    if(BHT_SUCCESS != (result = bht_L0_write_mem32(dev_id, BHT_A429_LABLE_FILTER, &value, 1)))
        return result;

    value = 0;
    if(BHT_SUCCESS != (result = bht_L0_write_mem32(dev_id, BHT_A429_CFG_ENABLE, &value, 1)))
        return result;

    //TODO generate filter cfg change event
    
    return result;
}

bht_L0_u32 
bht_L1_a429_rx_chan_mib_clear(bht_L0_u32 dev_id, 
        bht_L0_u32 chan_num)
{
    return bht_L1_a429_mib_clear(dev_id, chan_num, BHT_L1_CHAN_TYPE_RX);
}

bht_L0_u32 
bht_L1_a429_rx_chan_mib_get(bht_L0_u32 dev_id, 
        bht_L0_u32 chan_num, 
        bht_L1_a429_mib_data_t *mib_data)
{
    return bht_L1_a429_mib_get(dev_id, chan_num, BHT_L1_CHAN_TYPE_RX, mib_data);
}

bht_L0_u32 
bht_L1_a429_rx_chan_recv(bht_L0_u32 dev_id, 
        bht_L0_u32 chan_num, 
        bht_L1_a429_rxp_t *rxp_buf, 
        bht_L0_u32 max_rxp, 
        bht_L0_u32 *rxp_num, 
        bht_L0_u32 opt)
{
    return BHT_FAILURE;
}


//#ifdef __cplusplus
//}
//#endif

