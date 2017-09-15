#include <samples/shared/diag_lib.h>
#include <wdc_lib.h>
#include <bht_L0.h>
#include <bht_L1.h>
#include <bht_L1_a429.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <bht_L0_plx9056.h>
#include <assert.h>

#ifdef WINDOWS_OPS
#include <windows.h>
#endif

#define DEVID BHT_DEVID_BACKPLANETYPE_PCI | BHT_DEVID_BOARDTYPE_PMCA429 | BHT_DEVID_BOARDNUM_01 
#define ARRAY_SIZE(a) (sizeof (a) / sizeof ((a)[0]))

/* the number of a429 data words send on tx channel */
#define A429_DATAWORD_TEST_NUM      (1*1024*1024)
/* the first channel number to be test */
#define A429_TEST_FIRST_CHAN_NUM		(5)
/* the toal number of channel to be test */
#define A429_TEST_TOTAL_CHAN_NUM		(1)

#define TX_PARITY BHT_L1_A429_PARITY_ODD
#define RX_PARITY BHT_L1_A429_PARITY_ODD

#define BAUD	BHT_L1_A429_BAUD_12_5K
//#define A429_RECV_MODE_SAMPLE
#ifndef A429_RECV_MODE_SAMPLE
#define RECV_MODE   BHT_L1_A429_RECV_MODE_LIST
#define THRESHOLD_COUNT 0x200
#define THRESHOLD_TIME	0x100
#else
#define RECV_MODE   BHT_L1_A429_RECV_MODE_SAMPLE
#define THRESHOLD_COUNT 0
#define THRESHOLD_TIME	0
#endif

#define PARAM_MODE_STATIC 0
//#define PARAM_MODE_MANUAL 1

//channel define 
#define A429_TX_CHAN1
#define A429_TX_CHAN2
#define A429_TX_CHAN3
#define A429_TX_CHAN4
#define A429_TX_CHAN5
#define A429_TX_CHAN6
#define A429_TX_CHAN7
#define A429_TX_CHAN8
#define A429_TX_CHAN9
#define A429_TX_CHAN10
#define A429_TX_CHAN11
#define A429_TX_CHAN12
#define A429_TX_CHAN13
#define A429_TX_CHAN14
#define A429_TX_CHAN15
#define A429_TX_CHAN16

#define A429_RX_CHAN1
#define A429_RX_CHAN2
#define A429_RX_CHAN3
#define A429_RX_CHAN4
#define A429_RX_CHAN5
#define A429_RX_CHAN6
#define A429_RX_CHAN7
#define A429_RX_CHAN8
#define A429_RX_CHAN9
#define A429_RX_CHAN10
#define A429_RX_CHAN11
#define A429_RX_CHAN12
#define A429_RX_CHAN13
#define A429_RX_CHAN14
#define A429_RX_CHAN15
#define A429_RX_CHAN16

typedef struct
{
	bht_L0_device_t *device;
	bht_L0_u32 chan_num;
	bht_L0_u32 data_word_num;
}a429_thread_arg_t;

typedef struct
{
    bht_L0_u32 chan_num;
    bht_L1_a429_chan_comm_param_t comm_param;
    bht_L0_u32 loop_en;
    bht_L1_a429_slope_e slope;
    bht_L1_a429_tx_chan_inject_param_t inject_param;
    bht_L0_u32 period;
}a429_tx_chan_param_t;

typedef struct
{
    bht_L0_u32 chan_num;
    bht_L1_a429_chan_comm_param_t comm_param;
    bht_L1_a429_rx_chan_gather_param_t gather_param;
}a429_rx_chan_param_t;

typedef enum
{
    INPUT_DATA_FORMAT_DEC,
    INPUT_DATA_FORMAT_HEX
}data_input_format_e;

#ifdef PARAM_MODE_STATIC
const static a429_tx_chan_param_t a429_test_tx_chan_param[] = 
{
#ifdef A429_TX_CHAN1
{1, {BHT_L1_A429_CHAN_WORK_MODE_OPEN, BAUD, TX_PARITY}, BHT_L1_DISABLE, 
    BHT_L1_A429_SLOPE_1_5_US, {BHT_L1_A429_WORD_BIT32, BHT_L1_A429_GAP_4BIT, BHT_L1_DISABLE}, 0},
#endif
#ifdef A429_TX_CHAN2
{2, {BHT_L1_A429_CHAN_WORK_MODE_OPEN, BAUD, TX_PARITY}, BHT_L1_DISABLE, 
    BHT_L1_A429_SLOPE_1_5_US, {BHT_L1_A429_WORD_BIT32, BHT_L1_A429_GAP_4BIT, BHT_L1_DISABLE}, 0},
#endif
#ifdef A429_TX_CHAN3
{3, {BHT_L1_A429_CHAN_WORK_MODE_OPEN, BAUD, TX_PARITY}, BHT_L1_DISABLE, 
    BHT_L1_A429_SLOPE_1_5_US, {BHT_L1_A429_WORD_BIT32, BHT_L1_A429_GAP_4BIT, BHT_L1_DISABLE}, 0},
#endif
#ifdef A429_TX_CHAN4
{4, {BHT_L1_A429_CHAN_WORK_MODE_OPEN, BAUD, TX_PARITY}, BHT_L1_DISABLE, 
    BHT_L1_A429_SLOPE_1_5_US, {BHT_L1_A429_WORD_BIT32, BHT_L1_A429_GAP_4BIT, BHT_L1_DISABLE}, 0},
#endif
#ifdef A429_TX_CHAN5
{5, {BHT_L1_A429_CHAN_WORK_MODE_OPEN, BAUD, TX_PARITY}, BHT_L1_DISABLE, 
    BHT_L1_A429_SLOPE_1_5_US, {BHT_L1_A429_WORD_BIT32, BHT_L1_A429_GAP_4BIT, BHT_L1_DISABLE}, 0},
#endif
#ifdef A429_TX_CHAN6
{6, {BHT_L1_A429_CHAN_WORK_MODE_OPEN, BAUD, TX_PARITY}, BHT_L1_DISABLE, 
    BHT_L1_A429_SLOPE_1_5_US, {BHT_L1_A429_WORD_BIT32, BHT_L1_A429_GAP_4BIT, BHT_L1_DISABLE}, 0},
#endif
#ifdef A429_TX_CHAN7
{7, {BHT_L1_A429_CHAN_WORK_MODE_OPEN, BAUD, TX_PARITY}, BHT_L1_DISABLE, 
    BHT_L1_A429_SLOPE_1_5_US, {BHT_L1_A429_WORD_BIT32, BHT_L1_A429_GAP_4BIT, BHT_L1_DISABLE}, 0},
#endif
#ifdef A429_TX_CHAN8
{8, {BHT_L1_A429_CHAN_WORK_MODE_OPEN, BAUD, TX_PARITY}, BHT_L1_DISABLE, 
    BHT_L1_A429_SLOPE_1_5_US, {BHT_L1_A429_WORD_BIT32, BHT_L1_A429_GAP_4BIT, BHT_L1_DISABLE}, 0},
#endif
#ifdef A429_TX_CHAN9
{9, {BHT_L1_A429_CHAN_WORK_MODE_OPEN, BAUD, TX_PARITY}, BHT_L1_DISABLE, 
    BHT_L1_A429_SLOPE_1_5_US, {BHT_L1_A429_WORD_BIT32, BHT_L1_A429_GAP_4BIT, BHT_L1_DISABLE}, 0},
#endif
#ifdef A429_TX_CHAN10
{10, {BHT_L1_A429_CHAN_WORK_MODE_OPEN, BAUD, TX_PARITY}, BHT_L1_DISABLE, 
    BHT_L1_A429_SLOPE_1_5_US, {BHT_L1_A429_WORD_BIT32, BHT_L1_A429_GAP_4BIT, BHT_L1_DISABLE}, 0},
#endif
#ifdef A429_TX_CHAN11
{11, {BHT_L1_A429_CHAN_WORK_MODE_OPEN, BAUD, TX_PARITY}, BHT_L1_DISABLE, 
    BHT_L1_A429_SLOPE_1_5_US, {BHT_L1_A429_WORD_BIT32, BHT_L1_A429_GAP_4BIT, BHT_L1_DISABLE}, 0},
#endif
#ifdef A429_TX_CHAN12
{12, {BHT_L1_A429_CHAN_WORK_MODE_OPEN, BAUD, TX_PARITY}, BHT_L1_DISABLE, 
    BHT_L1_A429_SLOPE_1_5_US, {BHT_L1_A429_WORD_BIT32, BHT_L1_A429_GAP_4BIT, BHT_L1_DISABLE}, 0},
#endif
#ifdef A429_TX_CHAN13
{13, {BHT_L1_A429_CHAN_WORK_MODE_OPEN, BAUD, TX_PARITY}, BHT_L1_DISABLE, 
    BHT_L1_A429_SLOPE_1_5_US, {BHT_L1_A429_WORD_BIT32, BHT_L1_A429_GAP_4BIT, BHT_L1_DISABLE}, 0},
#endif
#ifdef A429_TX_CHAN14
{14, {BHT_L1_A429_CHAN_WORK_MODE_OPEN, BAUD, TX_PARITY}, BHT_L1_DISABLE, 
    BHT_L1_A429_SLOPE_1_5_US, {BHT_L1_A429_WORD_BIT32, BHT_L1_A429_GAP_4BIT, BHT_L1_DISABLE}, 0},
#endif
#ifdef A429_TX_CHAN15
{15, {BHT_L1_A429_CHAN_WORK_MODE_OPEN, BAUD, TX_PARITY}, BHT_L1_DISABLE, 
    BHT_L1_A429_SLOPE_1_5_US, {BHT_L1_A429_WORD_BIT32, BHT_L1_A429_GAP_4BIT, BHT_L1_DISABLE}, 0},
#endif
#ifdef A429_TX_CHAN16
{16, {BHT_L1_A429_CHAN_WORK_MODE_OPEN, BAUD, TX_PARITY}, BHT_L1_DISABLE, 
    BHT_L1_A429_SLOPE_1_5_US, {BHT_L1_A429_WORD_BIT32, BHT_L1_A429_GAP_4BIT, BHT_L1_DISABLE}, 0},
#endif
};

