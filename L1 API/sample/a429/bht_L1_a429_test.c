#include <bht_L0.h>
#include <bht_L1.h>
#include <bht_L1_a429.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <bht_L0_plx9056.h>
#ifdef WINDOWS_OPS
#include <windows.h>
#endif

#define DEVID BHT_DEVID_BACKPLANETYPE_PCI | BHT_DEVID_BOARDTYPE_PMCA429 | BHT_DEVID_BOARDNUM_01 

/* the number of a429 data words send on tx channel */
#define A429_DATAWORD_TEST_NUM      (2*1024)
/* the first channel number to be test */
#define A429_TEST_FIRST_CHAN_NUM		(5)
/* the toal number of channel to be test */
#define A429_TEST_TOTAL_CHAN_NUM		(1)

#define BAUD	BHT_L1_A429_BAUD_200K
#define A429_RECV_MODE_SAMPLE
#ifndef A429_RECV_MODE_SAMPLE
#define THRESHOLD_COUNT 1022
#define THRESHOLD_TIME	5000
#endif

static bht_L0_u32 test_tx_buf[A429_DATAWORD_TEST_NUM];
static bht_L0_u32 test_rx_buf[16][A429_DATAWORD_TEST_NUM];

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

static bht_L0_u32 recv_err_flag = 0;

static void testbuf_rand(bht_L0_u32 *testbuf, bht_L0_u32 size)
{
    bht_L0_u32 idx;
    
    srand(time(NULL));

    for(idx = 0; idx < size; idx++)
        //testbuf[idx] = 0x7FFFFFFF & rand();
		testbuf[idx] = 0x7FFFFFFF & idx;
}

#ifdef WINDOWS_OPS
static DWORD WINAPI a429_channel_send_thread(const void * arg)
#endif
{
	const bht_L0_u32 dev_id = ((a429_send_thread_arg_t *)arg)->dev_id;
	const bht_L0_u32 chan_num = ((a429_send_thread_arg_t *)arg)->chan_num;
	const bht_L0_u32 data_word_num = ((a429_send_thread_arg_t *)arg)->data_word_num;
	bht_L0_u32 idx = 0;
	bht_L0_u32 result = BHT_SUCCESS;
	bht_L0_u32 value;
    bht_L1_a429_mib_data_t mib_data;

	printf("tx channel[%d] start\n", chan_num);

	bht_L0_msleep(2000);

	while(idx < A429_DATAWORD_TEST_NUM)
	{
		if(recv_err_flag == 1)
			break;
		if(BHT_SUCCESS != (result = bht_L1_a429_tx_chan_send(dev_id, chan_num, BHT_L1_A429_OPT_RANDOM_SEND, test_tx_buf[idx])))
		{
//			printf("tx_chan_send failed[idx = %d], error info : %s, result = %d\n", \
//				idx, bht_L1_error_to_string(result), result);
			bht_L0_msleep(1);
		}
        else
        {
//			printf("tx channel[%d] send index %d data %08x\n", chan_num, idx, test_tx_buf[idx]);
            idx++;
			if(idx %1024 == 0)
				printf("tx channel[%d] send %d/%d\n", chan_num, idx/1024, A429_DATAWORD_TEST_NUM/1024);
        }
	}

    bht_L0_msleep(5000);
    printf("tx channel[%d] send complete\n", chan_num);

    bht_L1_a429_chan_dump(dev_id, chan_num, BHT_L1_CHAN_TYPE_TX);
	bht_L1_a429_chan_dump(dev_id, chan_num, BHT_L1_CHAN_TYPE_RX);

#ifdef WINDOWS_OPS
	return 0;
#endif
}
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
	//system("pause");
    
	while(tot_num < A429_DATAWORD_TEST_NUM)
	{
	    if(BHT_SUCCESS != bht_L1_a429_rx_chan_recv(dev_id, chan_num, &rxp, 1, &num, BHT_L1_WAIT_FOREVER))
        {
            printf("rx channel[%d] recv err\n", chan_num);
            break;
        }
		if(test_tx_buf[tot_num] != (0x7FFFFFFF & rxp.data))
        {
			recv_err_flag = 1;
			printf("rx channel[%d] recv the index[%x] not match, t[%x]:r[%x]!!!", chan_num, tot_num,test_tx_buf[tot_num], 0x7FFFFFFF & rxp.data);
            break;
        }
        test_rx_buf[chan_num - 1][tot_num++] = 0x7FFFFFFF & rxp.data;
		if(tot_num %1024 == 0)
			printf("rx channel[%d] recv %d/%d\n", chan_num, tot_num/1024, A429_DATAWORD_TEST_NUM/1024);
//		printf("rx channel[%d] recv index %d data %08x\n", chan_num, tot_num, test_rx_buf[chan_num - 1][tot_num - 1]);
	}

    for(idx = 0; idx < tot_num; idx++)
    {
        if(test_tx_buf[idx] != test_rx_buf[chan_num - 1][idx])
        {
			printf("rx channel[%d] recv total[%d] bytes, the index[%x] not match, t[%x]:r[%x]!!!\n", chan_num, tot_num, idx, test_tx_buf[idx], test_rx_buf[chan_num - 1][idx]);
            return 0;
        }
    }
    if(A429_DATAWORD_TEST_NUM == tot_num)
        printf("rx channel[%d] test suc\n", chan_num);
    else
        printf("rx channel[%d] recv total[%d] bytes, do not recv complete[%d]!!!\n", chan_num, tot_num, A429_DATAWORD_TEST_NUM);

