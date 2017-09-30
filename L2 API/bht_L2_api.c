/* bht_L2_api.c - Layer 2 API*/

/*
 * Copyright (c) 2017-2023 Bin Hong Tech, Inc.
 *
 * The right to copy, distribute, modify or otherwise make use
 * of this software may be licensed only pursuant to the terms
 * of an applicable BinHong-tech license agreement.
 */
#include <bht_L2.h>

#define BHT_L2_DEVICE_MAX       16
#define PCILOAD_FLIENAME_A429    "C://Windows//System32//a429_fpga_top.bin"
#define BHT_L2_A429_DEVID_CHK_RTN(dev_id)\
    do\
    {\
        bht_L0_dtype_e dtype;\
        bht_L0_u32 _backplane_type = dev_id & 0xF0000000;\
    	bht_L0_u32 _board_type     = dev_id & 0x0FF00000;\
    	bht_L0_u32 _board_num      = dev_id & 0x000F0000;\
        \
        if((_backplane_type == BHT_DEVID_BACKPLANETYPE_PCI) || \
            (_board_type == BHT_DEVID_BOARDTYPE_PMCA429) || \
            (_board_num == BHT_L2_DEVICE_MAX))\
            return BHT_ERR_NO_DEVICE;\
    }while(0)


static bht_L1_device_handle_t device_list_a429[BHT_L2_DEVICE_MAX] = {NULL};
static bht_L1_device_handle_t device_list_1553[BHT_L2_DEVICE_MAX] = {NULL};

static bht_L1_device_handle_t
bht_L2_devid2handle(bht_L0_u32 dev_id)
{
    bht_L0_dtype_e dtype;
    bht_L0_u32 backplane_type = dev_id & 0xF0000000;
	bht_L0_u32 board_type     = dev_id & 0x0FF00000;
	bht_L0_u32 board_num      = dev_id & 0x000F0000;
        
    if((backplane_type == BHT_DEVID_BACKPLANETYPE_PCI) || 
        (board_type == BHT_DEVID_BOARDTYPE_PMCA429) || 
        (board_num == BHT_L2_DEVICE_MAX))
    {
        return NULL;
    }

    return device_list_a429[board_num];
}

bht_L0_u32 
bht_L2_device_probe(bht_L0_u32 dev_id)
{
    bht_L0_u32 board_type;
    bht_L0_u32 board_no;
    bht_L0_u32 backplane;
    bht_L0_dtype_e dtype;
    bht_L0_u32 device_no;

    BHT_L2_A429_DEVID_CHK_RTN(dev_id);
    
    if(BHT_SUCCESS != bht_L1_init())
        return BHT_ERR_LOW_LEVEL_DRIVER_ERR;
        
    dtype = BHT_L0_DEVICE_TYPE_ARINC429;
    if(!(device_no = bht_L1_device_scan(dtype)))
        return BHT_ERR_NO_DEVICE;

    if(BHT_SUCCESS != bht_L1_device_open(dtype, board_no, 
        &device_list_a429[board_no], PCILOAD_FLIENAME_A429))
        return BHT_ERR_NO_DEVICE;

    return BHT_SUCCESS;

}

bht_L0_u32 
bht_L2_device_remove(bht_L0_u32 dev_id)
{
    bht_L0_u32 result = BHT_SUCCESS;
    bht_L1_device_handle_t device;
    bht_L0_u32 board_num = dev_id & 0x000F0000;

    device = bht_L2_devid2handle(dev_id);
    if(NULL == device)
        return BHT_ERR_DEVICE_NOT_OPEN;
    
    if(BHT_SUCCESS == (result = bht_L1_device_close(device)))
        device_list_a429[board_num] = NULL;
    
    return result;    
}

bht_L0_u32 
bht_L2_device_softreset(bht_L0_u32 dev_id)
{
    bht_L1_device_handle_t device;

    device = bht_L2_devid2handle(dev_id);
    if(NULL == device)
        return BHT_ERR_DEVICE_NOT_OPEN;

    return bht_L1_device_reset(device);
}