const static a429_rx_chan_param_t a429_test_rx_chan_param[] = 
{
#ifdef A429_RX_CHAN1
{1, {BHT_L1_A429_CHAN_WORK_MODE_OPEN, BAUD, RX_PARITY}, {BHT_L1_ENABLE,
    RECV_MODE, THRESHOLD_COUNT, THRESHOLD_TIME}},
#endif
#ifdef A429_RX_CHAN2
{2, {BHT_L1_A429_CHAN_WORK_MODE_OPEN, BAUD, RX_PARITY}, {BHT_L1_ENABLE,
    RECV_MODE, THRESHOLD_COUNT, THRESHOLD_TIME}},
#endif
#ifdef A429_RX_CHAN3
{3, {BHT_L1_A429_CHAN_WORK_MODE_OPEN, BAUD, RX_PARITY}, {BHT_L1_ENABLE,
    RECV_MODE, THRESHOLD_COUNT, THRESHOLD_TIME}},
#endif
#ifdef A429_RX_CHAN4
{4, {BHT_L1_A429_CHAN_WORK_MODE_OPEN, BAUD, RX_PARITY}, {BHT_L1_ENABLE,
    RECV_MODE, THRESHOLD_COUNT, THRESHOLD_TIME}},
#endif
#ifdef A429_RX_CHAN5
{5, {BHT_L1_A429_CHAN_WORK_MODE_OPEN, BAUD, RX_PARITY}, {BHT_L1_ENABLE,
    RECV_MODE, THRESHOLD_COUNT, THRESHOLD_TIME}},
#endif
#ifdef A429_RX_CHAN6
{6, {BHT_L1_A429_CHAN_WORK_MODE_OPEN, BAUD, RX_PARITY}, {BHT_L1_ENABLE,
    RECV_MODE, THRESHOLD_COUNT, THRESHOLD_TIME}},
#endif
#ifdef A429_RX_CHAN7
{7, {BHT_L1_A429_CHAN_WORK_MODE_OPEN, BAUD, RX_PARITY}, {BHT_L1_ENABLE,
    RECV_MODE, THRESHOLD_COUNT, THRESHOLD_TIME}},
#endif
#ifdef A429_RX_CHAN8
{8, {BHT_L1_A429_CHAN_WORK_MODE_OPEN, BAUD, RX_PARITY}, {BHT_L1_ENABLE,
    RECV_MODE, THRESHOLD_COUNT, THRESHOLD_TIME}},
#endif
#ifdef A429_RX_CHAN9
{9, {BHT_L1_A429_CHAN_WORK_MODE_OPEN, BAUD, RX_PARITY}, {BHT_L1_ENABLE,
    RECV_MODE, THRESHOLD_COUNT, THRESHOLD_TIME}},
#endif
#ifdef A429_RX_CHAN10
{10, {BHT_L1_A429_CHAN_WORK_MODE_OPEN, BAUD, RX_PARITY}, {BHT_L1_ENABLE,
    RECV_MODE, THRESHOLD_COUNT, THRESHOLD_TIME}},
#endif
#ifdef A429_RX_CHAN11
{11, {BHT_L1_A429_CHAN_WORK_MODE_OPEN, BAUD, RX_PARITY}, {BHT_L1_ENABLE,
    RECV_MODE, THRESHOLD_COUNT, THRESHOLD_TIME}},
#endif
#ifdef A429_RX_CHAN12
{12, {BHT_L1_A429_CHAN_WORK_MODE_OPEN, BAUD, RX_PARITY}, {BHT_L1_ENABLE,
    RECV_MODE, THRESHOLD_COUNT, THRESHOLD_TIME}},
#endif
#ifdef A429_RX_CHAN13
{13, {BHT_L1_A429_CHAN_WORK_MODE_OPEN, BAUD, RX_PARITY}, {BHT_L1_ENABLE,
    RECV_MODE, THRESHOLD_COUNT, THRESHOLD_TIME}},
#endif
#ifdef A429_RX_CHAN14
{14, {BHT_L1_A429_CHAN_WORK_MODE_OPEN, BAUD, RX_PARITY}, {BHT_L1_ENABLE,
    RECV_MODE, THRESHOLD_COUNT, THRESHOLD_TIME}},
#endif
#ifdef A429_RX_CHAN15
{15, {BHT_L1_A429_CHAN_WORK_MODE_OPEN, BAUD, RX_PARITY}, {BHT_L1_ENABLE,
    RECV_MODE, THRESHOLD_COUNT, THRESHOLD_TIME}},
#endif
#ifdef A429_RX_CHAN16
{16, {BHT_L1_A429_CHAN_WORK_MODE_OPEN, BAUD, RX_PARITY}, {BHT_L1_ENABLE,
    RECV_MODE, THRESHOLD_COUNT, THRESHOLD_TIME}},
#endif
};
#endif

static bht_L0_u32 test_tx_buf[A429_DATAWORD_TEST_NUM];
static bht_L0_u32 test_rx_buf[16][A429_DATAWORD_TEST_NUM];
#ifdef WINDOWS_OPS
static HANDLE hThread[32] = {0};
#endif
static bht_L0_u32 thread_count = 0;
static a429_thread_arg_t arg_tx[16 + 1] = {0};
static a429_thread_arg_t arg_rx[16 + 1] = {0};

static bht_L0_u32 recv_err_flag = 0;

static void testbuf_rand(bht_L0_u32 *testbuf, bht_L0_u32 size)
{
    bht_L0_u32 idx;
    
    srand(time(NULL));

    for(idx = 0; idx < size; idx++)
        testbuf[idx] = 0x7FFFFFFF & rand();
		//testbuf[idx] = 0x7FFFFFFF & idx;
}

static DIAG_INPUT_RESULT DIAG_GetMenuOption(PDWORD pdwOption, DWORD dwMax)
{
    static CHAR sInput[256];
    int iRet;

    if (!pdwOption)
    {
        //DIAG_ERR("DIAG_GetMenuOption: Error - NULL option pointer\n");
        return DIAG_INPUT_FAIL;
    }

    printf("Enter option: ");
    
    fgets(sInput, sizeof(sInput), stdin);

    iRet = sscanf(sInput, "%ld", pdwOption);

    if (iRet < 1)
    {
        printf("Invalid option\n");
        return DIAG_INPUT_FAIL;
    }
    
    if (DIAG_EXIT_MENU == *pdwOption)
        return DIAG_INPUT_SUCCESS;

    if (!dwMax)
        return DIAG_INPUT_SUCCESS;
    
    if (*pdwOption > dwMax)
    {
        printf("Invalid option: Option must be %s%ld, or %d to exit\n",
            (1 == dwMax) ? "" : "between 1 - ", dwMax, DIAG_EXIT_MENU);
        return DIAG_INPUT_FAIL;
    }
        
    return DIAG_INPUT_SUCCESS;
}

static bht_L0_s32 DIAG_GetNumber(char * name, data_input_format_e format, bht_L0_u32 min, bht_L0_u32 max)
{
    bht_L0_s32 n = 0x7FFFFFFF;
    bht_L0_u8 sInput[256];
    bht_L0_s32 iRet;
    
    if(NULL == name)
        assert(0);

    do
    {
        if(min < max) 
            printf("Please input the %s (%d - %d) : %s", name, min, max, (INPUT_DATA_FORMAT_DEC == format) ? "" : "0x");
        else
            printf("Please input the %s : %s", name, (INPUT_DATA_FORMAT_DEC == format) ? "" : "0x");

        fgets(sInput, sizeof(sInput), stdin);

        if(INPUT_DATA_FORMAT_DEC == format)
            iRet = sscanf(sInput, "%d", &n);
        else if(INPUT_DATA_FORMAT_HEX == format)
            iRet = sscanf(sInput, "%x", &n);

        if((min < max) && ((n > max) || (n < min)))
            continue;

        break;
    }while(1);

    return n; 
}

static bht_L0_u32 input_channel_num(bht_L1_chan_type_e chtyp)
{
    bht_L0_u8 sInput[256];
    bht_L0_s32 iRet;
    bht_L0_s32 chan_num;
    
    
    do
    {
        printf("Please input the %schannel number : ", (BHT_L1_CHAN_TYPE_RX == chtyp) ? "RX " : \
            ((BHT_L1_CHAN_TYPE_TX == chtyp) ? "TX " : ""));

        fgets(sInput, sizeof(sInput), stdin);

        iRet = sscanf(sInput, "%d", &chan_num);
    
        if((iRet < 1) || (chan_num < 1) || (chan_num > 16))
        {
            printf("channel number invalid, need between 1 and 16\n");
            continue;
        }

        break;
    }while(1);

    return chan_num;
}

static bht_L1_chan_type_e input_chan_type(void)
{
    bht_L0_u32 option;
    bht_L1_chan_type_e type;
    
    do
    {
        printf("1.RX channel\n");
        printf("2.TX channel\n");
        if (DIAG_INPUT_FAIL == DIAG_GetMenuOption(&option, 2))
        {
            continue;
        }
        if(1 == option)
            type = BHT_L1_CHAN_TYPE_RX;
        else
            type = BHT_L1_CHAN_TYPE_TX;
        
        break;
    }while(1);

    return type;
}

static DWORD WINAPI a429_channel_send_thread(const void * arg)
{
	const bht_L0_device_t *device = ((a429_thread_arg_t *)arg)->dev_id;
	const bht_L0_u32 chan_num = ((a429_thread_arg_t *)arg)->chan_num;
	const bht_L0_u32 data_word_num = ((a429_thread_arg_t *)arg)->data_word_num;
	bht_L0_u32 idx = 0;
	bht_L0_u32 result = BHT_SUCCESS;
	bht_L0_u32 value;
    bht_L1_a429_mib_data_t mib_data;

	printf("tx channel[%d] start\n", chan_num);

	bht_L0_msleep(20);

	while(idx < data_word_num)
	{
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
				printf("tx channel[%d] send %d/%d\n", chan_num, idx/1024, data_word_num/1024);
        }
	}

    bht_L0_msleep(1000);
    printf("tx channel[%d] send complete\n", chan_num);

	return 0;
}

