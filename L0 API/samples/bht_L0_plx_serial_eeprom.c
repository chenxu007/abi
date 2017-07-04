/* bht_L0_serial_eeprom.c */

/*
 * Copyright (c) 2017 Bin Hong Tech, Inc.
 *
 * The right to copy, distribute, modify or otherwise make use
 * of this software may be licensed only pursuant to the terms
 * of an applicable Wind River license agreement.
 */

/*
modification history
--------------------
01a,17may17,cx_  add file
*/

#include <bht_L0.h>
#include <bht_L1.h>
#include <bht_L0_plx9056.h>
#include <bht_L1_a429.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <fcntl.h>

#define EESK  0x01000000   /* bit 24 RW */
#define EECS  0x02000000   /* bit 25 RW */
#define EEDI  0x04000000   /* bit 26 RW */
#define EEDO  0x08000000   /* bit 27 RO */
#define EEDS  0x80000000   /* bit 31 RW */
#define DEVID BHT_DEVID_BACKPLANETYPE_PCI | BHT_DEVID_BOARDTYPE_PMCA429 | BHT_DEVID_BOARDNUM_01 
#define EEPROM_SIZE 128
#define ACTION_READ_ALL
//#define ACTION_ERASE
//#define ACTION_INIT_EEPROM

int hatoi(bht_L0_u8* str, bht_L0_u32 length)     
{     
    bht_L0_u8  revstr[16]={0};  //����ʮ�������ַ����ĳ��ȣ�����ע�����鲻ҪԽ��  
    bht_L0_u32   num[16]={0};     
    bht_L0_u32   count=1;     
    bht_L0_u32   result=0;  
	bht_L0_s32 i;

    strcpy(revstr,str);

    for   (i = length-1; i >= 0; i--)     
    {     
        if ((revstr[i]>='0') && (revstr[i]<='9'))     
            num[i]=revstr[i]-48;//�ַ�0��ASCIIֵΪ48  
        else if ((revstr[i]>='a') && (revstr[i]<='f'))     
            num[i]=revstr[i]-'a'+10;     
        else if ((revstr[i]>='A') && (revstr[i]<='F'))     
            num[i]=revstr[i]-'A'+10;     
        else     
            num[i]=0;   
        result=result+num[i]*count;     
        count=count*16;//ʮ������(����ǰ˽��ƾ����������8)      
    }     
    return result;     
}

