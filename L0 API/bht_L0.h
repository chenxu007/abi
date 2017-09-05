/* bht_L0.h - Layer 0 API*/

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

#ifndef __BHT_L0_H__
#define __BHT_L0_H__

#ifdef __cplusplus
extern "C" {
#endif

#define BHT_L0_API_VERSION          0x01000000      /* Version V 1.0.0.0 */

/* PCI Vendor and Device IDs */
#define BHT_PCI_VENDOR_ORIGIN           0x10B5
#define BHT_PCI_DEVICE_ORIGIN		    0x9056

#define BHT_PCI_VENDOR_ID               0x8620

#define BHT_PCI_DEVICE_ID_TEST1553		0x0001
#define BHT_PCI_DEVICE_ID_PMC1553		0x0010
#define BHT_PCI_DEVICE_ID_PC104P1553	0x0012
#define BHT_PCI_DEVICE_ID_PCI1553		0x0014
#define BHT_PCI_DEVICE_ID_PCCD1553		0x0016
#define BHT_PCI_DEVICE_ID_PCI104E1553	0x0018
#define BHT_PCI_DEVICE_ID_XMC1553		0x001A
#define BHT_PCI_DEVICE_ID_ECD54_1553	0x001C
#define BHT_PCI_DEVICE_ID_PCIE4L1553	0x001E
#define BHT_PCI_DEVICE_ID_PCIE1L1553	0x0020
#define BHT_PCI_DEVICE_ID_MPCIE1553		0x0022
#define BHT_PCI_DEVICE_ID_XMCMW			0x0024

#define BHT_PCI_DEVICE_ID_TESTA429		0x0014  /* TEST-A429 uses PCI-1553 board with ARINC bit file */
#define BHT_PCI_DEVICE_ID_PMCA429		0x0002
#define BHT_PCI_DEVICE_ID_PC104PA429	0x0112
#define BHT_PCI_DEVICE_ID_PCIA429		0x0114
#define BHT_PCI_DEVICE_ID_PCCDA429		0x0116
#define BHT_PCI_DEVICE_ID_PCI104EA429	0x0118
#define BHT_PCI_DEVICE_ID_XMCA429		0x011A
#define BHT_PCI_DEVICE_ID_ECD54_A429	0x011C
#define BHT_PCI_DEVICE_ID_PCIE4LA429	0x011E
#define BHT_PCI_DEVICE_ID_PCIE1LA429    0x0120
#define BHT_PCI_DEVICE_ID_MPCIEA429     0x0122
#define BHT_PCI_DEVICE_ID_PC104PA429LTV 0x0124

/* Device ID Constants - Backplane Type (4 bits) -	(DEVID & 0xF0000000) */
#define BHT_DEVID_BACKPLANETYPE_SIMULATED	0x00000000
#define BHT_DEVID_BACKPLANETYPE_PCI			0x10000000
#define BHT_DEVID_BACKPLANETYPE_ENET		0x20000000

/* Device ID Constants - Board Type (8 bits) -		(DEVID & 0x0FF00000) */
#define BHT_DEVID_BOARDTYPE_SIM1553			0x00000000
#define BHT_DEVID_BOARDTYPE_TEST1553		0x00100000
#define BHT_DEVID_BOARDTYPE_PMC1553			0x00200000
#define BHT_DEVID_BOARDTYPE_PC104P1553		0x00300000
#define BHT_DEVID_BOARDTYPE_PCI1553			0x00400000
#define BHT_DEVID_BOARDTYPE_PCCD1553		0x00500000
#define BHT_DEVID_BOARDTYPE_PCI104E1553		0x00600000
#define BHT_DEVID_BOARDTYPE_XMC1553			0x00700000
#define BHT_DEVID_BOARDTYPE_ECD54_1553		0x00800000
#define BHT_DEVID_BOARDTYPE_PCIE4L1553		0x00900000
#define BHT_DEVID_BOARDTYPE_PCIE1L1553		0x00A00000
#define BHT_DEVID_BOARDTYPE_MPCIE1553		0x00B00000
#define BHT_DEVID_BOARDTYPE_XMCMW			0x00C00000

#define BHT_DEVID_BOARDTYPE_SIMA429			0x01000000
#define BHT_DEVID_BOARDTYPE_TESTA429		0x01100000
#define BHT_DEVID_BOARDTYPE_PMCA429			0x01200000
#define BHT_DEVID_BOARDTYPE_PC104PA429		0x01300000
#define BHT_DEVID_BOARDTYPE_PCIA429			0x01400000
#define BHT_DEVID_BOARDTYPE_PCCDA429		0x01500000
#define BHT_DEVID_BOARDTYPE_PCI104EA429		0x01600000
#define BHT_DEVID_BOARDTYPE_XMCA429			0x01700000
#define BHT_DEVID_BOARDTYPE_ECD54_A429		0x01800000
#define BHT_DEVID_BOARDTYPE_PCIE4LA429		0x01900000
#define BHT_DEVID_BOARDTYPE_PCIE1LA429		0x01A00000
#define BHT_DEVID_BOARDTYPE_MPCIEA429		0x01B00000
#define BHT_DEVID_BOARDTYPE_PC104PA429LTV	0x01C00000

#define BHT_DEVID_BOARDTYPE_UNINITIALIZED	0x02000000

/* Device ID Constants - Product (backplane | board type)
 * For defining DEVID in user applications.
 */
#define BHT_PRODUCT_SIM1553		BHT_DEVID_BACKPLANETYPE_SIMULATED | BHT_DEVID_BOARDTYPE_SIM1553
#define BHT_PRODUCT_TEST1553	BHT_DEVID_BACKPLANETYPE_PCI | BHT_DEVID_BOARDTYPE_TEST1553
#define BHT_PRODUCT_PMC1553		BHT_DEVID_BACKPLANETYPE_PCI | BHT_DEVID_BOARDTYPE_PMC1553
#define BHT_PRODUCT_PC104P1553	BHT_DEVID_BACKPLANETYPE_PCI | BHT_DEVID_BOARDTYPE_PC104P1553
#define BHT_PRODUCT_PCI1553		BHT_DEVID_BACKPLANETYPE_PCI | BHT_DEVID_BOARDTYPE_PCI1553
#define BHT_PRODUCT_PCCD1553	BHT_DEVID_BACKPLANETYPE_PCI | BHT_DEVID_BOARDTYPE_PCCD1553
#define BHT_PRODUCT_PCI104E1553	BHT_DEVID_BACKPLANETYPE_PCI | BHT_DEVID_BOARDTYPE_PCI104E1553
#define BHT_PRODUCT_XMC1553		BHT_DEVID_BACKPLANETYPE_PCI | BHT_DEVID_BOARDTYPE_XMC1553
#define BHT_PRODUCT_ECD54_1553	BHT_DEVID_BACKPLANETYPE_PCI | BHT_DEVID_BOARDTYPE_ECD54_1553
#define BHT_PRODUCT_PCIE4L1553	BHT_DEVID_BACKPLANETYPE_PCI | BHT_DEVID_BOARDTYPE_PCIE4L1553
#define BHT_PRODUCT_PCIE1L1553	BHT_DEVID_BACKPLANETYPE_PCI | BHT_DEVID_BOARDTYPE_PCIE1L1553
#define BHT_PRODUCT_MPCIE1553	BHT_DEVID_BACKPLANETYPE_PCI | BHT_DEVID_BOARDTYPE_MPCIE1553
#define BHT_PRODUCT_XMCMW		BHT_DEVID_BACKPLANETYPE_PCI | BHT_DEVID_BOARDTYPE_XMCMW

#define BHT_PRODUCT_SIMA429		BHT_DEVID_BACKPLANETYPE_SIMULATED | BHT_DEVID_BOARDTYPE_SIMA429
#define BHT_PRODUCT_TESTA429	BHT_DEVID_BACKPLANETYPE_PCI | BHT_DEVID_BOARDTYPE_TESTA429
#define BHT_PRODUCT_PMCA429		BHT_DEVID_BACKPLANETYPE_PCI | BHT_DEVID_BOARDTYPE_PMCA429
#define BHT_PRODUCT_PC104PA429	BHT_DEVID_BACKPLANETYPE_PCI | BHT_DEVID_BOARDTYPE_PC104PA429
#define BHT_PRODUCT_PCIA429		BHT_DEVID_BACKPLANETYPE_PCI | BHT_DEVID_BOARDTYPE_PCIA429
#define BHT_PRODUCT_PCCDA429	BHT_DEVID_BACKPLANETYPE_PCI | BHT_DEVID_BOARDTYPE_PCCDA429
#define BHT_PRODUCT_PCI104EA429	BHT_DEVID_BACKPLANETYPE_PCI | BHT_DEVID_BOARDTYPE_PCI104EA429
#define BHT_PRODUCT_XMCA429		BHT_DEVID_BACKPLANETYPE_PCI | BHT_DEVID_BOARDTYPE_XMCA429
#define BHT_PRODUCT_ECD54_A429	BHT_DEVID_BACKPLANETYPE_PCI | BHT_DEVID_BOARDTYPE_ECD54_A429
#define BHT_PRODUCT_PCIE4LA429	BHT_DEVID_BACKPLANETYPE_PCI | BHT_DEVID_BOARDTYPE_PCIE4LA429
#define BHT_PRODUCT_PCIE1LA429	BHT_DEVID_BACKPLANETYPE_PCI | BHT_DEVID_BOARDTYPE_PCIE1LA429
#define BHT_PRODUCT_MPCIEA429	BHT_DEVID_BACKPLANETYPE_PCI | BHT_DEVID_BOARDTYPE_MPCIEA429
#define BHT_PRODUCT_PC104PA429LTV	BHT_DEVID_BACKPLANETYPE_PCI | BHT_DEVID_BOARDTYPE_PC104PA429LTV

/* Device ID Constants - Board Number (4 bits) -	(DEVID & 0x000F0000) */
#define BHT_DEVID_BOARDNUM_01				0x00000000
#define BHT_DEVID_BOARDNUM_02				0x00010000
#define BHT_DEVID_BOARDNUM_03				0x00020000
#define BHT_DEVID_BOARDNUM_04				0x00030000
#define BHT_DEVID_BOARDNUM_05				0x00040000
#define BHT_DEVID_BOARDNUM_06				0x00050000
#define BHT_DEVID_BOARDNUM_07				0x00060000
#define BHT_DEVID_BOARDNUM_08				0x00070000
#define BHT_DEVID_BOARDNUM_09				0x00080000
#define BHT_DEVID_BOARDNUM_10				0x00090000
#define BHT_DEVID_BOARDNUM_11				0x000A0000
#define BHT_DEVID_BOARDNUM_12				0x000B0000
#define BHT_DEVID_BOARDNUM_13				0x000C0000
#define BHT_DEVID_BOARDNUM_14				0x000D0000
#define BHT_DEVID_BOARDNUM_15				0x000E0000
#define BHT_DEVID_BOARDNUM_16				0x000F0000

/* Device ID Constants - Channel Type (8 bits) -	(DEVID & 0x0000FF00) */
#define BHT_DEVID_CHANNELTYPE_GLOBALS		0x00000100
#define BHT_DEVID_CHANNELTYPE_1553			0x00001000
#define BHT_DEVID_CHANNELTYPE_A429			0x00002000
#define BHT_DEVID_CHANNELTYPE_WMUX			0x00003000

/* Device ID Constants - Channel Number (8 bits) -	(DEVID & 0x000000FF) */
#define BHT_DEVID_CHANNELNUM_01				0x00000000
#define BHT_DEVID_CHANNELNUM_02				0x00000001
#define BHT_DEVID_CHANNELNUM_03				0x00000002
#define BHT_DEVID_CHANNELNUM_04				0x00000003
#define BHT_DEVID_CHANNELNUM_05				0x00000004
#define BHT_DEVID_CHANNELNUM_06				0x00000005
#define BHT_DEVID_CHANNELNUM_07				0x00000006
#define BHT_DEVID_CHANNELNUM_08				0x00000007
#define BHT_DEVID_CHANNELNUM_09				0x00000008
#define BHT_DEVID_CHANNELNUM_10				0x00000009
#define BHT_DEVID_CHANNELNUM_11				0x0000000A
#define BHT_DEVID_CHANNELNUM_12				0x0000000B
#define BHT_DEVID_CHANNELNUM_13				0x0000000C
#define BHT_DEVID_CHANNELNUM_14				0x0000000D
#define BHT_DEVID_CHANNELNUM_15				0x0000000E
#define BHT_DEVID_CHANNELNUM_16				0x0000000F

/* Memory sizes by board type and channel type for SIMULATED devices */
#define BHT_MEMSIZE_SIM1553_CHAN			0x100000	/* 1MB for a SIM1553 channel */
#define BHT_MEMSIZE_SIMA429_CHAN			0x100000	/* 1MB for a SIMA429 channel */

/* Layer 0 Error Codes (1 to 999) */
#define BHT_SUCCESS						0		/*!< \brief Function call completed without error. */
#define BHT_FAILURE						1		/*!< \brief Function call completed with error. */
#define BHT_ERR_MEM_MAP_SIZE			2		/*!< \brief Invalid memory map size. */
#define BHT_ERR_NO_DEVICE				3		/*!< \brief Device not found */
#define BHT_ERR_CANT_OPEN_DEV			4		/*!< \brief Can't open device */
#define BHT_ERR_DEV_NOT_INITED			5		/*!< \brief Device not initialized */
#define BHT_ERR_DEV_ALREADY_OPEN		6		/*!< \brief Device already open */
#define BHT_ERR_UNSUPPORTED_BACKPLANE	7		/*!< \brief Unsupported backplane in DevID */
#define BHT_ERR_UNSUPPORTED_BOARDTYPE	8		/*!< \brief Unsupported board type in DevID */
#define BHT_ERR_UNSUPPORTED_CHANNELTYPE 9		/*!< \brief Unsupported channel type in DevID */
#define BHT_ERR_CANT_OPEN_DRIVER		10		/*!< \brief Can't open driver */
#define BHT_ERR_CANT_SET_DRV_OPTIONS	11		/*!< \brief Can't set driver options */
#define BHT_ERR_CANT_GET_DEV_INFO		12		/*!< \brief Can't get device info */
#define BHT_ERR_INVALID_BOARD_NUM		13		/*!< \brief Invalid board number */
#define BHT_ERR_INVALID_CHANNEL_NUM		14		/*!< \brief Invalid channel number */
#define BHT_ERR_DRIVER_READ_FAIL		15		/*!< \brief Driver read memory failure */
#define BHT_ERR_DRIVER_WRITE_FAIL		16		/*!< \brief Driver write memory failure */
#define BHT_ERR_DEVICE_CLOSE_FAIL		17		/*!< \brief Device close failure */
#define BHT_ERR_DRIVER_CLOSE_FAIL		18		/*!< \brief Driver close failure */
#define BHT_ERR_KP_OPEN_FAIL     		19		/*!< \brief Kernel Plug-In Open failure */
#define BHT_ERR_WINDRIVER_INIT_FAIL     20		/*!< \brief WinDriver lib init failure */
#define BHT_ERR_MEM_ALLOC_FAIL          21		/*!< \brief malloc failure */
#define BHT_ERR_DRIVER_INT_ATTACH_FAIL  22
#define BHT_ERR_DRIVER_INT_DETACH_FAIL  23

#ifndef NULL
#define NULL 0UL
#endif

#ifndef OK
#define OK 0UL
#endif

#ifndef ERROR
#define ERROR -1
#endif

 #define BHT_FALSE                   0
 #define BHT_TRUE                    1

#define BHT_WAITFOREVER             (-1)

typedef unsigned char    bht_L0_u8;
typedef unsigned short   bht_L0_u16;
typedef unsigned int     bht_L0_u32;
typedef signed int       bht_L0_s32;
typedef unsigned long    bht_L0_u64;
typedef signed int       bht_L0_sem;
typedef signed int       bht_L0_b_sem;
typedef signed int       bht_L0_m_sem;
typedef signed int       bht_L0_c_sem;

typedef enum
{
    BHT_L0_FALSE = 0,
    BHT_L0_TRUE = 1 
};



typedef void (*BHT_L0_USER_ISRFUNC)(void*);

#ifndef _BITS_H_
enum 
{
    BIT0  = 0x00000001, BIT1  = 0x00000002, BIT2  = 0x00000004, BIT3  = 0x00000008,
    BIT4  = 0x00000010, BIT5  = 0x00000020, BIT6  = 0x00000040, BIT7  = 0x00000080,
    BIT8  = 0x00000100, BIT9  = 0x00000200, BIT10 = 0x00000400, BIT11 = 0x00000800,
    BIT12 = 0x00001000, BIT13 = 0x00002000, BIT14 = 0x00004000, BIT15 = 0x00008000,
    BIT16 = 0x00010000, BIT17 = 0x00020000, BIT18 = 0x00040000, BIT19 = 0x00080000,
    BIT20 = 0x00100000, BIT21 = 0x00200000, BIT22 = 0x00400000, BIT23 = 0x00800000,
    BIT24 = 0x01000000, BIT25 = 0x02000000, BIT26 = 0x04000000, BIT27 = 0x08000000,
    BIT28 = 0x10000000, BIT29 = 0x20000000, BIT30 = 0x40000000,
    BIT31 = 0x80000000
};
#endif

__declspec(dllexport)  void 
bht_L0_msleep(bht_L0_u32 msdelay);

__declspec(dllexport)  bht_L0_u32 
bht_L0_map_memory(bht_L0_u32 dev_id, 
        void * arg);

__declspec(dllexport)  bht_L0_u32 
bht_L0_unmap_memory(bht_L0_u32 dev_id);

__declspec(dllexport)  bht_L0_u32 
bht_L0_read_mem32(bht_L0_u32 dev_id, 
        bht_L0_u32 offset, 
        bht_L0_u32 *data, 
        bht_L0_u32 count);
        
__declspec(dllexport)  bht_L0_u32 
bht_L0_read_mem32_dma(bht_L0_u32 dev_id, 
        bht_L0_u32 offset, 
        bht_L0_u32 *data, 
        bht_L0_u32 count);
        
__declspec(dllexport)  bht_L0_u32 
bht_L0_write_mem32(bht_L0_u32 dev_id, 
        bht_L0_u32 offset, 
        bht_L0_u32 *data, 
        bht_L0_u32 count);
        
__declspec(dllexport)  bht_L0_u32 
bht_L0_read_mem16(bht_L0_u32 dev_id, 
        bht_L0_u32 offset, 
        bht_L0_u16 *data, 
        bht_L0_u32 count);
        
__declspec(dllexport)  bht_L0_u32 
bht_L0_write_mem16(bht_L0_u32 dev_id, 
        bht_L0_u32 offset, 
        bht_L0_u16 *data, 
        bht_L0_u32 count);
        
__declspec(dllexport)  bht_L0_u32 
bht_L0_read_setupmem32(bht_L0_u32 dev_id, 
        bht_L0_u32 offset, 
        bht_L0_u32 *data, 
        bht_L0_u32 count);
        
__declspec(dllexport)  bht_L0_u32 
bht_L0_write_setupmem32(bht_L0_u32 dev_id, 
        bht_L0_u32 offset, 
        bht_L0_u32 *data, 
        bht_L0_u32 count);

__declspec(dllexport)  bht_L0_u32 
bht_L0_read_setupmem16(bht_L0_u32 dev_id, 
        bht_L0_u32 offset, 
        bht_L0_u16 *data, 
        bht_L0_u32 count);

__declspec(dllexport)  bht_L0_u32 
bht_L0_write_setupmem16(bht_L0_u32 dev_id, 
        bht_L0_u32 offset, 
        bht_L0_u16 *data, 
        bht_L0_u32 count);
        
__declspec(dllexport)  bht_L0_u32 
bht_L0_attach_inthandler(bht_L0_u32 dev_id, 
        bht_L0_u32 chan_regoffset, 
        BHT_L0_USER_ISRFUNC isr, 
        void * arg);

__declspec(dllexport)  bht_L0_u32 
bht_L0_detach_inthandler(bht_L0_u32 dev_id);

__declspec(dllexport)  bht_L0_sem
bht_L0_semc_create(bht_L0_u32 initial_cnt, bht_L0_u32 max_cnt);

__declspec(dllexport)  bht_L0_u32 
bht_L0_sem_take(bht_L0_sem sem, bht_L0_s32 timeout_ms);

__declspec(dllexport)  bht_L0_u32
bht_L0_sem_give(bht_L0_sem sem);


#ifdef __cplusplus
}
#endif

#endif