bht_L0_u32 
bht_L2_device_version(bht_L0_u32 dev_id, 
        bht_L0_u32 *version)
{
    bht_L1_device_handle_t device;

    device = bht_L2_devid2handle(dev_id);
    if(NULL == device)
        return BHT_ERR_DEVICE_NOT_OPEN;

    return bht_L1_device_hw_version(device, version);
}

bht_L0_u32 
bht_L2_a429_default_init(bht_L0_u32 dev_id)
{
    bht_L1_device_handle_t device;

    device = bht_L2_devid2handle(dev_id);
    if(NULL == device)
        return BHT_ERR_DEVICE_NOT_OPEN;

    return BHT_SUCCESS;
}

bht_L0_u32 
bht_L2_a429_irigb_mode_cfg(bht_L0_u32 dev_id, 
        bht_L1_a429_irigb_mode_e mode)
{
    bht_L1_device_handle_t device;

    device = bht_L2_devid2handle(dev_id);
    if(NULL == device)
        return BHT_ERR_DEVICE_NOT_OPEN;

    return bht_L1_a429_irigb_mode(device, &mode, BHT_L1_PARAM_OPT_SET);
}

bht_L0_u32 
bht_L2_a429_irigb_time(bht_L0_u32 dev_id, 
        bht_L1_a429_irigb_time_t *ti, 
        bht_L1_param_opt_e param_opt)
{
    bht_L1_device_handle_t device;

    device = bht_L2_devid2handle(dev_id);
    if(NULL == device)
        return BHT_ERR_DEVICE_NOT_OPEN;

    return bht_L1_a429_irigb_time(device, ti, param_opt);
}

bht_L0_u32 
bht_L2_a429_tx_chan_comm_param(bht_L0_u32 dev_id, 
        bht_L0_u32 chan_num,
        bht_L1_a429_chan_comm_param_t *comm_param, 
        bht_L1_param_opt_e param_opt)
{
    bht_L0_u32 result = BHT_SUCCESS;
    bht_L1_device_handle_t device;
    bht_L1_a429_baud_rate_e baud;
    bht_L1_a429_parity_e parity;
    bht_L1_a429_chan_stat_e chan_stat;

    device = bht_L2_devid2handle(dev_id);
    if(NULL == device)
        return BHT_ERR_DEVICE_NOT_OPEN;

    if(BHT_L1_PARAM_OPT_GET == param_opt)
    {
        if(BHT_SUCCESS != (result = bht_L1_a429_chan_baud(device, chan_num, \
            BHT_L1_CHAN_TYPE_TX, &baud, param_opt)))
            return result;
        if(BHT_SUCCESS != (result = bht_L1_a429_chan_parity(device, chan_num, \
            BHT_L1_CHAN_TYPE_TX, &parity, param_opt)))
            return result;
        if(BHT_SUCCESS != (result = bht_L1_a429_chan_get_stat(device, chan_num, \
            BHT_L1_CHAN_TYPE_TX, &chan_stat)))
            return result;
        
        comm_param->work_mode = chan_stat;
        comm_param->baud = baud;
        comm_param->par = parity;
    }
    else
    {
        baud = comm_param->baud;
        parity = comm_param->par;
        
        if(BHT_SUCCESS != (result = bht_L1_a429_chan_baud(device, chan_num, \
            BHT_L1_CHAN_TYPE_TX, &baud, param_opt)))
            return result;
        if(BHT_SUCCESS != (result = bht_L1_a429_chan_parity(device, chan_num, \
            BHT_L1_CHAN_TYPE_TX, &parity, param_opt)))
            return result;

        if(comm_param->work_mode == BHT_L1_A429_CHAN_WORK_MODE_OPEN)
            return bht_L1_a429_chan_open(device, chan_num, BHT_L1_CHAN_TYPE_TX);
        else if(comm_param->work_mode == BHT_L1_A429_CHAN_WORK_MODE_CLOSE)
            return bht_L1_a429_chan_close(device, chan_num, BHT_L1_CHAN_TYPE_TX);
        else if(comm_param->work_mode == BHT_L1_A429_CHAN_WORK_MODE_CLOSE_AND_CLEAR)
            return bht_L1_a429_chan_close_and_clear_fifo(device, chan_num, BHT_L1_CHAN_TYPE_TX);
        else
            return BHT_FAILURE;
    }

    return result;
}