static DWORD WINAPI a429_channel_recv_thread(const void * arg)
{
	const bht_L0_device_t *device = ((a429_thread_arg_t *)arg)->dev_id;
    const bht_L0_u32 chan_num = ((a429_thread_arg_t *)arg)->chan_num;
    const bht_L0_u32 data_word_num = ((a429_thread_arg_t *)arg)->data_word_num;
	bht_L0_u32 idx = 0;
	bht_L0_u32 result = BHT_SUCCESS;
	bht_L0_u32 value;
    bht_L0_u32 intr_state;
    bht_L1_a429_rxp_t rxp;
    bht_L0_u32 num;
    bht_L0_u32 tot_num = 0;

    printf("rx channel[%d] start\n", chan_num);
	//system("pause");
    
	while(tot_num < data_word_num)
	{
	    if(BHT_SUCCESS != bht_L1_a429_rx_chan_recv(dev_id, chan_num, &rxp, 1, &num, BHT_L1_WAIT_FOREVER))
        {
            printf("rx channel[%d] recv err\n", chan_num);
            break;
        }

        test_rx_buf[chan_num - 1][tot_num++] = 0x7FFFFFFF & rxp.data;
		if(tot_num %1024 == 0)
			printf("rx channel[%d] recv %d/%d\n", chan_num, tot_num/1024, data_word_num/1024);
		printf("rx channel[%d] recv index %d data %08x\n", chan_num, tot_num, test_rx_buf[chan_num - 1][tot_num - 1]);
	}

    for(idx = 0; idx < tot_num; idx++)
    {
        if(test_tx_buf[idx] != test_rx_buf[chan_num - 1][idx])
        {
			printf("rx channel[%d] recv total[%d] bytes, the index[0x%x] not match, t[0x%x]:r[0x%x]!!!\n", chan_num, tot_num, idx, test_tx_buf[idx], test_rx_buf[chan_num - 1][idx]);
            return 0;
        }
    }
    if(data_word_num == tot_num)
        printf("rx channel[%d] test suc\n", chan_num);
    else
        printf("rx channel[%d] recv total[%d] bytes, do not recv complete[%d]!!!\n", chan_num, tot_num, A429_DATAWORD_TEST_NUM);

	return 0;
}

static void tx_thread_creat(void)
{
    DWORD option;
    bht_L0_u32 chan_num;
    bht_L0_u8 sInput[256];
    bht_L0_s32 iRet;

    chan_num = input_channel_num(BHT_L1_CHAN_TYPE_TX);

    do
    {
        printf("Please input the number of 429 words you want to send :");
        fgets(sInput, sizeof(sInput), stdin);

        iRet = sscanf(sInput, "%d", &option);
        if(option > A429_DATAWORD_TEST_NUM)
        {
            printf("The number of 429 words can't be above %d\n", A429_DATAWORD_TEST_NUM);
            continue;
        }
        arg_tx[chan_num].data_word_num = option;
		break;
    }while(1);

    arg_tx[chan_num].dev_id = DEVID;
    arg_tx[chan_num].chan_num = chan_num;

    hThread[thread_count] = CreateThread(NULL, 100 * 1024, (LPTHREAD_START_ROUTINE)a429_channel_send_thread, (LPVOID)&arg_tx[chan_num], 0, NULL);

    if(NULL == hThread[thread_count])
	{
		printf("CreateThread TX channel[%d] send thread failed\n", arg_tx[chan_num].chan_num);
	}
    else
        thread_count++;
    
}

static void tx_thread_creat_all(void)
{
    DWORD option;
    bht_L0_u32 idx;
    bht_L0_u32 data_word_num;

    data_word_num = DIAG_GetNumber("number of 429 words you want to send", INPUT_DATA_FORMAT_DEC, 0, A429_DATAWORD_TEST_NUM);

    for(idx = 0; idx < 16; idx++)
    {
        arg_tx[idx].dev_id = DEVID;
        arg_tx[idx].data_word_num = data_word_num;
        arg_tx[idx].chan_num = idx + 1;

        hThread[thread_count] = CreateThread(NULL, 100 * 1024, (LPTHREAD_START_ROUTINE)a429_channel_send_thread, (LPVOID)&arg_tx[idx], 0, NULL);

        if(NULL == hThread[thread_count])
    	{
    		printf("CreateThread TX channel[%d] send thread failed\n", arg_tx[idx].chan_num);
            break;
    	}
        else
            thread_count++;
    }
}

static void rx_thread_creat(void)
{
    DWORD option;
    bht_L0_u32 chan_num;

    chan_num = input_channel_num(BHT_L1_CHAN_TYPE_RX);
    arg_rx[chan_num].chan_num = chan_num;    

    arg_rx[chan_num].data_word_num = DIAG_GetNumber("number of 429 words you want to recv", INPUT_DATA_FORMAT_DEC, 0, A429_DATAWORD_TEST_NUM);

    arg_rx[chan_num].dev_id = DEVID;

    hThread[thread_count] = CreateThread(NULL, 100 * 1024, (LPTHREAD_START_ROUTINE)a429_channel_recv_thread, (LPVOID)&arg_rx[chan_num], 0, NULL);

    if(NULL == hThread[thread_count])
	{
		printf("CreateThread RX channel[%d] recv thread failed\n", arg_tx[chan_num].chan_num);
	}
    else
        thread_count++;
    
}

static void rx_thread_creat_all(void)
{
    DWORD option;
    bht_L0_u32 idx;
    bht_L0_u32 data_word_num;

    data_word_num = DIAG_GetNumber("number of 429 words you want to recv", INPUT_DATA_FORMAT_DEC, 0, A429_DATAWORD_TEST_NUM);

    for(idx = 0; idx < 16; idx++)
    {
        arg_rx[idx].dev_id = DEVID;
        arg_rx[idx].data_word_num = data_word_num;
        arg_rx[idx].chan_num = idx + 1;

        hThread[thread_count] = CreateThread(NULL, 100 * 1024, (LPTHREAD_START_ROUTINE)a429_channel_recv_thread, (LPVOID)&arg_rx[idx], 0, NULL);

        if(NULL == hThread[thread_count])
    	{
    		printf("CreateThread RX channel[%d] recv thread failed\n", arg_tx[idx].chan_num);
            break;
    	}
        else
            thread_count++;
    }
    
}

static void irigb_test(bht_L0_device_t *device)
{
    bht_L0_u32 option, option1;
    bht_L1_a429_irigb_mode_e mode;
    bht_L1_a429_irigb_time_t irigb_time;
    
    printf("irigb test menu\n");
    printf("-------------------------\n");

    do
    {
        printf("1. irigb mode config\n");
        printf("2. irigb time config\n");
        printf("%d. EXIT\n", DIAG_EXIT_MENU);

        if (DIAG_INPUT_FAIL == DIAG_GetMenuOption(&option,2))
            continue;

        switch(option)
        {
            case 1:
                do
                {
                    printf("1. Slave\n");
                    printf("2. Master\n");
                    printf("%d. EXIT\n", DIAG_EXIT_MENU);

                    if (DIAG_INPUT_FAIL == DIAG_GetMenuOption(&option1,2))
                        continue;

                    if(option1 != DIAG_EXIT_MENU)
                    {
                        if(1 == option1)
                            mode = BHT_L1_A429_IRIGB_MODE_SLAVE;
                        else if(2 == option1)
                            mode = BHT_L1_A429_IRIGB_MODE_MASTER;

                        bht_L1_a429_irigb_mode_cfg(dev_id, mode);
                    }
                }while(option1 != DIAG_EXIT_MENU);
                
                break;
            case 2:
                do
                {
                    printf("1. Query time\n");
                    printf("2. Set time\n");
                    printf("%d. EXIT\n", DIAG_EXIT_MENU);

                    if (DIAG_INPUT_FAIL == DIAG_GetMenuOption(&option1,2))
                        continue;

                    if(option1 != DIAG_EXIT_MENU)
                    {
                        if(1 == option1)
                        {
                            bht_L1_a429_irigb_time(dev_id, &irigb_time, BHT_L1_PARAM_OPT_GET);
                            printf("day  : %d\nhour : %d\nmin  : %d\nsec  : %d\nms   : %d\nus   : %d\n",
                                irigb_time.tm.tm_day, irigb_time.tm.tm_hour, irigb_time.tm.tm_min, 
                                irigb_time.tm.tm_sec, irigb_time.tm.tm_ms, irigb_time.tm.tm_us);
                        }
                        else if(2 == option1)
                        {
                            irigb_time.tm.tm_day = DIAG_GetNumber("day", INPUT_DATA_FORMAT_DEC, 0, 365);
                            irigb_time.tm.tm_hour = DIAG_GetNumber("hour", INPUT_DATA_FORMAT_DEC, 0, 23);
                            irigb_time.tm.tm_min = DIAG_GetNumber("min", INPUT_DATA_FORMAT_DEC, 0, 59);
                            irigb_time.tm.tm_sec = DIAG_GetNumber("sec", INPUT_DATA_FORMAT_DEC, 0, 59);
                            irigb_time.tm.tm_ms = DIAG_GetNumber("ms", INPUT_DATA_FORMAT_DEC, 0, 999);
                            irigb_time.tm.tm_us = DIAG_GetNumber("us", INPUT_DATA_FORMAT_DEC, 0, 999);
                            bht_L1_a429_irigb_time(dev_id, &irigb_time, BHT_L1_PARAM_OPT_SET);
                        }
                    }
                }while(option1 != DIAG_EXIT_MENU);
                break;
        }
        
    }while(option != DIAG_EXIT_MENU);
}

