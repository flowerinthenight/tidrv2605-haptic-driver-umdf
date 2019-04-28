#include "Internal.h"
// #include "SpbAccelerometer.h" // IDL Generated File
#include "LenHapticDriverDrv2605.h" // IDL Generated File
#include "Device.h"
#include "__dump.h"

#include "Driver.h"
#include "Driver.tmh"

/////////////////////////////////////////////////////////////////////////
//
//  CMyDriver::CMyDriver
//
//  Object constructor function
//
/////////////////////////////////////////////////////////////////////////
CMyDriver::CMyDriver()
{
}

/////////////////////////////////////////////////////////////////////////
//
//  CMyDriver::OnDeviceAdd
//
//  The framework call this function when device is detected. This driver
//  creates a device callback object
//
//  Parameters:

//      pDriver     - pointer to an IWDFDriver object
//      pDeviceInit - pointer to a device initialization object
//
//  Return Values:
//      S_OK: device initialized successfully
//
/////////////////////////////////////////////////////////////////////////
HRESULT CMyDriver::OnDeviceAdd(_In_ IWDFDriver *pDriver, _In_ IWDFDeviceInitialize *pDeviceInit)
{
    CComObject<CMyDevice>* pMyDevice = nullptr;
    HRESULT hr;

	L2(WFN, L"Called.");

    hr = CMyDevice::CreateInstance(pDriver, pDeviceInit, &pMyDevice);

    if (FAILED(hr)) {
        Trace(
            TRACE_LEVEL_ERROR,
            "Failed to create instance of CMyDevice, %!HRESULT!",
            hr);
    }

    if (SUCCEEDED(hr)) {
        hr = pMyDevice->Configure();

        if (FAILED(hr)) {
            Trace(
                TRACE_LEVEL_ERROR,
                "Failed to configure CMyDevice %p, %!HRESULT!",
                pMyDevice,
                hr);
        }

        // Release the pMyDevice pointer when done. 
        // Note: UMDF holds a reference to it above
        SAFE_RELEASE(pMyDevice);
    }

    return hr;
}
/////////////////////////////////////////////////////////////////////////
//
//  CMyDriver::OnInitialize
//
//  The framework calls this function just after loading the driver. The driver
//  can perform any global, device independent intialization in this routine.
//
/////////////////////////////////////////////////////////////////////////
HRESULT CMyDriver::OnInitialize(_In_ IWDFDriver *pDriver)
{
    UNREFERENCED_PARAMETER(pDriver);
	L2(WFN, L"Called.");
    return S_OK;
}

/////////////////////////////////////////////////////////////////////////
//
//  CMyDriver::OnDeinitialize
//
//  The framework calls this function just before de-initializing itself. All
//  WDF framework resources should be released by driver before returning
//  from this call.
//
/////////////////////////////////////////////////////////////////////////
void CMyDriver::OnDeinitialize(_In_ IWDFDriver *pDriver)
{
    UNREFERENCED_PARAMETER(pDriver);
    return;
}
