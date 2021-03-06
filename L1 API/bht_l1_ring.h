#ifndef __BHT_L1_RING_H__
#define __BHT_L1_RING_H__

#ifdef __cplusplus
extern "C" {
#endif

/* this ring buffer method can be used in single producer, multi customers */

/* ring buf define */
//#define RING_BUF_DEFINE(_name, _element)
struct ring_buf
{
    unsigned char * base;
    unsigned int item_size;
    unsigned int item_cnt;
    unsigned int item_in;
    unsigned int item_out;
};

#if 1
extern struct ring_buf * ring_buf_init(struct ring_buf *r, unsigned char *base, unsigned int item_size, unsigned int item_cnt);


extern unsigned char * ring_buf_put(struct ring_buf *r, unsigned char * item);

extern unsigned int ring_buf_get(struct ring_buf *r, unsigned char * item);

#else
static inline struct ring_buf * ring_buf_init(struct ring_buf *r, unsigned char *base, unsigned int item_size, unsigned int item_cnt)
{
//    struct ring_buf * r = (struct ring_buf *)malloc(sizeof(struct ring_buf));
    
    if(item_cnt & (item_cnt - 1) != 0)
        return NULL;
    
    if(NULL != r)
    {
        r->item_size = item_size;
        r->item_cnt  = item_cnt;
        r->item_in = 0;
        r->item_out = 0;
        r->base = base;
        r->base = (unsigned char *) malloc(item_cnt * item_size);
        if(NULL == r->base)
        {
            free(r);
            r = NULL;
        }
    }
    
    return r;
}

static inline unsigned char * ring_buf_put(struct ring_buf *r, unsigned char * item)
{
    unsigned int out_temp;
    
    if((NULL == item) || (NULL == r) || (NULL == r->base))
        return NULL;
    
    out_temp = r->item_out;
    
    /* not full, then put */
    if((r->item_in - out_temp) == r->item_cnt))
        return NULL;
    
    memcpy((base + r->item_size * (r->item_in & (r->item_cnt - 1))), item, r->item_size);
    
    r->item_in++;
    
    return item;
}

static inline unsigned int ring_buf_get(struct ring_buf *r, unsigned char * item)
{
    unsigned int in_temp;
    
    if((NULL == item) || (NULL == r) || (NULL == r->base))
        return 0;
    
    in_temp = r->item_in;
    
    /* not empty, the get */
    if(r->item_in == r->item_out)
        return 0;
    
    memcpy(item, (base + r->item_size * (r->item_out & (r->item_cnt - 1))), r->item_size);
    
    r->item_out++;
    
    return r->item_size;
}

static inline void ring_buf_free(struct ring_buf * r)
{
    if(r)
    {
        if(r->base)
            free(r->base);
        free(r);
    }
}
#endif

/* ring buf initialize */
//#define RING_BUF_INIT(_name, _element, _size, )





#ifdef __cplusplus
}
#endif

#endif
