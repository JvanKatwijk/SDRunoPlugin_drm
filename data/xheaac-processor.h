#
/*
 *    Copyright (C) 2020
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of the DRM plugin
 *
 *    drm plugin is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 *
 *    drm plugin is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with drm plugin if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef	__XHEAAC_PROCESSOR__
#define	__XHEAAC_PROCESSOR__

#include	<stdint.h>
#include        <cstring>
#include	<vector>
#include	<complex>
#include	"aacdecoder_lib.h"
#include	"up-filter.h"
#include	"..\support\checkcrc.h"
#include	"..\basics.h"
#include	"..\ringbuffer.h"
#include	"..\aac-handler.h"

class	SDRunoPlugin_drmUi;
class	stateDescriptor;

class	xheaacProcessor {
public:
			xheaacProcessor	(stateDescriptor *,
	                                 SDRunoPlugin_drmUi *,
	                                 aacHandler	*,
	                                 RingBuffer<std::complex<float>> *);
			~xheaacProcessor	();
	void		process_usac	(uint8_t *v, int16_t mscIndex,
                                         int16_t startHigh, int16_t lengthHigh,
                                         int16_t startLow, int16_t lengthLow);
private:
	stateDescriptor	*theState;
	SDRunoPlugin_drmUi	*m_form;
	RingBuffer<std::complex<float>> *audioOut;
	checkCRC	theCRC;
	upFilter	upFilter_24000;
        upFilter	upFilter_12000;
	void		resetBuffers	();
	void		processFrame	(int);
	int		currentRate;
	std::vector<uint8_t>
        		getAudioInformation (stateDescriptor *drm,
                                                        int streamId);
	int		numFrames;
	void		writeOut	(int16_t *, int16_t, int32_t);
	void		toOutput	(std::complex<float> *, int16_t);
	void		playOut		(std::vector<uint8_t>);
	aacHandler	*aacFunctions;

//	added to support inclusion of the last phase
	void		reinit		(std::vector<uint8_t>, int);
	void		init		();
        void    decodeFrame     (uint8_t        *audioFrame,
                                 uint32_t       frameSize,
                                 bool           *conversionOK,
                                 int16_t        *buffer,
                                 int16_t        *samples,
                                 int32_t        *pcmRate);

	uint8_t         aac_isInit;
        uint8_t         prev_audioSamplingRate;
        uint8_t         prevSBR_flag;
        uint8_t         prev_audioMode;
        int16_t         audioChannel;
//
	HANDLE_AACDECODER       handle;
        UCHAR           *theBuffer;
        uint32_t        bufferP;
        std::vector<uint8_t>    currentConfig;
};

#endif