bht_L0_u32 
bht_L2_a429_tx_chan_inject_param(bht_L0_u32 dev_id, 
        bht_L0_u32 chan_num, 
        bht_L1_a429_tx_chan_inject_param_t *inject_param, 
        bht_L1_param_opt_e param_opt)
{
    bht_L0_u32 result = BHT_SUCCESS;
    bht_L1_device_handle_t device;
    bht_L1_a429_err_type_e err_type;

    device = bht_L2_devid2handle(dev_id);
    if(NULL == device)
        return BHT_ERR_DEVICE_NOT_OPEN;

    if(BHT_L1_PARAM_OPT_SET == param_opt)
    {
        if((inject_param->tb_bits == BHT_L1_A429_WORD_BIT31) &&
            (inject_param->tb_gap == BHT_L1_A429_GAP_4BIT) &&
            (inject_param->tb_par_en == BHT_L1_DISABLE))
            err_type = BHT_L1_A429_ERR_TYPE_31BIT;
        else if((inject_param->tb_bits == BHT_L1_A429_WORD_BIT33) &&
            (inject_param->tb_gap == BHT_L1_A429_GAP_4BIT) &&
            (inject_param->tb_par_en == BHT_L1_DISABLE))
            err_type = BHT_L1_A429_ERR_TYPE_33BIT;
        else if((inject_param->tb_bits == BHT_L1_A429_WORD_BIT32) &&
            (inject_param->tb_gap == BHT_L1_A429_GAP_2BIT) &&
            (inject_param->tb_par_en == BHT_L1_DISABLE))
            err_type = BHT_L1_A429_ERR_TYPE_2GAP;
        else if((inject_param->tb_bits == BHT_L1_A429_WORD_BIT32) &&
            (inject_param->tb_gap == BHT_L1_A429_GAP_4BIT) &&
            (inject_param->tb_par_en == BHT_L1_ENABLE))
            err_type = BHT_L1_A429_ERR_TYPE_PARITY;
        else
            return BHT_ERR_BAD_INPUT;
    }

    if(BHT_SUCCESS != (result = bht_L1_a429_tx_chan_err_inject(
        device, chan_num, &err_type, param_opt)))
        goto end;

    if(BHT_L1_PARAM_OPT_GET == param_opt)
    {
        switch(err_type)
        {
            case BHT_L1_A429_ERR_TYPE_NONE:
                inject_param->tb_bits = BHT_L1_A429_WORD_BIT32;
                inject_param->tb_gap = BHT_L1_A429_GAP_4BIT;
                inject_param->tb_par_en = BHT_L1_DISABLE;
                break;
            case BHT_L1_A429_ERR_TYPE_31BIT:
                inject_param->tb_bits = BHT_L1_A429_WORD_BIT31;
                inject_param->tb_gap = BHT_L1_A429_GAP_4BIT;
                inject_param->tb_par_en = BHT_L1_DISABLE;
                break;
            case BHT_L1_A429_ERR_TYPE_33BIT:
                inject_param->tb_bits = BHT_L1_A429_WORD_BIT33;
                inject_param->tb_gap = BHT_L1_A429_GAP_4BIT;
                inject_param->tb_par_en = BHT_L1_DISABLE;
                break;
            case BHT_L1_A429_ERR_TYPE_2GAP:
                inject_param->tb_bits = BHT_L1_A429_WORD_BIT32;
                inject_param->tb_gap = BHT_L1_A429_GAP_2BIT;
                inject_param->tb_par_en = BHT_L1_DISABLE;
                break;
            case BHT_L1_A429_ERR_TYPE_PARITY:
                inject_param->tb_bits = BHT_L1_A429_WORD_BIT32;
                inject_param->tb_gap = BHT_L1_A429_GAP_4BIT;
                inject_param->tb_par_en = BHT_L1_ENABLE;
                break;
            default:
                result = BHT_FAILURE;
                goto end;
        }
    }
    
end:
    return result;
}

