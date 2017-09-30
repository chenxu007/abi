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
    case BHT_ERR_WINDRIVER_INIT_FAIL:
		return("Layer 0 - WinDriver initialize failed");
		break;
    case BHT_ERR_SEM_CREAT_FAIL:
		return("Layer 0 - Semaphore create failed");
		break;
    case BHT_ERR_SEM_TAKE:
		return("Layer 0 - Semaphore take failed");
		break;
    case BHT_ERR_SEM_GIVE:
		return("Layer 0 - Semaphore give failed");
		break;
    case BHT_ERR_SEM_DESTROY:
		return("Layer 0 - Semaphore destroy failed");
		break;
	case BHT_ERR_BAD_INPUT:
		return("Layer 1 - Bad input parameter");
		break;
    case BHT_ERR_UNSUPPORTED_DTYPE:
		return("Layer 1 - Unsupported device type");
		break;        
	case BHT_ERR_BUFFER_FULL:
		return("Layer 1 - Buffer is full");
		break;
	case BHT_ERR_TIMEOUT:
		return("Layer 1 - Timeout error");
		break;
	case BHT_ERR_DEVICEINUSE:
		return("Layer 1 - Device in use already, or not properly closed");
		break;
	case BHT_ERR_NO_DATA_AVAILABLE:
		return("Layer 1 - No Data Available");
		break;
    case BHT_ERR_DEVICE_NOT_INIT:
		return("Layer 1 - Device not initialize");
		break;
    case BHT_ERR_DEVICE_NOT_OPEN:
		return("Layer 1 - Device not open");
		break;
	default:
		return("UNKNOWN ERROR/STATUS CODE!");
		break;
	}
}