bht_L0_u32 
bht_L0_plx_eeprom_read(bht_L0_u32 dev_id,
        bht_L0_u32 addr,
        bht_L0_u16 *buf, 
        bht_L0_u32 count)
{
    bht_L0_u32 copy;  
    bht_L0_u32 i,x;

    bht_L0_read_setupmem32 (dev_id, PLX9056_CNTRL, &copy, 1);

    /* clear current state (step 1) */
    /* (1) Clear the Input Enable Bits */
    copy &= ~EECS;
    copy &= ~EESK;
    copy &= ~EEDI;
    copy &= ~EEDO;
    copy &= ~EEDS;

    bht_L0_write_setupmem32 (dev_id,  PLX9056_CNTRL,&copy, 1);

    /* select chip  (step 2 3) */
    /* (2) Set EECS high */
    copy |=  EECS;

    bht_L0_write_setupmem32 (dev_id,  PLX9056_CNTRL,&copy, 1);

    /* (3) Toggle EESK high, then low */
    copy |=  EESK;

    bht_L0_write_setupmem32 (dev_id,  PLX9056_CNTRL,&copy, 1);

    copy &= ~EESK;

    bht_L0_write_setupmem32 (dev_id,  PLX9056_CNTRL,&copy, 1);

    /* start bit 1 (step 4,5) */
    /* (4) Set the EEDI bit high (Start bit) */
    copy |=  EEDI;

    bht_L0_write_setupmem32 (dev_id,  PLX9056_CNTRL,&copy, 1);

    /* (5) Toggle EESK high, then low */
    copy |=  EESK;

    bht_L0_write_setupmem32 (dev_id,  PLX9056_CNTRL,&copy, 1);

    copy &= ~EESK;

    bht_L0_write_setupmem32 (dev_id,  PLX9056_CNTRL,&copy, 1);

    /* opcode 10 read (step 6,7,8) */
    /* (6) Toggle EESK high, then low */
    copy |=  EESK;

    bht_L0_write_setupmem32 (dev_id,  PLX9056_CNTRL,&copy, 1);

    copy &= ~EESK;

    bht_L0_write_setupmem32 (dev_id,  PLX9056_CNTRL,&copy, 1);


    /* (7) Clear EEDI */
    copy &= (~EEDI);

    bht_L0_write_setupmem32 (dev_id,  PLX9056_CNTRL,&copy, 1);

    /* (8) Toggle EESK high, then low */
    copy |=  EESK;

    bht_L0_write_setupmem32 (dev_id,  PLX9056_CNTRL,&copy, 1);

    copy &= ~EESK;

    bht_L0_write_setupmem32 (dev_id,  PLX9056_CNTRL,&copy, 1);

    /* address A7~A0 for 8 times EESK (EEDI is 0) */
    /* (9) Toggle EESK high, then low, 8 times (serial EEPROM address 0) */
    for (i = 0; i< 8; i++)
    {
        if((addr<<i) & 0x80)
           copy|=EEDI;
        else
           copy&=~EEDI;


        bht_L0_write_setupmem32 (dev_id,  PLX9056_CNTRL,&copy, 1);

        copy |=  EESK;

        bht_L0_write_setupmem32 (dev_id,  PLX9056_CNTRL,&copy, 1);

        copy &= ~EESK;

        bht_L0_write_setupmem32 (dev_id,  PLX9056_CNTRL,&copy, 1);

    }

    /* Enable EEDO */
    /* (10) Set the EEDO Input Enable to 1 to float the EEDO input for reading */
    copy |=EEDS;

    bht_L0_write_setupmem32 (dev_id,  PLX9056_CNTRL,&copy, 1);


    /* (11) Toggle EESK high, then low, 16 times, (12) reading in a bit after each clock */
    for (i = 0; i < count; i++) 
    {
        buf[i] = 0;
        for (x = 0; x < 16; x++)
        {
            buf[i] = buf[i] << 1;
            
            copy |=  EESK;

            bht_L0_write_setupmem32 (dev_id,  PLX9056_CNTRL,&copy, 1);

            copy &= ~EESK;

            bht_L0_write_setupmem32 (dev_id,  PLX9056_CNTRL,&copy, 1);

            /*bht_L0_read_setupmem32 (PCI_BAR0(cardnum) + PLX9056_CNTRL, &readValue); 	*/	
            bht_L0_read_setupmem32 (dev_id, PLX9056_CNTRL, &copy, 1);

            if (copy & EEDO)
            {
                buf[i] |= 0x01;
            }
            else
            {
                buf[i] &= 0xFFFE;
            }
        }
    }

    /* (13) Clear the EECS bit */
    copy &= ~EECS;

    bht_L0_write_setupmem32 (dev_id,  PLX9056_CNTRL,&copy, 1);

    /* (14) Toggle EESK high, then low.  The read is complete */
    copy |=  EESK;

    bht_L0_write_setupmem32 (dev_id,  PLX9056_CNTRL,&copy, 1);

    copy &= ~EESK;

    bht_L0_write_setupmem32 (dev_id,  PLX9056_CNTRL,&copy, 1);


    /* clear current state (step 1) */
    /* (1) Clear the Input Enable Bits */
//    copy &= ~EECS;
//    copy &= ~EESK;
//    copy &= ~EEDI;
//    copy &= ~EEDS;

    /* bht_L0_write_setupmem32 (PCI_BAR0(cardnum) + PLX9056_CNTRL, copy);  */ 
//    bht_L0_write_setupmem32 (dev_id,  PLX9056_CNTRL,&copy, 1);

    return BHT_SUCCESS;
}

