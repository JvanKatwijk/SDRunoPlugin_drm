
#include	<sstream>
#include	<unoevent.h>
#include	<iunoplugincontroller.h>
#include	<vector>
#include	<sstream>
#include	<chrono>
#include	<Windows.h>

#include "SDRunoPlugin_drm.h"
#include "SDRunoPlugin_drmUi.h"

//
//	drm specifics

#include	"drm-bandfilter.h"
#include	".\support\drm-shifter.h"
#include	"utilities.h"

#include	"ofdm\timesync.h"
#include	"ofdm\freqsyncer.h"
#include	"ofdm\word-collector.h"
#include	"ofdm\correlator.h"
#include	"support/my-array.h"
#include	"equalizer\referenceframe.h"
#include	"equalizer\equalizer-1.h"
#include	"fac\fac-processor.h"
#include	"fac\fac-tables.h"
#include	"sdc\sdc-processor.h"

#define  _USE_MATH_DEFINES
#include	<math.h>

	SDRunoPlugin_drm::
	               SDRunoPlugin_drm (IUnoPluginController& controller) :
	                                 IUnoPlugin (controller),
	                                 m_form (*this, controller),
	                                 m_worker (nullptr),
	                                 inputBuffer  (16 * 32768),
	                                 theMixer     (INRATE),
	                                 passbandFilter (25, 5000, INRATE),
	                                 audioFilter (15, 24000, INRATE),
	                                 theDecimator (INRATE / WORKING_RATE),
	                                 drmAudioBuffer (32768),
	                                 my_Reader (&inputBuffer,
	                                            2 * 16384, &m_form),
	                                 aacFunctions (&m_form),
	                                 my_backendController (&m_form, 4,
	                                                      &aacFunctions,
	                                                      &drmAudioBuffer),
	                                 theState (1, 3) {
	m_controller	= &controller;
	running. store (false);
	m_controller	-> RegisterAudioProcessor(0, this);
	m_controller	-> SetDemodulatorType (0,
	                         IUnoPluginController::DemodulatorIQOUT);
	int selectedFrequency
                        = m_controller -> GetVfoFrequency (0);
	int centerFrequency
	                 = m_controller -> GetCenterFrequency (0);

	drmAudioRate	= m_controller -> GetAudioSampleRate(0);

	drmError	= false;		// not used

	nSymbols	= 25;
	modeInf. Mode	= 2;
	modeInf. Spectrum	= 3;

	m_worker	=
	       new std::thread (&SDRunoPlugin_drm::WorkerFunction, this);
}

	SDRunoPlugin_drm::~SDRunoPlugin_drm () {	
	running. store (false);
	my_Reader. stop ();
	m_worker        -> join ();
//	m_controller    -> UnregisterStreamProcessor (0, this);
	m_controller    -> UnregisterAudioProcessor (0, this);
	delete m_worker;
	m_worker = nullptr;
}
//
//	Called by the underlying system
//
void	SDRunoPlugin_drm::
	         StreamProcessorProcess (channel_t channel,
	                                 Complex* buffer,
	                                 int	length,
	                                 bool& modified) { }
//
//	drmAudiobuffer has complex elements, we extract floats
void	SDRunoPlugin_drm::AudioProcessorProcess (channel_t channel,
	                                         float* buffer,
	                                         int length,
	                                         bool& modified) {
//
//	Handling IQ input, note that SDRuno interchanges I and Q elements
	if (!modified) {
	   for (int i = 0; i < length; i++) {
	      std::complex<DRM_FLOAT> sample =
	                   std::complex<DRM_FLOAT>(buffer [2 * i +  1],
	                                           buffer [2 * i]);
	      sample = passbandFilter.Pass (sample);
//	since Offset == 0, we do not need a shift here
//	      sample = theMixer.do_shift (sample, theOffset);
	      if (theDecimator.Pass (sample, &sample))
	         inputBuffer.putDataIntoBuffer (&sample, 1);
	   }
	}
//	for the audio out, length in complexes, but buffer is in floats
//	so its actual size is 2 * length
//	Note further that the design was for 48 k output samples and 
//	IQ requires 192, so we upsample a factor of 4
	int amount;
	amount = drmAudioBuffer. GetRingBufferReadAvailable ();
	if (amount >= length / 4) 
	   amount = length / 4;
	std::complex<float>*lbuf =
	              (std::complex<float> *)
	                 (_malloca (amount * sizeof (std::complex<float>)));
	drmAudioBuffer. getDataFromBuffer (lbuf, length / 4);
	int bufferP = 0;
	for (int i = 0; i < amount; i ++) {
	   std::complex<float> s = audioFilter. Pass (lbuf [i]);
	   buffer [bufferP ++] = real (s);
	   buffer [bufferP ++] = imag (s);
	   for (int j = 0; j < 3; j ++) {
	      s = audioFilter. Pass (std::complex<float> (0, 0));
	      buffer [bufferP ++] =  real (s);
	      buffer [bufferP ++] =  imag (s);
	   }
	}
//	If what we have available is less than the buffersize,
//	fil it with 0
	for (int i = bufferP / 2; i < length; i ++) {
	   std::complex<float> s =
	                audioFilter. Pass (std::complex<float> (0, 0));
	   buffer [2 * i] = real (s);
	   buffer [2 * i+ 1] = imag (s);
	}
	modified = true;
}

