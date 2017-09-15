#ifndef __BHT_L0_DEVICE_H__
#define __BHT_L0_DEVICE_H__

#include <bht_L0.h>

typedef struct bht_L0_device_t
{
    bht_L0_dtype_e dtype;
    bht_L0_itype_e itype;
    bht_L0_ltype_e ltype;
    bht_L0_u32 device_no;

    void *lld_hand;
    bht_L0_sem mutex_sem;
    bht_L0_s32 last_err;
    bht_L0_u32 err_count;

    void (*reset_hook)(bht_L0_device_t);

    void *private;
}bht_L0_device_t;

#endif
