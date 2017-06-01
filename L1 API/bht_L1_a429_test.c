#include <bht_L0.h>
#include <bht_L1.h>
#include <bht_L1_a429.h>
#include <stdio.h>
#ifdef WINDOWS_OPS
#include <windows.h>
#endif

#define DEVID BHT_DEVID_BACKPLANETYPE_PCI | BHT_DEVID_BOARDTYPE_PMCA429 | BHT_DEVID_BOARDNUM_01 

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


#ifdef WINDOWS_OPS
static DWORD WINAPI a429_channel_send_thread(const void * arg)
#endif
{
	const bht_L0_u32 dev_id = ((a429_send_thread_arg_t *)arg)->dev_id;
	const bht_L0_u32 chan_num = ((a429_send_thread_arg_t *)arg)->chan_num;
	const bht_L0_u32 data_word_num = ((a429_send_thread_arg_t *)arg)->data_word_num;
	bht_L0_u32 idx = 0;
	bht_L0_u32 result = BHT_SUCCESS;

	printf("%s dev_id = 0x%08x, chan_num = %d\n", __FUNCTION__, dev_id, chan_num);

	while(idx ++ < data_word_num)
	{
		if(BHT_SUCCESS != (result = bht_L1_a429_tx_chan_send(dev_id, chan_num, idx)))
		{
			printf("tx_chan_send failed, error info : %s, result = %d\n", \
				bht_L1_error_to_string(result), result);
			break;
		}
		else
			printf("%s send data 0x%08x\n", __FUNCTION__, idx);
		
		bht_L0_msleep(1000);
	}

#ifdef WINDOWS_OPS
	return 0;
#endif
}
#ifdef WINDOWS_OPS
static DWORD WINAPI a429_channel_recv_thread(const void * arg)
#endif
{
	const bht_L0_u32 dev_id = ((a429_send_thread_arg_t *)arg)->dev_id;
	bht_L0_u32 idx = 0;
	bht_L0_u32 result = BHT_SUCCESS;
	bht_L0_u32 value;

	while(1)
	{
		// check receive interrupt
		if(BHT_SUCCESS != (result = bht_L0_read_mem32(dev_id, BHT_A429_INTR_CHANNEL_VECTOR, &value, 1)))
		{
			printf("%s read channel vector failed, error info: %s ,result = %d\n", \
				__FUNCTION__, bht_L1_error_to_string(result), result);
			break;
		}

		if(0 != value)
		{
			for(idx = 16; idx < 32; idx++)
			{
				if(value & (0x01 << idx))
				{
					bht_L0_u32 chan_status;
					bht_L1_a429_rxp_t rxp;
					//choose channel idx
					if(BHT_SUCCESS != (result = bht_L0_write_mem32(dev_id, BHT_A429_CHANNEL_NUM_RECV, &idx, 1)))
					{
						printf("%s write channel choose failed, error info: %s ,result = %d\n", \
							__FUNCTION__, bht_L1_error_to_string(result), result);
						break;
					}
					//channel staus
//check_status:		if(BHT_SUCCESS != (result = bht_L0_read_mem32(dev_id, BHT_A429_STATUS_CHANNEL_RECV, &chan_status, 1)))
//					{
//						printf("%s read channel status failed, error info: %s ,result = %d\n", \
//							__FUNCTION__, bht_L1_error_to_string(result), result);
//						break;
//					}
					//if not empty, read it
//					if(chan_status & BIT0)
					{
						memset((void*)&rxp, 0x00, sizeof(rxp));
						bht_L0_read_mem32(dev_id, BHT_A429_WORD_RD_CHANNEL_FREASH, &rxp.data, 1);
//						bht_L0_read_mem32(dev_id, BHT_A429_WORD_TIMR_RD_CHANNEL, &rxp.timestamp, 1);
						printf("%s recv data : %08x , timestamp : %08x\n", __FUNCTION__, rxp.data, rxp.timestamp);

//						goto check_status;
					}

				}
			}
		}
		bht_L0_msleep(1000);
	}

#ifdef WINDOWS_OPS
	return 0;
#endif
}