void	SDRunoPlugin_drm::HandleEvent (const UnoEvent& ev) {
	int filterOffset = m_controller->GetFilterBandwidth(0) / 2;
	switch (ev. GetType ()) {
	   case UnoEvent::FrequencyChanged:
	     break;

	   case UnoEvent::CenterFrequencyChanged:
		  break;

	   default:
	      m_form. HandleEvent (ev);
	      break;
	}
}

void	SDRunoPlugin_drm::WorkerFunction () {
int16_t	blockCount      = 0;
bool	inSync;
int16_t	symbol_no       = 0;
bool	frameReady;
int counter = 0;
DRM_FLOAT     deltaFreqOffset         = 0;
DRM_FLOAT     sampleclockOffset       = 0;


	running. store (true);
	while (running. load ()) {
	   try {
	      if (!running. load ())
	         throw (21);
	      counter++;
	      m_form. set_timeSyncLabel	(false);
	      m_form. set_facSyncLabel	(false);
	      m_form. set_sdcSyncLabel	(false);
	      m_form. set_audioModeLabel	(std::string (""));
	      m_form. hide_channel_1 ();
	      m_form. hide_channel_2 ();
	      m_form. showService ("");
	
	      theState. cleanUp ();
	      my_Reader. waitfor (Ts_of (Mode_A));
	  
//      First step: find mode and starting point
	      modeInf. Mode = -1;
	      while (running. load () && (modeInf. Mode == -1)) {
	         my_Reader. shiftBuffer (Ts_of (Mode_A) / 3);
	         getMode (&my_Reader, &modeInf);
	      }
	
	      if (!running. load ())
	         throw (20);

	      m_form. set_modeIndicator (modeInf. Mode);
	      m_form. set_timeOffsetDisplay (modeInf. timeOffset_integer);
	      m_form. set_smallOffsetDisplay (modeInf. freqOffset_fractional);

	      my_Reader. shiftBuffer (modeInf. timeOffset_integer);
	      frequencySync (&my_Reader, &modeInf);

	      m_form. set_timeSyncLabel		(true);
	      m_form. set_modeIndicator		(modeInf. Mode);
	      m_form. set_spectrumIndicator	(modeInf. Spectrum);
	      m_form. set_intOffsetDisplay	(modeInf. freqOffset_integer);

	      theState. Mode		= modeInf. Mode;
	      theState. Spectrum	= modeInf. Spectrum;
	      int nrSymbols		= symbolsperFrame (modeInf. Mode);
	      int nrCarriers       = Kmax (modeInf. Mode, modeInf. Spectrum) -
	                             Kmin (modeInf. Mode, modeInf. Spectrum) + 1;

	      myArray<std::complex<DRM_FLOAT>> inbank (nrSymbols, nrCarriers);
	      myArray<theSignal> outbank (nrSymbols, nrCarriers);
	      correlator myCorrelator (&modeInf);
	      equalizer_1 my_Equalizer (modeInf.Mode,
	                                modeInf.Spectrum, 2);
	      std::vector<std::complex<DRM_FLOAT>> displayVector;
	      displayVector. resize (Kmax (modeInf. Mode, modeInf. Spectrum) -
	                             Kmin (modeInf. Mode, modeInf. Spectrum) + 1);
	      wordCollector my_wordCollector (&m_form,
	                                      &my_Reader,
	                                      &modeInf,
	                                      WORKING_RATE);
	      facProcessor my_facProcessor (&m_form, &modeInf);

//	   we know that - when starting - we are not "in sync" yet
	      inSync	= false;
//
//	   we read one full frame after which we start looking for a 
//	   match
	      for (int symbol = 0; symbol < nrSymbols - 1; symbol ++) {
	         my_wordCollector. getWord (inbank. element (symbol),
	                                    modeInf. freqOffset_integer,
	                                    modeInf. timeOffset_fractional,
	                                    modeInf. freqOffset_fractional
	                                   );
	         myCorrelator. correlate (inbank. element (symbol), symbol);
	       }

	      int16_t errors       = 0;
	      int  lc      = nrSymbols - 1;
//      We keep on reading here until we are satisfied that the
//      frame that is in, looks like a decent frame, just by the
//      correlation on the first word
	      while (running. load ()) {
	         my_wordCollector. getWord (inbank. element (lc),
	                                    modeInf. freqOffset_integer,
	                                    modeInf. timeOffset_fractional,
	                                    modeInf. freqOffset_fractional
	                                   );
	         myCorrelator. correlate (inbank. element (lc), lc);
	         lc = (lc + 1) % symbolsperFrame (modeInf. Mode);
	         if (myCorrelator. bestIndex (lc))  {
	            break;
	         }
	      }
		
//      from here on, we know that in the input bank, the frames occupy the
//      rows "lc" ... "(lc + symbolsinFrame) % symbolsinFrame"
//      so, once here, we know that the frame starts with index lc,
//      so let us equalize the last symbolsinFrame words in the buffer
	      for (symbol_no = 0; symbol_no < nrSymbols; symbol_no ++)
	         (void) my_Equalizer.
	            equalize (inbank. element ((lc + symbol_no) % nrSymbols),
	                      symbol_no,
	                      &outbank,
                              &modeInf. timeOffset_fractional,
                              &deltaFreqOffset,
                              &sampleclockOffset,
	                      displayVector);

	      lc           = (lc + symbol_no) % symbol_no;
	      symbol_no    = 0;
	      frameReady   = false;

	      while (running. load () && !frameReady) {
		  my_wordCollector.getWord(inbank.element(lc),
				  modeInf.freqOffset_integer,
				  false,        // no-op
				  modeInf.timeOffset_fractional,
				  deltaFreqOffset,  // tracking value
				  sampleclockOffset); // tracking value

	         frameReady = my_Equalizer. 
	                            equalize (inbank. element (lc),
	                                      symbol_no,
	                                      &outbank,
	                                      &modeInf. timeOffset_fractional,
                                              &deltaFreqOffset,
                                              &sampleclockOffset,
	                                      displayVector);
			
	         lc = (lc + 1) % nrSymbols;
	         symbol_no = (symbol_no + 1) % nrSymbols;
	      }
	      if (!running.load())
	         throw (37);

//	when we are here, we do have  our first full "frame".
//	so, we will be convinced that we are OK when we have a decent FAC
	      inSync = my_facProcessor.  processFAC  (&outbank, &theState);

	  //	one test:
	      if (!inSync)
	         throw (33);
	      if (modeInf.Spectrum != getSpectrum(&theState))
	         throw (34);
	      m_form.set_facSyncLabel (true);
		
//
//	prepare for sdc processing
//	Since computing the position of the sdc Cells depends (a.o)
//	on FAC and other data cells, we better create the table here.
	      sdcTable. resize (sdcCells (&modeInf));
	      set_sdcCells (&modeInf);
	      sdcProcessor my_sdcProcessor (&m_form, &modeInf,
	                                    sdcTable, &theState);

	      bool	superframer		= false;
	      int	missers			= 0;
	      bool	firstTime		= true;
	      DRM_FLOAT	deltaFreqOffset		= 0;
	      DRM_FLOAT	sampleclockOffset	= 0;
		  
		
	      while (true) {
//	when we are here, we can start thinking about  SDC's and superframes
//	The first frame of a superframe has an SDC part
	         if (isFirstFrame (&theState)) {
	            bool sdcOK = my_sdcProcessor. processSDC (&outbank);
	            m_form. set_sdcSyncLabel (sdcOK);
	            if (sdcOK) {
	               blockCount	= 0;
	            }
//
//	if we seem to have the start of a superframe, we
//	re-create a backend with the right parameters
	            if (!superframer && sdcOK)
	               my_backendController. reset (&theState);
	            superframer	= sdcOK;
	         }
//
//	when here, add the current frame to the superframe.
//	Obviously, we cannot garantee that all data is in order
	         if (superframer)
	            addtoSuperFrame (&modeInf, blockCount ++, &outbank);

//	when we are here, it is time to build the next frame
	         frameReady	= false;
	         for (int i = 0; !frameReady && (i < nrSymbols); i ++) {
	            my_wordCollector.
	               getWord (inbank. element ((lc + i) % nrSymbols),
	                        modeInf. freqOffset_integer,	// initial value
	                        firstTime,
	                        modeInf. timeOffset_fractional,
	                        deltaFreqOffset,	// tracking value
	                        sampleclockOffset	// tracking value
	                      );
	            firstTime	= false;
	            frameReady =
	                  my_Equalizer.
	                         equalize (inbank. element ((lc + i) % nrSymbols),
	                                   (symbol_no + i) % nrSymbols,
	                                   &outbank,
	                                   &modeInf. timeOffset_fractional,
	                                   &deltaFreqOffset,
	                                   &sampleclockOffset,
	                                   displayVector);
	         }
	
	         if (!frameReady)	// should not happen???
	            throw (36);
			 
//	OK, let us check the FAC
	         bool success  = my_facProcessor.
	                          processFAC (&outbank, &theState);
	         if (success) {
	            m_form. set_facSyncLabel	(true);
	            missers = 0;
	         }
	         else {
	            m_form. set_facSyncLabel	(false);
	            m_form. set_sdcSyncLabel	(false);
	            superframer		= false;
	            if (missers++ < 2)
	               continue;
	            throw (35);	// ... or give up and start all over
	         }
	      }	// end of main loop
	   } catch (int e) {
	   if (!running. load ())
	      return;
	   }
	}
}

