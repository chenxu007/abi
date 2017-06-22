#include <bht_L0.h>
#include <bht_L1.h>
#include <bht_L1_a429.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#ifdef WINDOWS_OPS
#include <windows.h>
#endif


#define DEVID BHT_DEVID_BACKPLANETYPE_PCI | BHT_DEVID_BOARDTYPE_PMCA429 | BHT_DEVID_BOARDNUM_01 
#define A429_DATA_MASK      0x7FFFFFFF

#define A429_DATAWORD_TEST_NUM      (2000)
#define A429_TEST_CHAN_NUM			(1)
#define A429_CUR_TEST_CHAN_NUM		(2)

#define BAUD	BHT_L1_A429_BAUD_12_5K
#define A429_RECV_MODE_SAMPLE
#ifndef A429_RECV_MODE_SAMPLE
#define THRESHOLD_COUNT 1022
#define THRESHOLD_TIME	5000
#endif

typedef struct
{
	bht_L0_u32 dev_id;
	bht_L0_u32 chan_num;
	bht_L0_u32 data_word_num;
}a429_send_thread_arg_t;

typedef struct
{
	bht_L0_u32 dev_id;
	bht_L0_u32 chan_num;
}a429_recv_thread_arg_t;

static bht_L0_u32 app_recv_data_count = 0;


#ifdef WINDOWS_OPS
static DWORD WINAPI a429_channel_recv_thread(const void * arg)
#endif
{
	const bht_L0_u32 dev_id = ((a429_recv_thread_arg_t *)arg)->dev_id;
    const bht_L0_u32 chan_num = ((a429_recv_thread_arg_t *)arg)->chan_num;
	bht_L0_u32 idx = 0;   
	bht_L0_u32 result = BHT_SUCCESS;
	bht_L0_u32 value;
    bht_L0_u32 intr_state;
    bht_L1_a429_rxp_t rxp;
    bht_L0_u32 num;
    bht_L0_u32 tot_num = 0;

    printf("rx channel[%d] start\n", chan_num);
    
	while(1)
	{
	    if(BHT_SUCCESS != bht_L1_a429_rx_chan_recv(dev_id, chan_num, &rxp, 1, &num, BHT_L1_WAIT_FOREVER))
        {
            printf("rx channel[%d] recv err\n", chan_num);
            break;
        }
        app_recv_data_count++;
//        printf("chan[%d] recv timestamp:%08x data:%08x\n", chan_num, rxp.timestamp, rxp.data & A429_DATA_MASK);
	}

    printf("%s end\n", __FUNCTION__);

#ifdef WINDOWS_OPS
	return 0;
#endif
}

