#include <bht_L0_types.h>
#include <bht_L0.h>
#include <bht_L1.h>


const char * bht_L1_error_to_string(bht_L0_u32 err_num)
{
	switch (err_num)
	{
	case BHT_SUCCESS:
		return("Function call completed without error");
		break;
	case BHT_FAILURE:
		return("Function call completed with error");
		break;
		/*    case BHT_ERR_MEM_MAP_SIZE:
			   return("Layer 0 - Invalid memory map size");
			   break;*/
	case BHT_ERR_NO_DEVICE:
		return("Layer 0 - Device not found");
		break;
	case BHT_ERR_CANT_OPEN_DEV:
		return("Layer 0 - Can't open device");
		break;
	case BHT_ERR_DEV_NOT_INITED:
		return("Layer 0 - Device not initialized");
		break;
	case BHT_ERR_DEV_ALREADY_OPEN:
		return("Layer 0 - Device already open");
		break;
	case BHT_ERR_UNSUPPORTED_BACKPLANE:
		return("Layer 0 - Unsupported backplane in DevID");
		break;
	case BHT_ERR_UNSUPPORTED_BOARDTYPE:
		return("Layer 0 - Unsupported board type in DevID");
		break;
	case BHT_ERR_UNSUPPORTED_CHANNELTYPE:
		return("Layer 0 - Unsupported channel type in DevID");
		break;
	case BHT_ERR_CANT_OPEN_DRIVER:
		return("Layer 0 - Can't open driver");
		break;
	case BHT_ERR_CANT_SET_DRV_OPTIONS:
		return("Layer 0 - Can't set driver options");
		break;
	case BHT_ERR_CANT_GET_DEV_INFO:
		return("Layer 0 - Can't get device info");
		break;
	case BHT_ERR_INVALID_BOARD_NUM:
		return("Layer 0 - Invalid board number");
		break;
	case BHT_ERR_INVALID_CHANNEL_NUM:
		return("Layer 0 - Invalid channel number");
		break;
	case BHT_ERR_DRIVER_READ_FAIL:
		return("Layer 0 - Driver read memory failure");
		break;
	case BHT_ERR_DRIVER_WRITE_FAIL:
		return("Layer 0 - Driver write memory failure");
		break;
	case BHT_ERR_DEVICE_CLOSE_FAIL:
		return("Layer 0 - Device close failure");
		break;
	case BHT_ERR_DRIVER_CLOSE_FAIL:
		return("Layer 0 - Driver close failure");
		break;
	case BHT_ERR_KP_OPEN_FAIL:
		return("Layer 0 - Kernel Plug-In Open failure");
		break;
	case BHT_ERR_BAD_INPUT:
		return("Layer 1 - Bad input parameter");
		break;
#if 0
	case BHT_ERR_MEM_TEST_FAIL:
		return("Layer 1 - Failed memory test");
		break;
	case BHT_ERR_MEM_MGT_NO_INIT:
		return("Layer 1 - Memory Management not initialized for the device ID");
		break;
	case BHT_ERR_MEM_MGT_INIT:
		return("Layer 1 - Memory Management already initialized for the device ID");
		break;
	case BHT_ERR_MEM_MGT_NO_MEM:
		return("Layer 1 - Not enough memory available");
		break;
	case BHT_ERR_BAD_DEV_TYPE:
		return("Layer 1 - Bad device type in device ID");
		break;
	case BHT_ERR_RT_FT_UNDEF:
		return("Layer 1 - RT Filter Table not defined");
		break;
	case BHT_ERR_RT_SA_UNDEF:
		return("Layer 1 - RT Subaddress not defined");
		break;
	case BHT_ERR_RT_SA_CDP_UNDEF:
		return("Layer 1 - RT SA CDP not defined");
		break;
	case BHT_ERR_IQ_NO_NEW_ENTRY:
		return("Layer 1 - No new entry in interrupt queue");
		break;
	case BHT_ERR_NO_BCCB_TABLE:
		return("Layer 1 - BCCB Table Pointer is zero");
		break;
	case BHT_ERR_BCCB_ALREADY_ALLOCATED:
		return("Layer 1 - BCCB already allocated");
		break;
	case BHT_ERR_BCCB_NOT_ALLOCATED:
		return("Layer 1 - BCCB has not been allocated");
		break;
#endif
	case BHT_ERR_BUFFER_FULL:
		return("Layer 1 - 1553-ARINC PB (CDP/PCB or RXP/PXP) buffer is full");
		break;
	case BHT_ERR_TIMEOUT:
		return("Layer 1 - Timeout error");
		break;
		//    case BHT_ERR_BAD_CHAN_NUM:
		//       return("Layer 1 - Bad channel number, channel does not exist on this board or is not initialized");
		//       break;
		//    case BHT_ERR_BITFAIL:
		//       return("Layer 1 - Built-In Test failure");
		//       break;
	case BHT_ERR_DEVICEINUSE:
		return("Layer 1 - Device in use already, or not properly closed");
		break;
		//    case BHT_ERR_NO_TXCB_TABLE:
		//       return("Layer 1 - TXCB Table Pointer is zero");
		//       break;
		//    case BHT_ERR_TXCB_ALREADY_ALLOCATED:
		//       return("Layer 1 - TXCB already allocated");
		//       break;
		//    case BHT_ERR_TXCB_NOT_ALLOCATED:
		//       return("Layer 1 - TXCB has not been allocated");
		//       break;
		//    case BHT_ERR_PBCB_TOOMANYPXPS:
		//       return("Layer 1 - PBCB Too Many PXPs For PBCB Allocation");
		//       break;
		//    case BHT_ERR_NORXCHCVT_ALLOCATED:
		//       return("Layer 1 - RX CH - No CVT Option Defined at Init");
		//       break;
	case BHT_ERR_NO_DATA_AVAILABLE:
		return("Layer 1 - No Data Available");
		break;

	default:
		return("UNKNOWN ERROR/STATUS CODE!");
		break;
	}
}