//      just for readability
uint8_t SDRunoPlugin_drm::getSpectrum     (stateDescriptor *f) {
uint8_t val = f -> spectrumBits;
	return val <= 5 ? val : 3;
}
//

void	SDRunoPlugin_drm::getMode (Reader *my_Reader, smodeInfo *m) {
timeSyncer  my_Syncer (my_Reader, WORKING_RATE,  nSymbols);
	my_Syncer. getMode (m);
}

void    SDRunoPlugin_drm::frequencySync (Reader *my_Reader, smodeInfo *m) {
freqSyncer my_Syncer (my_Reader, m, WORKING_RATE, &m_form);
	my_Syncer. frequencySync (m);
}

int16_t	SDRunoPlugin_drm::sdcCells (smodeInfo *m) {
static
int m1_table []	= {167, 190, 359, 405, 754, 846};
static
int m2_table [] = {130, 150, 282, 322, 588, 1500};

	switch (m -> Mode) {
	   case Mode_A:
	      return m1_table [m -> Spectrum];

	   default:
	   case Mode_B:
	      return m2_table [m -> Spectrum];

	   case Mode_C:
	      return 288;

	   case Mode_D:
	      return 152;
	}
	return 288;
}

void	SDRunoPlugin_drm::set_sdcCells (smodeInfo *modeInf) {
uint8_t	Mode	= modeInf -> Mode;
uint8_t	Spectrum = modeInf -> Spectrum;
int	carrier;
int	cnt	= 0;
	for (carrier = Kmin(Mode, Spectrum);
	     carrier <= Kmax(Mode, Spectrum); carrier++) {
	   if (isSDCcell (modeInf, 0, carrier)) {
		  sdcTable[cnt].symbol = 0;
		  sdcTable[cnt].carrier = carrier;
		  cnt++;
	   }
	}
	
	for (carrier = Kmin (Mode, Spectrum);
	     carrier <= Kmax (Mode, Spectrum); carrier ++) {
	   if (isSDCcell (modeInf, 1, carrier)) {
	      sdcTable [cnt]. symbol = 1;
	      sdcTable [cnt]. carrier = carrier;
	      cnt ++;
	   }
	}
	   
	if ((Mode == Mode_C) || (Mode == Mode_D)) {
	   for (carrier = Kmin (Mode, Spectrum);
	        carrier <= Kmax (Mode, Spectrum); carrier ++) 
	      if (isSDCcell (modeInf, 2, carrier)) {
	         sdcTable [cnt]. symbol = 2;
	         sdcTable [cnt]. carrier = carrier;
	         cnt ++;
	      }
	}

//	fprintf (stderr, "for Mode %d, spectrum %d, we have %d sdc cells\n",
//	                       Mode, Spectrum, cnt);
}

