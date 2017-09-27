#ifndef __BHT_L1_DEF_H__
#define __BHT_L1_DEF_H__

#define BHT_L1_READ_MEM32(device, offset, pdata, count, result, error_label)\
    do\
    {\
        bht_L0_u32 _ret;\
		if(BHT_SUCCESS != (_ret = bht_L0_read_mem32(device, offset, (pdata), count)))\
        {\
            result = _ret;\
			goto error_label;\
        }\
	}while(0)
#define BHT_L1_WRITE_MEM32(device, offset, pdata, count, result, error_label)\
    do\
    {\
        bht_L0_u32 _ret;\
		if(BHT_SUCCESS != (_ret = bht_L0_write_mem32(device, offset, (pdata), count)))\
		{\
            result = _ret;\
			goto error_label;\
        }\
	}while(0)
#define BHT_L1_SEM_TAKE(sem, timeout_ms, result, error_label)\
	do\
	{\
	    bht_L0_u32 _ret;\
		if(BHT_SUCCESS != (_ret = bht_L0_sem_take(sem, timeout_ms)))\
		{\
            result = _ret;\
			goto error_label;\
        }\
	}while(0)
#define BHT_L1_SEM_GIVE(sem, result, error_label)\
	do\
	{\
	    bht_L0_u32 _ret;\
		if(BHT_SUCCESS != (_ret = bht_L0_sem_give(sem)))\
		{\
            result = _ret;\
			goto error_label;\
        }\
	}while(0)

#endif