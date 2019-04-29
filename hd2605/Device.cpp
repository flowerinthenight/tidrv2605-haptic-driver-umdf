#include "Internal.h"

#include "Queue.h"
#include "Drv2605.h"
#include "DRV2605-SLOS825A.h"
#include "__dump.h"

#include "Device.h"
#include "Device.tmh"

// {CF8D6763-D0DC-4FBF-BBE9-B88AAA234113}
DEFINE_GUID(GUID_DEVINTERFACE_HAPTIC_UMDF, 0xcf8d6763, 0xd0dc, 0x4fbf, 0xbb, 0xe9, 0xb8, 0x8a, 0xaa, 0x23, 0x41, 0x13);

/////////////////////////////////////////////////////////////////////////
//
//  CMyDevice::CMyDevice
//
//  Object constructor function
//
/////////////////////////////////////////////////////////////////////////
CMyDevice::CMyDevice() :
    m_spWdfDevice(nullptr)
    // m_spClassExtension(nullptr),
    // m_pSensorDdi(nullptr)
{

}

/////////////////////////////////////////////////////////////////////////
//
//  CMyDevice::~CMyDevice
//
//  Object destructor function
//
/////////////////////////////////////////////////////////////////////////
CMyDevice::~CMyDevice()
{
	SAFE_RELEASE(m_pHapticDevice);
}

/////////////////////////////////////////////////////////////////////////
//
//  CMyDevice::CreateInstance
//
//  This static method is used to create and initialize an instance of
//  CMyDevice for use with a given hardware device.
//
//  Parameters:
//      pDriver     - pointer to an IWDFDriver interface
//      pDeviceInit - pointer to an interface used to intialize the device
//      ppMyDevice  - pointer to a location to place the CMyDevice instance
//
//  Return Values:
//      status
//
/////////////////////////////////////////////////////////////////////////
HRESULT CMyDevice::CreateInstance(
    _In_  IWDFDriver *pDriver,
    _In_  IWDFDeviceInitialize *pDeviceInit,
    _Out_ CComObject<CMyDevice> **ppMyDevice)
{
    FuncEntry();

    L2(WFN, L"Called.");

    CComObject<CMyDevice>* pMyDevice = nullptr;

    HRESULT hr = CComObject<CMyDevice>::CreateInstance(&pMyDevice);

    if (SUCCEEDED(hr)) {
        pMyDevice->AddRef();

        hr = pMyDevice->Initialize(pDriver, pDeviceInit);

        if (SUCCEEDED(hr)) {
            *ppMyDevice = pMyDevice;
        }
    }

    FuncExit();
    return hr;
}

/////////////////////////////////////////////////////////////////////////
//
//  CMyDevice::Initialize
//
//  This method initializes the device callback object and creates the
//  partner device object.
//
//  Parameters:
//      pDriver     - pointer to an IWDFDriver interface
//      pDeviceInit - pointer to an interface used to intialize the device
//
//  Return Values:
//      status
//
/////////////////////////////////////////////////////////////////////////
HRESULT CMyDevice::Initialize(
    _In_  IWDFDriver *pDriver,
    _In_  IWDFDeviceInitialize *pDeviceInit)
{
    FuncEntry();

    L2(WFN, L"Called.");
    
    CComPtr<IUnknown> spCallback;
    CComPtr<IWDFDevice> spIWDFDevice;
    CComPtr<IWDFDevice3> spIWDFDevice3;
    HRESULT hr;
    
    // Prepare device parameters
    pDeviceInit->SetLockingConstraint(None); 
    pDeviceInit->SetPowerPolicyOwnership(TRUE);

    hr = QueryInterface(IID_PPV_ARGS(&spCallback));

    if (SUCCEEDED(hr)) {
        // Create the IWDFDevice object
        hr = pDriver->CreateDevice(pDeviceInit, spCallback, &spIWDFDevice);

        if (FAILED(hr)) {
            Trace(
                TRACE_LEVEL_ERROR,
                "Failed to create the IWDFDevice object, %!HRESULT!",
                hr);
        }

        if (SUCCEEDED(hr)) {   
            // Assign context
            hr = spIWDFDevice->AssignContext(nullptr, (void*)this);

            if (FAILED(hr)) {
                Trace(
                    TRACE_LEVEL_ERROR,
                    "Failed to assign context, %!HRESULT!",
                    hr);
            }
        }
    }
        
    if (SUCCEEDED(hr)) {
        WUDF_DEVICE_POWER_POLICY_IDLE_SETTINGS idleSettings;
        WUDF_DEVICE_POWER_POLICY_IDLE_SETTINGS_INIT(
            &idleSettings,
            IdleCannotWakeFromS0);

        // Set delay timeout value. This specifies the time
        // delay between WDF detecting the device is idle
        // and WDF requesting a Dx power transition on the 
        // device's behalf.
        idleSettings.IdleTimeout = 100;
        
        // Opt-in to D3Cold to allow the platform to remove 
        // power when the device is idle and enters D3.
        idleSettings.ExcludeD3Cold = WdfFalse;

        // Get a pointer to the IWDFDevice3 interface and
        // assign the idle settings.
        hr = spIWDFDevice->QueryInterface(IID_PPV_ARGS(&spIWDFDevice3));

        if (SUCCEEDED(hr)) {
            hr = spIWDFDevice3->AssignS0IdleSettingsEx(&idleSettings);
        }
    }
        
    if (SUCCEEDED(hr)) {
        // Ensure device is disable-able
        spIWDFDevice->SetPnpState(WdfPnpStateNotDisableable, WdfFalse);
        spIWDFDevice->CommitPnpState();

        // Store the IWDFDevice pointer
        m_spWdfDevice = spIWDFDevice;
    }

    FuncExit();
    return hr;
}

