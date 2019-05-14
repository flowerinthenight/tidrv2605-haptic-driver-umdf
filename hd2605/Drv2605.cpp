#include "Internal.h"
#include "DRV2605-SLOS825A.h"

#include "Drv2605.h"
#include "Drv2605.tmh"

#include "__dump.h"

/////////////////////////////////////////////////////////////////////////
//
//  CDrv2605::CDrv2605()
//
//  Constructor
//
/////////////////////////////////////////////////////////////////////////
CDrv2605::CDrv2605() :
    m_spRequest(nullptr),
    m_pSpbRequest(nullptr),
    m_pDataBuffer(nullptr),
    m_bInitialized(FALSE)
{

}

/////////////////////////////////////////////////////////////////////////
//
//  CDrv2605::~CDrv2605()
//
//  Destructor
//
/////////////////////////////////////////////////////////////////////////
CDrv2605::~CDrv2605()
{
    SAFE_RELEASE(m_pSpbRequest);

    if (m_pDataBuffer != nullptr) {
        delete[] m_pDataBuffer;
        m_pDataBuffer = nullptr;
    }
}

VOID CDrv2605::Initialize(
    _In_ IWDFDevice *pWdfDevice,
    _In_ IWDFCmResourceList *pWdfResourcesRaw,
    _In_ IWDFCmResourceList *pWdfResourcesTranslated)
{
    FuncEntry();
    
    L2(WFN, L"Entry -->");
    
    HRESULT hr = S_OK;
    LARGE_INTEGER requestId;
    
    requestId.QuadPart = 0;

    if (m_bInitialized == FALSE) {
        // Create the request object
        hr = CComObject<CSpbRequest>::CreateInstance(&m_pSpbRequest);
        
        if (SUCCEEDED(hr)) {
            m_pSpbRequest->AddRef();
            hr = m_pSpbRequest->QueryInterface(IID_PPV_ARGS(&m_spRequest));
        }
        
        // TODO: CoCreateInstance rather than calling
        //       CreateInstance on the class and querying
        //       the required interface.
        
        //// Create the request object
        //hr = CoCreateInstance(
        //    __uuidof(SpbRequest), // CLSID_SpbRequest
        //    nullptr,
        //    CLSCTX_INPROC_SERVER,
        //    IID_PPV_ARGS(&m_spRequest));
        
        if (SUCCEEDED(hr)) {
        	hr = ParseResources( pWdfDevice, pWdfResourcesRaw, pWdfResourcesTranslated, &requestId);
        }
        
        if (FAILED(hr)) {
        	L2(WFN, L"ParseResources failed (%x).", hr);
        }
        
        if (SUCCEEDED(hr)) {
            // Create and initialize the request object
            hr = InitializeRequest(pWdfDevice, requestId);
        }
        
        if (SUCCEEDED(hr)) {
        	L2(WFN, L"m_bInitialized = TRUE");
        	m_bInitialized = TRUE;
        }
    }

    if (FAILED(InitializeDeviceSequence())) {
        L2(WFN, L"InitializeDeviceSequence failed!");
    }

    FuncExit();
}

