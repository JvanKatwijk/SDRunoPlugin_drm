
#include	"aac-handler.h"
#include	"SDRunoPlugin_drmUi.h"


static HANDLE_AACDECODER 
	 aacDecoder_Open_dummy (TRANSPORT_TYPE transportFmt, UINT nrOfLayers) {
	(void)transportFmt; (void)nrOfLayers;
	return 0;
}

static void aacDecoder_Close_dummy (HANDLE_AACDECODER self) {
	(void)self;
}

static AAC_DECODER_ERROR
	aacDecoder_ConfigRaw_dummy (HANDLE_AACDECODER self,
                                                  UCHAR *conf[],
                                                  const UINT length[]) {
	(void)self; (void)conf; (void)length;
	return AAC_DEC_UNKNOWN;
}

static CStreamInfo *
	aacDecoder_GetStreamInfo_dummy (HANDLE_AACDECODER self) {
	(void)self;
	return nullptr;
}

static AAC_DECODER_ERROR
	aacDecoder_Fill_dummy (HANDLE_AACDECODER self,
                                             UCHAR *pBuffer[],
                                             const UINT bufferSize[],
                                             UINT *bytesValid) {
	(void)self; (void)pBuffer; (void)bufferSize; (void)bytesValid;
	return AAC_DEC_UNKNOWN;
}

static AAC_DECODER_ERROR
	aacDecoder_DecodeFrame_dummy (HANDLE_AACDECODER self,
                                                    INT_PCM *pTimeData,
                                                    const INT timeDataSize,
                                                    const UINT flags) {
	(void)self; (void)pTimeData; (void)timeDataSize; (void)flags;
	return AAC_DEC_UNKNOWN;
}
	
	aacHandler::aacHandler		(SDRunoPlugin_drmUi *m_form) {
	wchar_t *libname = (wchar_t *)L"libfdk-aac-2.dll";
	Handle	= LoadLibrary (libname);
	if (Handle == nullptr) {
	   std::string errorMessage = "please install libfdk-aac-2.dll";
	   m_form ->  set_messageLabel (errorMessage);
	   goto Err;
	}

	aacDecoder_Open = (pfn_aacDecoder_Open)
                               GETPROCADDRESS (Handle, "aacDecoder_Open");
        if (aacDecoder_Open == nullptr) {
           m_form -> set_messageLabel ("Could not find aacDecoder_Open\n");
	   goto Err;
        }

	aacDecoder_Close = (pfn_aacDecoder_Close)
                           GETPROCADDRESS (Handle, "aacDecoder_Close");
        if (aacDecoder_Close == nullptr) {
           m_form -> set_messageLabel ("Could not find aacDecoder_Close\n");
	   goto Err;
        }

	aacDecoder_ConfigRaw = (pfn_aacDecoder_ConfigRaw)
                           GETPROCADDRESS (Handle, "aacDecoder_ConfigRaw");
        if (aacDecoder_ConfigRaw == nullptr) {
           m_form -> set_messageLabel ("Could not find aacDecoder_ConfigRaw\n");
	   goto Err;
        }

	aacDecoder_GetStreamInfo = (pfn_aacDecoder_GetStreamInfo)
                            GETPROCADDRESS (Handle, "aacDecoder_GetStreamInfo");
        if (aacDecoder_GetStreamInfo == nullptr) {
           m_form -> set_messageLabel ("Could not find aacDecoder_GetStreamInfo\n");
	   goto Err;
        }

	aacDecoder_Fill = (pfn_aacDecoder_Fill)
                            GETPROCADDRESS (Handle, "aacDecoder_Fill");
        if (aacDecoder_Fill == nullptr) {
           m_form -> set_messageLabel ("Could not find aacDecoder_Fill\n");
	   goto Err;
        }

	aacDecoder_DecodeFrame = (pfn_aacDecoder_DecodeFrame)
                            GETPROCADDRESS (Handle, "aacDecoder_DecodeFrame");
        if (aacDecoder_DecodeFrame == nullptr) {
           m_form -> set_messageLabel ("Could not find aacDecoder_DecodeFrame\n");
	   goto Err;
        }

	return;

Err:
	if (Handle != nullptr)
           FreeLibrary (Handle);
	Handle	= nullptr;

	aacDecoder_Open			= aacDecoder_Open_dummy;
	aacDecoder_Close		= aacDecoder_Close_dummy;
        aacDecoder_ConfigRaw		= aacDecoder_ConfigRaw_dummy;
        aacDecoder_GetStreamInfo	= aacDecoder_GetStreamInfo_dummy;
        aacDecoder_Fill			= aacDecoder_Fill_dummy;
        aacDecoder_DecodeFrame		= aacDecoder_DecodeFrame_dummy;
}

	aacHandler::~aacHandler		() {
	if (Handle != nullptr)
	   FreeLibrary (Handle);
}


