#ifndef __BHT_L0_TYPES_H__
#define __BHT_L0_TYPES_H__

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
typedef signed int       bht_L0_bool;
typedef signed int       bht_L0_sem;

typedef enum
{
    BHT_L0_FALSE = 0,
    BHT_L0_TRUE = 1 
};

typedef enum
{
    BHT_L0_DEVICE_TYPE_PMCA429 = 0x00,
    BHT_L0_DEVICE_TYPE_PCIA429,
    BHT_L0_DEVICE_TYPE_CPCIA429,
    BHT_L0_DEVICE_TYPE_PXIA429,
    BHT_L0_DEVICE_TYPE_PMC1553,
    BHT_L0_DEVICE_TYPE_UNINITIALIZED,
    BHT_L0_DEVICE_TYPE_MAX
}bht_L0_dtype_e;

typedef enum
{
    BHT_L0_INTERFACE_TYPE_SIM = 0,
    BHT_L0_INTERFACE_TYPE_PCI,
    BHT_L0_INTERFACE_TYPE_PCIE,
    BHT_L0_INTERFACE_TYPE_ENET
}bht_L0_itype_e;

typedef enum
{
    BHT_L0_LOGIC_TYPE_A429 = 0,
    BHT_L0_LOGIC_TYPE_1553 = 1
}bht_L0_ltype_e;

typedef struct
{
    bht_L0_dtype_e dtype;
    bht_L0_itype_e itype;
    bht_L0_ltype_e ltype;
}bht_L0_dtypeinfo_t;

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

typedef struct _bht_L0_device_t_
{
	bht_L0_dtype_e dtype;
	bht_L0_itype_e itype;
	bht_L0_ltype_e ltype;
	bht_L0_u32 device_no;

	void *lld_hand;
	bht_L0_sem mutex_sem;

	bht_L0_u32 (*reset) (struct _bht_L0_device_t_*);

	void *private;
}bht_L0_device_t;

#endif