bht_L0_u32 
bht_L0_plx_eeprom_write_enable(bht_L0_u32 dev_id)
{
    bht_L0_u32 copy;  
    bht_L0_u32 i,x;

    bht_L0_read_setupmem32 (dev_id, PLX9056_CNTRL, &copy, 1);    

    /* clear current state (step 1) */
    /* (1) Clear the Input Enable Bits */
    copy &= ~EECS;
    copy &= ~EESK;
    copy &= ~EEDI;
    copy &= ~EEDO;
    copy &= ~EEDS;

    bht_L0_write_setupmem32 (dev_id,  PLX9056_CNTRL,&copy, 1);

    /* select chip  (step 2 3) */
    /* (2) Set EECS high */
    copy |=  EECS;

    bht_L0_write_setupmem32 (dev_id,  PLX9056_CNTRL,&copy, 1);

    /* (3) Toggle EESK high, then low */
    copy |=  EESK;

    bht_L0_write_setupmem32 (dev_id,  PLX9056_CNTRL,&copy, 1);

    copy &= ~EESK;

    bht_L0_write_setupmem32 (dev_id,  PLX9056_CNTRL,&copy, 1);

    /* start bit 1 (step 4,5) */
    /* (4) Set the EEDI bit high (Start bit) */
    copy |=  EEDI;

    bht_L0_write_setupmem32 (dev_id,  PLX9056_CNTRL,&copy, 1);

    /* (5) Toggle EESK high, then low */
    copy |=  EESK;

    bht_L0_write_setupmem32 (dev_id,  PLX9056_CNTRL,&copy, 1);

    copy &= ~EESK;

    bht_L0_write_setupmem32 (dev_id,  PLX9056_CNTRL,&copy, 1);

    /* opcode 00 write enable */
    /* (6) Toggle EESK high, then low */
    copy &=  (~EEDI);

    bht_L0_write_setupmem32 (dev_id,  PLX9056_CNTRL,&copy, 1);
    
    copy |=  EESK;

    bht_L0_write_setupmem32 (dev_id,  PLX9056_CNTRL,&copy, 1);

    copy &= ~EESK;

    bht_L0_write_setupmem32 (dev_id,  PLX9056_CNTRL,&copy, 1);

    /* (8) Toggle EESK high, then low */
    copy |=  EESK;

    bht_L0_write_setupmem32 (dev_id,  PLX9056_CNTRL,&copy, 1);

    copy &= ~EESK;

    bht_L0_write_setupmem32 (dev_id,  PLX9056_CNTRL,&copy, 1);

    /* address A7~A0 for 8 times EESK (EEDI is 0) */
    /* (9) Toggle EESK high, then low, 8 times (serial EEPROM address 0) */
    copy |= EEDI;

    bht_L0_write_setupmem32 (dev_id,  PLX9056_CNTRL,&copy, 1);

    for (i = 0; i< 8; i++)
    {
        copy |=  EESK;

        bht_L0_write_setupmem32 (dev_id,  PLX9056_CNTRL,&copy, 1);

        copy &= ~EESK;

        bht_L0_write_setupmem32 (dev_id,  PLX9056_CNTRL,&copy, 1);
    }

    /* Clear the EECS bit */
    copy &= ~EECS;
    bht_L0_write_setupmem32 (dev_id,  PLX9056_CNTRL,&copy, 1);

    return BHT_SUCCESS;
}