#ifdef WINDOWS_OPS
	return 0;
#endif
}

int main (void)
{
    int result;
	bht_L0_u32 value, idx;
    bht_L0_u32 chan_num;
	bht_L1_a429_chan_comm_param_t comm_param;
	bht_L1_a429_rx_chan_gather_param_t gather_param;
    bht_L1_a429_tx_chan_inject_param_t inject_param;
	a429_send_thread_arg_t arg_tx[A429_TEST_TOTAL_CHAN_NUM] = {0};
	a429_recv_thread_arg_t arg_rx[A429_TEST_TOTAL_CHAN_NUM] = {0};
#ifdef WINDOWS_OPS
	HANDLE hThread[32] = {0};
#endif

    /* generate rand number */
    testbuf_rand(test_tx_buf, sizeof(test_tx_buf)/ sizeof(test_tx_buf[0]));

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

    /* transmit channel 1-16 and receive channel 1-16 loopback test */
    for(idx = 0; idx < A429_TEST_TOTAL_CHAN_NUM; idx++)
    {
		chan_num = A429_TEST_FIRST_CHAN_NUM + idx;
        
        //common param config
    	comm_param.work_mode = BHT_L1_A429_CHAN_WORK_MODE_ENABLE;
    	comm_param.baud = BAUD;
    	comm_param.par = BHT_L1_A429_PARITY_ODD;
    	
    	if(BHT_SUCCESS != (result = bht_L1_a429_tx_chan_comm_param(DEVID, chan_num, &comm_param, BHT_L1_PARAM_OPT_SET)))
    	{
    		printf("tx_chan_comm_param set failed, error info: %s, result = %d\n", \
    			bht_L1_error_to_string(result), result);
    		goto test_error;
    	}
 
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

        //loopback enable	
    	if(BHT_SUCCESS != (result = bht_L1_a429_tx_chan_loop(DEVID, chan_num, BHT_L1_OPT_ENABLE)))
    	{
    		printf("tx_chan_loop enable failed, error info: %s, result = %d\n", \
    			bht_L1_error_to_string(result), result);
    		goto test_error;
    	}
    }

#ifdef WINDOWS_OPS0
	for(idx = 0; idx < A429_TEST_TOTAL_CHAN_NUM; idx++)
    {   
		//create send thread
		arg_tx[idx].dev_id = DEVID;
		arg_tx[idx].chan_num = A429_TEST_FIRST_CHAN_NUM + idx;
    	if(NULL == (hThread[2 * idx] = CreateThread(NULL, 20000* 1024, (LPTHREAD_START_ROUTINE)a429_channel_send_thread, (LPVOID)&arg_tx[idx], 0, NULL)))
    	{
    		printf("CreateThread failed\n");
    		goto test_error;
    	}

        arg_rx[idx].dev_id = DEVID;
        arg_rx[idx].chan_num = idx + A429_TEST_FIRST_CHAN_NUM;
    	//create receive thread
        if(NULL == (hThread[2 * idx + 1] = CreateThread(NULL, 20000* 1024, (LPTHREAD_START_ROUTINE)a429_channel_recv_thread, (LPVOID)&arg_rx[idx], 0, NULL)))
    	{
    		printf("CreateThread failed\n");
    		goto test_error;
    	}
    }

	//Wait until all threads have terminated.
	WaitForMultipleObjects(2 * A429_TEST_TOTAL_CHAN_NUM, hThread, TRUE, INFINITE);

    //Close all thread handle supon completion.
//    for(idx = 0; idx < (2*A429_TEST_TOTAL_CHAN_NUM); idx++)
//        CloseHandle(hThread[idx]);
#else
#if 1
	{
		bht_L0_u32 num;
		bht_L1_a429_rxp_t rxp;

		if(BHT_SUCCESS != (result = bht_L1_a429_tx_chan_send(DEVID, A429_TEST_FIRST_CHAN_NUM, BHT_L1_A429_OPT_RANDOM_SEND, 0x1234)))
		{
			printf("tx_chan_send failed[idx = %d], error info : %s, result = %d\n", \
				idx, bht_L1_error_to_string(result), result);
		}

		if(BHT_SUCCESS != bht_L1_a429_rx_chan_recv(DEVID, A429_TEST_FIRST_CHAN_NUM, &rxp, 1, &num, 1000))
		{
			printf("rx channel[%d] recv err\n", A429_TEST_FIRST_CHAN_NUM);
		}
	}
#endif
#endif

test_error:
#if 1
    if(BHT_SUCCESS != (result = bht_L0_read_setupmem32(DEVID, PLX9056_INTCSR, &value, 1)))
        return result;
	printf("value = %08x\n", value);
    
	value = 0;
	if(BHT_SUCCESS != (result = bht_L0_write_setupmem32(DEVID, PLX9056_INTCSR, &value, 1)))
        return result;
	
	if(BHT_SUCCESS != (result = bht_L0_read_setupmem32(DEVID, PLX9056_INTCSR, &value, 1)))
        return result;
	printf("value = %08x\n", value);

	bht_L1_a429_chan_dump(DEVID, A429_TEST_FIRST_CHAN_NUM, BHT_L1_CHAN_TYPE_RX);
	bht_L1_a429_chan_dump(DEVID, A429_TEST_FIRST_CHAN_NUM, BHT_L1_CHAN_TYPE_TX);


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