bht_L0_u32
bht_L2_a429_tx_chan_period_param(bht_L0_u32 dev_id,
        bht_L0_u32 chan_num,
        bht_L0_u32 * period,
        bht_L1_param_opt_e param_opt)
{
    bht_L0_u32 result = BHT_SUCCESS;
    bht_L1_device_handle_t device;
    bht_L1_a429_send_mode_e send_mode;

    device = bht_L2_devid2handle(dev_id);
    if(NULL == device)
        return BHT_ERR_DEVICE_NOT_OPEN;

    if(BHT_L1_PARAM_OPT_SET == param_opt)
    {
        if(*period == 0)
        {
            send_mode = BHT_L1_A429_SEND_MODE_NONPERIOD;
            return bht_L1_a429_tx_chan_send_mode(device, chan_num, 
                &send_mode, param_opt);
        }
        else
        {
            send_mode = BHT_L1_A429_SEND_MODE_PERIOD;
            if(BHT_SUCCESS != (result = bht_L1_a429_tx_chan_send_mode(device, 
                chan_num, &send_mode, param_opt)))
                goto end;

            return bht_L1_a429_tx_chan_period(device, chan_num, period, param_opt);
        }
    }
    else
    {
        if(BHT_SUCCESS != (result = bht_L1_a429_tx_chan_send_mode(device, 
                chan_num, &send_mode, param_opt)))
                goto end;
        if(BHT_L1_A429_SEND_MODE_NONPERIOD == send_mode)
            *period = 0;
        else
        {
            return bht_L1_a429_tx_chan_period(device, chan_num, period, param_opt);
        }
    }
    
end:
    return result;
}

bht_L0_u32 
bht_L2_a429_tx_chan_loop(bht_L0_u32 dev_id, 
        bht_L0_u32 chan_num, 
        bht_L0_u32 opt)
{
    bht_L0_u32 result = BHT_SUCCESS;
    bht_L1_device_handle_t device;
    bht_L1_able_e able = BHT_L1_ENABLE;

    device = bht_L2_devid2handle(dev_id);
    if(NULL == device)
        return BHT_ERR_DEVICE_NOT_OPEN;

    return bht_L1_a429_chan_loop(device, chan_num, &able, BHT_L1_PARAM_OPT_SET);
}

bht_L0_u32 
bht_L2_a429_tx_chan_mib_clear(bht_L0_u32 dev_id, 
        bht_L0_u32 chan_num)
{
    bht_L0_u32 result = BHT_SUCCESS;
    bht_L1_device_handle_t device;
    bht_L1_able_e able = BHT_L1_ENABLE;

    device = bht_L2_devid2handle(dev_id);
    if(NULL == device)
        return BHT_ERR_DEVICE_NOT_OPEN;

    return bht_L1_a429_chan_clear_mib(device, chan_num, BHT_L1_CHAN_TYPE_TX);
}

bht_L0_u32 
bht_L2_a429_tx_chan_mib_get(bht_L0_u32 dev_id, 
        bht_L0_u32 chan_num, 
        bht_L1_a429_mib_data_t *mib_data)
{
    bht_L0_u32 result = BHT_SUCCESS;
    bht_L1_device_handle_t device;
    bht_L1_able_e able = BHT_L1_ENABLE;

    device = bht_L2_devid2handle(dev_id);
    if(NULL == device)
        return BHT_ERR_DEVICE_NOT_OPEN;

    return bht_L1_a429_chan_get_mib(device, chan_num, BHT_L1_CHAN_TYPE_TX, mib_data);
}

