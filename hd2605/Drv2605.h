#pragma once

class CDrv2605 :
    public CComObjectRoot
{
// Public methods
public:
    CDrv2605();
    ~CDrv2605();

    DECLARE_NOT_AGGREGATABLE(CDrv2605)

    BEGIN_COM_MAP(CDrv2605)
    END_COM_MAP()
    
    VOID Initialize(
		_In_ IWDFDevice *pWdfDevice,
		_In_ IWDFCmResourceList *pWdfResourcesRaw,
		_In_ IWDFCmResourceList *pWdfResourcesTranslated);

	HRESULT ParseResources(
		_In_ IWDFDevice *pWdfDevice,
		_In_ IWDFCmResourceList *pWdfResourcesRaw,
		_In_ IWDFCmResourceList *pWdfResourcesTranslated,
		_Out_ LARGE_INTEGER *pRequestId);

	HRESULT InitializeRequest(_In_ IWDFDevice *pWdfDevice, _In_ LARGE_INTEGER id);
	HRESULT InitializeDeviceSequence();
	HRESULT TestInterface1(int libIndex);

// Private methods
private:
	HRESULT ReadRegister(
        _In_ BYTE reg,
        _Out_writes_(dataBufferLength) BYTE *pDataBuffer,
        _In_ ULONG dataBufferLength,
        _In_ ULONG delayInUs);
    
    HRESULT WriteRegister(
		_In_ BYTE reg,
        _In_reads_(dataBufferLength) BYTE *pDataBuffer,
        _In_ ULONG dataBufferLength);

// Members
private:
    
	// The request data buffer and status members
    BYTE *m_pDataBuffer;
	CComAutoCriticalSection m_CriticalSection;
	CComPtr<IRequest> m_spRequest;

    // TODO: Remove these when interfaces are registered
    CComObject<CSpbRequest> *m_pSpbRequest;

    // Track initialization state of sensor device
    BOOL m_bInitialized;
};