bool	SDRunoPlugin_drm::isFACcell (smodeInfo *m,
	                             int16_t symbol, int16_t carrier) {
int16_t	i;
struct facElement *facTable     = getFacTableforMode (m -> Mode);

//	we know that FAC cells are always in positive carriers
	if (carrier < 0)
	   return false;
	for (i = 0; facTable [i]. symbol != -1; i ++) {
	   if (facTable [i]. symbol > symbol)
	      return false;
	   if ((facTable [i]. symbol == symbol) &&
	       (facTable [i]. carrier == carrier))
	      return true;
	}
	return false;
}

bool	SDRunoPlugin_drm::isSDCcell (smodeInfo *m,
	                             int16_t symbol, int16_t carrier) {
	if (carrier == 0)
	   return false;
	if ((m -> Mode == 1) && ((carrier == -1) || (carrier == 1)))
	   return false;

	if (symbol > 2)
	   return false;
	if (isTimeCell (m -> Mode, symbol, carrier))
	   return false;
	if (isFreqCell (m -> Mode, symbol, carrier))
	   return false;
	if (isPilotCell (m -> Mode, symbol, carrier))
	   return false;
	if (isFACcell (m, symbol, carrier))
	   return false;
	return true;
}

bool	SDRunoPlugin_drm::isDatacell (smodeInfo *m,
	                              int16_t symbol,
	                              int16_t carrier, int16_t blockno) {
	if (carrier == 0)
	   return false;
	if (m -> Mode == 1 && (carrier == -1 || carrier == 1))
	   return false;
//
//	these are definitely SDC cells
	if ((blockno == 0) && ((symbol == 0) || (symbol == 1)))
	   return false;
//
	if ((blockno == 0) && ((m -> Mode == 3 ) || (m -> Mode == 4)))
	   if (symbol == 2)
	      return false;
	if (isFreqCell (m -> Mode, symbol, carrier))
	   return false;
	if (isPilotCell (m -> Mode, symbol, carrier))
	   return false;
	if (isTimeCell (m -> Mode, symbol, carrier))
	   return false;
	if (isFACcell (m, symbol, carrier))
	   return false;

	return true;
}