/////////////////////////////////////////////////////////////////////////
//
//  CMyDevice::Configure
//  
//  This method is called after the device callback object has been initialized 
//  and returned to the driver.  It would setup the device's queues and their 
//  corresponding callback objects.
//
//  Parameters:
//
//  Return Values:
//      status
//
/////////////////////////////////////////////////////////////////////////
HRESULT CMyDevice::Configure()
{
    FuncEntry();

    L2(WFN, L"Called.");

    HRESULT hr = CMyQueue::CreateInstance(m_spWdfDevice, this);

    if (FAILED(hr)) {
        Trace(
            TRACE_LEVEL_ERROR,
            "Failed to create instance of CMyQueue, %!HRESULT!",
            hr);
    }

	hr = m_spWdfDevice->CreateDeviceInterface(&GUID_DEVINTERFACE_HAPTIC_UMDF, NULL);

	if (FAILED(hr)) {
		L2(WFN, L"CreateDeviceInterface failed (%x).", hr);
	} else {
		L2(WFN, L"Device interface created successfully.");
	}

	hr = m_spWdfDevice->AssignDeviceInterfaceState(&GUID_DEVINTERFACE_HAPTIC_UMDF, NULL, TRUE);

	if (FAILED(hr)) {
		L2(WFN, L"AssignDeviceInterfaceState set to TRUE failed (%x).", hr);
	} else {
		L2(WFN, L"Device interface enabled.");
	}

    FuncExit();
    return hr;
}

/////////////////////////////////////////////////////////////////////////
//
//  CMyDevice::OnPrepareHardware
//
//  Called by UMDF to prepare the hardware for use. In our case
//  we create the SensorDdi object and initialize the Sensor Class Extension
//
//  Parameters:
//      pWdfDevice - pointer to an IWDFDevice object representing the
//          device
//      pWdfResourcesRaw - pointer the raw resource list
//      pWdfResourcesTranslated - pointer to the translated resource list
//
//  Return Values:
//      status
//
/////////////////////////////////////////////////////////////////////////
HRESULT CMyDevice::OnPrepareHardware(
    _In_ IWDFDevice3 *pWdfDevice,
    _In_ IWDFCmResourceList *pWdfResourcesRaw,
    _In_ IWDFCmResourceList *pWdfResourcesTranslated)
{
    FuncEntry();

	L2(WFN, L"Called.");

	// UNREFERENCED_PARAMETER(pWdfDevice);
	// UNREFERENCED_PARAMETER(pWdfResourcesRaw);
	// UNREFERENCED_PARAMETER(pWdfResourcesTranslated);

    HRESULT hr = S_OK;

	hr = CComObject<CDrv2605>::CreateInstance(&m_pHapticDevice); 

	if (FAILED(hr)) {
        Trace(
            TRACE_LEVEL_ERROR,
            "Failed to create the sensor DDI, %!HRESULT!", 
            hr);

		L2(WFN, L"Failed to create the haptic device (%x).", hr);
    }

	if (SUCCEEDED(hr)) {
		m_pHapticDevice->Initialize(pWdfDevice, pWdfResourcesRaw, pWdfResourcesTranslated);

		DWORD initialIndex = 1;

		hr = GetPropertyStoreDword(L"LibIndex", &initialIndex);

		if (FAILED(hr)) {
			// No initial index. Set defaul to 1.
			hr = SetPropertyStoreDword(L"LibIndex", 1);
		}
		
		if (FAILED(hr)) {
			L2(WFN, L"SetPropertyStoreDword error 0x%x", hr);
			hr = S_OK;
		}

		// Create interface thread
		m_InterfaceThread = CreateThread(
			NULL,
			0,
			(LPTHREAD_START_ROUTINE)InterfaceThread,
			(LPVOID)this,
			0,
			NULL);

		m_TerminateEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

		if (!m_TerminateEvent) {
			L2(WFN, L"m_TerminateEvent error!");
		}

		if (m_InterfaceThread) {
			L2(WFN, L"m_InterfaceThread 0x%x created.", m_InterfaceThread);
		}
	}

    FuncExit();
    return hr;
}

