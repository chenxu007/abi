#ifndef __BHT_L1_A429_H__
#define __BHT_L1_A429_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <bht_L1.h>
#include <bht_L0.h>
#include <bht_L0_types.h>
#include <bht_L1_ring.h>

#define SUPPORT_CONFIG_FROM_XML
#define SUPPORT_DEFAULT_PARAM_SAVE
#define BHT_L1_A429_CHAN_MAX               16
#define BHT_L1_A429_RXP_BUF_MAX            1024

#ifdef SUPPORT_CONFIG_FROM_XML
#include <mxml.h>
#define STRING_DEVTYPE_A429 "ARINC429"
#define STRING_DEVTYPE_1553 "MIL-STD-1553B"
#define STRING_CHANTYPE_RX "RX"
#define STRING_CHANTYPE_TX "TX"
#define STRING_ENABLE "Enable"
#define STRING_DISABLE "Disable"
#define STRING_VERIFY_ODD "ODD"
#define STRING_VERIFY_EVEN "EVEN"
#define STRING_VERIFY_NONE "NONE"
#define STRING_RECVMODE_LIST "List"
#define STRING_RECVMODE_Sample "Sample"
#define STRING_WORKMODE_OPEN "Open"
#define STRING_WORKMODE_CLOSE "Close"
#define STRING_WORKMODE_CLOSEANDCLEARFIFO "CloseAndClearFIFO"
#define STRING_ERRINJECTTYPE_NONE "ErrInjectTypeNone"
#define STRING_ERRINJECTTYPE_31BIT "ErrInjectType31Bit"
#define STRING_ERRINJECTTYPE_33BIT "ErrInjectType33Bit"
#define STRING_ERRINJECTTYPE_2GAP "ErrInjectType2Gap"
#define STRING_ERRINJECTTYPE_PARITY "ErrInjectTypeParity"
#endif
    
#define BHT_L1_A429_CHAN_NUM_CHK_RTN(chan_num)\
	do\
	{\
	    if((chan_num > BHT_L1_A429_CHAN_MAX) || (chan_num < 1))\
            return BHT_ERR_BAD_INPUT;\
	}while(0);
#define BHT_L1_A429_CFG(device, able, result, error_label)\
	do\
    {\
        bht_L0_u32 temp_value = able;\
		if(BHT_SUCCESS != (result = bht_L0_write_mem32(device, BHT_A429_CFG_ENABLE, &temp_value, 1)))\
            goto error_label;\
	}while (0);

/*----------------  A429_REG -----------------*/

/***************  通用寄存器******************/
#define BHT_A429_DEVICE_SOFT_RESET			0X0000
#define BHT_A429_DEVICE_VERSION				0X0004
#define BHT_A429_DEVICE_STATE				0X0008
#define BHT_A429_CARD_TYPE					0X000C
#define BHT_A429_IRIG_B_SET					0X0010
#define BHT_A429_HOST_TIME1					0X0014
#define BHT_A429_HOST_TIME2					0X0018
#define BHT_A429_DICRETE_IO					0X001C
#define BHT_A429_INTR_SET					0X0020
#define BHT_A429_INTR_CLR					0X0024
#define BHT_A429_INTR_STATE					0X0028
#define BHT_A429_INTR_CHANNEL_VECTOR		0X002C
#define BHT_A429_INTR_EN					0X0030
#define BHT_A429_LOOP_EN					0X0040
#define BHT_A429_SLOPE_CTRL					0X0044

/**************  数据处理寄存器*************/
#define BHT_A429_CHANNEL_NUM_SEND			0X2000
#define BHT_A429_CHANNEL_NUM_RECV			0X2100

#define BHT_A429_STATUS_CHANNEL_SEND		0X2004
#define BHT_A429_STATUS_CHANNEL_RECV		0X2104

#define BHT_A429_WORD_WR_CHANNEL			0X2008
#define BHT_A429_WORD_RD_CHANNEL_TIMESTAMP	0X2108	
#define BHT_A429_WORD_RD_CHANNEL_FREASH		0X210C
#define BHT_A429_WORD_RD_CHANNEL_NOFREASH	0X210D

#define BHT_A429_TIME_TAG_HIGH_CHANNEL		0X2010
#define BHT_A429_TIME_TAG_LOW_CHANNEL		0X2014   

