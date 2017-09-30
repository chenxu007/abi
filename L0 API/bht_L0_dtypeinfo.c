#include <bht_L0.h>

const bht_L0_dtypeinfo_t bht_L0_dtypeinfo_items[BHT_L0_DEVICE_TYPE_MAX] =
{
    {BHT_L0_DEVICE_TYPE_ARINC429     , BHT_L0_INTERFACE_TYPE_PCI , BHT_L0_LOGIC_TYPE_A429    },
//    {BHT_L0_DEVICE_TYPE_PCIA429     , BHT_L0_INTERFACE_TYPE_PCI , BHT_L0_LOGIC_TYPE_A429    },
//    {BHT_L0_DEVICE_TYPE_CPCIA429    , BHT_L0_INTERFACE_TYPE_PCI , BHT_L0_LOGIC_TYPE_A429    },
//    {BHT_L0_DEVICE_TYPE_PXIA429     , BHT_L0_INTERFACE_TYPE_PCI , BHT_L0_LOGIC_TYPE_A429    },
    {BHT_L0_DEVICE_TYPE_1553     , BHT_L0_INTERFACE_TYPE_PCI , BHT_L0_LOGIC_TYPE_1553    },
    {BHT_L0_DEVICE_TYPE_UNINITIALIZED, BHT_L0_INTERFACE_TYPE_PCI    , BHT_L0_LOGIC_TYPE_A429}
};