static void mib_dump(bht_L0_device_t *device)
{
    DWORD option;
    bht_L0_u32 chan_num;
    bht_L0_u32 result;
    bht_L0_u32 recv_num;
    bht_L1_chan_type_e type;
    bht_L1_a429_mib_data_t mib_data;
    

    printf("mib dump menu\n");
    printf("--------------------\n");
    
    do
    {
        printf("1. dump the mib data of all channel\n");
        printf("2. dump driver recieve status on all RX channel\n");
        printf("3. dump the mib data and register config\n");
        printf("%d. EXIT\n", DIAG_EXIT_MENU);

        if (DIAG_INPUT_FAIL == DIAG_GetMenuOption(&option,3))
        {
            continue;
        }

        switch(option)
        {
        case 1:
            type = input_chan_type();
            printf("-------------------------------------------------------------------------------\n");
            printf("|  %-4d   |  %-4d   |  %-4d   |  %-4d   |  %-4d   |  %-4d   |  %-4d   |  %-4d  \n",\
                1,2,3,4,5,6,7,8);
            printf("-------------------------------------------------------------------------------\n");
            for(chan_num = 1; chan_num <= 16; chan_num++)
            {
                if(9 == chan_num)
                {
                    printf("\n-------------------------------------------------------------------------------\n");
                    printf("|  %-4d   |  %-4d   |  %-4d   |  %-4d   |  %-4d   |  %-4d   |  %-4d   |  %-4d  \n",\
                        9,10,11,12,13,14,15,16);
                    printf("-------------------------------------------------------------------------------\n");
                }
                
                if(BHT_L1_CHAN_TYPE_RX == type)
                {
                    if(BHT_SUCCESS != (result = bht_L1_a429_rx_chan_mib_get(dev_id, chan_num, &mib_data)))
                    {
                        printf("rx mib get failed, %s (result = %d)", bht_L1_error_to_string(result), result);
                        break;
                    }
                }
                else
                {
                    if(BHT_SUCCESS != (result = bht_L1_a429_tx_chan_mib_get(dev_id, chan_num, &mib_data)))
                    {
                        printf("tx mib get failed, %s (result = %d)", bht_L1_error_to_string(result), result);
                        break;
                    }
                }

                printf("|%-4x/%-4x", mib_data.cnt, mib_data.err_cnt);
            }
            printf("\n-------------------------------------------------------------------------------\n");
            break;
        case 2:
            printf("------------------------------------------------\n");
            printf("| %-4d| %-4d| %-4d| %-4d| %-4d| %-4d| %-4d| %-4d|\n",\
                1,2,3,4,5,6,7,8);
            printf("------------------------------------------------\n");
            for(chan_num = 1; chan_num <= 16; chan_num++)
            {
                if(9 == chan_num)
                {
                    printf("\n------------------------------------------------\n");
                    printf("| %-4d| %-4d| %-4d| %-4d| %-4d| %-4d| %-4d| %-4d|\n",\
                        9,10,11,12,13,14,15,16);
                    printf("------------------------------------------------\n");
                }
                if(BHT_SUCCESS != bht_L1_a429_rx_chan_stat(dev_id, chan_num, &recv_num))
                    break;
                
                printf("| %-4x", recv_num);
            }
            printf("\n------------------------------------------------\n");
            if(chan_num <= 16)
                printf("Query all channel failed\n");
            break;
        case 3:
            type = input_chan_type();
            chan_num = input_channel_num(type);
            bht_L1_a429_chan_dump(dev_id, chan_num, type);
            break;
        }
        
    }while(DIAG_EXIT_MENU != option);
}

enum
{
    BAUD_PARITY_QUERY_ALL = 1,
    BAUD_PARITY_CONFIG,
    BAUD_PARITY_CONFIG_ALL,
    BAUD_PARITY_CONFIG_EXIT = DIAG_EXIT_MENU,
};
static void baud_rate(bht_L0_device_t *device)
{
    DWORD option, option1, option2;
    bht_L0_u32 result;
    bht_L0_u32 chan_num;
    bht_L1_chan_type_e type;
    bht_L1_a429_chan_comm_param_t comm_param;    

    printf("Baud rate / parity config menu\n");
    printf("--------------------------------------\n");
    do
    {
        printf("%d. Query Baud / parity with all channel\n", BAUD_PARITY_QUERY_ALL);
        printf("%d. Baud / parity config\n", BAUD_PARITY_CONFIG);
        printf("%d. Baud / parity config the same with all channel\n", BAUD_PARITY_CONFIG_ALL);
        printf("%d. EXIT\n", BAUD_PARITY_CONFIG_EXIT);

        if (DIAG_INPUT_FAIL == DIAG_GetMenuOption(&option, BAUD_PARITY_CONFIG_ALL))
        {
            continue;
        }

        switch(option)
        {
        case BAUD_PARITY_QUERY_ALL:
            type = input_chan_type();
            printf("-------------------------------------------------------------------------------\n");
            printf("|  %-4d   |  %-4d   |  %-4d   |  %-4d   |  %-4d   |  %-4d   |  %-4d   |  %-4d  \n",\
                1,2,3,4,5,6,7,8);
            printf("-------------------------------------------------------------------------------\n");
            for(chan_num = 1; chan_num <= 16; chan_num++)
            {
                if(9 == chan_num)
                {
                    printf("\n-------------------------------------------------------------------------------\n");
                    printf("|  %-4d   |  %-4d   |  %-4d   |  %-4d   |  %-4d   |  %-4d   |  %-4d   |  %-4d  \n",\
                        9,10,11,12,13,14,15,16);
                    printf("-------------------------------------------------------------------------------\n");
                }
                
                if(BHT_L1_CHAN_TYPE_RX == type)
                {
                    if(BHT_SUCCESS != (result = bht_L1_a429_rx_chan_comm_param(dev_id, chan_num, &comm_param, BHT_L1_PARAM_OPT_GET)))
                    {
                        printf("rx commom param get failed, %s (result = %d)", bht_L1_error_to_string(result), result);
                        break;
                    }
                }
                else
                {
                    if(BHT_SUCCESS != (result = bht_L1_a429_tx_chan_comm_param(dev_id, chan_num, &comm_param, BHT_L1_PARAM_OPT_GET)))
                    {
                        printf("tx commom param get failed, %s (result = %d)", bht_L1_error_to_string(result), result);
                        break;
                    }
                }

                printf("|%-0.1fk/%s", comm_param.baud /1000.0, (comm_param.par == BHT_L1_A429_PARITY_ODD) ? "ODD" : \
                    ((comm_param.par == BHT_L1_A429_PARITY_EVEN) ? "EVEN" : "NONE"));
            }
            printf("\n-------------------------------------------------------------------------------\n");
            break;
        case BAUD_PARITY_CONFIG:
            do
            {
                printf("1. Baud config\n");
                printf("2. Parity config\n");
                printf("%d. EXIT\n", DIAG_EXIT_MENU);

                if (DIAG_INPUT_FAIL == DIAG_GetMenuOption(&option1, 2))
                    continue;

                switch(option1)
                {
                case 1:
                case 2:
                    type = input_chan_type();
                    chan_num = input_channel_num(type);
                    
                    if(BHT_L1_CHAN_TYPE_RX == type)
                    {
                        if(BHT_SUCCESS != (result = bht_L1_a429_rx_chan_comm_param(dev_id, chan_num, &comm_param, BHT_L1_PARAM_OPT_GET)))
                        {
                            printf("rx commom param get failed, %s (result = %d)", bht_L1_error_to_string(result), result);
                            break;
                        }
                    }
                    else
                    {
                        if(BHT_SUCCESS != (result = bht_L1_a429_tx_chan_comm_param(dev_id, chan_num, &comm_param, BHT_L1_PARAM_OPT_GET)))
                        {
                            printf("tx commom param get failed, %s (result = %d)", bht_L1_error_to_string(result), result);
                            break;
                        }
                    }

                    do
                    {
                        if(1 == option1)
                        {
                        printf("1.baud rate 12.5K\n");
                        printf("2.baud rate 50K\n");
                        printf("3.baud rate 100K\n");
                        printf("4.baud rate 200K\n");
                        printf("5.baud rate input\n");
                        }
                        else
                        {
                        printf("1.ODD parity\n");
                        printf("2.EVEN parity\n");
                        printf("3.NONE parity\n");
                        }
                        printf("%d. EXIT\n", DIAG_EXIT_MENU);

                        if (DIAG_INPUT_FAIL == DIAG_GetMenuOption(&option2, 3))
                            continue;

                        if(1 == option1)
                            switch(option2)
                            {
                            case 1:
                                comm_param.baud = BHT_L1_A429_BAUD_12_5K;
                                break;
                            case 2:
                                comm_param.baud = BHT_L1_A429_BAUD_50K;
                                break;
                            case 3:
                                comm_param.baud = BHT_L1_A429_BAUD_100K;
                                break;
                            case 4:
                                comm_param.baud = BHT_L1_A429_BAUD_200K;
                                break;
                            case 5:
                                comm_param.baud = DIAG_GetNumber("baud rate", INPUT_DATA_FORMAT_DEC, 5000, 500000);
                                break;
                            
                            }                            
                        else
                            switch(option2)
                            {
                            case 1:
                                comm_param.par = BHT_L1_A429_PARITY_ODD;
                                break;
                            case 2:
                                comm_param.baud = BHT_L1_A429_PARITY_EVEN;
                                break;
                            case 3:
                                comm_param.baud = BHT_L1_A429_PARITY_NONE;
                                break;
                            }
                            
                        if(option2 != DIAG_EXIT_MENU)
                        {
                            if(BHT_L1_CHAN_TYPE_RX == type)
                            {
                                if(BHT_SUCCESS != (result = bht_L1_a429_rx_chan_comm_param(dev_id, chan_num, &comm_param, BHT_L1_PARAM_OPT_SET)))
                                {
                                    printf("rx commom param get failed, %s (result = %d)", bht_L1_error_to_string(result), result);
                                    break;
                                }
                            }
                            else
                            {
                                if(BHT_SUCCESS != (result = bht_L1_a429_tx_chan_comm_param(dev_id, chan_num, &comm_param, BHT_L1_PARAM_OPT_SET)))
                                {
                                    printf("tx commom param get failed, %s (result = %d)", bht_L1_error_to_string(result), result);
                                    break;
                                }
                            }
                        }
                    }while(option2 != DIAG_EXIT_MENU);

                    break;
                    }
            }while(option1 != DIAG_EXIT_MENU);
            
            break;
        case BAUD_PARITY_CONFIG_ALL:
            do
            {
                printf("1. Baud config\n");
                printf("2. Parity config\n");
                printf("%d. EXIT\n", DIAG_EXIT_MENU);

                if (DIAG_INPUT_FAIL == DIAG_GetMenuOption(&option1, 2))
                    continue;

                switch(option1)
                {
                case 1:
                case 2:
                    type = input_chan_type();
                    
                    if(BHT_L1_CHAN_TYPE_RX == type)
                    {
                        if(BHT_SUCCESS != (result = bht_L1_a429_rx_chan_comm_param(dev_id, chan_num, &comm_param, BHT_L1_PARAM_OPT_GET)))
                        {
                            printf("rx commom param get failed, %s (result = %d)", bht_L1_error_to_string(result), result);
                            break;
                        }
                    }
                    else
                    {
                        if(BHT_SUCCESS != (result = bht_L1_a429_tx_chan_comm_param(dev_id, chan_num, &comm_param, BHT_L1_PARAM_OPT_GET)))
                        {
                            printf("tx commom param get failed, %s (result = %d)", bht_L1_error_to_string(result), result);
                            break;
                        }
                    }

                    do
                    {
                        if(1 == option1)
                        {
                        printf("1.baud rate 12.5K\n");
                        printf("2.baud rate 100K\n");
                        printf("3.baud rate 200K\n");
                        }
                        else
                        {
                        printf("1.ODD parity\n");
                        printf("2.EVEN parity\n");
                        printf("3.NONE parity\n");
                        }
                        printf("%d. EXIT\n", DIAG_EXIT_MENU);

                        if (DIAG_INPUT_FAIL == DIAG_GetMenuOption(&option2, 3))
                            continue;

                        if(1 == option1)
                            switch(option2)
                            {
                            case 1:
                                comm_param.baud = BHT_L1_A429_BAUD_12_5K;
                                break;
                            case 2:
                                comm_param.baud = BHT_L1_A429_BAUD_100K;
                                break;
                            case 3:
                                comm_param.baud = BHT_L1_A429_BAUD_200K;
                                break;
                            }                            
                        else
                            switch(option2)
                            {
                            case 1:
                                comm_param.par = BHT_L1_A429_PARITY_ODD;
                                break;
                            case 2:
                                comm_param.baud = BHT_L1_A429_PARITY_EVEN;
                                break;
                            case 3:
                                comm_param.baud = BHT_L1_A429_PARITY_NONE;
                                break;
                            }
                            
                        if(option2 != DIAG_EXIT_MENU)
                        {
                            if(BHT_L1_CHAN_TYPE_RX == type)
                            {
                                if(BHT_SUCCESS != (result = bht_L1_a429_rx_chan_comm_param(dev_id, chan_num, &comm_param, BHT_L1_PARAM_OPT_SET)))
                                {
                                    printf("rx commom param get failed, %s (result = %d)", bht_L1_error_to_string(result), result);
                                    break;
                                }
                            }
                            else
                            {
                                if(BHT_SUCCESS != (result = bht_L1_a429_tx_chan_comm_param(dev_id, chan_num, &comm_param, BHT_L1_PARAM_OPT_SET)))
                                {
                                    printf("tx commom param get failed, %s (result = %d)", bht_L1_error_to_string(result), result);
                                    break;
                                }
                            }
                        }
                    }while(option2 != DIAG_EXIT_MENU);

                    break;
                    }
            }while(option1 != DIAG_EXIT_MENU);
            break;
        }
    }while(BAUD_PARITY_CONFIG_EXIT != option);
}