HRESULT CDrv2605::InitializeDeviceSequence()
{
    HRESULT hr = S_OK;
    BYTE* pReadBuffer = new BYTE[1];
    BYTE* pWriteBuffer = new BYTE[1];

    if (m_bInitialized == TRUE) {
#if 0
		WriteDeviceData initSeq[16] = {
			/* 01 */ { DRV2605_REG_MODE, 0x00 },
			/* 02 */ { DRV2605_REG_REALTIME_PLAYBACK_IN, 0x00 },
			/* 03 */ { DRV2605_REG_WAVEFORM_SEQUENCER_00, 0x00 },
			/* 04 */ { DRV2605_REG_WAVEFORM_SEQUENCER_01, 0x00 },
			/* 05 */ { DRV2605_REG_OVERDRIVE_TIME_OFFSET, 0x00 },
			/* 06 */ { DRV2605_REG_SUSTAIN_TIME_OFFSET_POSITIVE, 0x00 },
			/* 07 */ { DRV2605_REG_SUSTAIN_TIME_OFFSET_NEGATIVE, 0x00 },
			/* 08 */ { DRV2605_REG_BRAKETIME_OFFSET, 0x00 },
			/* 09 */ { DRV2605_REG_RATED_VOLTAGE, 0x53 },
			/* 10 */ { DRV2605_REG_OVERDRIVE_CLAMP_VOLTAGE, 0xA4 },
			/* 11 */ { DRV2605_REG_FEEDBACK_CTRL, 0xEF },
			/* 12 */ { DRV2605_REG_CTRL_1, 0x93 },
			/* 13 */ { DRV2605_REG_CTRL_2, 0xF5 },
			/* 14 */ { DRV2605_REG_CTRL_3, 0xA0 },
			/* 15 */ { DRV2605_REG_MODE, 0x07 },
			/* 16 */ { DRV2605_REG_GO, 0x01 },
		};

		for (int itr = 0; itr < 16; itr++) {
			pWriteBuffer[0] = initSeq[itr].data;
			hr = WriteRegister(initSeq[itr].reg, pWriteBuffer, 1);
		}
#endif

		/*
		 *
		1.	Device Initialize and diagnostics (Please apply once at power on or resume timing)
			[Remove] 1.	Set “Data 00” to “register address 01” --> Clear Mode
			[Remove] 2.	Set “Data 00” to “register address 02” --> Clear Playback
			[Remove] 3.	Set “Data 00” to “register address 04” --> Clear Waveform sequencer
			[Remove] 4.	Set “Data 00” to “register address 05” --> Clear Waveform sequencer
			[Remove] 5.	Set “Data 00” to “register address 0D” --> Clear Over drive time
			[Remove] 6.	Set “Data 00” to “register address 0E” --> Clear Sustain time
			[Remove] 7.	Set “Data 00” to “register address 0F” --> Clear Sustain time
			[Remove] 8.	Set “Data 00” to “register address 10” --> Clear brake time
			9.	Set “Data 50” to “register address 16” --> Set Rated voltage
			[Update] 10. Set “Data 55” to “register address 17” --> Set Overdrive
			10. Set “Data 7F” to “register address 17” --> Set Overdrive
			11.	Set “Data EF” to “register address 1A” --> Se feedback
			12.	Set “Data 93” to “register address 1B” --> Set Control1
			13.	Set “Data F5” to “register address 1C” --> Set control2
			14.	Set “Data A0” to “register address 1D” --> Set Control3
			15.	Set “Data 07” to “register address 01” --> Set Auto calibration
			16.	Set “Data 01” to “register address 0C” --> Go command
			17.	Wait 1000msec (need to have 1msec to send next vibration command)
			18.	Set “Data 06” to “register address 03” --> Library select for LRA
			19.	Set “Data 17” to register address 04” --> Set waveform sequencer
			20.	Set “Data 00” to “register address 05” --> Set waveform sequencer
			21.	Set “Data 00” to “register address 01” --> Set internal trigger

		2.	To Apply vibration command
			[Remove] 1.	Set “register address 1D” to “Data A0” --> Set control3
			[Remove] 2.	Set “register address 16” to “Data 53” --> set rated voltage
			[Remove] 3.	Set “register address 17” to “Data A4” --> Set overdrive
			[Remove] 4.	Set “register address 18” to “Data 06” --> set calibration data
			[Remove] 5.	Set “register address 19” to “Data F2” --> set calibration data
			[Remove] 6.	Set “register address 1A” to “Data EF” --> Set feedback
			7. Set “Data 01” to “register address 0C” -->Go command
			[Remove] 8.	Wait 1msec
			[Remove] 9.	Set “register address 04” to “Data 19” --> Set another waveform sequencer
			[Remove] 10. Set “register address 05” to “Data 00” --> Clear waveform sequencer
			[Remove] 11. Set “register address 01” to “Data 00” --> Clear Mode
			[Remove] 12. Set “register address 1D” to “Data A0” --> Set Control3
			[Remove] 13. Set “register address 16” to “Data 53” --> Set rated voltage
			[Remove] 14. Set “register address 17” to “Data A4” --> set overdrive
			[Remove] 15. Set “register address 18” to “Data 06” --> set calibration data
			[Remove] 16. Set “register address 1A” to “Data EF” --> Set feedback
			[Remove] 17. Set “register address 0C” to “Data 01” --> Go command
			[Remove] 18. Wait 1msec
			[Remove] 19. Set “register address 1B” to “Data 93” --> set control1
			[Remove] 20. Set “register address 1C” to “Data F5” --> set control2
			[Remove] 21. Set “register address 1D” to “Data A0” --> set control3
		 */

        WriteDeviceData initSeq1[8] = {
            /* 09 */ { DRV2605_REG_RATED_VOLTAGE, 0x50 },
            /* 10 */ { DRV2605_REG_OVERDRIVE_CLAMP_VOLTAGE, 0x7F },
            /* 11 */ { DRV2605_REG_FEEDBACK_CTRL, 0xEF },
            /* 12 */ { DRV2605_REG_CTRL_1, 0x93 },
            /* 13 */ { DRV2605_REG_CTRL_2, 0xF5 },
            /* 14 */ { DRV2605_REG_CTRL_3, 0xA0 },
            /* 15 */ { DRV2605_REG_MODE, 0x07 },
            /* 16 */ { DRV2605_REG_GO, 0x01 },
        };

        for (int itr = 0; itr < 8; itr++) {
            pWriteBuffer[0] = initSeq1[itr].data;
            hr = WriteRegister(initSeq1[itr].reg, pWriteBuffer, 1);
        }

        Sleep(1000);
        
        WriteDeviceData initSeq2[4] = {
            /* 18 */ { DRV2605_REG_LIBRARY_SELECTION, 0x06 },
            /* 19 */ { DRV2605_REG_WAVEFORM_SEQUENCER_00, 0x17 },
            /* 20 */ { DRV2605_REG_WAVEFORM_SEQUENCER_01, 0x00 },
            /* 21 */ { DRV2605_REG_MODE, 0x00 },
        };

        for (int itr = 0; itr < 4; itr++) {
            pWriteBuffer[0] = initSeq2[itr].data;
            hr = WriteRegister(initSeq2[itr].reg, pWriteBuffer, 1);
        }
    }

    // Delete the buffer allocations
    if (pReadBuffer != nullptr) {
        delete[] pReadBuffer;
    }

    if (pWriteBuffer != nullptr) {
        delete[] pWriteBuffer;
    }

    return hr;
}