/////////////////////////////////////////////////////////////////////////
//
//  CMyDevice::OnReleaseHardware
//
//  Called by WUDF to uninitialize the hardware.
//
//  Parameters:
//      pWdfDevice - pointer to an IWDFDevice object for the device
//      pWdfResourcesTranslated - pointer to the translated resource list
//
//  Return Values:
//      status
//
/////////////////////////////////////////////////////////////////////////
HRESULT CMyDevice::OnReleaseHardware(
    _In_ IWDFDevice3 *pWdfDevice,
    _In_ IWDFCmResourceList *pWdfResourcesTranslated)
{
    FuncEntry();

	L2(WFN, L"Called.");

    UNREFERENCED_PARAMETER(pWdfDevice);
    UNREFERENCED_PARAMETER(pWdfResourcesTranslated);

    HRESULT hr = S_OK;

	if (m_TerminateEvent) {
		SetEvent(m_TerminateEvent);
	}

	if (m_InterfaceThread) {
		WaitForSingleObject(m_InterfaceThread, INFINITE);
	}

	if (m_TerminateEvent) CloseHandle(m_TerminateEvent);

    // Release the IWDFDevice handle, if it matches
    if (pWdfDevice == m_spWdfDevice.p) {
        m_spWdfDevice.Release();
    }

    FuncExit();
    return hr;
}

/////////////////////////////////////////////////////////////////////////
//
//  CMyDevice::OnD0Entry
//
//  This method is called after a new device enters the system
//
//  Parameters:
//      pWdfDevice    - pointer to a device object
//      previousState - previous WDF power state
//
//  Return Values:
//      status
//
/////////////////////////////////////////////////////////////////////////
HRESULT CMyDevice::OnD0Entry(
    _In_ IWDFDevice *pWdfDevice,
    _In_ WDF_POWER_DEVICE_STATE previousState)
{
    FuncEntry();

	L2(WFN, L"Called.");

    UNREFERENCED_PARAMETER(pWdfDevice);
    UNREFERENCED_PARAMETER(previousState);

	HRESULT hr = S_OK;

    FuncExit();
    return hr;
}

/////////////////////////////////////////////////////////////////////////
//
//  CMyDevice::OnD0Exit
//
//  This method is called when a device leaves the system
//
//  Parameters:
//      pWdfDevice - pointer to a device object
//      newState   - new WDF power state
//
//  Return Values:
//      status
//
/////////////////////////////////////////////////////////////////////////
HRESULT CMyDevice::OnD0Exit(
    _In_ IWDFDevice *pWdfDevice,
    _In_ WDF_POWER_DEVICE_STATE newState)
{
    FuncEntry();

	L2(WFN, L"Called.");

    UNREFERENCED_PARAMETER(pWdfDevice);
    UNREFERENCED_PARAMETER(newState);

	HRESULT hr = S_OK;

    FuncExit();
    return hr;
}

/////////////////////////////////////////////////////////////////////////
//
//  CMyDevice::OnCleanupFile
//
//  This method is called when the file handle to the device is closed
//
//  Parameters:
//      pWdfFile - pointer to a file object
//
//  Return Values:
//      none
//
/////////////////////////////////////////////////////////////////////////
VOID CMyDevice::OnCleanupFile(_In_ IWDFFile *pWdfFile)
{
    FuncEntry();

	UNREFERENCED_PARAMETER(pWdfFile);

    FuncExit();
    return;
}