bht_L0_u32 
bht_L0_plx_eeprom_write_disable(bht_L0_u32 dev_id)
{
    bht_L0_u32 copy;  
    bht_L0_u32 i,x;

    bht_L0_read_setupmem32 (dev_id, PLX9056_CNTRL, &copy, 1);    

    /* clear current state (step 1) */
    /* (1) Clear the Input Enable Bits */
    copy &= ~EECS;
    copy &= ~EESK;
    copy &= ~EEDI;
    copy &= ~EEDO;
    copy &= ~EEDS;

    bht_L0_write_setupmem32 (dev_id,  PLX9056_CNTRL,&copy, 1);

    /* select chip  (step 2 3) */
    /* (2) Set EECS high */
    copy |=  EECS;

    bht_L0_write_setupmem32 (dev_id,  PLX9056_CNTRL,&copy, 1);

    /* (3) Toggle EESK high, then low */
    copy |=  EESK;

    bht_L0_write_setupmem32 (dev_id,  PLX9056_CNTRL,&copy, 1);

    copy &= ~EESK;

    bht_L0_write_setupmem32 (dev_id,  PLX9056_CNTRL,&copy, 1);

    /* start bit 1 (step 4,5) */
    /* (4) Set the EEDI bit high (Start bit) */
    copy |=  EEDI;

    bht_L0_write_setupmem32 (dev_id,  PLX9056_CNTRL,&copy, 1);

    /* (5) Toggle EESK high, then low */
    copy |=  EESK;

    bht_L0_write_setupmem32 (dev_id,  PLX9056_CNTRL,&copy, 1);

    copy &= ~EESK;

    bht_L0_write_setupmem32 (dev_id,  PLX9056_CNTRL,&copy, 1);

    /* opcode 00 write disable */
    /* (6) Toggle EESK high, then low */
    copy &=  (~EEDI);

    bht_L0_write_setupmem32 (dev_id,  PLX9056_CNTRL,&copy, 1);
    
    copy |=  EESK;

    bht_L0_write_setupmem32 (dev_id,  PLX9056_CNTRL,&copy, 1);

    copy &= ~EESK;

    bht_L0_write_setupmem32 (dev_id,  PLX9056_CNTRL,&copy, 1);

    /* (8) Toggle EESK high, then low */
    copy |=  EESK;

    bht_L0_write_setupmem32 (dev_id,  PLX9056_CNTRL,&copy, 1);

    copy &= ~EESK;

    bht_L0_write_setupmem32 (dev_id,  PLX9056_CNTRL,&copy, 1);

    /* address A7~A0 for 8 times EESK (EEDI is 0) */
    /* (9) Toggle EESK high, then low, 8 times (serial EEPROM address 0) */
    copy &= (~EEDI);

    bht_L0_write_setupmem32 (dev_id,  PLX9056_CNTRL,&copy, 1);

    for (i = 0; i< 8; i++)
    {
        copy |=  EESK;

        bht_L0_write_setupmem32 (dev_id,  PLX9056_CNTRL,&copy, 1);

        copy &= ~EESK;

        bht_L0_write_setupmem32 (dev_id,  PLX9056_CNTRL,&copy, 1);
    }

    /* Clear the EECS bit */
    copy &= ~EECS;
    bht_L0_write_setupmem32 (dev_id,  PLX9056_CNTRL,&copy, 1);

    return BHT_SUCCESS;
}

bht_L0_u32 
bht_L0_plx_eeprom_reload(bht_L0_u32 dev_id)
{
    bht_L0_u32 copy;  
    bht_L0_u32 i,x;

    bht_L0_read_setupmem32 (dev_id, PLX9056_CNTRL, &copy, 1);

    copy |= PLX9056_CNTRL_RELOAD_CONFIG;
    copy |= PLX9056_CNTRL_RESET;
    bht_L0_write_setupmem32 (dev_id, PLX9056_CNTRL, &copy, 1);

    bht_L0_msleep(1);

    copy &= (~PLX9056_CNTRL_RESET);
    bht_L0_write_setupmem32 (dev_id, PLX9056_CNTRL, &copy, 1);

    /* wait initialize complete */
    bht_L0_msleep(1000);

    /* clear reload bit */
    copy &= (~PLX9056_CNTRL_RELOAD_CONFIG);
    bht_L0_write_setupmem32 (dev_id, PLX9056_CNTRL, &copy, 1);
//    do
//    {
//        bht_L0_write_setupmem32 (dev_id, PLX9056_CNTRL, &copy, 1);
//    }while();
    
}