HRESULT CDrv2605::ParseResources(
	_In_ IWDFDevice *pWdfDevice,
	_In_ IWDFCmResourceList *pWdfResourcesRaw,
    _In_ IWDFCmResourceList *pWdfResourcesTranslated,
    _Out_ LARGE_INTEGER *pRequestId)
{
    FuncEntry();

    UNREFERENCED_PARAMETER(pWdfDevice); // used in interrupt only

    HRESULT hr = S_OK;

    PCM_PARTIAL_RESOURCE_DESCRIPTOR pDescriptor;
    PCM_PARTIAL_RESOURCE_DESCRIPTOR pDescriptorRaw;
    
    ULONG resourceCount;
    BOOLEAN fRequestFound = FALSE;
    // BOOLEAN fInterruptFound = FALSE;
    UCHAR connectionClass;
    UCHAR connectionType;

    UNREFERENCED_PARAMETER(pWdfResourcesRaw);

    if (pWdfResourcesTranslated == nullptr) {
        hr = E_INVALIDARG;
    }

    if (SUCCEEDED(hr)) {
        resourceCount = pWdfResourcesTranslated->GetCount();

        // Loop through the resources and save the relevant ones
        for (ULONG i = 0; i < resourceCount; i++) {
            pDescriptor = pWdfResourcesTranslated->GetDescriptor(i);
            pDescriptorRaw = pWdfResourcesRaw->GetDescriptor(i);

            if ((pDescriptor == nullptr) || (pDescriptorRaw == nullptr)) {
                hr = E_POINTER;
                break;
            }

            switch (pDescriptor->Type) {
                case CmResourceTypeConnection:
                    // Check against the expected connection types
                    connectionClass = pDescriptor->u.Connection.Class;
                    connectionType = pDescriptor->u.Connection.Type;

                    if ((connectionClass == CM_RESOURCE_CONNECTION_CLASS_SERIAL) &&
                        (connectionType == CM_RESOURCE_CONNECTION_TYPE_SERIAL_I2C)) {
                        if (fRequestFound == FALSE) {
                            // Save the request id
                            pRequestId->LowPart = pDescriptor->u.Connection.IdLowPart;
                            pRequestId->HighPart = pDescriptor->u.Connection.IdHighPart;

                            L2(WFN, L"[%d] IdLowPart = 0x%x", i, pRequestId->LowPart);
                            L2(WFN, L"[%d] IdHighPart = 0x%x", i, pRequestId->HighPart);

                            fRequestFound = TRUE;
                        } else {
                            hr = HRESULT_FROM_WIN32(ERROR_INVALID_PARAMETER);
                            Trace(TRACE_LEVEL_ERROR, "Duplicate resource found - %!HRESULT!", hr);
                        }
                    }

                    break;
                case CmResourceTypeInterrupt:
                    /*
                    if (fInterruptFound == FALSE) {
                        hr = ConnectInterrupt(
                            pWdfDevice,
                            pDescriptorRaw,
                            pDescriptor);

                        if (SUCCEEDED(hr)) {
                            fInterruptFound = TRUE;
                        }
                    } else {
                        Trace(
                            TRACE_LEVEL_WARNING,
                            "Duplicate interrupt resource found, ignoring");
                    }
                    */

                    break;
                default:
                    // Ignore all other descriptors
                    break;
            }

            if (FAILED(hr)) {
                break;
            }
        }

        if (SUCCEEDED(hr) && ((fRequestFound == FALSE) /*|| (fInterruptFound == FALSE)*/)) {
            hr = HRESULT_FROM_WIN32(ERROR_RESOURCE_NOT_PRESENT);
            Trace(TRACE_LEVEL_ERROR, "Failed to find required resource - %!HRESULT!", hr);
        }
    }

    FuncExit();

    return hr;
}

