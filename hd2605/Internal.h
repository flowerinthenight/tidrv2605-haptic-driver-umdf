#ifndef _INTERNAL_H_
#define _INTERNAL_H_

#pragma once

#define UMDF_USING_NTSTATUS

#include <windows.h>
#include <winternl.h>
#include <ntstatus.h>
#include <atlbase.h>
#include <atlcom.h>
#include <atlcoll.h>
#include <atlstr.h>
#include <gpio.h>

_Analysis_mode_(_Analysis_code_type_user_driver_);  // Macro letting the compiler know this is not a kernel driver (this will help surpress needless warnings)

// Common WPD and WUDF headers
#include <devioctl.h>
#include <initguid.h>
#include <propkeydef.h>
#include <propvarutil.h>
#include "PortableDeviceTypes.h"
#include "PortableDeviceClassExtension.h"
#include "PortableDevice.h"

// Headers for Sensor specific defines and WpdCommands
#include "Sensors.h"
#include <SensorsClassExtension.h>

// Headers for internal interfaces
#include "Request.h"

// TODO: Remove these
#include "SpbRequest.h"

// Resource hub defines
#define RESHUB_USE_HELPER_ROUTINES
#include "reshub.h"

#include "Trace.h"

// One forward-declare that pretty much everyone is going to need to know about
class CMyDevice;

//
// Test properties
//

//2f808247-7cdb-4319-bf5b-e16ab67f7344
DEFINE_GUID(SENSOR_PROPERTY_TEST_GUID,              \
    0X2F808247, 0X7CDB, 0X4319, 0XBF, 0X5B, 0XE1, 0X6A, 0XB6, 0X7F, 0X73, 0X44);
DEFINE_PROPERTYKEY(SENSOR_PROPERTY_TEST_REGISTER,   \
    0X2F808247, 0X7CDB, 0X4319, 0XBF, 0X5B, 0XE1, 0X6A, 0XB6, 0X7F, 0X73, 0X44, 2); //[VT_UI4]
DEFINE_PROPERTYKEY(SENSOR_PROPERTY_TEST_DATA_SIZE,  \
    0X2F808247, 0X7CDB, 0X4319, 0XBF, 0X5B, 0XE1, 0X6A, 0XB6, 0X7F, 0X73, 0X44, 3); //[VT_UI4]
DEFINE_PROPERTYKEY(SENSOR_PROPERTY_TEST_DATA,       \
    0X2F808247, 0X7CDB, 0X4319, 0XBF, 0X5B, 0XE1, 0X6A, 0XB6, 0X7F, 0X73, 0X44, 4); //[VT_VECTOR|VT_UI1]

///////////////////////////////////////////////////////////////////
// Common macro expansions that are used throughout the project
#define SAFE_RELEASE(p)     {if ((p)) { (p)->Release(); (p) = nullptr; }}
#define ARRAY_SIZE(x)       (sizeof(x) / sizeof(x[0]))

#define LEAVE_BLOCK __leave

#define CHK_FAIL_LEAVE(dmp) \
{ \
	if (FAILED((hr))) { \
		(dmp); __leave; \
	} \
}

#define CHK_FALSE_LEAVE(value, dmp) \
{ \
	if ((value) == FALSE) { \
		(dmp); __leave; \
	} \
}

#define CHK_NULL_LEAVE(value, dmp) \
{ \
	if ((value) == NULL) { \
		(dmp); __leave; \
	} \
}

#define SINGLE_DO_WHILE_START		do
#define SINGLE_DO_WHILE_END			while (FALSE)
#define SINGLE_DO_WHILE_BREAK		break;

#define CHK_FAIL_BREAK(dmp) \
{ \
	if (FAILED((hr))) { \
		(dmp); break; \
	} \
}

#define CHK_FALSE_BREAK(value, dmp) \
{ \
	if ((value) == FALSE) { \
		(dmp); break; \
	} \
}

#define CHK_NULL_BREAK(value, dmp) \
{ \
	if ((value) == NULL) { \
		(dmp); break; \
	} \
}

#define IOCTL_INDEX					0x800
#define FILE_DEVICE_TEST			0x65500

typedef struct _HapticInterfaceContext {
	DWORD libIndex;
} HapticInterfaceContext, *PHapticInterfaceContext;

#define IOCTL_INTERFACE_0			CTL_CODE(FILE_DEVICE_TEST, \
										IOCTL_INDEX, \
										METHOD_BUFFERED, \
										FILE_ANY_ACCESS)

#define IOCTL_INTERFACE_1			CTL_CODE(FILE_DEVICE_TEST, \
										IOCTL_INDEX + 1, \
										METHOD_BUFFERED, \
										FILE_ANY_ACCESS)

typedef struct __WriteDevData {
	BYTE reg;
	BYTE data;
} WriteDeviceData, *PWriteDeviceData;

#endif // _INTERNAL_H_