bht_L0_u32 
bht_L0_plx_eeprom_write(bht_L0_u32 dev_id,
        bht_L0_u32 addr,
        bht_L0_u16 *buf, 
        bht_L0_u32 count)
{
    bht_L0_u32 copy;  
    bht_L0_u16 *promValuesPtr = buf;   
    bht_L0_u32 i,x;

    bht_L0_read_setupmem32 (dev_id, PLX9056_CNTRL, &copy, 1);    

    /* clear current state (step 1) */
    /* (1) Clear the Input Enable Bits */
    copy &= ~EECS;
    copy &= ~EESK;
    copy &= ~EEDI;
    copy &= ~EEDO;
    copy &= ~EEDS;

    bht_L0_write_setupmem32 (dev_id,  PLX9056_CNTRL,&copy, 1);

    /* select chip  (step 2 3) */
    /* (2) Set EECS high */
    copy |=  EECS;

    bht_L0_write_setupmem32 (dev_id,  PLX9056_CNTRL,&copy, 1);

    /* (3) Toggle EESK high, then low */
    copy |=  EESK;

    bht_L0_write_setupmem32 (dev_id,  PLX9056_CNTRL,&copy, 1);

    copy &= ~EESK;

    bht_L0_write_setupmem32 (dev_id,  PLX9056_CNTRL,&copy, 1);

    /* start bit 1 (step 4,5) */
    /* (4) Set the EEDI bit high (Start bit) */
    copy |=  EEDI;

    bht_L0_write_setupmem32 (dev_id,  PLX9056_CNTRL,&copy, 1);

    /* (5) Toggle EESK high, then low */
    copy |=  EESK;

    bht_L0_write_setupmem32 (dev_id,  PLX9056_CNTRL,&copy, 1);

    copy &= ~EESK;

    bht_L0_write_setupmem32 (dev_id,  PLX9056_CNTRL,&copy, 1);

    /* opcode 10 read (step 6,7,8) */
    /* (6) Toggle EESK high, then low */
    copy &=  (~EEDI);

    bht_L0_write_setupmem32 (dev_id,  PLX9056_CNTRL,&copy, 1);
    
    copy |=  EESK;

    bht_L0_write_setupmem32 (dev_id,  PLX9056_CNTRL,&copy, 1);

    copy &= ~EESK;

    bht_L0_write_setupmem32 (dev_id,  PLX9056_CNTRL,&copy, 1);


    /* (7) Clear EEDI */
    copy |= (EEDI);

    bht_L0_write_setupmem32 (dev_id,  PLX9056_CNTRL,&copy, 1);

    /* (8) Toggle EESK high, then low */
    copy |=  EESK;

    bht_L0_write_setupmem32 (dev_id,  PLX9056_CNTRL,&copy, 1);

    copy &= ~EESK;

    bht_L0_write_setupmem32 (dev_id,  PLX9056_CNTRL,&copy, 1);

    /* address A7~A0 for 8 times EESK (EEDI is 0) */
    /* (9) Toggle EESK high, then low, 8 times (serial EEPROM address 0) */
    for (i = 0; i< 8; i++)
    {
        if((addr<<i) & 0x80)
           copy|=EEDI;
        else
           copy&=~EEDI;


        bht_L0_write_setupmem32 (dev_id,  PLX9056_CNTRL,&copy, 1);

        copy |=  EESK;

        bht_L0_write_setupmem32 (dev_id,  PLX9056_CNTRL,&copy, 1);

        copy &= ~EESK;

        bht_L0_write_setupmem32 (dev_id,  PLX9056_CNTRL,&copy, 1);

    }

    /* Enable EEDO */
    /* (10) Set the EEDO Input Enable to 1 to float the EEDO input for reading */
//    copy |=EEDS;

//    bht_L0_write_setupmem32 (dev_id,  PLX9056_CNTRL,&copy, 1);


    /* (11) Toggle EESK high, then low, 16 times, (12) reading in a bit after each clock */
    for (i = 0; i < count; i++) 
    {
//        buf[i] = 0;
        for (x = 0; x < 16; x++)
        {
            if((buf[i] << x) & 0x8000)
                copy |= EEDI;
            else
                copy &= ~EEDI;

            bht_L0_write_setupmem32 (dev_id,  PLX9056_CNTRL,&copy, 1);
            
            copy |=  EESK;

            bht_L0_write_setupmem32 (dev_id,  PLX9056_CNTRL,&copy, 1);

            copy &= ~EESK;

            bht_L0_write_setupmem32 (dev_id,  PLX9056_CNTRL,&copy, 1);
        }
    }

    /* (13) Clear the EECS bit */
    copy &= ~EECS;

    bht_L0_write_setupmem32 (dev_id,  PLX9056_CNTRL,&copy, 1);

    /* Enable EEDO */
    copy |= EEDS;

    bht_L0_write_setupmem32 (dev_id,  PLX9056_CNTRL,&copy, 1);

    copy |= EECS;

    bht_L0_write_setupmem32 (dev_id,  PLX9056_CNTRL,&copy, 1);

    /* wait write complete */
    do
    {
        bht_L0_read_setupmem32 (dev_id,  PLX9056_CNTRL,&copy, 1);
    }while(!(copy & EEDO));

    /* (14) Toggle EESK high, then low.  The read is complete */
//    copy |=  EESK;

//    bht_L0_write_setupmem32 (dev_id,  PLX9056_CNTRL,&copy, 1);

//    copy &= ~EESK;

//    bht_L0_write_setupmem32 (dev_id,  PLX9056_CNTRL,&copy, 1);


    /* clear current state (step 1) */
    /* (1) Clear the Input Enable Bits */
    copy &= ~EECS;
    copy &= ~EESK;
    copy &= ~EEDI;
    copy &= ~EEDS;

    /* bht_L0_write_setupmem32 (PCI_BAR0(cardnum) + PLX9056_CNTRL, copy);  */ 
    bht_L0_write_setupmem32 (dev_id,  PLX9056_CNTRL,&copy, 1);

    return BHT_SUCCESS;
}