static void loop_cfg(void)
{
    DWORD option;
    bht_L0_u32 result;
    bht_L0_u32 chan_num;
    bht_L0_u32 opt = BHT_L1_ENABLE;    

    chan_num = input_channel_num(BHT_L1_CHAN_TYPE_TX);

    do
    {
        printf("1.loop enable\n");
        printf("2.loop disable\n");
        if (DIAG_INPUT_FAIL == DIAG_GetMenuOption(&option, 2))
        {
            continue;
        }
		if(2 == option)
            opt = BHT_L1_DISABLE;

        break;
    }while(1);

    if(BHT_SUCCESS != (result = bht_L1_a429_tx_chan_loop(DEVID, chan_num, opt)))
        printf("%s err, message : %s [result = %d]\n", __FUNCTION__, bht_L1_error_to_string(result), result);
}

static void tx_slope_cfg(void)
{
#if 0
    DWORD option;
    bht_L0_u32 result;
    bht_L0_u32 chan_num;
    bht_L1_a429_slope_e slope;    

    chan_num = input_channel_num(BHT_L1_CHAN_TYPE_TX);

    do
    {
        printf("1.slope 1.5us\n");
        printf("2.slope 10us\n");
        if (DIAG_INPUT_FAIL == DIAG_GetMenuOption(&option, 2))
        {
            continue;
        }
		if(1 == option)
            slope = BHT_L1_A429_SLOPE_1_5_US;
        else
            slope = BHT_L1_A429_SLOPE_10_US;
        break;
    }while(1);

    if(BHT_SUCCESS != (result = bht_L1_a429_tx_chan_slope_cfg(DEVID, chan_num, slope)))
        printf("%s err, message : %s [result = %d]\n", __FUNCTION__, bht_L1_error_to_string(result), result);
#endif
}

static void tx_trouble_cfg(void)
{
    DWORD option, option1;
    bht_L0_u32 result;
    bht_L0_u32 chan_num;
    bht_L1_a429_tx_chan_inject_param_t inject_param;    
    
    chan_num = input_channel_num(BHT_L1_CHAN_TYPE_TX);

    do
    {
        if(BHT_SUCCESS != bht_L1_a429_tx_chan_inject_param(DEVID, chan_num, &inject_param, BHT_L1_PARAM_OPT_GET))
        {
            printf("get inject param failed\n");
            break;
        }
        printf("\n");
        printf("1.Bits count trouble\n");
        printf("2.Gap trouble\n");
        printf("3.Parity trouble enable/disable\n");
        printf("%d.EXIT\n", DIAG_EXIT_MENU);
        if (DIAG_INPUT_FAIL == DIAG_GetMenuOption(&option, 3))
        {
            continue;
        }

        switch(option)
        {
        case 1:
            do
            {
            printf("\n");
            printf("1. 31 bit \n");
            printf("2. 32 bit \n");
            printf("3. 33 bit \n");
            
            if (DIAG_INPUT_FAIL == DIAG_GetMenuOption(&option1, 3))
            {
                continue;
            }

            if(1 == option1)
                inject_param.tb_bits = BHT_L1_A429_WORD_BIT31;
            else if(2 == option1)
                inject_param.tb_bits = BHT_L1_A429_WORD_BIT32;
            else if(3 == option1)
                inject_param.tb_bits = BHT_L1_A429_WORD_BIT32;

            if(option1 != DIAG_EXIT_MENU)
            {
                if(BHT_SUCCESS != bht_L1_a429_tx_chan_inject_param(DEVID, chan_num, &inject_param, BHT_L1_PARAM_OPT_SET))
                {
                    printf("set inject param failed\n");
                }
            }
            }while(0);
            break;
        case 2:
            do
            {
            printf("\n");
            printf("1. 2 bit gaps \n");
            printf("2. 4 bit gaps \n");
            printf("%d.EXIT\n", DIAG_EXIT_MENU);
            if (DIAG_INPUT_FAIL == DIAG_GetMenuOption(&option1, 2))
            {
                continue;
            }

            if(1 == option1)
                inject_param.tb_gap = BHT_L1_A429_GAP_2BIT;
            else if(2 == option1)
                inject_param.tb_gap = BHT_L1_A429_GAP_4BIT;

            if(option1 != DIAG_EXIT_MENU)
            {
                if(BHT_SUCCESS != bht_L1_a429_tx_chan_inject_param(DEVID, chan_num, &inject_param, BHT_L1_PARAM_OPT_SET))
                {
                    printf("set inject param failed\n");
                }
            }
            }while(0);
            break;
        case 3:
            do
            {
            printf("\n");
            printf("1. %s parity trouble \n", (inject_param.tb_par_en == BHT_L1_ENABLE) ? "disable" : "enable");            
            printf("%d.EXIT\n", DIAG_EXIT_MENU);
            if (DIAG_INPUT_FAIL == DIAG_GetMenuOption(&option1, 1))
            {
                continue;
            }

            if(option1 != DIAG_EXIT_MENU)
            {
                inject_param.tb_par_en = ((inject_param.tb_par_en == BHT_L1_ENABLE) ? BHT_L1_DISABLE : BHT_L1_ENABLE);
                
                if(BHT_SUCCESS != bht_L1_a429_tx_chan_inject_param(DEVID, chan_num, &inject_param, BHT_L1_PARAM_OPT_SET))
                {
                    printf("set inject param failed\n");
                }
            }
            }while(0);
            break;
        }
		
    }while(option != DIAG_EXIT_MENU);
}

enum
{
    TX_PERIOD_QUERY_ALL = 1,
    TX_PERIOD_CONFIG,
    TX_PERIOD_CONFIG_ALL,
    TX_PERIOD_UPDATE_DATA,
    TX_PERIOD_UPDATE_DATA_ALL,
    TX_PERIOD_START,
    TX_PERIOD_START_ALL,
    TX_PERIOD_STOP,
    TX_PERIOD_STOP_ALL,
    TX_PERIOD_EXIT = DIAG_EXIT_MENU
};
static void tx_period_test(void)
{
    DWORD option;
    bht_L0_u32 result;
    bht_L0_u32 chan_num;
    bht_L0_u32 period;
    bht_L0_u32 data = 0;
    bht_L0_u32 index;
    bht_L1_a429_send_opt_e send_opt;
    
    printf("\n");
    printf("Tx channel period test menu\n");
    printf("--------------\n");

    do
    {
        printf("%d. Query all TX channel period attribute\n", TX_PERIOD_QUERY_ALL);
        printf("%d. TX channel period config\n", TX_PERIOD_CONFIG);
        printf("%d. Config all TX channel with the same period parameter\n", TX_PERIOD_CONFIG_ALL);
        printf("%d. Update the period send Data\n", TX_PERIOD_UPDATE_DATA);
        printf("%d. Update the all TX channel with the same period send Data\n", TX_PERIOD_UPDATE_DATA_ALL);
        printf("%d. Period send start\n", TX_PERIOD_START);
        printf("%d. Period send start all\n", TX_PERIOD_START_ALL);
        printf("%d. Period send stop\n", TX_PERIOD_STOP);
        printf("%d. Period send stop all\n", TX_PERIOD_STOP_ALL);
        printf("%d. EXIT\n", TX_PERIOD_EXIT);

        if (DIAG_INPUT_FAIL == DIAG_GetMenuOption(&option, TX_PERIOD_STOP_ALL))
        {
            continue;
        }

        switch(option)
        {
        case TX_PERIOD_QUERY_ALL:
            printf("------------------------------------------------\n");
            printf("| %-4d| %-4d| %-4d| %-4d| %-4d| %-4d| %-4d| %-4d|\n",\
                1,2,3,4,5,6,7,8);
            printf("------------------------------------------------\n");
            for(chan_num = 1; chan_num <= 16; chan_num++)
            {
                if(9 == chan_num)
                {
                    printf("\n------------------------------------------------\n");
                    printf("| %-4d| %-4d| %-4d| %-4d| %-4d| %-4d| %-4d| %-4d|\n",\
                        9,10,11,12,13,14,15,16);
                    printf("------------------------------------------------\n");
                }
                if(BHT_SUCCESS != bht_L1_a429_tx_chan_period_param(DEVID, chan_num, &period, BHT_L1_PARAM_OPT_GET))
                    break;
                
                printf("| %-4d", period);
            }
            printf("\n------------------------------------------------\n");
            if(chan_num <= 16)
                printf("Query all channel failed\n");
            break;
        case TX_PERIOD_CONFIG:
            chan_num = input_channel_num(BHT_L1_CHAN_TYPE_TX);
            period = DIAG_GetNumber("period", INPUT_DATA_FORMAT_DEC, 0, 0);
            if(BHT_SUCCESS != bht_L1_a429_tx_chan_period_param(DEVID, chan_num, &period, BHT_L1_PARAM_OPT_SET))
                printf("Period set failed, %s\n", bht_L1_error_to_string(result));
            break;
        case TX_PERIOD_CONFIG_ALL:
            period = DIAG_GetNumber("period", INPUT_DATA_FORMAT_DEC, 0, 0);
            for(chan_num = 1; chan_num <= 16; chan_num++)
            {
                if(BHT_SUCCESS != bht_L1_a429_tx_chan_period_param(DEVID, chan_num, &period, BHT_L1_PARAM_OPT_SET))
                    printf("channel %d period set failed, %s\n", chan_num, bht_L1_error_to_string(result));
            }
            break;
        case TX_PERIOD_UPDATE_DATA:         
        case TX_PERIOD_START:
        case TX_PERIOD_STOP:            
            chan_num = input_channel_num(BHT_L1_CHAN_TYPE_TX);
            
            if(TX_PERIOD_UPDATE_DATA == option)
            {
                send_opt = BHT_L1_A429_OPT_PERIOD_SEND_UPDATE; 
                data = DIAG_GetNumber("update data", INPUT_DATA_FORMAT_HEX, 0, 0);
            }
            else if(TX_PERIOD_START == option)
                send_opt = BHT_L1_A429_OPT_PERIOD_SEND_START;
            else if(TX_PERIOD_STOP == option)
                send_opt = BHT_L1_A429_OPT_PERIOD_SEND_STOP;
                        
            if(BHT_SUCCESS != (result = bht_L1_a429_tx_chan_send(DEVID, chan_num, send_opt, data)))
                printf("chan send failed, %s\n", chan_num, bht_L1_error_to_string(result));
            break;
        case TX_PERIOD_UPDATE_DATA_ALL:            
        case TX_PERIOD_START_ALL:
        case TX_PERIOD_STOP_ALL:
            if(TX_PERIOD_UPDATE_DATA_ALL == option)
            {
                send_opt = BHT_L1_A429_OPT_PERIOD_SEND_UPDATE; 
                data = DIAG_GetNumber("update data", INPUT_DATA_FORMAT_HEX, 0, 0);
            }
            else if(TX_PERIOD_START_ALL == option)
                send_opt = BHT_L1_A429_OPT_PERIOD_SEND_START;
            else if(TX_PERIOD_STOP_ALL == option)
                send_opt = BHT_L1_A429_OPT_PERIOD_SEND_STOP;
            
            for(chan_num = 1; chan_num <= 16; chan_num++)
            {
                if(BHT_SUCCESS != (result = bht_L1_a429_tx_chan_send(DEVID, chan_num, send_opt, data)))
                    printf("chan send failed, %s\n", chan_num, bht_L1_error_to_string(result));
            }
            break;
        case TX_PERIOD_EXIT:
            break;
        };
    }while(option != DIAG_EXIT_MENU);
}

