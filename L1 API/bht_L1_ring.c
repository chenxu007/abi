/* this ring buffer method can be used in single producer, multi customers */

#include <bht_L1_ring.h>
#include <string.h>
#include <stdlib.h>

struct ring_buf * 
ring_buf_init(unsigned int item_size, 
        unsigned int item_cnt)
{
    struct ring_buf *r; 
    
    if((item_cnt & (item_cnt - 1)) != 0)
        return NULL;

    r = (struct ring_buf *)malloc(sizeof(struct ring_buf));
    
    if(NULL != r)
    {
        r->item_size = item_size;
        r->item_cnt  = item_cnt;
        r->item_in = 0;
        r->item_out = 0;
        r->base = (unsigned char *) malloc(item_cnt * item_size);
        if(NULL == r->base)
        {
            free(r);
            r = NULL;
        }
    }
    
    return r;
}

unsigned char *
ring_buf_put(struct ring_buf *r, 
        unsigned char * item)
{
    unsigned int out_temp;
    
    if((NULL == item) || (NULL == r) || (NULL == r->base))
        return NULL;
    
    out_temp = r->item_out;
    
    /* not full, then put */
    if((r->item_in - out_temp) == r->item_cnt)
        return NULL;
    
    memcpy((r->base + r->item_size * (r->item_in & (r->item_cnt - 1))), item, r->item_size);
    
    r->item_in++;
    
    return item;
}

unsigned int 
ring_buf_get(struct ring_buf *r, 
        unsigned char * item)
{
    unsigned int in_temp;
    
    if((NULL == item) || (NULL == r) || (NULL == r->base))
        return 0;
    
    in_temp = r->item_in;
    
    /* not empty, the get */
    if(in_temp == r->item_out)
        return 0;
    
    memcpy(item, (r->base + r->item_size * (r->item_out & (r->item_cnt - 1))), r->item_size);
    
    r->item_out++;
    
    return r->item_size;
}

void 
ring_buf_free(struct ring_buf * r)
{
    if(r)
    {
        if(r->base)
            free(r->base);
        free(r);
    }
}