HRESULT CDrv2605::InitializeRequest(_In_ IWDFDevice *pWdfDevice, _In_ LARGE_INTEGER id)
{
    FuncEntry();

    HRESULT hr = S_OK;
	
    if (pWdfDevice == nullptr) {
        hr = E_INVALIDARG;
    }

    if (SUCCEEDED(hr)) {
        // Create the request object
        hr = CComObject<CSpbRequest>::CreateInstance(&m_pSpbRequest);
        
        if (SUCCEEDED(hr)) {
            m_pSpbRequest->AddRef();
            hr = m_pSpbRequest->QueryInterface(IID_PPV_ARGS(&m_spRequest));
        }

        // TODO: CoCreateInstance rather than calling
        //       CreateInstance on the class and querying
        //       the required interface.

        //// Create the request object
        //hr = CoCreateInstance(
        //    __uuidof(SpbRequest), // CLSID_SpbRequest
        //    nullptr,
        //    CLSCTX_INPROC_SERVER,
        //    IID_PPV_ARGS(&m_spRequest));

        if (FAILED(hr)) {
            Trace(TRACE_LEVEL_ERROR, "Failed to create the request object, %!HRESULT!", hr);
        }
        
        if (SUCCEEDED(hr)) {
            WCHAR DevicePathBuffer[RESOURCE_HUB_PATH_CHARS] = {0};
            UNICODE_STRING DevicePath;

            DevicePath.Buffer = DevicePathBuffer;
            DevicePath.Length = 0;
            DevicePath.MaximumLength = RESOURCE_HUB_PATH_SIZE;

            // Create the device path using the well known
            // resource hub path and the connection ID
            hr = HRESULT_FROM_NT( RESOURCE_HUB_CREATE_PATH_FROM_ID(
                &DevicePath,
                id.LowPart,
                id.HighPart));

            // Initialize the request object
            if (SUCCEEDED(hr)) {
                L2(WFN, L"RESOURCE_HUB_CREATE_PATH_FROM_ID success.");

                // NULL-terminate the buffer
                DevicePathBuffer[RESOURCE_HUB_PATH_CHARS - 1] = L'\0';

                hr = m_spRequest->Initialize(
                    pWdfDevice,
                    DevicePathBuffer);

                L2(WFN, L"DevicePathBuffer = %s", DevicePathBuffer);
            }
        }

        if (SUCCEEDED(hr)) {
            // Create the data buffer
            m_pDataBuffer = new BYTE[6]; // temp number (TBD)

            if (m_pDataBuffer == nullptr) {
                hr = E_OUTOFMEMORY;
            }

            /*
            pWriteBuffer[0] = 0x00;
            hr = WriteRegister(DRV2605_REG_MODE, pWriteBuffer, 1);

            if (SUCCEEDED(hr)) {
                L2(WFN, L"Actual device should be active now.");
            }

            if (FAILED(hr)) {
                L2(WFN, L"Failed to activate device (%x).", hr);
            }
            */
        }
    }

    FuncExit();

    return hr;
}