/**************  配置管理寄存器*************/
#define BHT_A429_CFG_ENABLE					0X1000
#define BHT_A429_CHOOSE_CHANNEL_NUM			0X1004
#define BHT_A429_CHANNEL_CFG				0X1008
#define BHT_A429_BAUD_RATE_SET				0X100C
#define BHT_A429_INT_THRESHOLD				0X1010
#define BHT_A429_LABLE_FILTER				0X1014	/*将接收通道配置和其过滤信息拆分为两个寄存器 modify 20140805*/
#define BHT_A429_MIBS_CLR					0X1020	/*MIB 清除寄存器地址修改*/

#define BHT_A429_NUM_WORD_CHANNEL0			0X1100
#define BHT_A429_NUM_ERR_WORD_CHANNEL0		0X1104
#define BHT_A429_NUM_WORD_CHANNEL1			0X1110
#define BHT_A429_NUM_ERR_WORD_CHANNEL1		0X1114
#define BHT_A429_NUM_WORD_CHANNEL2			0X1120
#define BHT_A429_NUM_ERR_WORD_CHANNEL2		0X1124
#define BHT_A429_NUM_WORD_CHANNEL3			0X1130
#define BHT_A429_NUM_ERR_WORD_CHANNEL3		0X1134
#define BHT_A429_NUM_WORD_CHANNEL4			0X1140
#define BHT_A429_NUM_ERR_WORD_CHANNEL4		0X1144
#define BHT_A429_NUM_WORD_CHANNEL5			0X1150
#define BHT_A429_NUM_ERR_WORD_CHANNEL5		0X1154
#define BHT_A429_NUM_WORD_CHANNEL6			0X1160
#define BHT_A429_NUM_ERR_WORD_CHANNEL6		0X1164
#define BHT_A429_NUM_WORD_CHANNEL7			0X1170
#define BHT_A429_NUM_ERR_WORD_CHANNEL7		0X1174
#define BHT_A429_NUM_WORD_CHANNEL8			0X1180
#define BHT_A429_NUM_ERR_WORD_CHANNEL8		0X1184
#define BHT_A429_NUM_WORD_CHANNEL9			0X1190
#define BHT_A429_NUM_ERR_WORD_CHANNEL9		0X1194
#define BHT_A429_NUM_WORD_CHANNEL10			0X11A0
#define BHT_A429_NUM_ERR_WORD_CHANNEL10		0X11A4
#define BHT_A429_NUM_WORD_CHANNEL11			0X11B0
#define BHT_A429_NUM_ERR_WORD_CHANNEL11		0X11B4
#define BHT_A429_NUM_WORD_CHANNEL12			0X11C0
#define BHT_A429_NUM_ERR_WORD_CHANNEL12		0X11C4
#define BHT_A429_NUM_WORD_CHANNEL13			0X11D0
#define BHT_A429_NUM_ERR_WORD_CHANNEL13		0X11D4
#define BHT_A429_NUM_WORD_CHANNEL14			0X11E0
#define BHT_A429_NUM_ERR_WORD_CHANNEL14		0X11E4
#define BHT_A429_NUM_WORD_CHANNEL15			0X11F0
#define BHT_A429_NUM_ERR_WORD_CHANNEL15		0X11F4
#define BHT_A429_NUM_WORD_CHANNEL16			0X1200
#define BHT_A429_NUM_ERR_WORD_CHANNEL16		0X1204
#define BHT_A429_NUM_WORD_CHANNEL17			0X1210
#define BHT_A429_NUM_ERR_WORD_CHANNEL17		0X1214
#define BHT_A429_NUM_WORD_CHANNEL18			0X1220
#define BHT_A429_NUM_ERR_WORD_CHANNEL18		0X1224
#define BHT_A429_NUM_WORD_CHANNEL19			0X1230
#define BHT_A429_NUM_ERR_WORD_CHANNEL19		0X1234
#define BHT_A429_NUM_WORD_CHANNEL20			0X1240
#define BHT_A429_NUM_ERR_WORD_CHANNEL20		0X1244
#define BHT_A429_NUM_WORD_CHANNEL21			0X1250
#define BHT_A429_NUM_ERR_WORD_CHANNEL21		0X1254
#define BHT_A429_NUM_WORD_CHANNEL22			0X1260
#define BHT_A429_NUM_ERR_WORD_CHANNEL22		0X1264
#define BHT_A429_NUM_WORD_CHANNEL23			0X1270
#define BHT_A429_NUM_ERR_WORD_CHANNEL23		0X1274
#define BHT_A429_NUM_WORD_CHANNEL24			0X1280
#define BHT_A429_NUM_ERR_WORD_CHANNEL24		0X1284
#define BHT_A429_NUM_WORD_CHANNEL25			0X1290
#define BHT_A429_NUM_ERR_WORD_CHANNEL25		0X1294
#define BHT_A429_NUM_WORD_CHANNEL26			0X12A0
#define BHT_A429_NUM_ERR_WORD_CHANNEL26		0X12A4
#define BHT_A429_NUM_WORD_CHANNEL27			0X12B0
#define BHT_A429_NUM_ERR_WORD_CHANNEL27		0X12B4
#define BHT_A429_NUM_WORD_CHANNEL28			0X12C0
#define BHT_A429_NUM_ERR_WORD_CHANNEL28		0X12C4
#define BHT_A429_NUM_WORD_CHANNEL29			0X12D0
#define BHT_A429_NUM_ERR_WORD_CHANNEL29		0X12D4
#define BHT_A429_NUM_WORD_CHANNEL30			0X12E0
#define BHT_A429_NUM_ERR_WORD_CHANNEL30		0X12E4
#define BHT_A429_NUM_WORD_CHANNEL31			0X12F0
#define BHT_A429_NUM_ERR_WORD_CHANNEL31		0X12F4
#define BHT_A429_TX_CHAN_SEND_PERIOD		0X1300
#define BHT_A429_TX_CHAN_SEND_PERIOD_CTRL   0X1304