int main111 (void)
{
    int result;
	bht_L0_u32 value, idx;
    bht_L0_u32 chan_num;
    bht_L0_u32 thread_cnt = 0;
	bht_L1_a429_chan_comm_param_t comm_param;
	bht_L1_a429_rx_chan_gather_param_t gather_param;
	a429_send_thread_arg_t arg_tx[A429_TEST_CHAN_NUM] = {0};
	a429_recv_thread_arg_t arg_rx[A429_TEST_CHAN_NUM] = {0};
    bht_L1_a429_mib_data_t mib_data;
#ifdef WINDOWS_OPS
	HANDLE hThread[32] = {0};
#endif

	/* probe device */
    if(BHT_SUCCESS == (result = bht_L1_device_probe(DEVID)))
        printf("device initialized succ\n");
    else
	{
		printf("device probe failed, error info: %s, result = %d\n", \
			bht_L1_error_to_string(result), result);
		goto test_error;
	}

	bht_L0_msleep(10);

	/* device deault initialize */
	if(BHT_SUCCESS == (result = bht_L1_a429_default_init(DEVID)))
        printf("device default initialized succ\n");
    else
	{
		printf("device default initialized failed, error info: %s, result = %d\n", \
			bht_L1_error_to_string(result), result);
		goto test_error;
	}

    //create recv thread
#ifdef WINDOWS_OPS
    arg_rx[thread_cnt].dev_id = DEVID;
	arg_rx[thread_cnt].chan_num = A429_CUR_TEST_CHAN_NUM;    

    if(NULL == (hThread[thread_cnt] = CreateThread(NULL, 20000* 1024, (LPTHREAD_START_ROUTINE)a429_channel_recv_thread, (LPVOID)&arg_rx[thread_cnt], 0, NULL)))
	{
		printf("CreateThread failed\n");
		goto test_error;
	}
    thread_cnt++;
#else
#error "operate system not support!!!"
#endif

    //common param config
    chan_num = A429_CUR_TEST_CHAN_NUM;

	comm_param.work_mode = BHT_L1_A429_CHAN_WORK_MODE_ENABLE;
	comm_param.baud = BAUD;
	comm_param.par = BHT_L1_A429_PARITY_EVEN;
	
	//the same common param with receive channel
	if(BHT_SUCCESS != (result = bht_L1_a429_rx_chan_comm_param(DEVID, chan_num, &comm_param, BHT_L1_PARAM_OPT_SET)))
	{
		printf("rx_chan_comm_param set failed, error info: %s, result = %d\n", \
			bht_L1_error_to_string(result), result);
		goto test_error;
	}

	//receive channel gather param config 
	gather_param.gather_enable = BHT_L1_ENABLE;
#ifdef A429_RECV_MODE_SAMPLE
	gather_param.recv_mode = BHT_L1_A429_RECV_MODE_SAMPLE;
	gather_param.threshold_count = 0;
	gather_param.threshold_time = 0;
#else
	gather_param.recv_mode = BHT_L1_A429_RECV_MODE_LIST;
	gather_param.threshold_count = THRESHOLD_COUNT;
	gather_param.threshold_time = THRESHOLD_TIME;
#endif
	if(BHT_SUCCESS != (result = bht_L1_a429_rx_chan_gather_param(DEVID, chan_num, &gather_param, BHT_L1_PARAM_OPT_SET)))
	{
		printf("gather_param set failed, error info: %s, result = %d\n", \
			bht_L1_error_to_string(result), result);
		goto test_error;
	}

	if(BHT_SUCCESS != (result = bht_L1_a429_tx_chan_comm_param(DEVID, chan_num, &comm_param, BHT_L1_PARAM_OPT_SET)))
	{
		printf("tx_chan_comm_param set failed, error info: %s, result = %d\n", \
			bht_L1_error_to_string(result), result);
		goto test_error;
	}

    //loopback enable	
	if(BHT_SUCCESS != (result = bht_L1_a429_tx_chan_loop(DEVID, chan_num, BHT_L1_OPT_ENABLE)))
	{
		printf("tx_chan_loop enable failed, error info: %s, result = %d\n", \
			bht_L1_error_to_string(result), result);
		goto test_error;
	}
    
    //period config
    value = 3;
    if(BHT_SUCCESS != (result = bht_L1_a429_tx_chan_period_param(DEVID, chan_num, &value, BHT_L1_PARAM_OPT_SET)))
    {
        printf("tx_chan_period_param set failed, error info: %s, result = %d\n", \
			bht_L1_error_to_string(result), result);
		goto test_error;
    }

    //period send update
    if(BHT_SUCCESS != (result = bht_L1_a429_tx_chan_send(DEVID, chan_num, BHT_L1_A429_OPT_PERIOD_SEND_UPDATE, 1)))
    {
        printf("bht_L1_a429_tx_chan_send update failed, error info: %s, result = %d\n", \
			bht_L1_error_to_string(result), result);
		goto test_error;
    }

    //period send start
    if(BHT_SUCCESS != (result = bht_L1_a429_tx_chan_send(DEVID, chan_num, BHT_L1_A429_OPT_PERIOD_SEND_START, 0)))
    {
        printf("bht_L1_a429_tx_chan_send start failed, error info: %s, result = %d\n", \
			bht_L1_error_to_string(result), result);
		goto test_error;
    }
	else
    {   
		printf("period send start\n");
    }

    //delay 10 seconds
    bht_L0_msleep(10000);

    //period send stop
    if(BHT_SUCCESS != (result = bht_L1_a429_tx_chan_send(DEVID, chan_num, BHT_L1_A429_OPT_PERIOD_SEND_STOP, 0)))
    {
        printf("bht_L1_a429_tx_chan_send start failed, error info: %s, result = %d\n", \
			bht_L1_error_to_string(result), result);
		goto test_error;
    }
	else
		printf("period send stop\n");

    //delay 1 second
	bht_L0_msleep(2000);

    //check the RX/TX statistics data
    bht_L1_a429_chan_dump(DEVID, chan_num, BHT_L1_CHAN_TYPE_TX);
	bht_L1_a429_chan_dump(DEVID, chan_num, BHT_L1_CHAN_TYPE_RX);
    printf("application RX count [0x%08x]\n", app_recv_data_count);

#ifdef WINDOWS_OPS		
	//Wait until all threads have terminated.
	WaitForMultipleObjects(thread_cnt, hThread, TRUE, INFINITE);

    //Close all thread handle supon completion.
    for(idx = 0; idx < thread_cnt; idx++)
        CloseHandle(hThread[idx]);

#endif

test_error:
#if 0
    if(BHT_SUCCESS != bht_L0_detach_inthandler(DEVID))
        printf("detach_inthandler fail\n");
	if(BHT_SUCCESS == (result = bht_L1_device_remove(DEVID)))
        printf("device remove succ\n");
	else
		printf("device remove failed, error info : %s\n", \
			bht_L1_error_to_string(result));
#endif
	system("pause");

	return 0;
}
