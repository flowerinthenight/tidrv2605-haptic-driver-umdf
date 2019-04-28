#include "Internal.h"
#include "Device.h"
#include "__dump.h"

#include "Queue.h"
#include "Queue.tmh"

/////////////////////////////////////////////////////////////////////////
//
//  CMyQueue::CMyQueue
//
//  Object constructor function
//
//  Initialize member variables
//
/////////////////////////////////////////////////////////////////////////
CMyQueue::CMyQueue() :
    m_pParentDevice(nullptr)
{
}

/////////////////////////////////////////////////////////////////////////
//
//  CMyQueue::~CMyQueue
//
//  Object destructor function
//
//
/////////////////////////////////////////////////////////////////////////
CMyQueue::~CMyQueue()
{
    // Release the reference that was incremented in CreateInstance()
    SAFE_RELEASE(m_pParentDevice);   
}

/////////////////////////////////////////////////////////////////////////
//
//  CMyQueue::CreateInstance
//
//  This function supports the COM factory creation system
//
//  Parameters:
//      parentDevice    - A pointer to the CWSSDevice object
//      ppUkwn          - pointer to a pointer to the queue to be returned
//
//  Return Values:
//      S_OK: The queue was created successfully
//
/////////////////////////////////////////////////////////////////////////
HRESULT CMyQueue::CreateInstance(_In_ IWDFDevice *pWdfDevice, CMyDevice *pMyDevice)
{
    CComObject<CMyQueue>* pMyQueue = nullptr;

    if (nullptr == pMyDevice) {
        return E_INVALIDARG;
    }

    HRESULT hr = CComObject<CMyQueue>::CreateInstance(&pMyQueue);

    if (SUCCEEDED(hr)) {
        // AddRef the object
        pMyQueue->AddRef();

        // Store the parent device object
        pMyQueue->m_pParentDevice = pMyDevice;

        // Increment the reference for the lifetime of the CMyQueue object.
        pMyQueue->m_pParentDevice->AddRef();

        CComPtr<IUnknown> spIUnknown;
        hr = pMyQueue->QueryInterface(IID_PPV_ARGS(&spIUnknown));

        if (SUCCEEDED(hr)) {
            // Create the framework queue
            CComPtr<IWDFIoQueue> spDefaultQueue;
            hr = pWdfDevice->CreateIoQueue( spIUnknown,
                                            TRUE,                        // DefaultQueue
                                            WdfIoQueueDispatchParallel,  // Parallel queue handling 
                                            TRUE,                        // PowerManaged
                                            TRUE,                        // AllowZeroLengthRequests
                                            &spDefaultQueue 
                                            );
            if (FAILED(hr)) {
                Trace(
                    TRACE_LEVEL_ERROR,
                    "Failed to create default I/O queue, %!HRESULT!", 
                    hr);
            }
        }

        // Release the pMyQueue pointer when done. Note: UMDF holds a reference to it above
        SAFE_RELEASE(pMyQueue);
    }

    return hr;
}
/////////////////////////////////////////////////////////////////////////
//
//  CMyQueue::OnDeviceIoControl
//
//  This method is called when an IOCTL is sent to the device
//
//  Parameters:
//      pQueue            - pointer to an IO queue
//      pRequest          - pointer to an IO request
//      ControlCode       - The IOCTL to process
//      InputBufferSizeInBytes - the size of the input buffer
//      OutputBufferSizeInBytes - the size of the output buffer
//
/////////////////////////////////////////////////////////////////////////
STDMETHODIMP_ (void) CMyQueue::OnDeviceIoControl(
    _In_ IWDFIoQueue *pQueue,
    _In_ IWDFIoRequest *pRequest,
    _In_ ULONG ControlCode,
         SIZE_T InputBufferSizeInBytes,
         SIZE_T OutputBufferSizeInBytes)
{
    UNREFERENCED_PARAMETER(pQueue);
    UNREFERENCED_PARAMETER(InputBufferSizeInBytes);
    UNREFERENCED_PARAMETER(OutputBufferSizeInBytes);
    
    DWORD dwWritten = 0;

	/*
    if (IS_WPD_IOCTL(ControlCode)) {
        m_pParentDevice->ProcessIoControl(  pQueue,
                                            pRequest,
                                            ControlCode,
                                            InputBufferSizeInBytes,
                                            OutputBufferSizeInBytes,
                                            &dwWritten);
    } else {
        // Unsupported request
        pRequest->CompleteWithInformation(HRESULT_FROM_WIN32(ERROR_NOT_SUPPORTED), 0);
    }
	*/

	HRESULT hr = S_OK;
	ULONG information = 0;
    bool completeRequest = true;

	switch (ControlCode) {
		case IOCTL_INTERFACE_0:
		case IOCTL_INTERFACE_1:
		{
			m_pParentDevice->ProcessIoControl(
				pQueue,
				pRequest,
				ControlCode,
				InputBufferSizeInBytes,
				OutputBufferSizeInBytes,
				&dwWritten);

			completeRequest = true;
			information = 0;

			break;
		}

		default:
		{
			L2(WFN, L"Def: HRESULT_FROM_WIN32(ERROR_INVALID_FUNCTION)");
			hr = HRESULT_FROM_WIN32(ERROR_INVALID_FUNCTION);
			break;
		}
	}

	if (completeRequest) {
        pRequest->CompleteWithInformation(hr, information);
    }

    return;
}