bht_L0_u32
bht_L2_a429_tx_chan_send(bht_L0_u32 dev_id, 
        bht_L0_u32 chan_num,
        bht_L1_a429_send_opt_e opt,
        bht_L0_u32 data)
{
    bht_L0_u32 result = BHT_SUCCESS;
    bht_L1_device_handle_t device;
    bht_L1_able_e able = BHT_L1_ENABLE;

    device = bht_L2_devid2handle(dev_id);
    if(NULL == device)
        return BHT_ERR_DEVICE_NOT_OPEN;

    switch(opt)
    {
        case BHT_L1_A429_OPT_RANDOM_SEND:
            return bht_L1_a429_tx_chan_send(device, chan_num, data);
        case BHT_L1_A429_OPT_PERIOD_SEND_UPDATE:
            return bht_L1_a429_tx_chan_update_data(device, chan_num, data);
        case BHT_L1_A429_OPT_PERIOD_SEND_START:
            return bht_L1_a429_tx_chan_start(device, chan_num);
        case BHT_L1_A429_OPT_PERIOD_SEND_STOP:
            return bht_L1_a429_tx_chan_stop(device, chan_num);
        default:
            result = BHT_FAILURE;
            break; 
    }

    return result;
}

bht_L0_u32 
bht_L2_a429_rx_chan_comm_param(bht_L0_u32 dev_id, 
        bht_L0_u32 chan_num,
        bht_L1_a429_chan_comm_param_t *comm_param, 
        bht_L1_param_opt_e param_opt)
{
    bht_L0_u32 result = BHT_SUCCESS;
    bht_L1_device_handle_t device;
    bht_L1_a429_baud_rate_e baud;
    bht_L1_a429_parity_e parity;
    bht_L1_a429_chan_stat_e chan_stat;

    device = bht_L2_devid2handle(dev_id);
    if(NULL == device)
        return BHT_ERR_DEVICE_NOT_OPEN;

    if(BHT_L1_PARAM_OPT_GET == param_opt)
    {
        if(BHT_SUCCESS != (result = bht_L1_a429_chan_baud(device, chan_num, \
            BHT_L1_CHAN_TYPE_RX, &baud, param_opt)))
            return result;
        if(BHT_SUCCESS != (result = bht_L1_a429_chan_parity(device, chan_num, \
            BHT_L1_CHAN_TYPE_RX, &parity, param_opt)))
            return result;
        if(BHT_SUCCESS != (result = bht_L1_a429_chan_get_stat(device, chan_num, \
            BHT_L1_CHAN_TYPE_RX, &chan_stat)))
            return result;
        
        comm_param->work_mode = chan_stat;
        comm_param->baud = baud;
        comm_param->par = parity;
    }
    else
    {
        baud = comm_param->baud;
        parity = comm_param->par;
        
        if(BHT_SUCCESS != (result = bht_L1_a429_chan_baud(device, chan_num, \
            BHT_L1_CHAN_TYPE_RX, &baud, param_opt)))
            return result;
        if(BHT_SUCCESS != (result = bht_L1_a429_chan_parity(device, chan_num, \
            BHT_L1_CHAN_TYPE_RX, &parity, param_opt)))
            return result;

        if(comm_param->work_mode == BHT_L1_A429_CHAN_WORK_MODE_OPEN)
            return bht_L1_a429_chan_open(device, chan_num, BHT_L1_CHAN_TYPE_RX);
        else if(comm_param->work_mode == BHT_L1_A429_CHAN_WORK_MODE_CLOSE)
            return bht_L1_a429_chan_close(device, chan_num, BHT_L1_CHAN_TYPE_RX);
        else if(comm_param->work_mode == BHT_L1_A429_CHAN_WORK_MODE_CLOSE_AND_CLEAR)
            return bht_L1_a429_chan_close_and_clear_fifo(device, chan_num, BHT_L1_CHAN_TYPE_RX);
        else
            return BHT_FAILURE;
    }

    return result;
}