int main (void)
{
    int result;
	bht_L0_u32 value, idx;
	bht_L1_a429_chan_comm_param_t comm_param;
	bht_L1_a429_rx_chan_gather_param_t gather_param;
	a429_send_thread_arg_t arg_tx = {DEVID, 1, 3};
	a429_recv_thread_arg_t arg_rx = {DEVID, 1};
#ifdef WINDOWS_OPS
	DWORD dwThreadId[2] = {0};
	HANDLE hThread[2] = {0};
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
	if(BHT_SUCCESS != (result = bht_L0_read_mem32(DEVID, BHT_A429_INTR_CHANNEL_VECTOR, &value, 1)))
	{
		printf("%s read channel vector failed, error info: %s ,result = %d\n", \
			__FUNCTION__, bht_L1_error_to_string(result), result);
		goto test_error;
	}
    printf("channel interrupt status 0x%08x\n", value);

	/* device deault initialize */
	if(BHT_SUCCESS == (result = bht_L1_a429_default_init(DEVID)))
        printf("device default initialized succ\n");
    else
	{
		printf("device default initialized failed, error info: %s, result = %d\n", \
			bht_L1_error_to_string(result), result);
		goto test_error;
	}

	bht_L0_msleep(10);
	if(BHT_SUCCESS != (result = bht_L0_read_mem32(DEVID, BHT_A429_INTR_CHANNEL_VECTOR, &value, 1)))
	{
		printf("%s read channel vector failed, error info: %s ,result = %d\n", \
			__FUNCTION__, bht_L1_error_to_string(result), result);
		goto test_error;
	}
    printf("channel interrupt status 0x%08x\n", value);

    /* transmit channel 1 and receive channel 1 loopback test */
	comm_param.work_mode = BHT_L1_A429_CHAN_WORK_MODE_ENABLE;
	comm_param.baud = BHT_L1_A429_BAUD_12_5K;
	comm_param.par = BHT_L1_A429_PARITY_ODD;
	
	if(BHT_SUCCESS != (result = bht_L1_a429_tx_chan_comm_param(DEVID, 1, &comm_param, BHT_L1_PARAM_OPT_SET)))
	{
		printf("tx_chan_comm_param set failed, error info: %s, result = %d\n", \
			bht_L1_error_to_string(result), result);
		goto test_error;
	}

	bht_L0_msleep(10);
	if(BHT_SUCCESS != (result = bht_L0_read_mem32(DEVID, BHT_A429_INTR_CHANNEL_VECTOR, &value, 1)))
	{
		printf("%s read channel vector failed, error info: %s ,result = %d\n", \
			__FUNCTION__, bht_L1_error_to_string(result), result);
		goto test_error;
	}
    printf("channel interrupt status 0x%08x\n", value);

	//the same common param with receive channel 1
	if(BHT_SUCCESS != (result = bht_L1_a429_rx_chan_comm_param(DEVID, 1, &comm_param, BHT_L1_PARAM_OPT_SET)))
	{
		printf("rx_chan_comm_param set failed, error info: %s, result = %d\n", \
			bht_L1_error_to_string(result), result);
		goto test_error;
	}

	bht_L0_msleep(10);
	if(BHT_SUCCESS != (result = bht_L0_read_mem32(DEVID, BHT_A429_INTR_CHANNEL_VECTOR, &value, 1)))
	{
		printf("%s read channel vector failed, error info: %s ,result = %d\n", \
			__FUNCTION__, bht_L1_error_to_string(result), result);
		goto test_error;
	}
    printf("channel interrupt status 0x%08x\n", value);

	//receive channel 1 gather param config 
	gather_param.gather_enable = BHT_L1_ENABLE;
	gather_param.recv_mode = BHT_L1_A429_RECV_MODE_SAMPLE;
	gather_param.threshold_count = 0;
	gather_param.threshold_time = 0;
	if(BHT_SUCCESS != (result = bht_L1_a429_rx_chan_gather_param(DEVID, 1, &gather_param, BHT_L1_PARAM_OPT_SET)))
	{
		printf("gather_param set failed, error info: %s, result = %d\n", \
			bht_L1_error_to_string(result), result);
		goto test_error;
	}

	bht_L0_msleep(10);
	if(BHT_SUCCESS != (result = bht_L0_read_mem32(DEVID, BHT_A429_INTR_CHANNEL_VECTOR, &value, 1)))
	{
		printf("%s read channel vector failed, error info: %s ,result = %d\n", \
			__FUNCTION__, bht_L1_error_to_string(result), result);
		goto test_error;
	}
    printf("channel interrupt status 0x%08x\n", value);

    //loopback enable	
	if(BHT_SUCCESS != (result = bht_L1_a429_tx_chan_loop(DEVID, 1, BHT_L1_OPT_ENABLE)))
	{
		printf("tx_chan_loop enable failed, error info: %s, result = %d\n", \
			bht_L1_error_to_string(result), result);
		goto test_error;
	}

#ifdef WINDOWS_OPS	
	//create send thread	
	if(NULL == (hThread[1] = CreateThread(NULL, 10000* 1024, (LPTHREAD_START_ROUTINE)a429_channel_send_thread, (LPVOID)&arg_tx, 0, NULL)))
	{
		printf("CreateThread failed\n");
		goto test_error;
	}

	//create receive thread
    if(NULL == (hThread[0] = CreateThread(NULL, 10000* 1024, (LPTHREAD_START_ROUTINE)a429_channel_recv_thread, &arg_rx, 0, &dwThreadId[0])))
	{
		printf("CreateThread failed\n");
		goto test_error;
	}

	//Wait until all threads have terminated.
	WaitForMultipleObjects(2,hThread,TRUE,INFINITE);

    //Close all thread handle supon completion.
    CloseHandle(hThread[0]);
	CloseHandle(hThread[1]);

#endif

test_error:
	if(BHT_SUCCESS == (result = bht_L1_device_remove(DEVID)))
        printf("device remove succ\n");
	else
		printf("device remove failed, error info : %s\n", \
			bht_L1_error_to_string(result));

	system("pause");

	return 0;
}