/////////////////////////////////////////////////////////////////////////
//
//  CDrv2605::ReadRegister
//
//  This method is used to read a buffer of data from the device's register
//  interface.  The first byte is read from the specified register, the second
//  from the next, etc.
//
//  Parameters:
//      reg - the first register to be read from
//      pDataBuffer - pointer to the output buffer
//      dataBufferLength - size of the output buffer
//
//  Return Values:
//      status
//
/////////////////////////////////////////////////////////////////////////
HRESULT CDrv2605::ReadRegister(
    _In_ BYTE reg,
    _Out_writes_(dataBufferLength) BYTE *pDataBuffer,
    _In_ ULONG dataBufferLength,
    _In_ ULONG delayInUs)
{
    FuncEntry();

    HRESULT hr = (m_bInitialized == TRUE) ? S_OK : E_UNEXPECTED;

    if (SUCCEEDED(hr) && pDataBuffer == nullptr) {
        hr = E_INVALIDARG;
    }

    if (SUCCEEDED(hr)) {
        Trace(TRACE_LEVEL_VERBOSE, "Read %lu bytes from register 0x%02x", dataBufferLength, reg);

        // Execute the write-read sequence
        hr = m_spRequest->CreateAndSendWriteReadSequence(
            &reg,
            1,
            pDataBuffer,
            dataBufferLength,
            delayInUs);

		if (SUCCEEDED(hr)) {
			L2(WFN, L"CreateAndSendWriteReadSequence success.");
		}

		if (FAILED(hr)) {
			L2(WFN, L"CreateAndSendWriteReadSequence failed (%x).", hr);
		}
    }

    if (FAILED(hr)) {
        Trace(TRACE_LEVEL_ERROR, "Failed to read from register 0x%02x, %!HRESULT!", reg, hr);
    }

    FuncExit();

    return hr;
}

/////////////////////////////////////////////////////////////////////////
//
//  CDrv2605::WriteRegister
//
//  This method is used to write a buffer of data to the device's register
//  interface.  The first byte is written to the specified register, the 
//  second from the next, etc.
//
//  Parameters:
//      reg - the first register to be written to
//      pDataBuffer - pointer to the output buffer
//      dataBufferLength - size of the output buffer
//
//  Return Values:
//      status
//
/////////////////////////////////////////////////////////////////////////
HRESULT CDrv2605::WriteRegister(
    _In_ BYTE reg,
	_In_reads_(dataBufferLength) BYTE *pDataBuffer,
	_In_ ULONG dataBufferLength)
{
    FuncEntry();

    HRESULT hr = (m_bInitialized == TRUE) ? S_OK : E_UNEXPECTED;

    if (SUCCEEDED(hr) && pDataBuffer == nullptr) {
        hr = E_INVALIDARG;
    }

    if (SUCCEEDED(hr)) {   
        // A write-write sequence is implemented with
        // a single write request. Allocate a buffer to
        // hold the register and data.
        ULONG bufferLength = dataBufferLength + 1;
        BYTE* pBuffer = new BYTE[bufferLength];

        if (pBuffer == nullptr) {
            hr = E_OUTOFMEMORY; 
        }

        if (SUCCEEDED(hr)) {
            // Fill the buffer
            memcpy(pBuffer, &reg, 1);
            memcpy((pBuffer + 1), pDataBuffer, dataBufferLength);
            
            Trace(TRACE_LEVEL_VERBOSE, "Write %lu bytes to register 0x%02x", dataBufferLength, reg);
            
            // Execute the write
            hr = m_spRequest->CreateAndSendWrite(
                pBuffer,
                bufferLength);
        }
           
        // Destroy the allocated buffer
        if (pBuffer != nullptr) {
            delete[] pBuffer;
        }

    }

    if (FAILED(hr)) {
        Trace(TRACE_LEVEL_ERROR, "Failed to write to register 0x%02x, %!HRESULT!", reg, hr);
    }

    FuncExit();

    return hr;
}