bht_L0_u32 
bht_L2_a429_rx_chan_gather_param(bht_L0_u32 dev_id,
        bht_L0_u32 chan_num,
        bht_L1_a429_rx_chan_gather_param_t *gather_param,
        bht_L1_param_opt_e param_opt)
{
    bht_L0_u32 result = BHT_SUCCESS;
    bht_L1_device_handle_t device;
    bht_L1_able_e able;

    device = bht_L2_devid2handle(dev_id);
    if(NULL == device)
        return BHT_ERR_DEVICE_NOT_OPEN;

    if(BHT_L1_PARAM_OPT_GET == param_opt)
    {
        if(BHT_SUCCESS != (result = bht_L1_a429_rx_chan_filter(device, chan_num, 
            &able, param_opt)))
            return result;
        if(BHT_SUCCESS != (result = bht_L1_a429_rx_chan_recv_mode(device, chan_num, \
            &gather_param->recv_mode, param_opt)))
            return result;
        if(BHT_SUCCESS != (result = bht_L1_a429_rx_chan_int_threshold(device, chan_num, \
            &gather_param->threshold_count, &gather_param->threshold_time, param_opt)))
            return result;
        
        gather_param->gather_enable = !able;
    }
    else
    {
        able = !gather_param->gather_enable;
        
        if(BHT_SUCCESS != (result = bht_L1_a429_rx_chan_filter(device, chan_num, 
            &able, param_opt)))
            return result;
        if(BHT_SUCCESS != (result = bht_L1_a429_rx_chan_recv_mode(device, chan_num, \
            &gather_param->recv_mode, param_opt)))
            return result;
        if(BHT_SUCCESS != (result = bht_L1_a429_rx_chan_int_threshold(device, chan_num, \
            &gather_param->threshold_count, &gather_param->threshold_time, param_opt)))
            return result;
    }

    return result;
}

bht_L0_u32 
bht_L2_a429_rx_chan_filter_cfg(bht_L0_u32 dev_id, 
        bht_L0_u32 chan_num, 
        bht_L1_a429_rx_chan_filter_t *filter,
        bht_L1_param_opt_e param_opt)
{
    bht_L0_u32 result = BHT_SUCCESS;
    bht_L1_device_handle_t device;
    bht_L1_able_e able;

    device = bht_L2_devid2handle(dev_id);
    if(NULL == device)
        return BHT_ERR_DEVICE_NOT_OPEN;

    return bht_L1_a429_rx_chan_filter_label(device, chan_num, filter->label, \
        filter->sdi, filter->ssm, &filter->filter_mode, param_opt);
}

bht_L0_u32 
bht_L2_a429_rx_chan_mib_clear(bht_L0_u32 dev_id, 
        bht_L0_u32 chan_num)
{
    bht_L0_u32 result = BHT_SUCCESS;
    bht_L1_device_handle_t device;
    bht_L1_able_e able = BHT_L1_ENABLE;

    device = bht_L2_devid2handle(dev_id);
    if(NULL == device)
        return BHT_ERR_DEVICE_NOT_OPEN;

    return bht_L1_a429_chan_clear_mib(device, chan_num, BHT_L1_CHAN_TYPE_RX);
}

bht_L0_u32 
bht_L2_a429_rx_chan_mib_get(bht_L0_u32 dev_id, 
        bht_L0_u32 chan_num, 
        bht_L1_a429_mib_data_t *mib_data)
{
    bht_L0_u32 result = BHT_SUCCESS;
    bht_L1_device_handle_t device;
    bht_L1_able_e able = BHT_L1_ENABLE;

    device = bht_L2_devid2handle(dev_id);
    if(NULL == device)
        return BHT_ERR_DEVICE_NOT_OPEN;

    return bht_L1_a429_chan_get_mib(device, chan_num, BHT_L1_CHAN_TYPE_RX, mib_data);
}

