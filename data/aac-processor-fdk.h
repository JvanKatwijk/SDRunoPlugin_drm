#
/*
 *    Copyright (C) 2020
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of the DRM plugin
 *
 *    DRM plugin is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 *
 *    DRM plugin is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with DRM plugin; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
#
#ifndef	__AAC_PROCESSOR_FDK__
#define	__AAC_PROCESSOR_FDK__

#include	<stdio.h>
#include	<stdint.h>
#include	<cstring>
#include	"up-filter.h"
#include	"aacdecoder_lib.h"
#include        "..\ringbuffer.h"
#include	"..\aac-handler.h"

class   stateDescriptor;
class   SDRunoPlugin_drmUi;


typedef	struct frame {
	int16_t length, startPos;
	uint8_t	aac_crc;
	uint8_t audio [512];
} audioFrame; 

class	aacProcessor_fdk {
public:
		aacProcessor_fdk   (stateDescriptor *,
	                            SDRunoPlugin_drmUi *,
	                            aacHandler	*,
	                            RingBuffer<std::complex<float>> *);
	        ~aacProcessor_fdk  ();

	void	process_aac	(uint8_t *, int16_t, int16_t,
	                                 int16_t, int16_t, int16_t);
private:
	stateDescriptor *theState;
	SDRunoPlugin_drmUi  *m_form;
	RingBuffer<std::complex<float>> *audioOut;
	upFilter	upFilter_24000;
	upFilter	upFilter_12000;
	int16_t         numFrames;
	int16_t         selectedAudioService;
	aacHandler	*aacFunctions;

	void		handle_uep_audio        (uint8_t *, int16_t,
	                                 int16_t, int16_t, int16_t, int16_t);
	void		handle_eep_audio (uint8_t *, int16_t, int16_t, int16_t);
	void		writeOut        (int16_t *, int16_t, int32_t);
	void		toOutput        (std::complex<float> *, int16_t);
	void		playOut         (int16_t);
//
//	added to support inclusion of the last phase
	void    reinit          (std::vector<uint8_t>, int);
	void	init		();
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
	std::vector<uint8_t>
	                getAudioInformation (stateDescriptor *, int);
};
#endif