HRESULT CDrv2605::TestInterface1(int libIndex)
{
	UNREFERENCED_PARAMETER(libIndex);

	HRESULT hr = S_OK;
	BYTE* pReadBuffer = new BYTE[1];
	BYTE* pWriteBuffer = new BYTE[1];

	if (m_bInitialized == TRUE) {
#if 0
		/*
		I captured Haptic control command on I2C bus.
		Your software is using following command to work DRM2605 driver.
		Set “register address 01” to “Data 00” - Clear Status
		Set “register address 04” to “Data 1B” - Set waveform sequencer 27 strong double click
		Set “register address 0C” to “Data 01” - Go command

		Maybe that is not enough to enlarge vibration.
		Please try to do following sequence. Any question, please let me know.
		Device Initialize and diagnostics (Please apply once at power on or resume timing)
		Set “register address 01” to “Data 00” - Clear Mode
		Set “register address 02” to “Data 00” - Clear Playback
		Set “register address 04” to “Data 00” - Clear Waveform sequencer
		Set “register address 05” to “Data 00” - Clear Waveform sequencer
		Set “register address 0D” to “Data 00” - Clear Over drive time
		Set “register address 0E” to “Data 00” - Clear Sustain time
		Set “register address 0F” to “Data 00” - Clear Sustain time
		Set “register address 10” to “Data 00” - Clear brake time
		Set “register address 16” to “Data 53” - Set Rated voltage
		Set “register address 17” to “Data A4” - Set Overdrive
		Set “register address 1A” to “Data EF” - Set feedback
		Set “register address 1B” to “Data 93” - Set Control1
		Set “register address 1C” to “Data F5” - Set control2
		Set “register address 1D” to “Data A0” - Set Control3
		Set “register address 01” to “Data 07” - Set Auto calibration
		Set “register address 0C” to “Data 01” - Go command
		Wait 1msec (need to have 1msec to send next vibration command)
		To Apply vibration command
		Set “register address 03” to “Data 06” - Library select for LRA
		Set “register address 04” to “Data 01” - Set waveform sequencer
		Set “register address 05” to “Data 00” - Set waveform sequencer
		Set “register address 01” to “Data 00” - Set internal trigger
		Set “register address 1D” to “Data A0” - Set control3
		Set “register address 16” to “Data 53” - Set rated voltage
		Set “register address 17” to “Data A4” - Set overdrive
		Set “register address 18” to “Data 06” - Set calibration data
		Set “register address 19” to “Data F2” - Set calibration data
		Set “register address 1A” to “Data EF” - Set feedback
		Set “register address 0C” to “Data 01” - Go command
		Wait 1msec
		Set “register address 04” to “Data 19” - Set another waveform sequencer
		Set “register address 05” to “Data 00” - Clear waveform sequencer
		Set “register address 01” to “Data 00” - Clear Mode
		Set “register address 1D” to “Data A0” - Set Control3
		Set “register address 16” to “Data 53” - Set rated voltage
		Set “register address 17” to “Data A4” - Set overdrive
		Set “register address 18” to “Data 06” - Set calibration data
		Set “register address 1A” to “Data EF” - Set feedback
		Set “register address 0C” to “Data 01” - Go command
		Wait 1msec
		Set “register address 1B” to “Data 93” - Set control1
		Set “register address 1C” to “Data F5” - Set control2
		Set “register address 1D” to “Data A0” - Set control3
		*/

		WriteDeviceData write1[11] = {
			/* 01 */ { DRV2605_REG_LIBRARY_SELECTION, 0x06 },
			/* 02 */ { DRV2605_REG_WAVEFORM_SEQUENCER_00, (BYTE)libIndex },
			/* 03 */ { DRV2605_REG_WAVEFORM_SEQUENCER_01, 0x00 },
			/* 04 */ { DRV2605_REG_MODE, 0x00 },
			/* 05 */ { DRV2605_REG_CTRL_3, 0xA0 },
			/* 06 */ { DRV2605_REG_RATED_VOLTAGE, 0x53 },
			/* 07 */ { DRV2605_REG_OVERDRIVE_CLAMP_VOLTAGE, 0xA4 },
			/* 08 */ { DRV2605_REG_AUTO_CALIB_COMPENSATION_RESULT, 0x06 },
			/* 09 */ { DRV2605_REG_AUTO_CALIB_BACK_EMF_RESULT, 0xF2 },
			/* 10 */ { DRV2605_REG_FEEDBACK_CTRL, 0xEF },
			/* 11 */ { DRV2605_REG_GO, 0x01 },
		};

		for (int itr = 0; itr < 11; itr++)
		{
			pWriteBuffer[0] = write1[itr].data;
			hr = WriteRegister(write1[itr].reg, pWriteBuffer, 1);
		}

		// 22
		Sleep(1);

		WriteDeviceData write2[3] = {
			/* 23 */ { DRV2605_REG_CTRL_1, 0x93 },
			/* 24 */ { DRV2605_REG_CTRL_2, 0xF5 },
			/* 25 */ { DRV2605_REG_CTRL_3, 0xA0 },
		};

		for (int itr = 0; itr < 3; itr++)
		{
			pWriteBuffer[0] = write2[itr].data;
			hr = WriteRegister(write2[itr].reg, pWriteBuffer, 1);
		}
#endif

		/*
		 *
		1.	Device Initialize and diagnostics (Please apply once at power on or resume timing)
			[Remove] 1.	Set “Data 00” to “register address 01” --> Clear Mode
			[Remove] 2.	Set “Data 00” to “register address 02” --> Clear Playback
			[Remove] 3.	Set “Data 00” to “register address 04” --> Clear Waveform sequencer
			[Remove] 4.	Set “Data 00” to “register address 05” --> Clear Waveform sequencer
			[Remove] 5.	Set “Data 00” to “register address 0D” --> Clear Over drive time
			[Remove] 6.	Set “Data 00” to “register address 0E” --> Clear Sustain time
			[Remove] 7.	Set “Data 00” to “register address 0F” --> Clear Sustain time
			[Remove] 8.	Set “Data 00” to “register address 10” --> Clear brake time
			9.	Set “Data 50” to “register address 16” --> Set Rated voltage
			[Update] 10. Set “Data 55” to “register address 17” --> Set Overdrive
			10. Set “Data 7F” to “register address 17” --> Set Overdrive
			11.	Set “Data EF” to “register address 1A” --> Se feedback
			12.	Set “Data 93” to “register address 1B” --> Set Control1
			13.	Set “Data F5” to “register address 1C” --> Set control2
			14.	Set “Data A0” to “register address 1D” --> Set Control3
			15.	Set “Data 07” to “register address 01” --> Set Auto calibration
			16.	Set “Data 01” to “register address 0C” --> Go command
			17.	Wait 1000msec (need to have 1msec to send next vibration command)
			18.	Set “Data 06” to “register address 03” --> Library select for LRA
			19.	Set “Data 17” to register address 04” --> Set waveform sequencer
			20.	Set “Data 00” to “register address 05” --> Set waveform sequencer
			21.	Set “Data 00” to “register address 01” --> Set internal trigger

		2.	To Apply vibration command
			[Remove] 1.	Set “register address 1D” to “Data A0” --> Set control3
			[Remove] 2.	Set “register address 16” to “Data 53” --> set rated voltage
			[Remove] 3.	Set “register address 17” to “Data A4” --> Set overdrive
			[Remove] 4.	Set “register address 18” to “Data 06” --> set calibration data
			[Remove] 5.	Set “register address 19” to “Data F2” --> set calibration data
			[Remove] 6.	Set “register address 1A” to “Data EF” --> Set feedback
			7. Set “Data 01” to “register address 0C” -->Go command
			[Remove] 8.	Wait 1msec
			[Remove] 9.	Set “register address 04” to “Data 19” --> Set another waveform sequencer
			[Remove] 10. Set “register address 05” to “Data 00” --> Clear waveform sequencer
			[Remove] 11. Set “register address 01” to “Data 00” --> Clear Mode
			[Remove] 12. Set “register address 1D” to “Data A0” --> Set Control3
			[Remove] 13. Set “register address 16” to “Data 53” --> Set rated voltage
			[Remove] 14. Set “register address 17” to “Data A4” --> set overdrive
			[Remove] 15. Set “register address 18” to “Data 06” --> set calibration data
			[Remove] 16. Set “register address 1A” to “Data EF” --> Set feedback
			[Remove] 17. Set “register address 0C” to “Data 01” --> Go command
			[Remove] 18. Wait 1msec
			[Remove] 19. Set “register address 1B” to “Data 93” --> set control1
			[Remove] 20. Set “register address 1C” to “Data F5” --> set control2
			[Remove] 21. Set “register address 1D” to “Data A0” --> set control3
		 */

		pWriteBuffer[0] = 0x01;
		hr = WriteRegister(DRV2605_REG_GO, pWriteBuffer, 1);
	}

	//
	// Delete the buffer allocations
	//
	if (pReadBuffer != nullptr) {
		delete[] pReadBuffer;
	}

	if (pWriteBuffer != nullptr) {
		delete[] pWriteBuffer;
	}

	return hr;
}