bht_L0_u32 
bht_L0_plx_eeprom_write_all(bht_L0_u32 dev_id,
        bht_L0_u16 value)
{
    bht_L0_u32 copy;  
    bht_L0_u32 i,x;

    bht_L0_read_setupmem32 (dev_id, PLX9056_CNTRL, &copy, 1);    

    /* clear current state (step 1) */
    /* (1) Clear the Input Enable Bits */
    copy &= ~EECS;
    copy &= ~EESK;
    copy &= ~EEDI;
    copy &= ~EEDO;
    copy &= ~EEDS;

    bht_L0_write_setupmem32 (dev_id,  PLX9056_CNTRL,&copy, 1);

    /* select chip  (step 2 3) */
    /* (2) Set EECS high */
    copy |=  EECS;

    bht_L0_write_setupmem32 (dev_id,  PLX9056_CNTRL,&copy, 1);

    /* (3) Toggle EESK high, then low */
    copy |=  EESK;

    bht_L0_write_setupmem32 (dev_id,  PLX9056_CNTRL,&copy, 1);

    copy &= ~EESK;

    bht_L0_write_setupmem32 (dev_id,  PLX9056_CNTRL,&copy, 1);

    /* start bit 1 (step 4,5) */
    /* (4) Set the EEDI bit high (Start bit) */
    copy |=  EEDI;

    bht_L0_write_setupmem32 (dev_id,  PLX9056_CNTRL,&copy, 1);

    /* (5) Toggle EESK high, then low */
    copy |=  EESK;

    bht_L0_write_setupmem32 (dev_id,  PLX9056_CNTRL,&copy, 1);

    copy &= ~EESK;

    bht_L0_write_setupmem32 (dev_id,  PLX9056_CNTRL,&copy, 1);

    /* opcode 00 write all (step 6,7,8) */
    /* (6) Toggle EESK high, then low */
    copy &=  (~EEDI);

    bht_L0_write_setupmem32 (dev_id,  PLX9056_CNTRL,&copy, 1);
    
    copy |=  EESK;

    bht_L0_write_setupmem32 (dev_id,  PLX9056_CNTRL,&copy, 1);

    copy &= ~EESK;

    bht_L0_write_setupmem32 (dev_id,  PLX9056_CNTRL,&copy, 1);

    /* (8) Toggle EESK high, then low */
    copy |=  EESK;

    bht_L0_write_setupmem32 (dev_id,  PLX9056_CNTRL,&copy, 1);

    copy &= ~EESK;

    bht_L0_write_setupmem32 (dev_id,  PLX9056_CNTRL,&copy, 1);

    /* address A7~A0 for 8 times EESK (EEDI is 0) */
    /* (9) Toggle EESK high, then low, 8 times (serial EEPROM address 0) */
    copy |=  EESK;

    bht_L0_write_setupmem32 (dev_id,  PLX9056_CNTRL,&copy, 1);

    copy &= ~EESK;

    bht_L0_write_setupmem32 (dev_id,  PLX9056_CNTRL,&copy, 1);
    
    copy |= EEDI;

    bht_L0_write_setupmem32 (dev_id,  PLX9056_CNTRL,&copy, 1);
    
    for (i = 0; i < 7; i++)
    {
        copy |=  EESK;

        bht_L0_write_setupmem32 (dev_id,  PLX9056_CNTRL,&copy, 1);

        copy &= ~EESK;

        bht_L0_write_setupmem32 (dev_id,  PLX9056_CNTRL,&copy, 1);

    }


    for (x = 0; x < 16; x++)
    {
        if((value << x) & 0x8000)
            copy |= EEDI;
        else
            copy &= ~EEDI;

        bht_L0_write_setupmem32 (dev_id,  PLX9056_CNTRL,&copy, 1);
        
        copy |=  EESK;

        bht_L0_write_setupmem32 (dev_id,  PLX9056_CNTRL,&copy, 1);

        copy &= ~EESK;

        bht_L0_write_setupmem32 (dev_id,  PLX9056_CNTRL,&copy, 1);
    }

    /* (13) Clear the EECS bit */
    copy &= ~EECS;

    bht_L0_write_setupmem32 (dev_id,  PLX9056_CNTRL,&copy, 1);

    /* Enable EEDO */
    copy |= EEDS;

    bht_L0_write_setupmem32 (dev_id,  PLX9056_CNTRL,&copy, 1);

    /* Toggle EESK high, then low */
    copy |=  EESK;

    bht_L0_write_setupmem32 (dev_id,  PLX9056_CNTRL,&copy, 1);

    copy &= ~EESK;

    bht_L0_write_setupmem32 (dev_id,  PLX9056_CNTRL,&copy, 1);

    copy |= EECS;

    bht_L0_write_setupmem32 (dev_id,  PLX9056_CNTRL,&copy, 1);

    /* wait write complete */
    do
    {
        bht_L0_read_setupmem32 (dev_id,  PLX9056_CNTRL,&copy, 1);
    }while(!(copy & EEDO));

    /* (14) Toggle EESK high, then low.  The read is complete */
//    copy |=  EESK;

//    bht_L0_write_setupmem32 (dev_id,  PLX9056_CNTRL,&copy, 1);

//    copy &= ~EESK;

//    bht_L0_write_setupmem32 (dev_id,  PLX9056_CNTRL,&copy, 1);


    /* clear current state (step 1) */
    /* (1) Clear the Input Enable Bits */
    copy &= ~EECS;
    copy &= ~EESK;
    copy &= ~EEDI;
    copy &= ~EEDS;

    /* bht_L0_write_setupmem32 (PCI_BAR0(cardnum) + PLX9056_CNTRL, copy);  */ 
    bht_L0_write_setupmem32 (dev_id,  PLX9056_CNTRL,&copy, 1);

    return BHT_SUCCESS;
}