bool	SDRunoPlugin_drm::isFirstFrame (stateDescriptor *f) {
uint8_t val     = f -> frameIdentity;
	return ((val & 03) == 0) || ((val & 03) == 03);
}

bool	SDRunoPlugin_drm::isLastFrame (stateDescriptor *f) {
uint8_t val     = f -> frameIdentity;
	return ((val & 03) == 02);
}

//
//	adding the contents of a frame to a superframe
void	SDRunoPlugin_drm::addtoSuperFrame (smodeInfo *m,
	                                 int16_t blockno, 
	                                 myArray<theSignal> *outbank) {
static	int	teller	= 0;
int16_t	symbol, carrier;
int16_t	   K_min		= Kmin (m -> Mode, m -> Spectrum);
int16_t	   K_max		= Kmax (m -> Mode, m -> Spectrum);

	if (isFirstFrame (&theState))
	   my_backendController. newFrame (&theState);

	for (symbol = 0; symbol < symbolsperFrame (m -> Mode); symbol ++) {
	   for (carrier = K_min; carrier <= K_max; carrier ++)
	      if (isDatacell (&modeInf, symbol, carrier, blockno)) {
	         my_backendController. addtoMux (blockno, teller ++,
	                                     outbank -> element (symbol)[carrier - K_min]);
	      }
	}

	if (isLastFrame (&theState)) {
	   my_backendController. endofFrame ();
	   teller = 0;
	}
}

void	SDRunoPlugin_drm::activate_channel_1	() {
	theState. activate_channel_1 ();
	m_form. showService (theState. streams [0]. serviceName);
}

void	SDRunoPlugin_drm::activate_channel_2	() {
	if ((theState. numofStreams > 1) &&
	    (theState. streams [1]. isAudio))
	theState. activate_channel_2 ();
	m_form. showService (theState. streams [1]. serviceName);
}


//