/////////////////////////////////////////////////////////////////////////
//
//  CMyDevice::ProcessIoControl
//
//  This method is a helper that takes the incoming IOCTL and forwards
//  it to the Windows Sensor Class Extension for processing.
//
//  Parameters:
//      pQueue                  - pointer to the UMDF queue that
//                                handled the request
//      pRequest                - pointer to the request
//      ControlCode             - the IOCTL code
//      InputBufferSizeInBytes  - size of the incoming IOCTL buffer
//      OutputBufferSizeInBytes - size of the outgoing IOCTL buffer
//      pcbWritten              - pointer to a DWORD containing the number
//                                of bytes returned
//
//  Return Values:
//      status
//
/////////////////////////////////////////////////////////////////////////
HRESULT CMyDevice::ProcessIoControl(
    _In_ IWDFIoQueue*     pQueue,
    _In_ IWDFIoRequest*   pRequest,
    _In_ ULONG            ControlCode,
         SIZE_T           InputBufferSizeInBytes,
         SIZE_T           OutputBufferSizeInBytes,
         DWORD*           pcbWritten)
{
    FuncEntry();

    UNREFERENCED_PARAMETER(pQueue);
	UNREFERENCED_PARAMETER(pRequest);
    UNREFERENCED_PARAMETER(ControlCode);
    UNREFERENCED_PARAMETER(InputBufferSizeInBytes);
    UNREFERENCED_PARAMETER(OutputBufferSizeInBytes);
    UNREFERENCED_PARAMETER(pcbWritten);

    HRESULT hr = S_OK;
    
	L2(WFN, L"Called.");

	IWDFIoRequest2 *pRequest2 = NULL;
	SIZE_T cbInBufferSize = 0;
	PVOID pInBuffer = NULL;

	switch (ControlCode) {
		case IOCTL_INTERFACE_0:
		{
			hr = pRequest->QueryInterface(IID_PPV_ARGS(&pRequest2));

			hr = pRequest2->RetrieveInputBuffer(sizeof(HapticInterfaceContext), &pInBuffer, &cbInBufferSize);

			PHapticInterfaceContext pBuffer = (PHapticInterfaceContext)pInBuffer;

			L2(WFN, L"Index from client = %d", pBuffer->libIndex);

			// m_pHapticDevice->TestInterface1((int)pBuffer->libIndex);

			break;
		}
		case IOCTL_INTERFACE_1:
		{
			m_pHapticDevice->TestInterface1(1);
		}
	}


	SAFE_RELEASE(pRequest2);

    FuncExit();
    return hr;
}

HRESULT CMyDevice::GetPropertyStoreDword(wchar_t *pszName, PDWORD pdwValue)
{
	IWDFPropertyStoreFactory *pPropertyStoreFactory = NULL;
	WDF_PROPERTY_STORE_ROOT RootSpecifier;
	IWDFNamedPropertyStore2 *pHardwarePropertyStore2 = NULL;
	PROPVARIANT pvValue;
	HRESULT hr = S_OK;

	__try {
		//
		// Get the property store factory interface.
		//
		hr = m_spWdfDevice->QueryInterface(IID_PPV_ARGS(&pPropertyStoreFactory));
		
		if (FAILED(hr)) {
			L2(WFN, L"Failed m_spWdfDevice->QueryInterface 0x%x", hr);
		}

		//
		// Initialize the WDF_PROPERTY_STORE_ROOT structure. We want to open the 
		// \Device Parameters subkey under the device's hardware key.
		//
		RtlZeroMemory(&RootSpecifier, sizeof(WDF_PROPERTY_STORE_ROOT));
		RootSpecifier.LengthCb = sizeof(WDF_PROPERTY_STORE_ROOT);
		RootSpecifier.RootClass = WdfPropertyStoreRootClassHardwareKey;
		RootSpecifier.Qualifier.HardwareKey.ServiceName = WDF_PROPERTY_STORE_HARDWARE_KEY_DEFAULT;

		//
		// Get the property store interface for the hardware key of the
		// device that m_FxDevice represents.
		//
		hr = pPropertyStoreFactory->RetrieveDevicePropertyStore(
			&RootSpecifier,
			WdfPropertyStoreCreateIfMissing,
			KEY_QUERY_VALUE | KEY_SET_VALUE,
			NULL,
			&pHardwarePropertyStore2,
			NULL
		);

		if (FAILED(hr)) {
			L2(WFN, L"Failed RetrieveDevicePropertyStore 0x%x", hr);
		}

		PropVariantInit(&pvValue);

		hr = pHardwarePropertyStore2->GetNamedValue(pszName, &pvValue);
		
		if (FAILED(hr)) {
			L2(WFN, L"Failed GetNamedValue 0x%x", hr);
		}

		*pdwValue = (DWORD)pvValue.uintVal;
	} __finally {
		SAFE_RELEASE(pHardwarePropertyStore2);
		SAFE_RELEASE(pPropertyStoreFactory);
	}

	return hr;
}

