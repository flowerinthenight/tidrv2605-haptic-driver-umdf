#ifndef _DEVICE_H_
#define _DEVICE_H_

#pragma once

class CSensorDdi;
class CDrv2605;

class ATL_NO_VTABLE CMyDevice :
    public CComObjectRootEx<CComMultiThreadModel>,
    public IPnpCallback,
    public IPnpCallbackHardware2,
    public IFileCallbackCleanup
{
public:
    ~CMyDevice();

    DECLARE_NOT_AGGREGATABLE(CMyDevice)

    BEGIN_COM_MAP(CMyDevice)
        COM_INTERFACE_ENTRY(IPnpCallback)
        COM_INTERFACE_ENTRY(IPnpCallbackHardware2)
        COM_INTERFACE_ENTRY(IFileCallbackCleanup)
    END_COM_MAP()

protected:
    CMyDevice();

    HRESULT Initialize(_In_ IWDFDriver *pDriver, _In_ IWDFDeviceInitialize *pDeviceInit);

// COM Interface methods
public:

    // IPnpCallback
    STDMETHOD  (OnD0Entry)(_In_ IWDFDevice *pWdfDevice, _In_ WDF_POWER_DEVICE_STATE previousState);
    STDMETHOD  (OnD0Exit)(_In_ IWDFDevice *pWdfDevice, _In_ WDF_POWER_DEVICE_STATE newState);
    STDMETHOD_ (VOID, OnSurpriseRemoval)(_In_ IWDFDevice*) { return; }
    STDMETHOD_ (HRESULT, OnQueryRemove)(_In_ IWDFDevice*)  { return S_OK; }
    STDMETHOD_ (HRESULT, OnQueryStop)(_In_ IWDFDevice*)    { return S_OK; }

    // IPnpCallbackHardware2
    STDMETHOD_ (HRESULT, OnPrepareHardware)(
        _In_ IWDFDevice3 *pWdfDevice,
        _In_ IWDFCmResourceList *pWdfResourcesRaw,
        _In_ IWDFCmResourceList *pWdfResourcesTranslated);

    STDMETHOD_ (HRESULT, OnReleaseHardware)(
        _In_ IWDFDevice3 *pWdfDevice,
        _In_ IWDFCmResourceList *pWdfResourcesTranslated);

    // IFileCallbackCleanup
    STDMETHOD_ (VOID, OnCleanupFile)(_In_ IWDFFile *pWdfFile);

public:
	HRESULT GetPropertyStoreDword(wchar_t *pszName, PDWORD pdwValue);
	HRESULT SetPropertyStoreDword(wchar_t *pszName, DWORD dwValue);

public:
    
    // The factory method used to create an instance of this device
    static HRESULT CreateInstance(
        _In_  IWDFDriver *pDriver,
        _In_  IWDFDeviceInitialize *pDeviceInit,
        _Out_ CComObject<CMyDevice> **ppMyDevice);

    HRESULT Configure();

    HRESULT ProcessIoControl(
        _In_ IWDFIoQueue *pQueue,
        _In_ IWDFIoRequest *pRequest,
        _In_ ULONG ControlCode,
             SIZE_T InputBufferSizeInBytes,
             SIZE_T OutputBufferSizeInBytes,
             DWORD *pcbWritten);

	// HANDLE m_InterfaceEvent;
	HANDLE m_TerminateEvent;
	HANDLE m_InterfaceThread;

	static DWORD InterfaceThread(LPVOID lpParam);

private:
    // Interface pointers
    CComPtr<IWDFDevice> m_spWdfDevice;
    // CComPtr<ISensorClassExtension> m_spClassExtension;

    // Class extension pointer
    // CComObject<CSensorDdi>* m_pSensorDdi;

public:
	CComObject<CDrv2605> *m_pHapticDevice;
};

#endif // _DEVICE_H_