bht_L0_u32 
bht_L2_a429_rx_chan_recv(bht_L0_u32 dev_id, 
        bht_L0_u32 chan_num, 
        bht_L1_a429_rxp_t *rxp_buf, 
        bht_L0_u32 max_rxp, 
        bht_L0_u32 *rxp_num, 
        bht_L0_s32 timeout_ms)
{
    bht_L0_u32 result = BHT_SUCCESS;
    bht_L1_device_handle_t device;
    bht_L1_able_e able = BHT_L1_ENABLE;

    device = bht_L2_devid2handle(dev_id);
    if(NULL == device)
        return BHT_ERR_DEVICE_NOT_OPEN;
    
    return bht_L1_a429_rx_chan_recv(device, chan_num, rxp_buf, max_rxp, rxp_num, timeout_ms);
}

bht_L0_u32
bht_L2_a429_rx_chan_stat(bht_L0_u32 dev_id, 
        bht_L0_u32 chan_num,
        bht_L0_u32 *recv_num)
{
    bht_L0_u32 result = BHT_SUCCESS;
    bht_L1_device_handle_t device;    

    device = bht_L2_devid2handle(dev_id);
    if(NULL == device)
        return BHT_ERR_DEVICE_NOT_OPEN;
    
    return bht_L1_a429_rx_chan_stat(device, chan_num, recv_num);
}


bht_L0_u32
bht_L2_a429_config_from_xml(bht_L0_u32 dev_id, 
        const char *filename)
{
    bht_L0_u32 result = BHT_SUCCESS;
    bht_L1_device_handle_t device;    

    device = bht_L2_devid2handle(dev_id);
    if(NULL == device)
        return BHT_ERR_DEVICE_NOT_OPEN;
    
    return bht_L1_device_config_from_xml(device, filename);
}

bht_L0_u32
bht_L2_a429_default_param_save(bht_L0_u32 dev_id)
{
    bht_L0_u32 result = BHT_SUCCESS;
    bht_L1_device_handle_t device;    

    device = bht_L2_devid2handle(dev_id);
    if(NULL == device)
        return BHT_ERR_DEVICE_NOT_OPEN;
    
    return bht_L1_device_default_param_save(device);
}

bht_L0_u32
bht_L2_bd_fpga_eeprom_read(bht_L0_u32 dev_id,
        bht_L0_u16 addr,
        bht_L0_u8 *data)
{
    bht_L0_u32 result = BHT_SUCCESS;
    bht_L1_device_handle_t device;    

    device = bht_L2_devid2handle(dev_id);
    if(NULL == device)
        return BHT_ERR_DEVICE_NOT_OPEN;
    
    return bht_L1_bd_fpga_eeprom_read(device, addr, data);
}

bht_L0_u32
bht_L2_bd_fpga_eeprom_write(bht_L0_u32 dev_id,
        bht_L0_u16 addr,
        bht_L0_u8 data)
{
    bht_L0_u32 result = BHT_SUCCESS;
    bht_L1_device_handle_t device;    

    device = bht_L2_devid2handle(dev_id);
    if(NULL == device)
        return BHT_ERR_DEVICE_NOT_OPEN;
    
    return bht_L1_bd_fpga_eeprom_write(device, addr, data);
}

bht_L0_u32 
bht_L2_bd_plx_eeprom_read(bht_L0_u32 dev_id,
        bht_L0_u32 addr,
        bht_L0_u16 *buf, 
        bht_L0_u32 count)
{
    bht_L0_u32 result = BHT_SUCCESS;
    bht_L1_device_handle_t device;    

    device = bht_L2_devid2handle(dev_id);
    if(NULL == device)
        return BHT_ERR_DEVICE_NOT_OPEN;
    
    return bht_L1_bd_plx_eeprom_read(device, addr, buf, count);
}

bht_L0_u32 
bht_L2_bd_plx_eeprom_write(bht_L0_u32 dev_id,
        bht_L0_u32 addr,
        bht_L0_u16 *buf, 
        bht_L0_u32 count)
{
    bht_L0_u32 result = BHT_SUCCESS;
    bht_L1_device_handle_t device;    

    device = bht_L2_devid2handle(dev_id);
    if(NULL == device)
        return BHT_ERR_DEVICE_NOT_OPEN;
    
    return bht_L1_bd_plx_eeprom_write(device, addr, buf, count);
}