HRESULT CMyDevice::SetPropertyStoreDword(wchar_t *pszName, DWORD dwValue)
{
	IWDFPropertyStoreFactory *pPropertyStoreFactory = NULL;
	WDF_PROPERTY_STORE_ROOT RootSpecifier;
	IWDFNamedPropertyStore2 *pHardwarePropertyStore2 = NULL;
	PROPVARIANT pvValue;
	HRESULT hr = S_OK;

	SINGLE_DO_WHILE_START {
		//
		// Get the property store factory interface.
		//
		hr = m_spWdfDevice->QueryInterface(IID_PPV_ARGS(&pPropertyStoreFactory));
		
		if (FAILED(hr)) {
			L2(WFN, L"Failed m_spWdfDevice->QueryInterface 0x%x", hr);
		}

		//
		// Initialize the WDF_PROPERTY_STORE_ROOT structure. We want to open the 
		// \Device Parameters subkey under the device's hardware key.
		//
		RtlZeroMemory(&RootSpecifier, sizeof(WDF_PROPERTY_STORE_ROOT));
		RootSpecifier.LengthCb = sizeof(WDF_PROPERTY_STORE_ROOT);
		RootSpecifier.RootClass = WdfPropertyStoreRootClassHardwareKey;
		RootSpecifier.Qualifier.HardwareKey.ServiceName = WDF_PROPERTY_STORE_HARDWARE_KEY_DEFAULT;

		//
		// Get the property store interface for the hardware key of the
		// device that m_FxDevice represents.
		//
		hr = pPropertyStoreFactory->RetrieveDevicePropertyStore(
			&RootSpecifier,
			WdfPropertyStoreCreateIfMissing,
			KEY_QUERY_VALUE | KEY_SET_VALUE,
			NULL,
			&pHardwarePropertyStore2,
			NULL
		);

		if (FAILED(hr)) {
			L2(WFN, L"Failed RetrieveDevicePropertyStore 0x%x", hr);
		}

		PropVariantInit(&pvValue);
		pvValue.vt = VT_UINT;
		pvValue.uintVal = (UINT)dwValue;

		hr = pHardwarePropertyStore2->SetNamedValue(pszName, &pvValue);
		
		if (FAILED(hr)) {
			L2(WFN, L"Failed SetNamedValue 0x%x", hr);
		}
	}
	#pragma warning(suppress: 4127)
	SINGLE_DO_WHILE_END;

	SAFE_RELEASE(pHardwarePropertyStore2);
	SAFE_RELEASE(pPropertyStoreFactory);

	return hr;
}

DWORD CMyDevice::InterfaceThread(LPVOID lpParam)
{
	L2(WFN, L"Thread entry -->");

	HRESULT hr = S_OK;
	CMyDevice *pMyDevice = (CMyDevice*)lpParam;

	HKEY hKey;
	REGSAM rs = KEY_NOTIFY;

	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Lenovo\\HapticInterface", 0, rs, &hKey) != ERROR_SUCCESS) {
		L2(WFN, L"Failed RegOpenKeyEx %d", GetLastError());
		return FALSE;
	}

	HANDLE hInterfaceEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	HANDLE hEvents[2] = { hInterfaceEvent, pMyDevice->m_TerminateEvent };

	while (TRUE) {
		if (!hEvents[0] || !hEvents[1]) {
			L2(WFN, L"Our control events are invalid. Terminate interface thread.");
			break;
		}

		LSTATUS lStatus = RegNotifyChangeKeyValue(
			hKey,
			TRUE,
			REG_NOTIFY_CHANGE_LAST_SET,
			hEvents[0],
			TRUE);

		if (lStatus != ERROR_SUCCESS) {
			L2(WFN, L"Failed RegOpenKeyEx %d", GetLastError());
			break;
		}

		L2(WFN, L"Waiting for events now.");

		DWORD wait = WaitForMultipleObjects(2, hEvents, FALSE, INFINITE);

		L2(WFN, L"Interface accessed!");

		if ((wait - WAIT_OBJECT_0) == 1) {
			L2(WFN, L"Requested to terminate...");
			break;
		}

		DWORD libIndex = 1;

		hr = pMyDevice->GetPropertyStoreDword(L"LibIndex", &libIndex);

		if (FAILED(hr)) {
			L2(WFN, L"Failed TestInterface1 0x%x", hr);
		}

		L2(WFN, L"Current LibIndex %d", libIndex);

		hr = pMyDevice->m_pHapticDevice->TestInterface1(libIndex);

		if (FAILED(hr)) {
			L2(WFN, L"Failed TestInterface1 0x%x", hr);
		}
	}

	if (hInterfaceEvent) CloseHandle(hInterfaceEvent);

	if (hKey) RegCloseKey(hKey);

	L2(WFN, L"<-- Thread exit");

	return 0;
}
