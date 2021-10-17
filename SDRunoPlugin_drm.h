#pragma once

#include	<thread>
#include	<mutex>
#include	<atomic>
#include	<iunoplugincontroller.h>
#include	<iunoplugin.h>
#include	<iunostreamobserver.h>
#include	<iunoaudioobserver.h>
#include	<iunoaudioprocessor.h>
#include	<iunostreamobserver.h>
#include	<iunoannotator.h>
#include	<nana/gui.hpp>
#include	<stdint.h>
#include	"SDRunoPlugin_drmUi.h"
#include	"aac-handler.h"

//      for the payload we have
#include	"ringbuffer.h"
#include	"decimator-25.h"
#include	"drm-bandfilter.h"
#include	"lowpassfilter.h"
#include	"utilities.h"
#include	"basics.h"
#include	"ofdm\reader.h"
#include	"msc\backend-controller.h"
#include	"parameters\state-descriptor.h"
#include	".\support\drm-shifter.h"
#include	"support\my-array.h"


#define         DECIMATOR       5
#define         INRATE          (2000000 / 32)
#define         INTERM_RATE     (INRATE / DECIMATOR)
#define         WORKING_RATE    12000
#define         FILTER_DEFAULT  21


#define         SIZE_IN         625
#define         SIZE_OUT        720
#define         SIZE_END        120

using namespace nana;

class SDRunoPlugin_drm : public IUnoPlugin,
	                            public IUnoStreamProcessor,
                                    public IUnoAudioProcessor {
public:
	
		SDRunoPlugin_drm (IUnoPluginController& controller);
	virtual ~SDRunoPlugin_drm ();

	virtual const char* GetPluginName() const override { return "SDRuno drm Plugin"; }

	// IUnoPlugin
	virtual
	void	HandleEvent(const UnoEvent& ev) override;
	virtual
        void    StreamProcessorProcess (channel_t channel,
                                        Complex *buffer,
                                        int length,
                                        bool& modified) override;
        virtual
        void    AudioProcessorProcess (channel_t channel,
                                       float *buffer,
                                       int length, bool& modified) override;
	void	activate_channel_1	();
	void	activate_channel_2	();
private:
	std::mutex		m_lock;
	SDRunoPlugin_drmUi      m_form;
	std::mutex	        locker;
	IUnoPluginController	*m_controller;
	RingBuffer<std::complex<DRM_FLOAT>>	inputBuffer;
	drmShifter	        theMixer;
	drmBandfilter	        passbandFilter;
	decimator_25	        theDecimator;
//	drmShifter	        localMixer;
	RingBuffer<std::complex<float>>	drmAudioBuffer;
	void			WorkerFunction	();
	std::thread		* m_worker;
	void			process		(std::complex<DRM_FLOAT>);
	void			processSample	(std::complex<DRM_FLOAT>);
	int			resample	(std::complex<DRM_FLOAT>,
                                                 std::complex<DRM_FLOAT> *);
	uint8_t			getSpectrum	(stateDescriptor *);
	void			getMode		(Reader *my_Reader,
	                                                 smodeInfo *m);


        std::vector<std::complex<DRM_FLOAT>> convBuffer;
        int		convIndex;
        int16_t		mapTable_int   [SIZE_OUT];
        DRM_FLOAT	mapTable_float [SIZE_OUT];
//
//
        std::atomic<bool>       running;

	bool		drmError;
	int		centerFrequency;
        int             VFOFRequency;
        int             selectedFrequency;
        int             Raw_Rate;
        int		drmAudioRate;
//
//
	Reader          my_Reader;              // single instance during life
	aacHandler	aacFunctions;
	backendController	my_backendController;
        int16_t         nSymbols;
        int32_t         sampleRate;
        int8_t          windowDepth;
        smodeInfo       modeInf;
	stateDescriptor	theState;
	void		frequencySync		(Reader	*my_Reader,
	                                         smodeInfo *m);
	int16_t		sdcCells		(smodeInfo *);
	bool		isFACcell		(smodeInfo *, int16_t, int16_t);
	bool		isSDCcell		(smodeInfo *, int16_t, int16_t);
	bool		isDatacell		(smodeInfo *,
                                                 int16_t,
                                                 int16_t,
	                                         int16_t);

	void		set_sdcCells		(smodeInfo *);
	std::vector<sdcCell> sdcTable;
	bool		isFirstFrame		(stateDescriptor *);
	bool		isLastFrame		(stateDescriptor *);
	void		addtoSuperFrame		(smodeInfo *,
                                                 int16_t,
	                                         myArray<theSignal> *);
};
