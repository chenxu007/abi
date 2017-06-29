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

#include <bht_L0.h>
#include <bht_L1.h>
#include <bht_L1_a429.h>
#include <bht_L1_plx9056.h>
#include <bht_L1_ring.h>
#include <assert.h>
#include <stdarg.h>

#define BHT_A429_DEVICE_MAX      16
#define BHT_A429_CHANNEL_MAX     16
#define A429_RXP_BUF_MAX         1024

#define A429_DEBUG 
#ifdef A429_DEBUG
#define DEBUG(x, ...)\
do\
{\
    va_list ap;\
    va_start(ap, x);\
    (void)printf(x, ap);\
    va_end(ap);\
}while(0);
#else
#define DEBUG(x, ...)
#endif

typedef struct
{
    bht_L0_u32 chan_num;
    bht_L1_a429_chan_comm_param_t comm_param;
    bht_L1_a429_rx_chan_gather_param_t gather_param;
//    bht_L0_u32 filter_mode;     /* 0 - Black list, 1 - White list*/
}a429_rx_chan_param_t;

typedef enum
{
    A429_TX_CHAN_TRANS_MODE_RANDOM = 0,
    A429_TX_CHAN_TRANS_MODE_PERIOD = 1,
}a429_tx_chan_trans_mode_e;

