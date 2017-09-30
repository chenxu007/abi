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
#include <bht_L0_types.h>
#include <bht_L0_plx9056.h>
#include <bht_L1.h>
#include <bht_L1_a429.h>
#include <bht_L1_defs.h>
#include <bht_L1_ring.h>
#include <assert.h>
#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define DEBUG

#ifdef DEBUG
#define DEBUG_PRINTF(format, ...)\
do\
{\
	(void)printf(format, ##__VA_ARGS__); \
}while (0);
#else
#define DEBUG_PRINTF(x)
#endif

bht_L0_u32 
bht_L1_a429_isr(bht_L0_device_t *device)
{
    bht_L0_u32 value, index;
    bht_L0_u32 result = BHT_SUCCESS;
    bht_L0_device_t *device0 = (bht_L0_device_t*)device;
    bht_L0_u32 int_vector;
    bht_L1_a429_rxp_t rxp;
    bht_L1_a429_cb_t *cb = device0->private;
    bht_L1_a429_chan_data_t *chan_data;

    BHT_L1_DEVICE_STATUS_CHK_RTN(device0);
    
    BHT_L1_SEM_TAKE(device0->mutex_sem, BHT_L1_WAIT_FOREVER, result, end);

    cb->int_cnt++;

    BHT_L1_READ_MEM32(device0, BHT_A429_INTR_STATE, &value, 1, result, release_sem);

    if(!(value & BIT4))
    {
        cb->int_stat_err_cnt++;
        goto release_sem;
    }
    
    value = BIT0;
    BHT_L1_WRITE_MEM32(device0, BHT_A429_INTR_CLR, &value, 1, result, release_sem);

    /* check 429 channel interrupt vector */
    BHT_L1_READ_MEM32(device0, BHT_A429_INTR_CHANNEL_VECTOR, &int_vector, 1, result, release_sem);

    if(0 == int_vector)
	{
		cb->int_vector_idle_err_cnt++;
        goto release_sem;
	}
    cb->int_valid_cnt++;
    
    do
    {
    	for(index = BHT_L1_A429_CHAN_MAX; index < (BHT_L1_A429_CHAN_MAX + cb->tot_chans); index++)
    	{
    	    if(0 == (int_vector & (0x01 << index)))
                continue;            

            chan_data = cb->chan_data + (index - BHT_L1_A429_CHAN_MAX);
            
    		//choose channel
    		BHT_L1_WRITE_MEM32(device0, BHT_A429_CHANNEL_NUM_RECV, &index, 1, result, release_sem);
            
            if(BHT_L1_A429_RECV_MODE_SAMPLE == chan_data->recv_mode)
            {
				BHT_L1_READ_MEM32(device0, BHT_A429_WORD_RD_CHANNEL_TIMESTAMP, ((bht_L0_u32*)&rxp), 2, result, release_sem);

                chan_data->recv_data_cnt++;
                chan_data->chk_times++;
                
                if(NULL == ring_buf_put(chan_data->rxp_ring, (unsigned char*)&rxp))
    				chan_data->lost_data_cnt++;

                if(BHT_SUCCESS != bht_L0_sem_give(chan_data->semc))
    			{
    				chan_data->semc_err_cnt++;
    			}
            }
            else
    		{
    		    BHT_L1_READ_MEM32(device0, BHT_A429_STATUS_CHANNEL_RECV, &value, 1, result, release_sem);                

                //check the list empty
                if((value & 0x01) == 0)
                {
                    bht_L0_u32 count = value >> 16;
                    chan_data->chk_times++;
                    
                    while(count)
                    {   
						BHT_L1_READ_MEM32(device0, BHT_A429_WORD_RD_CHANNEL_TIMESTAMP, ((bht_L0_u32*)&rxp), 2, result, release_sem);
                        
                        count--;

                        chan_data->recv_data_cnt++;

                        if(NULL == ring_buf_put(chan_data->rxp_ring, (unsigned char*)&rxp))
                            chan_data->lost_data_cnt++;
                        
                        if(BHT_SUCCESS != bht_L0_sem_give(chan_data->semc))
        				    chan_data->semc_err_cnt++;
                    }
                }
                else
                {
                    chan_data->chan_idle_err_cnt++;
                }
    		}
    	}

        BHT_L1_READ_MEM32(device0, BHT_A429_INTR_CHANNEL_VECTOR, &int_vector, 1, result, release_sem);
    }while(0 != int_vector);
    
release_sem:
    BHT_L1_SEM_GIVE(device0->mutex_sem, result, end);

    value = BIT0;
    BHT_L1_WRITE_MEM32(device0, BHT_A429_INTR_EN, &value, 1, result, end);
end:
	return NULL;
}

static bht_L0_u32
bht_L1_a429_chan_reset(bht_L0_device_t *device, 
        bht_L0_u32 chan_num, 
        bht_L1_chan_type_e chan_type)
{
    bht_L0_u32 result = BHT_SUCCESS;
    bht_L0_u32 value;
    bht_L1_a429_cb_t *cb = device->private;
    bht_L0_device_t *device0 = (bht_L0_device_t*)device;
    bht_L1_a429_chan_data_t *chan_data;
    bht_L1_a429_send_stat_t *send_stat;   

    BHT_L1_A429_CHAN_NUM_CHK_RTN(chan_num);
    
    if(BHT_L1_CHAN_TYPE_RX == chan_type)
    {
        chan_num += (BHT_L1_A429_CHAN_MAX - 1);
        chan_data = cb->chan_data + chan_num;

//        DEBUG_PRINTF("cb->chan_data = %p, chan_data = %p, chan_num = %d\n", \
//            cb->chan_data, chan_data, chan_num);

        chan_data->chan_idle_err_cnt = 0;
        chan_data->chk_times = 0;
        chan_data->lost_data_cnt = 0;
        chan_data->recv_data_cnt = 0;
        chan_data->semc_err_cnt = 0;

        BHT_L1_WRITE_MEM32(device0, BHT_A429_CHOOSE_CHANNEL_NUM, &chan_num, 1, result, end);
        BHT_L1_READ_MEM32(device0, BHT_A429_CHANNEL_CFG, &value, 1, result, end);
        chan_data->recv_mode = (bht_L1_a429_recv_mode_e)((value >> 15) & BIT0);
    }
    else
    {
        chan_num -= 1;
        send_stat = cb->send_stat + chan_num;

        send_stat->lost_data_cnt = 0;
        send_stat->send_data_cnt = 0;

        BHT_L1_WRITE_MEM32(device0, BHT_A429_CHOOSE_CHANNEL_NUM, &chan_num, 1, result, end);
        BHT_L1_READ_MEM32(device0, BHT_A429_CHANNEL_CFG, &value, 1, result, end);
        send_stat->send_mode = (value & BIT12) ? BHT_L1_A429_SEND_MODE_PERIOD :\
            BHT_L1_A429_SEND_MODE_NONPERIOD;
    }

end:
    return result;
}

bht_L0_u32
bht_L1_a429_reset(bht_L0_device_t *device)
{
    bht_L0_u32 value, index, count = 0;
    bht_L0_u32 chan_num;
    bht_L1_a429_cb_t *cb;
    bht_L0_u32 result = BHT_SUCCESS;
    bht_L0_device_t *device0 = (bht_L0_device_t*)device;
    
    BHT_L1_DEVICE_STATUS_CHK_RTN(device0);

    if(BHT_L0_LOGIC_TYPE_A429 != device0->ltype)
        return BHT_ERR_BAD_INPUT;
    cb = (bht_L1_a429_cb_t *)device0->private;
    if(NULL == cb)
        return BHT_ERR_BAD_INPUT;

    BHT_L1_SEM_TAKE(device0->mutex_sem, BHT_L1_WAIT_FOREVER, result, end);
    
    value = 1;
    BHT_L1_WRITE_MEM32(device0, BHT_A429_DEVICE_SOFT_RESET, &value, 1, result, release_sem);
    printf("resetting...\n\ntime escape %5d ms", count);
    do
    {
        bht_L0_msleep(10);
        printf("\b\b\b\b\b\b\b\b%5d ms", (count += 10));
        BHT_L1_READ_MEM32(device0, BHT_A429_DEVICE_STATE, &value, 1, result, release_sem);
    }while(value != 0x00000001);

    cb->err_cnt = 0;
    cb->int_cnt = 0;
    cb->int_stat_err_cnt = 0;
    cb->int_valid_cnt = 0;
    cb->int_vector_idle_err_cnt = 0;
    cb->last_err = BHT_SUCCESS;

    /* read totchans */
    cb->tot_chans = 0;
    BHT_L1_READ_MEM32(device0, BHT_A429_CARD_TYPE, &value, 1, result, release_sem);
    for(index = 0; index < 32; index++)
    {
        if(BIT0 & (value >> index))
            cb->tot_chans += 1;
    }    
    if(cb->tot_chans % 2)
    {
        result = BHT_ERR_DEFAULT_PARAM;
        goto release_sem;
    }
    cb->tot_chans = cb->tot_chans / 2;

    for(chan_num = 1; chan_num <= cb->tot_chans; chan_num++)
    {
        if(BHT_SUCCESS != bht_L1_a429_chan_reset(device, chan_num, BHT_L1_CHAN_TYPE_RX))
            goto release_sem;
        if(BHT_SUCCESS != bht_L1_a429_chan_reset(device, chan_num, BHT_L1_CHAN_TYPE_TX))
            goto release_sem;
    }
    
release_sem:
    BHT_L1_SEM_GIVE(device0->mutex_sem, result, end);
end:
    return result;
}

bht_L0_u32
bht_L1_a429_private_alloc(bht_L0_device_t *device)
{
    bht_L0_u32 chan_num;
    bht_L0_u32 value, index;
    bht_L1_a429_cb_t *cb;
    bht_L0_u32 result = BHT_SUCCESS;
    bht_L0_device_t *device0 = (bht_L0_device_t*)device;
    bht_L1_a429_recv_mode_e recv_mode;
    bht_L1_a429_chan_data_t *chan_data;
    bht_L1_a429_send_stat_t *send_stat;

    if(BHT_L0_LOGIC_TYPE_A429 != device0->ltype)
        return BHT_ERR_BAD_INPUT;
    
    /* alloc private data */
    if(NULL == (cb = (bht_L1_a429_cb_t *)calloc(1, sizeof(bht_L1_a429_cb_t))))
        return BHT_ERR_MEM_ALLOC_FAIL;

    DEBUG_PRINTF("reset\n");
    /* reset logic */
    value = 1;
    bht_L0_write_mem32(device0, BHT_A429_DEVICE_SOFT_RESET, &value, 1);
    do
    {
        bht_L0_msleep(1);
        bht_L0_read_mem32(device0, BHT_A429_DEVICE_STATE, &value, 1);
    }while(value != 0x00000001);

    /* read totchans */
    cb->tot_chans = 0;
    if(BHT_SUCCESS == (result = bht_L0_read_mem32(device0, \
        BHT_A429_CARD_TYPE, (bht_L0_u32*)&value, 1)))
    {
        for(index = 0; index < 32; index++)
        {
            if(BIT0 & (value >> index))
            {
                cb->tot_chans += 1;
            }
        }
        
        if(cb->tot_chans % 2)
        {
            result = BHT_ERR_DEFAULT_PARAM;
            goto alloc_err;
        }

        cb->tot_chans = cb->tot_chans / 2;
    }
    else
    {
        goto alloc_err;
    }
    /* alloc channel data */
    if(cb->tot_chans)
    {
        if(NULL == (cb->chan_data = (bht_L1_a429_chan_data_t*) \
            calloc(cb->tot_chans, sizeof(bht_L1_a429_chan_data_t))))
        {
            result = BHT_ERR_MEM_ALLOC_FAIL;
            goto alloc_err;
        }

        if(NULL == (cb->send_stat = (bht_L1_a429_send_stat_t*) \
            calloc(cb->tot_chans, sizeof(bht_L1_a429_send_stat_t))))
        {
            result = BHT_ERR_MEM_ALLOC_FAIL;
            goto alloc_err;
        }

        for(index = 0; index < cb->tot_chans; index++)
        {
            chan_data = cb->chan_data + index;
            send_stat = cb->send_stat + index;
            
            chan_data->semc = bht_L0_semc_create(0, BHT_L1_A429_RXP_BUF_MAX);
            if(chan_data->semc < 0)
            {
                result = BHT_ERR_SEM_CREAT_FAIL;
                goto  alloc_err;
            }

            chan_data->rxp_ring = ring_buf_init(sizeof(bht_L1_a429_rxp_t), BHT_L1_A429_RXP_BUF_MAX);
            if(NULL == chan_data->rxp_ring)
            {
                result = BHT_ERR_MEM_ALLOC_FAIL;
                goto  alloc_err;
            }

            chan_data->chan_idle_err_cnt = 0;
            chan_data->chk_times = 0;
            chan_data->lost_data_cnt = 0;
            chan_data->recv_data_cnt = 0;
            chan_data->semc_err_cnt = 0;

            chan_num = index + 1;
            BHT_L1_WRITE_MEM32(device0, BHT_A429_CHOOSE_CHANNEL_NUM, &chan_num, 1, result, alloc_err);
            BHT_L1_READ_MEM32(device0, BHT_A429_CHANNEL_CFG, &value, 1, result, alloc_err);
            chan_data->recv_mode = (bht_L1_a429_recv_mode_e)((value >> 15) & BIT0);

            send_stat = cb->send_stat + index;

            send_stat->lost_data_cnt = 0;
            send_stat->send_data_cnt = 0;

            BHT_L1_WRITE_MEM32(device0, BHT_A429_CHOOSE_CHANNEL_NUM, &chan_num, 1, result, alloc_err);
            BHT_L1_READ_MEM32(device0, BHT_A429_CHANNEL_CFG, &value, 1, result, alloc_err);
            send_stat->send_mode = (value & BIT12) ? BHT_L1_A429_SEND_MODE_PERIOD :\
                BHT_L1_A429_SEND_MODE_NONPERIOD;    
        }
            
    }

    /* attach isr and enable 429 device interrupt */
    if(BHT_SUCCESS != (result = bht_L0_attach_inthandler(device0, 0, \
        (BHT_L0_USER_ISRFUNC)bht_L1_a429_isr, (void*)device0)))
        return result;

    /* enable 429 device interrupt */
    value = BIT8 | BIT11 | BIT16 | BIT18 | BIT19;
    if(BHT_SUCCESS != (result = bht_L0_write_setupmem32(device0, PLX9056_INTCSR, &value, 1)))
        return result;

    /* enable 429 pci interrupt */
    value = BIT0;
    if(BHT_SUCCESS != (result = bht_L0_write_mem32(device0, BHT_A429_INTR_EN, &value, 1)))
        return result;

    /* link device private */
    device0->private = (void*)cb;
    
    return result;
    
alloc_err:
    if(cb && cb->chan_data)
    {
        for(index = 0; index < cb->tot_chans; index++)
        {
            if(NULL != ((bht_L1_a429_chan_data_t*)(cb->chan_data + index))->rxp_ring)
            {
                ring_buf_free(((bht_L1_a429_chan_data_t*)(cb->chan_data + index))->rxp_ring);
                ((bht_L1_a429_chan_data_t*)(cb->chan_data + index))->rxp_ring = NULL;
            }
        }
        free(cb->chan_data);
		free(cb->send_stat);
    }
    if(cb)
        free(cb);

    return result;
}

bht_L0_u32
bht_L1_a429_private_free(bht_L0_device_t *device)
{
    bht_L0_u32 value, index;
    bht_L0_u32 result = BHT_SUCCESS;
    bht_L1_a429_cb_t *cb;
    bht_L0_device_t *device0 = (bht_L0_device_t*)device;

    if(NULL == device0)
        return BHT_SUCCESS;

    cb = device0->private;

    /* diable 9056 interrupt */
    value = 0;
    if(BHT_SUCCESS != (result = bht_L0_write_setupmem32(device0, PLX9056_INTCSR, &value, 1)))
        return result;

    /* clear fpga pci interrupt */
    value = BIT0;
    BHT_L1_WRITE_MEM32(device0, BHT_A429_INTR_CLR, &value, 1, result, end);
    
    
    DEBUG_PRINTF("start datach interrupt\n");
    if(BHT_SUCCESS != (result = bht_L0_detach_inthandler(device0)))
        return result;
    DEBUG_PRINTF("datach interrupt succ\n");
    
    if(cb && cb->chan_data)
    {
        for(index = 0; index < cb->tot_chans; index++)
        {
            if(NULL != ((bht_L1_a429_chan_data_t*)(cb->chan_data + index))->rxp_ring)
            {
                ring_buf_free(((bht_L1_a429_chan_data_t*)(cb->chan_data + index))->rxp_ring);
                ((bht_L1_a429_chan_data_t*)(cb->chan_data + index))->rxp_ring = NULL;
            }
        }
        free(cb->chan_data);
        free(cb->send_stat);
    }
    if(cb)
        free(cb);

    device0->private = NULL;
end:
    return result;
}

bht_L0_u32 
bht_L1_a429_irigb_mode(bht_L1_device_handle_t device, 
        bht_L1_a429_irigb_mode_e *mode,
        bht_L1_param_opt_e param_opt)
{
    bht_L0_u32 result = BHT_SUCCESS;
    bht_L0_device_t *device0 = (bht_L0_device_t*)device;

    BHT_L1_DEVICE_STATUS_CHK_RTN(device0);

    BHT_L1_SEM_TAKE(device0->mutex_sem, BHT_L1_WAIT_FOREVER, result, end);

	if (BHT_L1_PARAM_OPT_SET == param_opt)
	{
		BHT_L1_WRITE_MEM32(device0, BHT_A429_IRIG_B_SET, ((bht_L0_u32*)mode), 1, result, release_sem);
	}
    else
        BHT_L1_READ_MEM32(device0, BHT_A429_IRIG_B_SET, ((bht_L0_u32*)mode), 1, result, release_sem);

release_sem:
	BHT_L1_SEM_GIVE(device0->mutex_sem, result, end);
end:
    return result;
}

bht_L0_u32 
bht_L1_a429_irigb_time(bht_L0_device_t *device, 
        bht_L1_a429_irigb_time_t *ti, 
        bht_L1_param_opt_e param_opt)
{
    bht_L0_u32 result = BHT_SUCCESS;
    bht_L0_device_t *device0 = (bht_L0_device_t*)device;

    BHT_L1_DEVICE_STATUS_CHK_RTN(device0);
    
    BHT_L1_SEM_TAKE(device0->mutex_sem, BHT_L1_WAIT_FOREVER, result, end);

	if (BHT_L1_PARAM_OPT_SET == param_opt)
	{
		BHT_L1_WRITE_MEM32(device0, BHT_A429_HOST_TIME1, ((bht_L0_u32*)ti), 2, result, release_sem);
	}        
	else
	{
		BHT_L1_READ_MEM32(device0, BHT_A429_HOST_TIME1, ((bht_L0_u32*)ti), 2, result, release_sem);
	}

release_sem:
	BHT_L1_SEM_GIVE(device0->mutex_sem, result, end);
end:
    return result;
}

bht_L0_u32 
bht_L1_a429_get_totchans(bht_L1_device_handle_t device,
        bht_L0_u32 *totchans)
{
    bht_L0_u32 value, index;
    bht_L0_u32 result = BHT_SUCCESS;
    bht_L0_device_t *device0 = (bht_L0_device_t*)device;

    *totchans = 0;

    BHT_L1_DEVICE_STATUS_CHK_RTN(device0);

    BHT_L1_SEM_TAKE(device0->mutex_sem, BHT_L1_WAIT_FOREVER, result, end);

    BHT_L1_READ_MEM32(device0, BHT_A429_CARD_TYPE, &value, 1, result, release_sem);
    
    for(index = 0; index < 32; index++)
    {
        if(BIT0 & (value >> index))
            *totchans = *totchans + 1;
    }
release_sem:
	BHT_L1_SEM_GIVE(device0->mutex_sem, result, end);
end:
    return result;
}

static bht_L0_u32 
bht_L1_a429_chan_ctrl(bht_L1_device_handle_t device,
        bht_L0_u32 chan_num,
        bht_L1_chan_type_e chan_type,
        bht_L1_a429_chan_stat_e stat)
{
    bht_L0_u32 value;
    bht_L0_u32 result = BHT_SUCCESS;
    bht_L0_device_t *device0 = (bht_L0_device_t*)device;

	BHT_L1_A429_CHAN_NUM_CHK_RTN(chan_num);	
	
    BHT_L1_A429_CFG(device0, BHT_L1_ENABLE, result, end);

	chan_num = (BHT_L1_CHAN_TYPE_RX == chan_type) ? (chan_num + BHT_L1_A429_CHAN_MAX - 1) :\
		(chan_num - 1);

	BHT_L1_WRITE_MEM32(device0, BHT_A429_CHOOSE_CHANNEL_NUM, &chan_num, 1, result, cfg_disable);

    BHT_L1_READ_MEM32(device0, BHT_A429_CHANNEL_CFG, &value, 1, result, cfg_disable);

	value &= (~(bht_L0_u32)(BIT29 | BIT30));
	value |= (bht_L0_u32)(stat << 29);	

	BHT_L1_WRITE_MEM32(device0, BHT_A429_CHANNEL_CFG, &value, 1, result, cfg_disable);	
cfg_disable:
	BHT_L1_A429_CFG(device0, BHT_L1_DISABLE, result, end); 
end:
	return result;
}

bht_L0_u32 
bht_L1_a429_chan_open(bht_L1_device_handle_t device,
        bht_L0_u32 chan_num,
        bht_L1_chan_type_e chan_type)
{
    bht_L0_u32 result = BHT_SUCCESS;
    bht_L0_device_t *device0 = (bht_L0_device_t*)device;

    BHT_L1_DEVICE_STATUS_CHK_RTN(device0);

    BHT_L1_SEM_TAKE(device0->mutex_sem, BHT_L1_WAIT_FOREVER, result, end);

	result = bht_L1_a429_chan_ctrl(device0, chan_num, chan_type, BHT_L1_A429_CHAN_STAT_OPEN);

release_sem:
	BHT_L1_SEM_GIVE(device0->mutex_sem, result, end);   
end:
    return result;
}

bht_L0_u32 
bht_L1_a429_chan_close(bht_L1_device_handle_t device,
        bht_L0_u32 chan_num,
        bht_L1_chan_type_e chan_type)
{
    bht_L0_u32 result = BHT_SUCCESS;
    bht_L0_device_t *device0 = (bht_L0_device_t*)device;

    BHT_L1_DEVICE_STATUS_CHK_RTN(device0);

    BHT_L1_SEM_TAKE(device0->mutex_sem, BHT_L1_WAIT_FOREVER, result, end);

	result = bht_L1_a429_chan_ctrl(device0, chan_num, chan_type, BHT_L1_A429_CHAN_STAT_CLOSE);

release_sem:
	BHT_L1_SEM_GIVE(device0->mutex_sem, result, end);   
end:
    return result;
}

bht_L0_u32 
bht_L1_a429_chan_close_and_clear_fifo(bht_L1_device_handle_t device,
        bht_L0_u32 chan_num,
        bht_L1_chan_type_e chan_type)
{
    bht_L0_u32 result = BHT_SUCCESS;
    bht_L0_device_t *device0 = (bht_L0_device_t*)device;

    BHT_L1_DEVICE_STATUS_CHK_RTN(device0);

    BHT_L1_SEM_TAKE(device0->mutex_sem, BHT_L1_WAIT_FOREVER, result, end);

	result = bht_L1_a429_chan_ctrl(device0, chan_num, chan_type, BHT_L1_A429_CHAN_STAT_CLOSE_AND_CLEAR_FIFO);

release_sem:
	BHT_L1_SEM_GIVE(device0->mutex_sem, result, end);   
end:
    return result;
}

bht_L0_u32 
bht_L1_a429_chan_get_stat(bht_L1_device_handle_t device,
        bht_L0_u32 chan_num,
        bht_L1_chan_type_e chan_type,
        bht_L1_a429_chan_stat_e *chan_stat)
{
    bht_L0_u32 value;
    bht_L0_u32 result = BHT_SUCCESS;
    bht_L0_device_t *device0 = (bht_L0_device_t*)device;

    BHT_L1_DEVICE_STATUS_CHK_RTN(device0);
    
	BHT_L1_A429_CHAN_NUM_CHK_RTN(chan_num);

	BHT_L1_SEM_TAKE(device0->mutex_sem, BHT_L1_WAIT_FOREVER, result, end);
	
    BHT_L1_A429_CFG(device0, BHT_L1_ENABLE, result, release_sem);

	chan_num = (BHT_L1_CHAN_TYPE_RX == chan_type) ? (chan_num + BHT_L1_A429_CHAN_MAX - 1) :\
		(chan_num - 1);

	BHT_L1_WRITE_MEM32(device0, BHT_A429_CHOOSE_CHANNEL_NUM, &chan_num, 1, result, cfg_disable);

    BHT_L1_READ_MEM32(device0, BHT_A429_CHANNEL_CFG, &value, 1, result, cfg_disable);
cfg_disable:
	BHT_L1_A429_CFG(device0, BHT_L1_DISABLE, result, release_sem);

	value &= (bht_L0_u32)(BIT29 | BIT30);
	*chan_stat = value >> 29;	
release_sem:
	BHT_L1_SEM_GIVE(device0->mutex_sem, result, end);
end:
	return result;
}

bht_L0_u32 
bht_L1_a429_chan_baud(bht_L1_device_handle_t device,
        bht_L0_u32 chan_num,
        bht_L1_chan_type_e chan_type,
        bht_L1_a429_baud_rate_e * baud,
        bht_L1_param_opt_e param_opt)
{
    bht_L0_u32 value;
    bht_L0_u32 result = BHT_SUCCESS;
    bht_L0_device_t *device0 = (bht_L0_device_t*)device;
    bht_L1_a429_slope_e slope;

    BHT_L1_DEVICE_STATUS_CHK_RTN(device0);
    
	BHT_L1_A429_CHAN_NUM_CHK_RTN(chan_num);
    
	BHT_L1_SEM_TAKE(device0->mutex_sem, BHT_L1_WAIT_FOREVER, result, end);
    
    BHT_L1_A429_CFG(device0, BHT_L1_ENABLE, result, release_sem);
	chan_num = (BHT_L1_CHAN_TYPE_RX == chan_type) ? (chan_num + BHT_L1_A429_CHAN_MAX - 1) :\
		(chan_num - 1);

	BHT_L1_WRITE_MEM32(device0, BHT_A429_CHOOSE_CHANNEL_NUM, &chan_num, 1, result, cfg_disable);

    if(BHT_L1_PARAM_OPT_GET == param_opt)
    {
		BHT_L1_READ_MEM32(device0, BHT_A429_BAUD_RATE_SET, ((bht_L0_u32*)&value), 1, result, cfg_disable);
        if(0 != value)
            *baud = 100 * 1000000 / value;
        else
            *baud = 0;
    }
	else
    {
        if(0 == (*baud))
        {
            result = BHT_ERR_BAD_INPUT;
            goto cfg_disable;
        }
        value = 100 * 1000000 / (*baud);
		BHT_L1_WRITE_MEM32(device0, BHT_A429_BAUD_RATE_SET, ((bht_L0_u32*)&value), 1, result, cfg_disable);

        /* slope config */
        if(BHT_L1_CHAN_TYPE_TX == chan_type)
        {
            if(BHT_L1_A429_BAUD_12_5K <= *baud)
                slope = BHT_L1_A429_SLOPE_1_5_US;
            else
                slope = BHT_L1_A429_SLOPE_10_US;

            BHT_L1_READ_MEM32(device0, BHT_A429_SLOPE_CTRL, &value, 1, result, cfg_disable);

            value &= ~(bht_L0_u32)(BIT0 << chan_num);
            value |= (slope) ? (BIT0 << chan_num) : 0;

            BHT_L1_WRITE_MEM32(device0, BHT_A429_SLOPE_CTRL, &value, 1, result, cfg_disable);
        }

        
    }
cfg_disable:
	BHT_L1_A429_CFG(device0, BHT_L1_DISABLE, result, release_sem);
release_sem:
	BHT_L1_SEM_GIVE(device0->mutex_sem, result, end);
end:
	return result;
}

bht_L0_u32 
bht_L1_a429_chan_parity(bht_L1_device_handle_t device,
        bht_L0_u32 chan_num,
        bht_L1_chan_type_e chan_type,
        bht_L1_a429_parity_e *parity,
        bht_L1_param_opt_e param_opt)
{
    bht_L0_u32 value, temp;
    bht_L0_u32 result = BHT_SUCCESS;
    bht_L0_device_t *device0 = (bht_L0_device_t*)device;

    BHT_L1_DEVICE_STATUS_CHK_RTN(device0);
    
	BHT_L1_A429_CHAN_NUM_CHK_RTN(chan_num);

	BHT_L1_SEM_TAKE(device0->mutex_sem, BHT_L1_WAIT_FOREVER, result, end);
	
    BHT_L1_A429_CFG(device0, BHT_L1_ENABLE, result, release_sem);

	chan_num = (BHT_L1_CHAN_TYPE_RX == chan_type) ? (chan_num + BHT_L1_A429_CHAN_MAX - 1) :\
		(chan_num - 1);

	BHT_L1_WRITE_MEM32(device0, BHT_A429_CHOOSE_CHANNEL_NUM, &chan_num, 1, result, cfg_disable);

    BHT_L1_READ_MEM32(device0, BHT_A429_CHANNEL_CFG, &value, 1, result, cfg_disable);

    if(BHT_L1_PARAM_OPT_GET == param_opt)
	{
	    if(BHT_L1_CHAN_TYPE_TX == chan_type)
    	{
    	    *parity = (value & BIT24) >> 24;
    	}
		else
		{
		    *parity = (value & (BIT13 | BIT14)) >> 13;
		}
	}
	else
	{
		if(BHT_L1_CHAN_TYPE_TX == chan_type)
    	{
    	    value &= (~(bht_L0_u32)BIT24);
			value |= *parity;
    	}
		else
		{
		    temp = *parity & 0x3;
			value &= (~(bht_L0_u32)(BIT13 | BIT14));
			value |= (temp << 13);
		}
		BHT_L1_WRITE_MEM32(device0, BHT_A429_CHANNEL_CFG, &value, 1, result, cfg_disable);
	}
cfg_disable:
	BHT_L1_A429_CFG(device0, BHT_L1_DISABLE, result, release_sem);
release_sem:
	BHT_L1_SEM_GIVE(device0->mutex_sem, result, end);
end:
	return result;
}

bht_L0_u32 
bht_L1_a429_chan_get_mib(bht_L1_device_handle_t device,
        bht_L0_u32 chan_num,
        bht_L1_chan_type_e chan_type,
        bht_L1_a429_mib_data_t *mib_data)
{
    bht_L0_u32 offset;
    bht_L0_u32 result = BHT_SUCCESS;
    bht_L0_device_t *device0 = (bht_L0_device_t*)device;
    #define BHT_A429_MIB_BASE_ADDR    BHT_A429_NUM_WORD_CHANNEL0
    #define BHT_A429_MIB_CHANNEL_STEP   0x00000010

    BHT_L1_DEVICE_STATUS_CHK_RTN(device0);
    
    BHT_L1_A429_CHAN_NUM_CHK_RTN(chan_num);

	BHT_L1_SEM_TAKE(device0->mutex_sem, BHT_L1_WAIT_FOREVER, result, end);

	chan_num = (BHT_L1_CHAN_TYPE_RX == chan_type) ? (chan_num + BHT_L1_A429_CHAN_MAX - 1) :\
		(chan_num - 1);
    
    offset = BHT_A429_MIB_BASE_ADDR + chan_num * BHT_A429_MIB_CHANNEL_STEP;
    BHT_L1_READ_MEM32(device0, offset, ((bht_L0_u32*)mib_data), 2, result, release_sem);
    
release_sem:
    BHT_L1_SEM_GIVE(device0->mutex_sem, result, end);
end:
    return result;
}

bht_L0_u32 
bht_L1_a429_chan_clear_mib(bht_L1_device_handle_t device,
        bht_L0_u32 chan_num,
        bht_L1_chan_type_e chan_type)
{
    bht_L0_u32 value;
    bht_L0_u32 result = BHT_SUCCESS;
    bht_L0_device_t *device0 = (bht_L0_device_t*)device;

    BHT_L1_DEVICE_STATUS_CHK_RTN(device0);
    
    BHT_L1_A429_CHAN_NUM_CHK_RTN(chan_num);

	BHT_L1_SEM_TAKE(device0->mutex_sem, BHT_L1_WAIT_FOREVER, result, end);

	chan_num = (BHT_L1_CHAN_TYPE_RX == chan_type) ? (chan_num + BHT_L1_A429_CHAN_MAX - 1) :\
		(chan_num - 1);

    value = BIT0 << chan_num;

    BHT_L1_WRITE_MEM32(device0, BHT_A429_MIBS_CLR, &value, 1, result, release_sem);

    if(BHT_SUCCESS != bht_L1_a429_chan_reset(device0, chan_num, chan_type))
        goto release_sem;
    
release_sem:
    BHT_L1_SEM_GIVE(device0->mutex_sem, result, end);
end:
    return result;
}

bht_L0_u32 
bht_L1_a429_chan_loop(bht_L1_device_handle_t device,
        bht_L0_u32 chan_num,
        bht_L1_able_e *able,
        bht_L1_param_opt_e param_opt)
{
    bht_L0_u32 value;
    bht_L0_u32 result = BHT_SUCCESS;
    bht_L0_device_t *device0 = (bht_L0_device_t*)device;

    BHT_L1_DEVICE_STATUS_CHK_RTN(device0);
    
    BHT_L1_A429_CHAN_NUM_CHK_RTN(chan_num);

	BHT_L1_SEM_TAKE(device0->mutex_sem, BHT_L1_WAIT_FOREVER, result, end);

	chan_num -= 1;

    BHT_L1_READ_MEM32(device0, BHT_A429_LOOP_EN, &value, 1, result, release_sem);

    if(BHT_L1_PARAM_OPT_GET == param_opt)
    {
        *able = (bht_L1_able_e)((value >> chan_num) & BIT0);
    }
	else
    {   
        value &= (~(bht_L0_u32)(BIT0 << chan_num));
        value |= (bht_L0_u32)(*able << chan_num);
        BHT_L1_WRITE_MEM32(device0, BHT_A429_LOOP_EN, &value, 1, result, release_sem);
    }

release_sem:
    BHT_L1_SEM_GIVE(device0->mutex_sem, result, end);
end:
    return result;
}        

bht_L0_u32 
bht_L1_a429_rx_chan_recv_mode(bht_L1_device_handle_t device,
        bht_L0_u32 chan_num,
        bht_L1_a429_recv_mode_e *recv_mode,
        bht_L1_param_opt_e param_opt)
{
    bht_L0_u32 value;
    bht_L0_u32 result = BHT_SUCCESS;
    bht_L0_device_t *device0 = (bht_L0_device_t*)device;
    bht_L1_a429_cb_t *cb = device0->private;

    BHT_L1_DEVICE_STATUS_CHK_RTN(device0);
    
	BHT_L1_A429_CHAN_NUM_CHK_RTN(chan_num);

	BHT_L1_SEM_TAKE(device0->mutex_sem, BHT_L1_WAIT_FOREVER, result, end);

    if(NULL == cb)
        goto release_sem;
	
    BHT_L1_A429_CFG(device0, BHT_L1_ENABLE, result, release_sem);

	chan_num += (BHT_L1_A429_CHAN_MAX - 1);

	BHT_L1_WRITE_MEM32(device0, BHT_A429_CHOOSE_CHANNEL_NUM, &chan_num, 1, result, cfg_disable);

    BHT_L1_READ_MEM32(device0, BHT_A429_CHANNEL_CFG, &value, 1, result, cfg_disable);

    if(BHT_L1_PARAM_OPT_GET == param_opt)
    {
        *recv_mode = (bht_L1_a429_recv_mode_e)((value >> 15) & BIT0);
    }
	else
    {   
        value &= (~(bht_L0_u32)BIT15);
        value |= (bht_L0_u32)(*recv_mode << 15);
        BHT_L1_WRITE_MEM32(device0, BHT_A429_CHANNEL_CFG, &value, 1, result, cfg_disable);
        cb->chan_data[chan_num - BHT_L1_A429_CHAN_MAX].recv_mode = *recv_mode;
    }
    
cfg_disable:
	BHT_L1_A429_CFG(device0, BHT_L1_DISABLE, result, release_sem);
release_sem:
	BHT_L1_SEM_GIVE(device0->mutex_sem, result, end);    
end:
	return result;
}

bht_L0_u32 
bht_L1_a429_rx_chan_int_threshold(bht_L1_device_handle_t device,
        bht_L0_u32 chan_num,
        bht_L0_u16 *threshold_count,
        bht_L0_u16 *threshold_time,
        bht_L1_param_opt_e param_opt)
{
    bht_L0_u32 value;
    bht_L0_u32 result = BHT_SUCCESS;
    bht_L0_device_t *device0 = (bht_L0_device_t*)device;

    BHT_L1_DEVICE_STATUS_CHK_RTN(device0);
    
	BHT_L1_A429_CHAN_NUM_CHK_RTN(chan_num);

	BHT_L1_SEM_TAKE(device0->mutex_sem, BHT_L1_WAIT_FOREVER, result, end);
	
    BHT_L1_A429_CFG(device0, BHT_L1_ENABLE, result, release_sem);

	chan_num += (16 - 1);

	BHT_L1_WRITE_MEM32(device0, BHT_A429_CHOOSE_CHANNEL_NUM, &chan_num, 1, result, cfg_disable);
        
    if(BHT_L1_PARAM_OPT_GET == param_opt)
    {
        BHT_L1_READ_MEM32(device0, BHT_A429_INT_THRESHOLD, &value, 1, result, cfg_disable);
        *threshold_count = value >> 16;
        *threshold_time = value % 0x10000;
    }
	else
    {   
        value = (*threshold_count << 16) | (*threshold_time);
        BHT_L1_WRITE_MEM32(device0, BHT_A429_INT_THRESHOLD, &value, 1, result, cfg_disable);        
    }
    
cfg_disable:
	BHT_L1_A429_CFG(device0, BHT_L1_DISABLE, result, release_sem);
release_sem:
	BHT_L1_SEM_GIVE(device0->mutex_sem, result, end);    
end:
	return result;
}

bht_L0_u32 
bht_L1_a429_rx_chan_filter(bht_L1_device_handle_t device,
        bht_L0_u32 chan_num,
        bht_L1_able_e *able,
        bht_L1_param_opt_e param_opt)
{
    bht_L0_u32 value;
    bht_L0_u32 result = BHT_SUCCESS;
    bht_L0_device_t *device0 = (bht_L0_device_t*)device;

    BHT_L1_DEVICE_STATUS_CHK_RTN(device0);
    
	BHT_L1_A429_CHAN_NUM_CHK_RTN(chan_num);

	BHT_L1_SEM_TAKE(device0->mutex_sem, BHT_L1_WAIT_FOREVER, result, end);
	
    BHT_L1_A429_CFG(device0, BHT_L1_ENABLE, result, release_sem);

	chan_num += (BHT_L1_A429_CHAN_MAX - 1);

	BHT_L1_WRITE_MEM32(device0, BHT_A429_CHOOSE_CHANNEL_NUM, &chan_num, 1, result, cfg_disable);

    BHT_L1_READ_MEM32(device0, BHT_A429_CHANNEL_CFG, &value, 1, result, cfg_disable);

    if(BHT_L1_PARAM_OPT_GET == param_opt)
    {
        *able = (value & BIT16) ? BHT_L1_DISABLE : BHT_L1_ENABLE;
    }
	else
    {   
//        DEBUG_PRINTF("1 value = 0x%08x\n", value);
        value &= (~(bht_L0_u32)BIT16);
        if(*able == BHT_L1_DISABLE)
            value |= BIT16;
//        DEBUG_PRINTF("2 value = 0x%08x\n", value);
        BHT_L1_WRITE_MEM32(device0, BHT_A429_CHANNEL_CFG, &value, 1, result, cfg_disable);
    }
cfg_disable:
	BHT_L1_A429_CFG(device0, BHT_L1_DISABLE, result, release_sem);
release_sem:
	BHT_L1_SEM_GIVE(device0->mutex_sem, result,end);
end:
	return result;
}

bht_L0_u32 
bht_L1_a429_rx_chan_filter_label(bht_L1_device_handle_t device,
        bht_L0_u32 chan_num,
        bht_L0_u8 label,
        bht_L0_u8 sdi,
        bht_L0_u8 ssm,
        bht_L1_a429_list_type_e *list_type,
        bht_L1_param_opt_e param_opt)
{
    bht_L0_u32 value;
    bht_L0_u32 result = BHT_SUCCESS;
    bht_L0_device_t *device0 = (bht_L0_device_t*)device;

    BHT_L1_DEVICE_STATUS_CHK_RTN(device0);
    
	BHT_L1_A429_CHAN_NUM_CHK_RTN(chan_num);

	BHT_L1_SEM_TAKE(device0->mutex_sem, BHT_L1_WAIT_FOREVER, result, end);    

    if(BHT_L1_PARAM_OPT_SET == param_opt)
    {
    	chan_num += (BHT_L1_A429_CHAN_MAX - 1);
        
        BHT_L1_A429_CFG(device0, BHT_L1_ENABLE, result, release_sem);

    	BHT_L1_WRITE_MEM32(device0, BHT_A429_CHOOSE_CHANNEL_NUM, &chan_num, 1, result, cfg_disable);

        value = (((*list_type) << 12) & BIT12) | ((ssm << 10) & (BIT10 | BIT11)) \
            | ((sdi << 8) & (BIT8 | BIT9)) | (label & 0xff);

        BHT_L1_WRITE_MEM32(device0, BHT_A429_LABLE_FILTER, &value, 1, result, cfg_disable);
cfg_disable:
        BHT_L1_A429_CFG(device0, BHT_L1_DISABLE, result, release_sem);
    }
	else
    {   
        chan_num -= 1;
        
        value = ((chan_num & 0xf) << 12) | ((ssm & 0x03) << 10) \
            | ((sdi & 0x03) << 8) | (label & 0xff);

        BHT_L1_WRITE_MEM32(device0, BHT_A429_FILTER_READ, &value, 1, result, release_sem);

        BHT_L1_READ_MEM32(device0, BHT_A429_FILTER_DATA, &value, 1, result, release_sem);
        
        *list_type = (bht_L1_a429_list_type_e)value;
    }

release_sem:
	BHT_L1_SEM_GIVE(device0->mutex_sem, result, end);
end:
	return result;
}

bht_L0_u32 
bht_L1_a429_rx_chan_recv(bht_L1_device_handle_t device, 
        bht_L0_u32 chan_num, 
        bht_L1_a429_rxp_t *rxp_buf, 
        bht_L0_u32 max_rxp, 
        bht_L0_u32 *rxp_num, 
        bht_L0_s32 timeout_ms)
{
    bht_L0_u32 result = BHT_SUCCESS;
    bht_L0_device_t *device0 = (bht_L0_device_t*)device;
    bht_L0_u32 cnt = 0;
    static bht_L0_s32 errn = 0;
    bht_L0_s32 ratio = 0;
    bht_L1_a429_cb_t *cb;

    BHT_L1_DEVICE_STATUS_CHK_RTN(device0);
    
    BHT_L1_A429_CHAN_NUM_CHK_RTN(chan_num);
    
    if((timeout_ms < BHT_L1_WAIT_FOREVER) || (max_rxp == 0))
        return BHT_ERR_BAD_INPUT;
    
    if(NULL == (cb = (bht_L1_a429_cb_t*) device0->private))
        return BHT_ERR_BAD_INPUT;

    if(BHT_L1_WAIT_FOREVER == timeout_ms)
        timeout_ms = 0x7FFFFFFF;
    
    chan_num -= 1;

    if(timeout_ms > 0)
        ratio = 1;

    do
    {
        if(BHT_SUCCESS == bht_L0_sem_take(cb->chan_data[chan_num].semc, ratio))
        {
            BHT_L1_SEM_TAKE(device0->mutex_sem, BHT_L1_WAIT_FOREVER, result, end);    
            if(0 < ring_buf_get(cb->chan_data[chan_num].rxp_ring, (unsigned char *)(rxp_buf + cnt)))
            {
                cnt++;
            }
            else
            {
                errn--;
            }
            BHT_L1_SEM_GIVE(device0->mutex_sem, result, end);    
        }
        //³¬Ê±
        else
        {
            timeout_ms -= ratio;
        }
		if(0 == timeout_ms)
			errn--;
    }while(timeout_ms && (cnt < max_rxp));

    *rxp_num = cnt;

    if(0 == cnt)
    {
//        bht_L1_a429_chan_dump(dev_id, chan_num, BHT_L1_CHAN_TYPE_RX);
        result = BHT_ERR_TIMEOUT;
    }

end:
    return result;
}
        
bht_L0_u32
bht_L1_a429_rx_chan_stat(bht_L0_device_t *device, 
        bht_L0_u32 chan_num,
        bht_L0_u32 *recv_num)
{
    bht_L1_a429_cb_t *cb;
    bht_L0_device_t *device0 = (bht_L0_device_t*)device;

    BHT_L1_DEVICE_STATUS_CHK_RTN(device0);
    
    BHT_L1_A429_CHAN_NUM_CHK_RTN(chan_num);

    if(NULL == (cb = (bht_L1_a429_cb_t*) device0->private))
        return BHT_ERR_BAD_INPUT;

    chan_num -= 1;
    
    *recv_num = cb->chan_data[chan_num].recv_data_cnt;

    return BHT_SUCCESS;
}

bht_L0_u32 
bht_L1_a429_tx_chan_send_mode(bht_L1_device_handle_t device,
        bht_L0_u32 chan_num,
        bht_L1_a429_send_mode_e *send_mode,
        bht_L1_param_opt_e param_opt)
{
    bht_L0_u32 value;
    bht_L0_u32 result = BHT_SUCCESS;
    bht_L0_device_t *device0 = (bht_L0_device_t*)device;

    BHT_L1_DEVICE_STATUS_CHK_RTN(device0);
    
	BHT_L1_A429_CHAN_NUM_CHK_RTN(chan_num);

	BHT_L1_SEM_TAKE(device0->mutex_sem, BHT_L1_WAIT_FOREVER, result, end);
	
    BHT_L1_A429_CFG(device0, BHT_L1_ENABLE, result, release_sem);

	chan_num -= 1;

	BHT_L1_WRITE_MEM32(device0, BHT_A429_CHOOSE_CHANNEL_NUM, &chan_num, 1, result, cfg_disable);

    BHT_L1_READ_MEM32(device0, BHT_A429_CHANNEL_CFG, &value, 1, result, cfg_disable);
//    DEBUG_PRINTF("1 value = 0x%08x\n", value);

    if(BHT_L1_PARAM_OPT_GET == param_opt)
    {
        *send_mode = (value & BIT12) ? BHT_L1_A429_SEND_MODE_PERIOD :\
            BHT_L1_A429_SEND_MODE_NONPERIOD;
    }
	else
    {
        if(((value & BIT12) && (BHT_L1_A429_SEND_MODE_NONPERIOD == *send_mode)) || \
            (!(value & BIT12) && (BHT_L1_A429_SEND_MODE_PERIOD == *send_mode)))
        {
            /*clear BIT12 */
            value &= ~(bht_L0_u32)BIT12;
            /*value bit12 */
            value |= (*send_mode == BHT_L1_A429_SEND_MODE_PERIOD) ? BIT12 : 0;

//            DEBUG_PRINTF("2 value = 0x%08x\n", value);
            BHT_L1_WRITE_MEM32(device0, BHT_A429_CHANNEL_CFG, &value, 1, result, cfg_disable);
        }
    }
    
cfg_disable:
	BHT_L1_A429_CFG(device0, BHT_L1_DISABLE, result, release_sem);
release_sem:
	BHT_L1_SEM_GIVE(device0->mutex_sem, result, end);    
end:
	return result;
}

bht_L0_u32 
bht_L1_a429_tx_chan_period(bht_L1_device_handle_t device,
        bht_L0_u32 chan_num,
        bht_L0_u32 *period,
        bht_L1_param_opt_e param_opt)
{
    bht_L0_u32 value;
    bht_L0_u32 result = BHT_SUCCESS;
    bht_L0_device_t *device0 = (bht_L0_device_t*)device;

    BHT_L1_DEVICE_STATUS_CHK_RTN(device0);
    
    BHT_L1_A429_CHAN_NUM_CHK_RTN(chan_num);

	BHT_L1_SEM_TAKE(device0->mutex_sem, BHT_L1_WAIT_FOREVER, result, end); 

    BHT_L1_A429_CFG(device0, BHT_L1_ENABLE, result, release_sem);

	chan_num -= 1;

	BHT_L1_WRITE_MEM32(device0, BHT_A429_CHOOSE_CHANNEL_NUM, &chan_num, 1, result, cfg_disable);

    BHT_L1_READ_MEM32(device0, BHT_A429_TX_CHAN_SEND_PERIOD, &value, 1, result, cfg_disable);
    
    if(BHT_L1_PARAM_OPT_GET == param_opt)
    {
        *period = value;
    }
    else
    {
        if(value != *period)
            BHT_L1_WRITE_MEM32(device0, BHT_A429_TX_CHAN_SEND_PERIOD, period, 1, result, cfg_disable);
    }
 
cfg_disable:
    BHT_L1_A429_CFG(device0, BHT_L1_DISABLE, result, release_sem);
release_sem:
	BHT_L1_SEM_GIVE(device0->mutex_sem, result, end);
end:
	return result;
}

bht_L0_u32 
bht_L1_a429_tx_chan_send(bht_L1_device_handle_t device,
        bht_L0_u32 chan_num,
        bht_L0_u32 data)
{
	bht_L0_u32 value;
    bht_L0_u32 result = BHT_SUCCESS;
    bht_L0_device_t *device0 = (bht_L0_device_t*)device;
    bht_L1_a429_cb_t *cb = device0->private;
    bht_L1_a429_send_stat_t * send_stat = NULL;
    bht_L0_u32 step = 0;

    BHT_L1_DEVICE_STATUS_CHK_RTN(device0);
    
    BHT_L1_A429_CHAN_NUM_CHK_RTN(chan_num);
    
	BHT_L1_SEM_TAKE(device0->mutex_sem, BHT_L1_WAIT_FOREVER, result, end); 

    chan_num -= 1;
    send_stat = (bht_L1_a429_send_stat_t*)(cb->send_stat + chan_num);

    BHT_L1_WRITE_MEM32(device0, BHT_A429_CHANNEL_NUM_SEND, &chan_num, 1, result, release_sem);

    BHT_L1_READ_MEM32(device0, BHT_A429_STATUS_CHANNEL_SEND, &value, 1, result, release_sem);
    if(value & BIT0)
	{
        result = BHT_ERR_BUFFER_FULL;
        goto release_sem;
	}

    BHT_L1_WRITE_MEM32(device0, BHT_A429_WORD_WR_CHANNEL, &data, 1, result, release_sem);
    send_stat->send_data_cnt++;
    
release_sem:
	BHT_L1_SEM_GIVE(device0->mutex_sem, result, end);
end:
	return result;
}

bht_L0_u32 
bht_L1_a429_tx_chan_update_data(bht_L1_device_handle_t device,
        bht_L0_u32 chan_num,
        bht_L0_u32 data)
{
    bht_L0_u32 result = BHT_SUCCESS;
    bht_L0_device_t *device0 = (bht_L0_device_t*)device;

    BHT_L1_DEVICE_STATUS_CHK_RTN(device0);
    
    BHT_L1_A429_CHAN_NUM_CHK_RTN(chan_num);

	BHT_L1_SEM_TAKE(device0->mutex_sem, BHT_L1_WAIT_FOREVER, result, end); 

    chan_num -= 1;

    BHT_L1_WRITE_MEM32(device0, BHT_A429_CHANNEL_NUM_SEND, &chan_num, 1, result, release_sem);

    BHT_L1_WRITE_MEM32(device0, BHT_A429_WORD_WR_CHANNEL, &data, 1, result, release_sem);
    
release_sem:
	BHT_L1_SEM_GIVE(device0->mutex_sem, result, end);
end:
	return result;
}

bht_L0_u32 
bht_L1_a429_tx_chan_start(bht_L1_device_handle_t device,
        bht_L0_u32 chan_num)
{
    bht_L0_u32 value;
    bht_L0_u32 result = BHT_SUCCESS;
    bht_L0_device_t *device0 = (bht_L0_device_t*)device;

    BHT_L1_DEVICE_STATUS_CHK_RTN(device0);
    
    BHT_L1_A429_CHAN_NUM_CHK_RTN(chan_num);

	BHT_L1_SEM_TAKE(device0->mutex_sem, BHT_L1_WAIT_FOREVER, result, end); 

    chan_num -= 1;

    BHT_L1_A429_CFG(device0, BHT_L1_ENABLE, result, release_sem);

    BHT_L1_WRITE_MEM32(device0, BHT_A429_CHOOSE_CHANNEL_NUM, &chan_num, 1, result, cfg_disable);

    value = BIT0;
    BHT_L1_WRITE_MEM32(device0, BHT_A429_TX_CHAN_SEND_PERIOD_CTRL, &value, 1, result, cfg_disable);

cfg_disable:
    BHT_L1_A429_CFG(device0, BHT_L1_DISABLE, result, release_sem);
release_sem:
	BHT_L1_SEM_GIVE(device0->mutex_sem, result, end);
end:
	return result;
}

bht_L0_u32 
bht_L1_a429_tx_chan_stop(bht_L1_device_handle_t device,
        bht_L0_u32 chan_num)
{
    bht_L0_u32 value;
    bht_L0_u32 result = BHT_SUCCESS;
    bht_L0_device_t *device0 = (bht_L0_device_t*)device;

    BHT_L1_DEVICE_STATUS_CHK_RTN(device0);
    BHT_L1_A429_CHAN_NUM_CHK_RTN(chan_num);

	BHT_L1_SEM_TAKE(device0->mutex_sem, BHT_L1_WAIT_FOREVER, result, end); 

    chan_num -= 1;

    BHT_L1_A429_CFG(device0, BHT_L1_ENABLE, result, release_sem);

    BHT_L1_WRITE_MEM32(device0, BHT_A429_CHOOSE_CHANNEL_NUM, &chan_num, 1, result, cfg_disable);

    value = 0;
    BHT_L1_WRITE_MEM32(device0, BHT_A429_TX_CHAN_SEND_PERIOD_CTRL, &value, 1, result, cfg_disable);

cfg_disable:
    BHT_L1_A429_CFG(device0, BHT_L1_DISABLE, result, release_sem);
release_sem:
	BHT_L1_SEM_GIVE(device0->mutex_sem, result, end);
end:
	return result;
}

bht_L0_u32 
bht_L1_a429_tx_chan_err_inject(bht_L1_device_handle_t device,
        bht_L0_u32 chan_num,
        bht_L1_a429_err_type_e *err_type,
        bht_L1_param_opt_e param_opt)
{
    bht_L0_u32 value, temp;
    bht_L0_u32 result = BHT_SUCCESS;
    bht_L0_device_t *device0 = (bht_L0_device_t*)device;
    bht_L1_a429_cfg_t cfg;

    BHT_L1_DEVICE_STATUS_CHK_RTN(device0);
	BHT_L1_A429_CHAN_NUM_CHK_RTN(chan_num);

	BHT_L1_SEM_TAKE(device0->mutex_sem, BHT_L1_WAIT_FOREVER, result, end);
	
    BHT_L1_A429_CFG(device0, BHT_L1_ENABLE, result, release_sem);

	chan_num -= 1;

	BHT_L1_WRITE_MEM32(device0, BHT_A429_CHOOSE_CHANNEL_NUM, &chan_num, 1, result, cfg_disable);
    BHT_L1_READ_MEM32(device0, BHT_A429_CHANNEL_CFG, (bht_L0_u32*)&cfg, 1, result, cfg_disable);
    DEBUG_PRINTF("cfg = 0x%08x, bit_count = %d, gap = %d, parity = %d\n", 
        cfg, cfg.bit_count_err, cfg.gap_err, cfg.parity_err);

    if(BHT_L1_PARAM_OPT_GET == param_opt)
	{        
	    if((BHT_L1_A429_WORD_BIT32 == cfg.bit_count_err)
            && (BHT_L1_A429_GAP_4BIT == cfg.gap_err)
            && (BHT_L1_DISABLE == cfg.parity_err))
        {
            *err_type = BHT_L1_A429_ERR_TYPE_NONE;
        }
        else if(BHT_L1_A429_WORD_BIT31 == cfg.bit_count_err)
        {
            *err_type = BHT_L1_A429_ERR_TYPE_31BIT;
        }
        else if(BHT_L1_A429_WORD_BIT33 == cfg.bit_count_err)
        {
            *err_type = BHT_L1_A429_ERR_TYPE_33BIT;
        }
        else if(BHT_L1_A429_GAP_2BIT == cfg.gap_err)
        {
            *err_type = BHT_L1_A429_ERR_TYPE_2GAP;
        }
        else if(BHT_L1_ENABLE == cfg.parity_err)
        {
            *err_type = BHT_L1_A429_ERR_TYPE_PARITY;
        }
	}
	else
	{
		switch(*err_type)
        {
            case BHT_L1_A429_ERR_TYPE_NONE:
                cfg.bit_count_err = BHT_L1_A429_WORD_BIT32;
                cfg.gap_err = BHT_L1_A429_GAP_4BIT;
                cfg.parity_err = BHT_L1_DISABLE;
                break;
            case BHT_L1_A429_ERR_TYPE_31BIT:
                cfg.bit_count_err = BHT_L1_A429_WORD_BIT31;
                cfg.gap_err = BHT_L1_A429_GAP_4BIT;
                cfg.parity_err = BHT_L1_DISABLE;
                break;
            case BHT_L1_A429_ERR_TYPE_33BIT:
                cfg.bit_count_err = BHT_L1_A429_WORD_BIT33;
                cfg.gap_err = BHT_L1_A429_GAP_4BIT;
                cfg.parity_err = BHT_L1_DISABLE;                
                break;
            case BHT_L1_A429_ERR_TYPE_2GAP:
                cfg.bit_count_err = BHT_L1_A429_WORD_BIT32;
                cfg.gap_err = BHT_L1_A429_GAP_2BIT;
                cfg.parity_err = BHT_L1_DISABLE;                
                break;
            case BHT_L1_A429_ERR_TYPE_PARITY:
                cfg.bit_count_err = BHT_L1_A429_WORD_BIT32;
                cfg.gap_err = BHT_L1_A429_GAP_4BIT;
                cfg.parity_err = BHT_L1_ENABLE;
                break;
            default:
                result =  BHT_ERR_BAD_INPUT;
                goto cfg_disable;
        } 
        DEBUG_PRINTF("cfg = 0x%08x, bit_count = %d, gap = %d, parity = %d\n", 
            cfg, cfg.bit_count_err, cfg.gap_err, cfg.parity_err);
		BHT_L1_WRITE_MEM32(device0, BHT_A429_CHANNEL_CFG, (bht_L0_u32*)&cfg, 1, result, cfg_disable);
	}
cfg_disable:
	BHT_L1_A429_CFG(device0, BHT_L1_DISABLE, result, release_sem);
release_sem:
	BHT_L1_SEM_GIVE(device0->mutex_sem, result, end);
end:
	return result;
}

bht_L0_u32
bht_L1_a429_chan_dump(bht_L0_device_t *device, 
        bht_L0_u32 chan_num, 
        bht_L1_chan_type_e type)
{
    bht_L0_u32 value, loop;
    bht_L0_u32 result = BHT_SUCCESS;
    bht_L1_a429_mib_data_t mib_data;
    bht_L0_device_t *device0 = (bht_L0_device_t*)device;
    bht_L1_a429_cb_t *cb = device0->private;
    bht_L1_a429_send_stat_t *send_stat = NULL;

    BHT_L1_DEVICE_STATUS_CHK_RTN(device0);
    BHT_L1_A429_CHAN_NUM_CHK_RTN(chan_num);

    BHT_L1_READ_MEM32(device0, BHT_A429_LOOP_EN, &loop, 1, result, end); 
    if(BHT_SUCCESS != bht_L1_a429_chan_get_mib(device, chan_num, type, &mib_data))
        goto end;
        
    if(BHT_L1_CHAN_TYPE_RX == type)
    {
        printf("RX chan[%d] dump\n", chan_num);

        chan_num += (BHT_L1_A429_CHAN_MAX - 1);
        printf("loop_en        0x0040 [0x%08x]\n", loop);
        BHT_L1_WRITE_MEM32(device0, BHT_A429_CHOOSE_CHANNEL_NUM, &chan_num, 1, result, end);
        BHT_L1_READ_MEM32(device0, BHT_A429_CHANNEL_CFG, &value, 1, result, end);
        printf("channel_cfg    0x1008 [0x%08x]\n", value);
        BHT_L1_READ_MEM32(device0, BHT_A429_BAUD_RATE_SET, &value, 1, result, end);
        printf("baud           0x100C [0x%08x]\n", value);
        BHT_L1_READ_MEM32(device0, BHT_A429_INT_THRESHOLD, &value, 1, result, end);
        printf("int_threshold  0x1010 [0x%08x]\n", value);        
        printf("mib cnt               [0x%08x]\n", mib_data.cnt);
        printf("mib err_cnt           [0x%08x]\n", mib_data.err_cnt);
        if(BHT_SUCCESS != bht_L1_a429_rx_chan_stat(device, chan_num, &value))
            goto end;
        printf("recv data count       [0x%08x]\n", value);
        BHT_L1_READ_MEM32(device0, BHT_A429_INTR_STATE, &value, 1, result, end);
        printf("pci int stat   0x0028 [0x%08x]\n", value);
        BHT_L1_READ_MEM32(device0, BHT_A429_INTR_CHANNEL_VECTOR, &value, 1, result, end);
        printf("chan int vecto 0x002C [0x%08x]\n", value);
        BHT_L1_WRITE_MEM32(device0, BHT_A429_CHANNEL_NUM_RECV, &chan_num, 1, result, end);
        BHT_L1_READ_MEM32(device0, BHT_A429_STATUS_CHANNEL_RECV, &value, 1, result, end);
        printf("chan status    0x2104 [0x%08x]\n", value);
    }
    else
    {        
        printf("TX chan[%d] dump\n", chan_num);
        chan_num -= 1;
        send_stat = (bht_L1_a429_send_stat_t *)(cb->send_stat + chan_num);
        printf("loop_en       0x0040 [0x%08x]\n", loop);
        BHT_L1_WRITE_MEM32(device0, BHT_A429_CHOOSE_CHANNEL_NUM, &chan_num, 1, result, end);
        BHT_L1_READ_MEM32(device0, BHT_A429_CHANNEL_CFG, &value, 1, result, end);
        printf("channel_cfg   0x1008 [0x%08x]\n", value);
        BHT_L1_READ_MEM32(device0, BHT_A429_BAUD_RATE_SET, &value, 1, result, end);
        printf("baud          0x100C [0x%08x]\n", value);
//        printf("period        0x1300 [0x%08x]\n", device_param->tx_chan_param[chan_num - 1].period);
        printf("mib cnt              [0x%08x]\n", mib_data.cnt);
        printf("mib err_cnt          [0x%08x]\n", mib_data.err_cnt);

        /* check channel status stat: full or not full */
        BHT_L1_WRITE_MEM32(device0, BHT_A429_CHANNEL_NUM_SEND, &chan_num, 1, result, end);
        BHT_L1_READ_MEM32(device0, BHT_A429_STATUS_CHANNEL_SEND, &value, 1, result, end);
        printf("channel send status  [0x%08x]\n", value);
        printf("channel send data cnt[0x%08x]\n", send_stat->send_data_cnt);
    }

    return BHT_SUCCESS;
end:
    printf("%s error\n", __FUNCTION__);
	return result;
}
