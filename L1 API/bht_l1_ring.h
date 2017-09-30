#ifndef __BHT_L1_RING_H__
#define __BHT_L1_RING_H__

#ifdef __cplusplus
extern "C" {
#endif

/* this ring buffer method can be used in single producer, multi customers */

struct ring_buf
{
    unsigned char * base;
    unsigned int item_size;
    unsigned int item_cnt;
    unsigned int item_in;
    unsigned int item_out;
};

struct ring_buf * 
ring_buf_init(unsigned int item_size, 
        unsigned int item_cnt);

unsigned char *
ring_buf_put(struct ring_buf *r, 
        unsigned char * item);

unsigned int 
ring_buf_get(struct ring_buf *r, 
        unsigned char * item);

void 
ring_buf_free(struct ring_buf * r);

#ifdef __cplusplus
}
#endif

#endif