#define BHT_A429_DEBUG_CHIPSCOPE_FREQ_DIV   0X5004

/* default parameter */
#define BHT_A429_SAVE_DEFAULT_PARAM_CTRL   0x6000
#define BHT_A429_SAVE_DEFAULT_PARAM_STATUS 0x6004

#define BHT_A429_FILTER_READ               0x6008
#define BHT_A429_FILTER_DATA               0x600C


typedef struct
{
    bht_L0_u32 resv2 : 12;
    bht_L0_u32 send_mode :1;
    bht_L0_u32 rx_parity :2;
    bht_L0_u32 recv_mode :1;
    bht_L0_u32 flt_en : 1;
    bht_L0_u32 revs1 : 7;
    bht_L0_u32 tx_parity : 1;
    bht_L0_u32 parity_err : 1;
    bht_L0_u32 gap_err : 1;
    bht_L0_u32 bit_count_err : 2;
    bht_L0_u32 chan_stat : 2;
    bht_L0_u32 chan_type : 1;
}bht_L1_a429_cfg_t;

typedef enum
{
    BHT_L1_A429_WORD_BIT32  = 0,
    BHT_L1_A429_WORD_BIT31  = 1,
    BHT_L1_A429_WORD_BIT33  = 2,
}bht_L1_a429_word_bit_e;

typedef enum
{
    BHT_L1_A429_GAP_4BIT  = 0,
    BHT_L1_A429_GAP_2BIT  = 1,
}bht_L1_a429_gap_e;

typedef enum
{
    BHT_L1_A429_SLOPE_10_US = 0, 
    BHT_L1_A429_SLOPE_1_5_US = 1
}bht_L1_a429_slope_e;

typedef struct
{
    bht_L0_sem semc;

    bht_L1_a429_recv_mode_e recv_mode;
    bht_L0_u32 chk_times;
    bht_L0_u32 recv_data_cnt;
    bht_L0_u32 lost_data_cnt;
    bht_L0_u32 semc_err_cnt;
    bht_L0_u32 chan_idle_err_cnt;
    struct ring_buf *rxp_ring;
}bht_L1_a429_chan_data_t;

typedef struct
{
    bht_L1_a429_send_mode_e send_mode;
    bht_L0_u32 send_data_cnt;
    bht_L0_u32 lost_data_cnt;    
}bht_L1_a429_send_stat_t;

typedef struct
{
    bht_L0_u32 tot_chans;                   /* total channels */

    bht_L0_s32 last_err;
    
    bht_L0_u32 err_cnt;

    bht_L0_u32 int_cnt;

    bht_L0_u32 int_valid_cnt;

    bht_L0_u32 int_stat_err_cnt;

    bht_L0_u32 int_vector_idle_err_cnt;

    
    
    bht_L1_a429_chan_data_t *chan_data;     /* rx channel data array */
    bht_L1_a429_send_stat_t *send_stat;
}bht_L1_a429_cb_t;

bht_L0_u32
bht_L1_a429_private_alloc(bht_L0_device_t *device);

bht_L0_u32
bht_L1_a429_private_free(bht_L0_device_t *device);

bht_L0_u32
bht_L1_a429_reset(bht_L0_device_t *device);
#ifdef __cplusplus
}
#endif

#endif