enum
{
    RX_GATHER_PARAM_CFG = 1,
    RX_GATHER_PARAM_CFG_ALL,
    RX_GATHER_EXIT = DIAG_EXIT_MENU
};
static void rx_gather_param_cfg(void)
{
    DWORD option, option1;
    bht_L0_u32 result;
    bht_L0_u32 chan_num;
    bht_L0_u32 start = 1, end = 16;
    bht_L1_a429_rx_chan_gather_param_t gather_param;
    
    printf("\n");
    printf("Rx channel gather param config menu\n");
    printf("--------------\n");

    do
    {
        printf("%d. RX channel gather param config\n", RX_GATHER_PARAM_CFG);
        printf("%d. config ALL RX channel with the same gather param\n", RX_GATHER_PARAM_CFG_ALL);
        printf("%d. EXIT\n", RX_GATHER_EXIT);

        if (DIAG_INPUT_FAIL == DIAG_GetMenuOption(&option, RX_GATHER_PARAM_CFG_ALL))
        {
            continue;
        }

        switch(option)
        {
        case RX_GATHER_PARAM_CFG:
            chan_num = input_channel_num(BHT_L1_CHAN_TYPE_RX);

            if(BHT_SUCCESS != (result = bht_L1_a429_rx_chan_gather_param(DEVID, chan_num, &gather_param, BHT_L1_PARAM_OPT_GET)))
            {
                printf("Get gather param failed, %s(%d)\n", bht_L1_error_to_string(result), result);
                break;
            }
            do
            {
                printf("1. gather enable/disable\n");
                printf("2. recieve mode sample/list\n");
                printf("3. interrupt threshold\n");
                printf("%d. EXIT\n", DIAG_EXIT_MENU);

                if (DIAG_INPUT_FAIL == DIAG_GetMenuOption(&option1, 3))
                {
                    continue;
                }

                switch(option1)
                {
                bht_L0_u32 option2;
                case 1:                    
                    do
                    {
                        printf("1. %s\n", gather_param.gather_enable ? "disable" : "enable");
                        printf("%d. EXIT\n", DIAG_EXIT_MENU);


                        if (DIAG_INPUT_FAIL == DIAG_GetMenuOption(&option2, 1))
                            continue;
                        if(option2 != DIAG_EXIT_MENU)
                        {
                            gather_param.gather_enable = (!gather_param.gather_enable);
                        }
                    }while(option2 != DIAG_EXIT_MENU);
                    break;
                case 2:
                    do
                    {
                        printf("1. %s\n", (gather_param.recv_mode == BHT_L1_A429_RECV_MODE_LIST) ? "Sample" : "List");
                        printf("%d. EXIT\n", DIAG_EXIT_MENU);


                        if (DIAG_INPUT_FAIL == DIAG_GetMenuOption(&option2, 1))
                            continue;
                        if(option2 != DIAG_EXIT_MENU)
                        {
                            gather_param.recv_mode = (gather_param.recv_mode == BHT_L1_A429_RECV_MODE_LIST) ? \
                                    BHT_L1_A429_RECV_MODE_SAMPLE : BHT_L1_A429_RECV_MODE_LIST;
                        }
                    }while(option2 != DIAG_EXIT_MENU);
                case 3:
                    gather_param.threshold_count = DIAG_GetNumber("interrupt threshold of A429 Words count", INPUT_DATA_FORMAT_DEC, 0, 1022);
                    gather_param.threshold_time = DIAG_GetNumber("interrupt threshold of time", INPUT_DATA_FORMAT_DEC, 0, 0xFFFF);
                    break;
                }

                if(DIAG_EXIT_MENU != option1)
                {
                    if(BHT_SUCCESS != (result = bht_L1_a429_rx_chan_gather_param(DEVID, chan_num, &gather_param, BHT_L1_PARAM_OPT_SET)))
                    {
                        printf("RX channel %d set gather param failed, %s (%d)\n", chan_num, bht_L1_error_to_string(result), result);
                    }
                }
            }while(DIAG_EXIT_MENU != option1);
            break;
        case RX_GATHER_PARAM_CFG_ALL:

            do
            {
                
                printf("Step 1/3. gather enable/disable menu\n");
                printf("1. enable\n");
                printf("2. diable\n");

                if (DIAG_INPUT_FAIL == DIAG_GetMenuOption(&option1, 2))
                    continue;
                if(option1 != DIAG_EXIT_MENU)
                {
                    if(1 == option1)
                        gather_param.gather_enable = BHT_L1_ENABLE;
                    else
                        gather_param.gather_enable = BHT_L1_DISABLE;

                    break; 
                }                               
            }while(1);

            do
            {
                
                printf("Step 2/3. recieve mode sample/list menu\n");
                printf("1. Sample\n");
                printf("2. List\n");

                if (DIAG_INPUT_FAIL == DIAG_GetMenuOption(&option1, 2))
                    continue;
                if(option1 != DIAG_EXIT_MENU)
                {
                    if(1 == option1)
                        gather_param.recv_mode = BHT_L1_A429_RECV_MODE_SAMPLE;
                    else
                        gather_param.recv_mode = BHT_L1_A429_RECV_MODE_LIST;
                    break;
                }
            }while(1);

            do
            {
                
                printf("Step 3/3. interrupt threshold menu\n");

                gather_param.threshold_count = DIAG_GetNumber("interrupt threshold of A429 Words count", INPUT_DATA_FORMAT_DEC, 0, 1022);
                gather_param.threshold_time = DIAG_GetNumber("interrupt threshold of time", INPUT_DATA_FORMAT_DEC, 0, 0xFFFF);
            }while(0);

            for(chan_num = 1; chan_num <= 16; chan_num++)
            {
                if(BHT_SUCCESS != (result = bht_L1_a429_rx_chan_gather_param(DEVID, chan_num, &gather_param, BHT_L1_PARAM_OPT_SET)))
                {
                    printf("RX channel %d set gather param failed, %s (%d)\n", chan_num, bht_L1_error_to_string(result), result);
                }
            }
            
            break;
        case RX_GATHER_EXIT:
            break;
        }
        
    }while(option != RX_GATHER_EXIT);
}

static void rx_filter_cfg(bht_L0_device_t *device)
{
    DWORD option, option1;
    bht_L0_u32 result;
    bht_L0_u32 chan_num;
    bht_L0_u32 rdwr = 0;
    bht_L0_u32 start = 1, end = 16;
    bht_L1_a429_rx_chan_filter_t filter_param;
    
    printf("\n");
    printf("Rx channel filter param config menu\n");
    printf("--------------\n");

    chan_num = input_channel_num(BHT_L1_CHAN_TYPE_RX);

    do
    {
        printf("1. Read\n");
        printf("2. Write\n");

        if (DIAG_INPUT_FAIL == DIAG_GetMenuOption(&rdwr, 2))
            continue;

        break;
    }while(1);       

    filter_param.label = DIAG_GetNumber("label", INPUT_DATA_FORMAT_DEC, 0, 255);
    filter_param.sdi = DIAG_GetNumber("sdi", INPUT_DATA_FORMAT_DEC, 0, 3);
    filter_param.ssm = DIAG_GetNumber("ssm", INPUT_DATA_FORMAT_DEC, 0, 3);

    if(2 == rdwr)
    {
        do
        {
            printf("1. White list mode\n");
            printf("2. Black list mode\n");

            if (DIAG_INPUT_FAIL == DIAG_GetMenuOption(&option, 2))
                continue;

            if(1 == option)
                filter_param.filter_mode = BHT_L1_A429_LIST_TYPE_WHITELIST;
            else if(2 == option)
                filter_param.filter_mode = BHT_L1_A429_LIST_TYPE_BLACKLIST;
            break;
        }while(1);
        
        if(BHT_SUCCESS != (bht_L1_a429_rx_chan_filter_cfg(dev_id, chan_num, &filter_param, BHT_L1_PARAM_OPT_SET)))
        {
            printf("rx chan[%d] filter param set failed\n", chan_num);
        }
    }
    else
    {
        if(BHT_SUCCESS != (bht_L1_a429_rx_chan_filter_cfg(dev_id, chan_num, &filter_param, BHT_L1_PARAM_OPT_GET)))
        {
            printf("rx chan[%d] filter param set failed\n", chan_num);
        }
        else
            printf("%s\n", filter_param.filter_mode ? "Wthite" : "Black");
    }
}