typedef struct
{
    bht_L0_u32 chan_num;
    bht_L0_u32 loop_enable;         /* 0 - disable, 1 - enable */
    bht_L0_u32 period;              /* send period, unit:ms,
                                       period = 0, random send; period > 0, period send */
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

typedef struct
{
    bht_L0_sem semc;
    bht_L0_u32 recv_data_count;
    struct ring_buf rxp_ring_buf;
    bht_L1_a429_rxp_t rxp_buf[A429_RXP_BUF_MAX];
}a429_chan_data_t;

static a429_device_param_t a429_device_param[BHT_A429_DEVICE_MAX] = {0};
/* store rx channel receive data */
static a429_chan_data_t a429_chan_data[BHT_A429_DEVICE_MAX][BHT_A429_CHANNEL_MAX] = {0};

static bht_L0_u32 isr_count = 0;
static bht_L0_u32 isr_vecter_idle_err_num = 0;
static bht_L0_u32 isr_rdwr_err_num = 0;
static bht_L0_u32 isr_ring_err_num = 0;
static bht_L0_u32 isr_sem_err_num = 0;
static bht_L0_u32 isr_list_err_num = 0;

static void * 
a429_isr(void *arg)
{
    bht_L0_u32 idx;
    bht_L0_u32 result;
    bht_L0_u32 value;
    bht_L1_a429_rxp_t rxp;
    const bht_L0_u32 dev_id = (bht_L0_u32) arg;
    const bht_L0_u32 board_num = (dev_id & 0x000F0000) >> 16;    
	bht_L0_u32 int_clear_flag = 0;
    struct ring_buf * r = NULL;
    bht_L0_sem semc;
    bht_L1_a429_recv_mod_e recv_mode;

    isr_count++;

    /* check 429 channel interrupt vector */
    if(BHT_SUCCESS != (result = bht_L0_read_mem32(dev_id, BHT_A429_INTR_CHANNEL_VECTOR, &value, 1)))
	{
		isr_rdwr_err_num++;
		goto isr_end;
	}
    if(0 == value)
	{
		isr_vecter_idle_err_num++;
        goto isr_end;
	}

	for(idx = 16; idx < 32; idx++)
	{
	    if(0 == (value & (0x01 << idx)))
            continue;
        
        r = &a429_chan_data[board_num][idx - 16].rxp_ring_buf;
        semc = a429_chan_data[board_num][idx - 16].semc;
        recv_mode = a429_device_param[board_num].rx_chan_param[idx - 16].gather_param.recv_mode;
        
		//choose channel
		if(BHT_SUCCESS != (result = bht_L0_write_mem32(dev_id, BHT_A429_CHANNEL_NUM_RECV, &idx, 1)))
		{
			isr_rdwr_err_num++;
			goto isr_end;
		}
        
        if(BHT_L1_A429_RECV_MODE_SAMPLE == recv_mode)
        {
            if(BHT_SUCCESS != (result = bht_L0_read_mem32(dev_id, BHT_A429_WORD_RD_CHANNEL_TIMESTAMP, &rxp.timestamp, 1)))
			{
				isr_rdwr_err_num++;
                goto isr_end;
			}
            if(BHT_SUCCESS != (result = bht_L0_read_mem32(dev_id, BHT_A429_WORD_RD_CHANNEL_FREASH, &rxp.data, 1)))
			{
				isr_rdwr_err_num++;
                goto isr_end;
			}
            if(NULL == ring_buf_put(r, (unsigned char*)&rxp))
			{
				isr_ring_err_num++;
                goto isr_end;
			}

            if(BHT_SUCCESS != bht_L0_sem_give(semc))
			{
				isr_sem_err_num++;
				goto isr_end;
			}
            a429_chan_data[board_num][idx - 16].recv_data_count++;
        }
        else
		{
            if(BHT_SUCCESS != (result = bht_L0_read_mem32(dev_id, BHT_A429_STATUS_CHANNEL_RECV, &value, 1)))
            {
				isr_rdwr_err_num++;
                goto isr_end;
			}
            //check the list empty
            if((value & 0x01) == 0)
            {
                bht_L0_u32 count = value >> 16;

                while(count--)
                {
                    if(BHT_SUCCESS != (result = bht_L0_read_mem32(dev_id, BHT_A429_WORD_RD_CHANNEL_TIMESTAMP, &rxp.timestamp, 1)))
                    {
        				isr_rdwr_err_num++;
                        goto isr_end;
        			}
                    if(BHT_SUCCESS != (result = bht_L0_read_mem32(dev_id, BHT_A429_WORD_RD_CHANNEL_FREASH, &rxp.data, 1)))
                    {
        				isr_rdwr_err_num++;
                        goto isr_end;
        			}
                    if(NULL == ring_buf_put(r, (unsigned char*)&rxp))
                    {
        				isr_ring_err_num++;
                        goto isr_end;
        			}
                    if(BHT_SUCCESS != bht_L0_sem_give(semc))
    				{
    					isr_sem_err_num++;
    					goto isr_end;
    				}
                    a429_chan_data[board_num][idx - 16].recv_data_count++;
                }
            }
            else
            {
                isr_list_err_num++;
            }
		}
	}

isr_end: 
    /* enable 429 logic pci interrupt */
    value = BIT0;
    if(BHT_SUCCESS != (result = bht_L0_write_mem32(dev_id, BHT_A429_INTR_EN, &value, 1)))
    {
        isr_rdwr_err_num++;
        return NULL;
    }

	return NULL;
}

static bht_L0_u32 
a429_cfg_reg_generate(bht_L1_chan_type_e chan_type, 
        bht_L1_a429_chan_comm_param_t *comm_param,
        bht_L1_a429_rx_chan_gather_param_t *gather_param,
        bht_L1_a429_tx_chan_inject_param_t *inject_param,
        a429_tx_chan_trans_mode_e trans_mode)
{
    bht_L0_u32 value = 0;

    assert(NULL != comm_param);
    
    value = value | (chan_type << 31) | (comm_param->work_mode << 29);

    if(BHT_L1_CHAN_TYPE_TX == chan_type)
    {
        value |= ((comm_param->par & 0x01) << 24);
        
        assert(NULL != inject_param);

        value = value | (inject_param->tb_bits << 27) | \
                (inject_param->tb_gap << 26) | \
                (inject_param->tb_par_en << 25);

		value |= (trans_mode << 12);
    }
    else
    {
        value |= (comm_param->par << 13);
        
        assert(NULL != gather_param);

        value = value | (gather_param->gather_enable << 16) | \
                (gather_param->recv_mode << 15);
    }

    return value;
}
/* a429 general*/

static bht_L0_u32 
a429_mib_clearall(bht_L0_u32 dev_id)
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
a429_mib_clear(bht_L0_u32 dev_id, 
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
a429_mib_get(bht_L0_u32 dev_id, 
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

static bht_L0_u32 
a429_chan_comm_param(bht_L0_u32 dev_id, 
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
    a429_tx_chan_trans_mode_e trans_mode = A429_TX_CHAN_TRANS_MODE_RANDOM;
    

    if((chan_num > 16) | (chan_num < 1))
        return BHT_ERR_INVALID_CHANNEL_NUM;
    
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
        trans_mode = (device_param->tx_chan_param[chan_num - 1].period > 0) ? \
            A429_TX_CHAN_TRANS_MODE_PERIOD : A429_TX_CHAN_TRANS_MODE_RANDOM;
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
//	printf("%s ,channel choose value = 0x%08x\n", __FUNCTION__, value);
    if(BHT_SUCCESS != (result = bht_L0_write_mem32(dev_id, BHT_A429_CHOOSE_CHANNEL_NUM, &value, 1)))
        return result;

    value = a429_cfg_reg_generate(chan_type, comm_param, gather_param, inject_param, trans_mode);
	printf("%s, chan[%d], common param, cfg register = 0x%08x\n", (chan_type == BHT_L1_CHAN_TYPE_TX) ? "TX" : "RX", chan_num, value);
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

//bht_L0_u32
//bht_L1_a429_chan_add(bht_L0_u32 dev_id, bht_L0_u32 chan_num, bht_L1_chan_type_e chan_type)
//{
//    bht_L0_u32 board_num = (dev_id & 0x000F0000) >> 16;

//    if((chan_num > 16) | (chan_num < 1))
//        return BHT_ERR_INVALID_CHANNEL_NUM;

//    if(BHT_L1_CHAN_TYPE_RX == chan_type)
//    {
//        a429_data_u * a429_data = (a429_data_u *)malloc(sizeof(a429_data_u));

//        if(NULL == a429_data)
//            return BHT_FAILURE;
//        
//        a429_device_data[board_num].rx_data[chan_num - 1] = a429_data;
//    }
//}

bht_L0_u32 
bht_L1_a429_irigb_mode_cfg(bht_L0_u32 dev_id, 
        bht_L1_a429_irigb_mode_e mode)
{
    bht_L0_u32 result = BHT_SUCCESS;
    bht_L0_u32 value = mode;

    return bht_L0_write_mem32(dev_id, BHT_A429_IRIG_B_SET, &value, 1);
}

bht_L0_u32 
bht_L1_a429_irigb_time(bht_L0_u32 dev_id, 
        bht_L1_a429_irigb_time_t *ti, 
        bht_L1_param_opt_e param_opt)
{
    bht_L0_u32 result = BHT_SUCCESS;
    bht_L0_u32 value;

    if(BHT_L1_PARAM_OPT_SET == param_opt)
        result = bht_L0_write_mem32(dev_id, BHT_A429_HOST_TIME1, (bht_L0_u32*)ti, 2);
    else
        result = bht_L0_read_mem32(dev_id, BHT_A429_HOST_TIME1, (bht_L0_u32*)ti, 2);

    return result;
}

bht_L0_u32 
bht_L1_a429_default_init(bht_L0_u32 dev_id)
{
    bht_L0_u32 board_num, chan_num;
    bht_L0_u32 result = BHT_SUCCESS;
    bht_L0_u32 value;

    board_num = (dev_id & 0x000F0000) >> 16;

    /* clear MIB */
    result = a429_mib_clearall(dev_id);

    /* record loop and slope register default stat */
    for(chan_num = 0; chan_num < BHT_A429_CHANNEL_MAX; chan_num++)
    {
        bht_L0_sem semc;
        /* send channel, slope default is 1.5us */
        a429_device_param[board_num].tx_chan_param[chan_num].slope = BHT_L1_A429_SLOPE_1_5_US;
        /* default not enable */
        a429_device_param[board_num].tx_chan_param[chan_num].loop_enable = BHT_L1_OPT_DISABLE;

        /* create rx channel data sync semphare */
        if(0 > (semc = bht_L0_semc_create(0, A429_RXP_BUF_MAX)))
            return BHT_FAILURE;
        a429_chan_data[board_num][chan_num].semc = semc;
    }

    /* attach isr and enable 429 device interrupt */
    if(BHT_SUCCESS != (result = bht_L0_attach_inthandler(dev_id, 0, (BHT_L0_USER_ISRFUNC)a429_isr, dev_id)))
        return result;
    
    /* enable 429 device interrupt */
    value = BIT8 | BIT11 | BIT16 | BIT18 | BIT19;
    if(BHT_SUCCESS != (result = bht_L0_write_setupmem32(dev_id, PLX9056_INTCSR, &value, 1)))
        return result;

    /* enable 429 pci interrupt */
    value = BIT0;
    if(BHT_SUCCESS != (result = bht_L0_write_mem32(dev_id, BHT_A429_INTR_EN, &value, 1)))
        return result;


    return result;
}

bht_L0_u32 bht_L1_a429_tx_chan_comm_param(bht_L0_u32 dev_id, 
        bht_L0_u32 chan_num,
        bht_L1_a429_chan_comm_param_t *comm_param, 
        bht_L1_param_opt_e param_opt)
{
    return a429_chan_comm_param(dev_id, chan_num, BHT_L1_CHAN_TYPE_TX, comm_param, param_opt);
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
    const bht_L0_u32 board_num = (dev_id & 0x000F0000) >> 16;
    const a429_device_param_t *device_param = NULL;
    bht_L1_a429_chan_comm_param_t * comm_param = NULL;
    bht_L1_a429_tx_chan_inject_param_t *old_inject_param = NULL;
    a429_tx_chan_trans_mode_e trans_mode;

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
    
    value = (chan_num - 1);
    if(BHT_SUCCESS != (result = bht_L0_write_mem32(dev_id, BHT_A429_CHOOSE_CHANNEL_NUM, &value, 1)))
        return result;

    trans_mode = (device_param->tx_chan_param[chan_num - 1].period > 0) ? \
            A429_TX_CHAN_TRANS_MODE_PERIOD : A429_TX_CHAN_TRANS_MODE_RANDOM;
    value = a429_cfg_reg_generate(BHT_L1_CHAN_TYPE_TX, comm_param, NULL, inject_param, trans_mode);
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
bht_L1_a429_tx_chan_period_param(bht_L0_u32 dev_id,
        bht_L0_u32 chan_num,
        bht_L0_u32 * period,
        bht_L1_param_opt_e param_opt)
{
    bht_L0_u32 value;
    bht_L0_u32 result = BHT_SUCCESS;
    bht_L0_u32 is_changed = BHT_L0_FALSE;
    bht_L0_u32 board_num = (dev_id & 0x000F0000) >> 16;
    const a429_device_param_t *device_param = NULL;
    const bht_L1_a429_chan_comm_param_t * comm_param = NULL;
    const bht_L1_a429_tx_chan_inject_param_t *inject_param = NULL;
    a429_tx_chan_trans_mode_e trans_mode;
    bht_L0_u32 *old_period;


    if((chan_num > 16) | (chan_num < 1))
        return BHT_ERR_INVALID_CHANNEL_NUM;
    
    //read
    device_param = &a429_device_param[board_num];
    comm_param = &device_param->tx_chan_param[chan_num - 1].comm_param;
    inject_param = &device_param->tx_chan_param[chan_num - 1].inject_param;
    old_period = &device_param->tx_chan_param[chan_num - 1].period;

    if(BHT_L1_PARAM_OPT_GET == param_opt)
    {
        memcpy((void*)period, (void*)old_period, sizeof(*period));
        return result;
    }
    
    //compare
    if((*old_period) != (*period))
        is_changed = BHT_L0_TRUE;

    if(BHT_L0_FALSE == is_changed)
        return result;

    //param set
    value = 1;
    if(BHT_SUCCESS != (result = bht_L0_write_mem32(dev_id, BHT_A429_CFG_ENABLE, &value, 1)))
        return result;
    
    value = (chan_num - 1);
    if(BHT_SUCCESS != (result = bht_L0_write_mem32(dev_id, BHT_A429_CHOOSE_CHANNEL_NUM, &value, 1)))
        return result;

    trans_mode = ((*period) > 0) ? A429_TX_CHAN_TRANS_MODE_PERIOD : A429_TX_CHAN_TRANS_MODE_RANDOM;
    value = a429_cfg_reg_generate(BHT_L1_CHAN_TYPE_TX, comm_param, NULL, inject_param, trans_mode);
    printf("TX, chan[%d], period param, cfg register = 0x%08x\n", chan_num, value);
    if(BHT_SUCCESS != (result = bht_L0_write_mem32(dev_id, BHT_A429_CHANNEL_CFG, &value, 1)))
        return result;

    if((*period) > 0)
    {
		printf("TX, chan[%d], period param, period regis = 0x%08x\n", chan_num, *period);
        if(BHT_SUCCESS != (result = bht_L0_write_mem32(dev_id, BHT_A429_TX_CHAN_SEND_PERIOD, period, 1)))
            return result;
    }
    else
    {
        result = bht_L1_a429_tx_chan_send(dev_id, chan_num, BHT_L1_A429_OPT_PERIOD_SEND_STOP, 0);
        assert(BHT_SUCCESS == result);
    }
    
    value = 0;
    if(BHT_SUCCESS != (result = bht_L0_write_mem32(dev_id, BHT_A429_CFG_ENABLE, &value, 1)))
        return result;
    
    //TODO generate chan common param change event

    memcpy((void*)old_period, (void*)period, sizeof(*old_period));
    
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

    printf("TX, chan[%d], loop enable, Loop register = 0x%08x\n", chan_num, value);
    
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
    return a429_mib_clear(dev_id, chan_num, BHT_L1_CHAN_TYPE_TX);
}

bht_L0_u32 
bht_L1_a429_tx_chan_mib_get(bht_L0_u32 dev_id, 
        bht_L0_u32 chan_num, 
        bht_L1_a429_mib_data_t *mib_data)
{
    return a429_mib_get(dev_id, chan_num, BHT_L1_CHAN_TYPE_TX, mib_data);
}

bht_L0_u32
bht_L1_a429_tx_chan_send(bht_L0_u32 dev_id, 
        bht_L0_u32 chan_num,
        bht_L1_a429_send_opt_e opt,
        bht_L0_u32 data)
{
    bht_L0_u32 value, idx;
    bht_L0_u32 result = BHT_SUCCESS;
    bht_L0_u32 is_changed = BHT_L0_FALSE;
    const a429_tx_chan_param_t *tx_chan_param = NULL;
    const bht_L0_u32 board_num = (dev_id & 0x000F0000) >> 16;
    bht_L0_u32 cnt = 0;    

    /*check input */
    if((chan_num > 16) | (chan_num < 1))
        return BHT_ERR_INVALID_CHANNEL_NUM;

    tx_chan_param = &a429_device_param[board_num].tx_chan_param[chan_num - 1];
    if(0 == tx_chan_param->period)
	{
        if(BHT_L1_A429_OPT_RANDOM_SEND != opt)
            return BHT_ERR_BAD_INPUT;
	}
    else
	{
        if(BHT_L1_A429_OPT_RANDOM_SEND == opt)
            return BHT_ERR_BAD_INPUT;
	}

    if(BHT_L1_A429_PARITY_NONE != tx_chan_param->comm_param.par)
    {
        for(idx = 0; idx < 31; idx++)
        {
            if((0x01 << idx) & data)
                cnt++;
        }
    }
    if(BHT_L1_A429_PARITY_ODD == tx_chan_param->comm_param.par)
    {
        if(cnt % 2 == 0)
            data |= BIT31;
        else
            data &= (~BIT31);
    }
    else if(BHT_L1_A429_PARITY_EVEN == tx_chan_param->comm_param.par)
    {
        if(cnt % 2 == 0)
            data &= (~BIT31);
        else
            data |= BIT31;
    }
    
    switch(opt)
    {
        case BHT_L1_A429_OPT_RANDOM_SEND:
            /* choose channel */
            value = 0x0F & (chan_num - 1);
            if(BHT_SUCCESS != (result = bht_L0_write_mem32(dev_id, BHT_A429_CHANNEL_NUM_SEND, &value, 1)))
                return result;
            /* check channel status stat: full or not full */
            if(BHT_SUCCESS != (result = bht_L0_read_mem32(dev_id, BHT_A429_STATUS_CHANNEL_SEND, &value, 1)))
                return result;
            
            if(value & BIT0)
        	{
        //	    printf("channel status stat : 0x%08x\n", value);
                return BHT_ERR_BUFFER_FULL;
        	}
            result = bht_L0_write_mem32(dev_id, BHT_A429_WORD_WR_CHANNEL, &data, 1);
            break;
        case BHT_L1_A429_OPT_PERIOD_SEND_UPDATE:
            /* choose channel */
            value = 0x0F & (chan_num - 1);
            if(BHT_SUCCESS != (result = bht_L0_write_mem32(dev_id, BHT_A429_CHANNEL_NUM_SEND, &value, 1)))
                return result;
            /* write send data */
            result = bht_L0_write_mem32(dev_id, BHT_A429_WORD_WR_CHANNEL, &data, 1);
            break;
        case BHT_L1_A429_OPT_PERIOD_SEND_START:
			value = 1;
			if(BHT_SUCCESS != (result = bht_L0_write_mem32(dev_id, BHT_A429_CFG_ENABLE, &value, 1)))
		        return result;
			value = 0x0F & (chan_num - 1);
			if(BHT_SUCCESS != (result = bht_L0_write_mem32(dev_id, BHT_A429_CHOOSE_CHANNEL_NUM, &value, 1)))
		        return result;
            value = BIT0;
            if(BHT_SUCCESS != (result = bht_L0_write_mem32(dev_id, BHT_A429_TX_CHAN_SEND_PERIOD_CTRL, &value, 1)))
		        return result;
			value = 0;
			if(BHT_SUCCESS != (result = bht_L0_write_mem32(dev_id, BHT_A429_CFG_ENABLE, &value, 1)))
		        return result;
            break;
        case BHT_L1_A429_OPT_PERIOD_SEND_STOP:
            value = 1;
			if(BHT_SUCCESS != (result = bht_L0_write_mem32(dev_id, BHT_A429_CFG_ENABLE, &value, 1)))
		        return result;
			value = 0x0F & (chan_num - 1);
			if(BHT_SUCCESS != (result = bht_L0_write_mem32(dev_id, BHT_A429_CHOOSE_CHANNEL_NUM, &value, 1)))
		        return result;
            value = 0;
            if(BHT_SUCCESS != (result = bht_L0_write_mem32(dev_id, BHT_A429_TX_CHAN_SEND_PERIOD_CTRL, &value, 1)))
		        return result;
			value = 0;
			if(BHT_SUCCESS != (result = bht_L0_write_mem32(dev_id, BHT_A429_CFG_ENABLE, &value, 1)))
		        return result;
            break;
        default:
            result = BHT_ERR_BAD_INPUT;
    }

    return result;
}

bht_L0_u32 
bht_L1_a429_rx_chan_comm_param(bht_L0_u32 dev_id, 
        bht_L0_u32 chan_num,
        bht_L1_a429_chan_comm_param_t *comm_param, 
        bht_L1_param_opt_e param_opt)
{
    return a429_chan_comm_param(dev_id, chan_num, BHT_L1_CHAN_TYPE_RX, comm_param, param_opt);
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
//	printf("%s ,channel choose value = 0x%08x\n", __FUNCTION__, value);
    if(BHT_SUCCESS != (result = bht_L0_write_mem32(dev_id, BHT_A429_CHOOSE_CHANNEL_NUM, &value, 1)))
        return result;

    value = a429_cfg_reg_generate(BHT_L1_CHAN_TYPE_RX, comm_param, gather_param, NULL, A429_TX_CHAN_TRANS_MODE_RANDOM);
	printf("RX, chan[%d], gather param, cfg register = 0x%08x\n", chan_num, value);
    if(BHT_SUCCESS != (result = bht_L0_write_mem32(dev_id, BHT_A429_CHANNEL_CFG, &value, 1)))
        return result;
    
    value = gather_param->threshold_count << 16;
	
    value |= gather_param->threshold_time;
//	printf("%s ,threshold value = 0x%08x\n", __FUNCTION__, value);
    if(BHT_SUCCESS != (result = bht_L0_write_mem32(dev_id, BHT_A429_INT_THRESHOLD, &value, 1)))
        return result;
    
    value = 0;
    if(BHT_SUCCESS != (result = bht_L0_write_mem32(dev_id, BHT_A429_CFG_ENABLE, &value, 1)))
        return result;
    
    //TODO generate chan common param change event

    //initialize ring buf
	{
        struct ring_buf * r = (struct ring_buf *)&(a429_chan_data[board_num][chan_num - 1].rxp_ring_buf.base);
		unsigned char *base = (unsigned char *)&(a429_chan_data[board_num][chan_num - 1].rxp_buf[0].timestamp);
        
        if(NULL == ring_buf_init(r, base, sizeof(bht_L1_a429_rxp_t), A429_RXP_BUF_MAX))
            return BHT_FAILURE;
    }
    
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
    const bht_L0_u32 board_num = (dev_id & 0x000F0000) >> 16;
    
    if((chan_num > 16) | (chan_num < 1))
        return BHT_ERR_INVALID_CHANNEL_NUM;

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
    return a429_mib_clear(dev_id, chan_num, BHT_L1_CHAN_TYPE_RX);
}

bht_L0_u32 
bht_L1_a429_rx_chan_mib_get(bht_L0_u32 dev_id, 
        bht_L0_u32 chan_num, 
        bht_L1_a429_mib_data_t *mib_data)
{
    return a429_mib_get(dev_id, chan_num, BHT_L1_CHAN_TYPE_RX, mib_data);
}

bht_L0_u32 
bht_L1_a429_rx_chan_recv(bht_L0_u32 dev_id, 
        bht_L0_u32 chan_num, 
        bht_L1_a429_rxp_t *rxp_buf, 
        bht_L0_u32 max_rxp, 
        bht_L0_u32 *rxp_num, 
        bht_L0_s32 timeout_ms)
{
    bht_L0_u32 result = BHT_SUCCESS;
    bht_L0_u32 cnt = 0;
    const bht_L0_u32 board_num = (dev_id & 0x000F0000) >> 16;
    bht_L1_a429_recv_mod_e recv_mode;
    static bht_L0_s32 errn = 0;
    bht_L0_s32 ratio = 0;

    if((timeout_ms < BHT_L1_WAIT_FOREVER) || (max_rxp == 0) ||
        (chan_num < 1) || (chan_num > 16))
        return BHT_ERR_BAD_INPUT;

    if(BHT_L1_WAIT_FOREVER == timeout_ms)
        timeout_ms = 0x7FFFFFFF;

    if(timeout_ms > 0)
        ratio = 1;

    do
    {
        const a429_chan_data_t *chan_data = &a429_chan_data[board_num][chan_num - 1];
		const struct ring_buf * r = &a429_chan_data[board_num][chan_num - 1].rxp_ring_buf;
        
        if(BHT_SUCCESS == bht_L0_sem_take(chan_data->semc, ratio))
        {
            if(0 < ring_buf_get(r, (unsigned char *)(rxp_buf + cnt)))
            {
                cnt++;
            }
            else
            {
                errn--;
            }
        }
        //超时
        else
        {
            timeout_ms -= ratio;
        }
		if(0 == timeout_ms)
			errn--;
    }while(timeout_ms && (cnt < max_rxp));

    *rxp_num = cnt;

    return result;
}

bht_L0_u32
bht_L1_a429_chan_dump(bht_L0_u32 dev_id, 
        bht_L0_u32 chan_num, 
        bht_L1_chan_type_e type)
{
    bht_L0_u32 loop_r = 0;
    bht_L0_u32 cfg_r;    
    bht_L0_u32 baud_r;
    bht_L0_u32 int_threshold_r;
    bht_L1_a429_mib_data_t mib_data;
    const a429_device_param_t *device_param;
    const bht_L1_a429_chan_comm_param_t *comm_param;
    const bht_L1_a429_tx_chan_inject_param_t *inject_param = NULL;
    const bht_L1_a429_rx_chan_gather_param_t *gather_param = NULL;
    const bht_L0_u32 board_num = (dev_id & 0x000F0000) >> 16;
    a429_tx_chan_trans_mode_e trans_mode = A429_TX_CHAN_TRANS_MODE_RANDOM;
    bht_L0_u32 idx;
    const a429_chan_data_t* chan_data;

    if((chan_num < 1) || (chan_num > 16))
        return BHT_ERR_BAD_INPUT;

    device_param = &a429_device_param[board_num];
    if(BHT_L1_CHAN_TYPE_RX == type)
    {
        comm_param = &device_param->rx_chan_param[chan_num - 1].comm_param;
        gather_param = &device_param->rx_chan_param[chan_num - 1].gather_param;
        int_threshold_r = ((bht_L0_u32)gather_param->threshold_count) << 16 | gather_param->threshold_time;
        chan_data = &a429_chan_data[board_num][chan_num - 1];
    }
    else
    {
        comm_param = &device_param->tx_chan_param[chan_num - 1].comm_param;
        inject_param = &device_param->tx_chan_param[chan_num - 1].inject_param;
        trans_mode = (device_param->tx_chan_param[chan_num - 1].period > 0) ? \
            A429_TX_CHAN_TRANS_MODE_PERIOD : A429_TX_CHAN_TRANS_MODE_RANDOM;
    }

    for(idx = 0; idx < BHT_A429_CHANNEL_MAX; idx++)
    {
        if(device_param->tx_chan_param[idx].loop_enable == BHT_L1_OPT_ENABLE)
            loop_r |= (0x01 << idx);
    }

    baud_r = 100 * 1000000 / comm_param->baud;

    cfg_r = a429_cfg_reg_generate(type, comm_param, gather_param, inject_param, trans_mode);

    a429_mib_get(dev_id, chan_num, type, &mib_data);
    
	printf("param size = %d\ndata size = %d\n", sizeof(a429_device_param), sizeof(a429_chan_data));
    if(BHT_L1_CHAN_TYPE_RX == type)
    {
        printf("RX chan[%d] dump\n", chan_num);
        printf("loop_en       0x0040 [0x%08x]\n", loop_r);
        printf("channel_cfg   0x1008 [0x%08x]\n", cfg_r);
        printf("baud          0x100C [0x%08x]\n", baud_r);
        printf("int_threshold 0x1010 [0x%08x]\n", int_threshold_r);
        printf("mib cnt              [0x%08x]\n", mib_data.cnt);
        printf("mib err_cnt          [0x%08x]\n", mib_data.err_cnt);
        printf("recv data count      [0x%08x]\n", chan_data->recv_data_count);
    }
    else
    {
        printf("TX chan[%d] dump\n", chan_num);
        printf("loop_en       0x0040 [0x%08x]\n", loop_r);
        printf("channel_cfg   0x1008 [0x%08x]\n", cfg_r);
        printf("baud          0x100C [0x%08x]\n", baud_r);
        printf("period        0x1300 [0x%08x]\n", device_param->tx_chan_param[chan_num - 1].period);
        printf("mib cnt              [0x%08x]\n", mib_data.cnt);
        printf("mib err_cnt          [0x%08x]\n", mib_data.err_cnt);
    }

    return BHT_SUCCESS;
}

