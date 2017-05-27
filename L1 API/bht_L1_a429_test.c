#include <bht_L0.h>
#include <bht_L1.h>

#define DEVID BHT_DEVID_BACKPLANETYPE_PCI | BHT_DEVID_BOARDTYPE_PMCA429 | BHT_DEVID_BOARDNUM_01 

void main (void)
{
    int result;
    
    if(BHT_SUCCESS == (result = bht_L1_device_probe(DEVID)))
        printf("initialized succ\n");
    else
        printf("%s\n", bht_L1_error_to_string(result));

    system("pause");
}