static void mib_clear(void)
{
    DWORD option;
    bht_L0_u32 result;
    bht_L0_u32 chan_num;
    bht_L1_chan_type_e type;
    bht_L1_a429_chan_comm_param_t comm_param;    

//    do
//    {
//        printf("1. RX channel\n");
//        printf("2. TX channel\n");
//        printf("%d. EXIT\n", DIAG_EXIT_MENU);
//        if (DIAG_INPUT_FAIL == DIAG_GetMenuOption(&option, 2))
//        {
//            continue;
//        }
//        if(1 == option)
//            type = BHT_L1_CHAN_TYPE_RX;
//        else if(DIAG_EXIT_MENU == option)
//            return;
//        else
//            type = BHT_L1_CHAN_TYPE_RX;
//        
//        break;
//    }while(1);
    type = input_chan_type();
    
    chan_num = input_channel_num(type);

    if(BHT_L1_CHAN_TYPE_RX == type)
        if(BHT_SUCCESS != (result = bht_L1_a429_rx_chan_mib_clear(DEVID, chan_num)))
        {
            printf("%s err, message : %s [result = %d]\n", __FUNCTION__, bht_L1_error_to_string(result), result);
            return;
        }
    else
        if(BHT_SUCCESS != (result = bht_L1_a429_tx_chan_mib_clear(DEVID, chan_num)))
        {
            printf("%s err, message : %s [result = %d]\n", __FUNCTION__, bht_L1_error_to_string(result), result);
            return;
        }
}

/* Read/write address menu options */
enum {
    MENU_RW_ADDR_SET_ADDR_SPACE = 1,
    MENU_RW_ADDR_READ,
    MENU_RW_ADDR_WRITE,
    MENU_RW_ADDR_EXIT = DIAG_EXIT_MENU
};

static void rw_mem(bht_L0_u32 bar, WDC_DIRECTION direction)
{
    DWORD dwStatus;
    DWORD dwOffset;
    bht_L0_u32 value, result;

//    printf("%s", (WDC_READ == direction) ?
//        "Enter offset to read from : 0x" : "Enter offset to write to : 0x");
//    scanf("%x", &dwOffset);

    dwOffset = DIAG_GetNumber((WDC_READ == direction) ?
        "offset to read from" : "offset to write to", INPUT_DATA_FORMAT_HEX, 0, 0);
//    printf("offset = 0x%08x\n", dwOffset);

    if(WDC_READ != direction)
    {
//        printf("Enter the value to write : 0x");
//        scanf("%x", &value);

        value = DIAG_GetNumber("value to write", INPUT_DATA_FORMAT_HEX, 0, 0);
//        printf("value = 0x%08x\n", value);
    }

    if(0 == bar)
    {
        if(WDC_READ == direction)
        {
            result = bht_L0_read_setupmem32(DEVID, dwOffset, &value, 1);
        }
        else
        {
            result = bht_L0_write_setupmem32(DEVID, dwOffset, &value, 1);
        }
    }
    else
    {
        if(WDC_READ == direction)
        {
            result = bht_L0_read_mem32(DEVID, dwOffset, &value, 1);
        }
        else
        {
            result = bht_L0_write_mem32(DEVID, dwOffset, &value, 1);
        }
    }

    printf("%s Adress 0x%08x ", (direction == WDC_READ) ? "Read" : "Write", dwOffset);
    
    if(BHT_SUCCESS != result)
       printf(" failed \n");
    else if(direction == WDC_WRITE)
        printf(" succ\n");
    else
        printf(" 0x%08x\n", value);
}

static void rw_mem_diag(void)
{
    DWORD option;
    bht_L0_u32 result;
    static DWORD dwAddrSpace = 0;  

    do
    {
        printf("\n");
        printf("Read/write the device's memory and IO ranges\n");
        printf("---------------------------------------------\n");
        printf("%d. Change active address space for read/write "
            "(currently: BAR %ld)\n", MENU_RW_ADDR_SET_ADDR_SPACE, dwAddrSpace);
        printf("%d. Read from active address space\n", MENU_RW_ADDR_READ);
        printf("%d. Write to active address space\n", MENU_RW_ADDR_WRITE);
        printf("%d. Exit menu\n", MENU_RW_ADDR_EXIT);
        printf("\n");
        
        if (DIAG_INPUT_FAIL == DIAG_GetMenuOption(&option,
            MENU_RW_ADDR_WRITE))
        {
            continue;
        }
        
        switch (option)
        {
        case MENU_RW_ADDR_EXIT: /* Exit menu */
            break;
        case MENU_RW_ADDR_SET_ADDR_SPACE: /* Set active address space for read/write address requests */
            do
            {
                printf("1. BAR0\n");
                printf("2. BAR2\n");
                printf("\n");

                if (DIAG_INPUT_FAIL == DIAG_GetMenuOption(&option, 2))
                {
                    continue;
                }
                
                if(option != MENU_RW_ADDR_EXIT)
                {
                    dwAddrSpace = (1 == option) ? 0 : 2;
                    break;
                }
                
            }while(1);
            
            break;
        case MENU_RW_ADDR_READ:  /* Read from a memory or I/O address */
        case MENU_RW_ADDR_WRITE: /* Write to a memory or I/O address */
        {
            bht_L0_u32 direction =
                (MENU_RW_ADDR_READ == option) ? WDC_READ : WDC_WRITE;

            rw_mem(dwAddrSpace, direction);
            
            break;
        }
        }
    }while (MENU_RW_ADDR_EXIT != option);
}

static void version_check(bht_L0_device_t *device)
{
    bht_L0_u32 result;
    bht_L0_u32 logic_version;

    if(BHT_SUCCESS != (result = bht_L1_device_version(dev_id, &logic_version)))
    {
        printf("Read device version failed, %s(result = %d)\n", bht_L1_error_to_string(result), result);
    }
    else
    {
        printf("Device Logic version %x, driver version %x\n", logic_version, BHT_L1_API_VERSION);
    }
}

static void param_default_config(bht_L0_device_t *device)
{
    int result;
	bht_L0_u32 value, idx, i;
    bht_L0_u32 chan_num;
    a429_tx_chan_param_t *tp;
    a429_rx_chan_param_t *rp;
    
    for(idx = 0; idx < ARRAY_SIZE(a429_test_rx_chan_param); idx++)
    {
        rp = &a429_test_rx_chan_param[idx];
        if(BHT_SUCCESS != (bht_L1_a429_rx_chan_comm_param(DEVID, rp->chan_num, &rp->comm_param, BHT_L1_PARAM_OPT_SET)))
        {
            printf("default param set : rx chan[%d] common param set failed\n", rp->chan_num);
            break;
        }
        if(BHT_SUCCESS != (bht_L1_a429_rx_chan_gather_param(DEVID, rp->chan_num, &rp->gather_param, BHT_L1_PARAM_OPT_SET)))
        {
            printf("default param set : rx chan[%d] gather param set failed\n", rp->chan_num);
            break;
        }
    }

    for(idx = 0; idx < ARRAY_SIZE(a429_test_tx_chan_param); idx++)
    {
        tp = &a429_test_tx_chan_param[idx];
        if(BHT_SUCCESS != (bht_L1_a429_tx_chan_comm_param(DEVID, tp->chan_num, &tp->comm_param, BHT_L1_PARAM_OPT_SET)))
        {
            printf("default param set : tx chan[%d] common param set failed\n", tp->chan_num);
            break;
        }
        if(BHT_SUCCESS != (bht_L1_a429_tx_chan_inject_param(DEVID, tp->chan_num, &tp->inject_param, BHT_L1_PARAM_OPT_SET)))
        {
            printf("default param set : tx chan[%d] inject param set failed\n", tp->chan_num);
            break;
        }
        if(BHT_SUCCESS != (bht_L1_a429_tx_chan_loop(DEVID, tp->chan_num, tp->loop_en)))
        {
            printf("default param set : tx chan[%d] loop param set failed\n", tp->chan_num);
            break;
        }
//        if(BHT_SUCCESS != (bht_L1_a429_tx_chan_slope_cfg(DEVID, tp->chan_num, tp->slope)))
//        {
//            printf("default param set : tx chan[%d] slope param set failed\n", tp->chan_num);
//            break;
//        }
        if(BHT_SUCCESS != (bht_L1_a429_tx_chan_period_param(DEVID, tp->chan_num, &tp->period, BHT_L1_PARAM_OPT_SET)))
        {
            printf("default param set : tx chan[%d] period param set failed\n", tp->chan_num);
            break;
        }
    }

    //enable fpga pci interrupt 
    value = BIT0;
    if(BHT_SUCCESS != (result = bht_L0_write_mem32(dev_id, BHT_A429_INTR_EN, &value, 1)))
        return result;

}

struct flt_label
{
    bht_L0_u8 label;                /* label */ 
    bht_L0_u8 sdi  : 2;             /* Source/Destination Identifier */ 
    bht_L0_u8 ssm  : 2;             /* Sign/Status Matrix */
    bht_L0_u8 revs : 4;             /* reserve bits*/
};

static void generate_tx_data(void)
{
    bht_L0_u32 option;
    bht_L0_u32 position = 0, count;
    bht_L0_u32 data;
    struct flt_label flt;

    
    printf("generate tx data menu\n");
    printf("------------------------\n");
    do
    {
        printf("1. generate tx data\n");
        printf("%d. EXIT\n", DIAG_EXIT_MENU);

        if (DIAG_INPUT_FAIL == DIAG_GetMenuOption(&option, 1))
        {
            continue;
        }

        if(DIAG_EXIT_MENU != option)
        {
            flt.label = DIAG_GetNumber("label", INPUT_DATA_FORMAT_DEC, 0, 255);
            flt.sdi = DIAG_GetNumber("sdi", INPUT_DATA_FORMAT_DEC, 0, 3);
            flt.ssm = DIAG_GetNumber("ssm", INPUT_DATA_FORMAT_DEC, 0, 3);
            printf("current position %d\n", position);
            count = DIAG_GetNumber("how many you want", INPUT_DATA_FORMAT_DEC, 0, (A429_DATAWORD_TEST_NUM - option));

            for(; count > 0; count--)
                test_tx_buf[position++] = flt.label | (flt.sdi << 8) | (flt.ssm << 29);
            
        }
        
    }while(option != DIAG_EXIT_MENU);
}