int main (void)
{
    int fd;
    int result;
	bht_L0_u32 value, idx;
    bht_L0_u16 value16;
    bht_L0_u32 addr = 0;
    char buffer[100];
    char * filename = "eeprom-value.txt";

    if(BHT_SUCCESS != (result = bht_L0_map_memory(DEVID, NULL)))
        goto plx_eeprom_err;

    /* plx9056 init */
    /* check 9056 initiate stat */
    if(BHT_SUCCESS != (result = bht_L0_read_setupmem32(DEVID, PLX9056_LMISC1, &value, 1)))
        goto plx_eeprom_err;
    
    if((value & BIT2) == 0)
        goto plx_eeprom_err;
    
    /* enable memory space */
    if(BHT_SUCCESS != (result = bht_L0_read_setupmem32(DEVID, PLX9056_PCICR, &value, 1)))
        goto plx_eeprom_err;
    
    value |= BIT1;
    if(BHT_SUCCESS != (result = bht_L0_write_setupmem32(DEVID, PLX9056_PCICR, &value, 1)))
        goto plx_eeprom_err;

    /* config INTCSR */
    value = BIT8 | BIT11 | BIT16 | BIT18 | BIT19;
    if(BHT_SUCCESS != (result = bht_L0_write_setupmem32(DEVID, PLX9056_INTCSR, &value, 1)))
        goto plx_eeprom_err;
    
    /* command 0xc-read mult line; 0xe- read line; 0x6- read */
    value = 0xd767c;
    if(BHT_SUCCESS != (result = bht_L0_write_setupmem32(DEVID, PLX9056_CNTRL, &value, 1)))
        goto plx_eeprom_err;

#ifdef ACTION_READ_ALL

    if(0 > (fd = open("eeprom-value.txt", O_RDWR | O_CREAT | O_TRUNC, 0666)))
	{
	    goto plx_eeprom_err;
	}
    /* read the entire eeprom data */
    for(addr = 0; addr < EEPROM_SIZE; addr += 1)
    {
        bht_L0_plx_eeprom_read(DEVID, addr, &value16, 1);
        
        sprintf(buffer, "%04x\n", value16);

        write(fd, buffer, strlen(buffer));

        printf(buffer);
    }

    close(fd);
#endif	

#ifdef ACTION_ERASE
    value16 = 0xFFFF;
    bht_L0_plx_eeprom_write_enable(DEVID);
	for(addr = 0; addr < EEPROM_SIZE; addr++)
	{
		bht_L0_plx_eeprom_write(DEVID, addr, &value16, 1);
	}
    bht_L0_plx_eeprom_write_disable(DEVID);
#endif

#ifdef ACTION_INIT_EEPROM
    if(0 > (fd = open("I:\\work\\chenxu\\proj\\ABI\\src\\proj\\A429WinDrv\\Debug\\old-eeprom-value.txt",O_BINARY | O_RDONLY, 0666)))
	{
	    goto plx_eeprom_err;
	}
    
    bht_L0_plx_eeprom_write_enable(DEVID);
    
    for(addr = 0; addr < EEPROM_SIZE; addr++)
	{
		memset(buffer, 0x00, sizeof(buffer));
	    /* read value from file */
	    read(fd, buffer, 4);
		value16 = hatoi(buffer, 4);
        /* write to eeprom */
		bht_L0_plx_eeprom_write(DEVID, addr, &value16, 1);
        /* read the '\n' char */
        read(fd, buffer, 2);
	}
    
    bht_L0_plx_eeprom_write_disable(DEVID);

	close(fd);

#endif
    
plx_eeprom_err:

    bht_L0_unmap_memory(DEVID);
    
	system("pause");
    
	return 0;
}