static void chips_cope_freq_div(bht_L0_device_t *device)
{
    bht_L0_u32 option;
    bht_L0_u32 result;
    static bht_L0_u32 divisor = 0, div;

    printf("Chips cope frequency divisor menu(current divisor %d)\n", divisor);
    printf("------------------------\n");

    do
    {
        printf("1. Chips Cope frequency divisor config\n");
        printf("%d. EXIT\n", DIAG_EXIT_MENU);

        if (DIAG_INPUT_FAIL == DIAG_GetMenuOption(&option, 1))
        {
            continue;
        }

        if(DIAG_EXIT_MENU != option)
        {
            div = DIAG_GetNumber("divisor", INPUT_DATA_FORMAT_DEC, 0, 0);

            if(BHT_SUCCESS != (result = bht_L0_write_mem32(dev_id, BHT_A429_DEBUG_CHIPSCOPE_FREQ_DIV, &div, 1)))
                printf("write chips cope divisor failed, %s(result = %d)\n", bht_L1_error_to_string(result), result);
            else
                divisor = div;
        }
        
    }while(option != DIAG_EXIT_MENU);
    
}

enum
{
    OPTION_BAUD_CFG = 1,
    OPTION_TX_LOOP_CFG,
    OPTION_TX_SLOPE_CFG,
    OPTION_TX_TROUBLE_CFG,
    OPTION_TX_PERIOD_TEST,
    OPTION_TX_THREAD_CREAT,
    OPTION_TX_THREAD_CREAT_ALL,
    OPTION_RX_GATHER_CFG,
    OPTION_RX_FILTER_CFG,
    OPTION_RX_THREAD_CREAT,
    OPTION_RX_THREAD_CREAT_ALL,
    OPTION_IRIGB_TEST,
    OPTION_MIB_DUMP,
    OPTION_MIB_CLEAR,
    OPTION_RW_ADDR,
    OPTION_DEVICE_SOFT_RESET_TEST,
    OPTION_FPGA_EEPROM_TEST,
    OPTION_VERSION,
    OPTION_BRAKE,
    OPTION_PARAM_DEFAULT_CONFIG,
    OPTION_PCI_LOAD_FPGA,
    OPTION_GENERATE_TX_DATA,
    OPTION_CHIPSCOPE_FREQ_DIV,
    OPTION_CONFIG_FROM_XML,
    OPTION_SAVE_DEFAULT_PARAM,
    OPTION_EXIT = DIAG_EXIT_MENU
};

static void menu(bht_L0_device_t *device)
{
    DWORD option;
    bht_L0_u32 result;
    
    do
    {
        printf("\n");
        printf("Arinc429 main menu\n");
        printf("--------------\n");
        printf("%d. Baud rate config\n", OPTION_BAUD_CFG);
        printf("%d. TX channel Loop enable/diable\n", OPTION_TX_LOOP_CFG);
        printf("%d. TX channel slope config\n", OPTION_TX_SLOPE_CFG);
        printf("%d. TX channel trouble inject config\n", OPTION_TX_TROUBLE_CFG);
        printf("%d. TX channel period test\n", OPTION_TX_PERIOD_TEST);
        printf("%d. TX channel send thread creat\n", OPTION_TX_THREAD_CREAT);
        printf("%d. TX channel send thread creat with all TX channels\n", OPTION_TX_THREAD_CREAT_ALL);
        printf("%d. RX channel gather param config\n", OPTION_RX_GATHER_CFG);
        printf("%d. RX channel filter param config\n", OPTION_RX_FILTER_CFG);
        printf("%d. RX channel recieve thread creat\n", OPTION_RX_THREAD_CREAT);
        printf("%d. RX channel recieve thread creat with all RX channels\n", OPTION_RX_THREAD_CREAT_ALL);
        printf("%d. IRIG-B test\n", OPTION_IRIGB_TEST);
        printf("%d. MIB dump\n", OPTION_MIB_DUMP);
        printf("%d. MIB clear\n", OPTION_MIB_CLEAR);
        printf("%d. Read/write memory and IO addresses on the device\n", OPTION_RW_ADDR);
        printf("%d. Device soft reset test\n", OPTION_DEVICE_SOFT_RESET_TEST);
        printf("%d. Fpga eeprom test\n", OPTION_FPGA_EEPROM_TEST);
        printf("%d. Version check\n", OPTION_VERSION);
        printf("%d. Brake\n", OPTION_BRAKE);
        printf("%d. Param default config\n", OPTION_PARAM_DEFAULT_CONFIG);
        printf("%d. Pci load fpga\n", OPTION_PCI_LOAD_FPGA);    
        printf("%d. Generate Tx Data\n", OPTION_GENERATE_TX_DATA);    
        printf("%d. ChipsCope frequency divisor\n", OPTION_CHIPSCOPE_FREQ_DIV);  
        printf("%d. Config from xml file\n", OPTION_CONFIG_FROM_XML);  
        printf("%d. Save default param\n", OPTION_SAVE_DEFAULT_PARAM); 
        
        printf("%d. EXIT\n", OPTION_EXIT);
        
        if (DIAG_INPUT_FAIL == DIAG_GetMenuOption(&option, OPTION_EXIT))
        {
            continue;
        }

        switch (option)
        {
        case OPTION_EXIT: /* Exit menu */
            break;
        case OPTION_BAUD_CFG:
            baud_rate(dev_id);
            break;        
        case OPTION_TX_LOOP_CFG:
            loop_cfg();
            break;
        case OPTION_TX_SLOPE_CFG:
            tx_slope_cfg();
            break;
        case OPTION_TX_TROUBLE_CFG:
            tx_trouble_cfg();
            break;
        case OPTION_TX_PERIOD_TEST:
            tx_period_test();
            break;
        case OPTION_TX_THREAD_CREAT: 
            tx_thread_creat();
            break;
        case OPTION_TX_THREAD_CREAT_ALL: 
            tx_thread_creat_all();
            break;
        case OPTION_RX_GATHER_CFG:
            rx_gather_param_cfg();
            break;
        case OPTION_RX_FILTER_CFG:
            rx_filter_cfg(dev_id);
            break;
        case OPTION_RX_THREAD_CREAT: 
            rx_thread_creat();
            break;
        case OPTION_RX_THREAD_CREAT_ALL: 
            rx_thread_creat_all();
            break;
        case OPTION_IRIGB_TEST:
            irigb_test(dev_id);
            break;
        case OPTION_MIB_DUMP:
            mib_dump(DEVID);
            break;
        case OPTION_MIB_CLEAR:
            mib_clear();
            break;
        case OPTION_RW_ADDR:
            rw_mem_diag();
            break;
        case OPTION_DEVICE_SOFT_RESET_TEST:
            bht_L1_device_softreset(DEVID);
            break;
        case OPTION_FPGA_EEPROM_TEST:
            if(BHT_SUCCESS != bht_L1_bd_fpga_eeprom_test(DEVID))
                printf("Fpga eeprom test failed !!!\n");
            else
                printf("Fpga eeprom test succ\n");
            break;
        case OPTION_VERSION:
            version_check(dev_id);
            break;
        case OPTION_BRAKE:
            option = OPTION_BRAKE;
            break;
        case OPTION_PARAM_DEFAULT_CONFIG:
            param_default_config(dev_id);
            break;
        case OPTION_PCI_LOAD_FPGA:
            if(BHT_SUCCESS != (result = bht_L1_device_load(dev_id)))
                printf("%s err, message : %s [result = %d]\n", __FUNCTION__, bht_L1_error_to_string(result), result);
            else
                printf("Pci load fpga success\n", __FUNCTION__, bht_L1_error_to_string(result), result);
            break;
        case OPTION_GENERATE_TX_DATA:
            generate_tx_data();
            break;
        case OPTION_CHIPSCOPE_FREQ_DIV:
            chips_cope_freq_div(dev_id);
            break;
        case OPTION_CONFIG_FROM_XML:
#ifdef SUPPORT_CONFIG_FROM_XML
            if(BHT_SUCCESS != bht_L1_a429_config_from_xml(dev_id, "./config.xml"))
                printf("config from xml failed\n");
            else
                printf("config from xml succ\n");
#endif
            break;
		case OPTION_SAVE_DEFAULT_PARAM:
#ifdef SUPPORT_DEFAULT_PARAM_SAVE
            if(BHT_SUCCESS != bht_L1_a429_default_param_save(dev_id))
                printf("save default param failed\n");
            else
                printf("save default param succ\n");
#endif
            break;

        
        }
    } while (OPTION_EXIT != option);
}

void main(void)
{
    int result;
	bht_L0_u32 value, idx, i;
    bht_L0_u32 chan_num;
    a429_tx_chan_param_t *tp;
    a429_rx_chan_param_t *rp;
    
    /* generate rand number */
    testbuf_rand(test_tx_buf, sizeof(test_tx_buf)/ sizeof(test_tx_buf[0]));

	/* probe device */
    if(BHT_SUCCESS == (result = bht_L1_device_probe(DEVID)))
        printf("device initialized succ\n");
    else
	{
	    bht_L1_device_remove(DEVID);
		printf("device probe failed, error info: %s, result = %d\n", \
			bht_L1_error_to_string(result), result);
		//goto test_error;
	}

	bht_L0_msleep(10);

	/* device deault initialize */
	if(BHT_SUCCESS == (result = bht_L1_a429_default_init(DEVID)))
        printf("device default initialized succ\n");
    else
	{
	    bht_L1_device_remove(DEVID);
		printf("device default initialized failed, error info: %s, result = %d\n", \
			bht_L1_error_to_string(result), result);
		//goto test_error;
	}
    
    menu(DEVID);

    //Wait until all threads have terminated.
	WaitForMultipleObjects(thread_count, hThread, TRUE, INFINITE);

    //Close all thread handle supon completion.
    for(idx = 0; idx < thread_count; idx++)
        CloseHandle(hThread[idx]);

test_error:
	system("pause");

	return 0;
}




