#
/*
 *    Copyright (C) 2013 .. 2020
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of the SDunoPlugin_drm
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
 *    along with drm plugin; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
#
#ifndef	__DATA_PROCESSOR__
#define	__DATA_PROCESSOR__

#include	<cstring>
#include	"..\ringbuffer.h"
#include	"..\aac-handler.h"
#include	"aac-processor-fdk.h"
#include	"xheaac-processor.h"
#include	"message-processor.h"

class	SDRunoPlugin_drmUi;
class	stateDescriptor;

class	dataProcessor {
public:
		dataProcessor	(stateDescriptor *,
	                         SDRunoPlugin_drmUi *,
	                         aacHandler	*,
	                         RingBuffer<std::complex<float>> *);
		~dataProcessor	();
	void	process		(uint8_t *, int16_t);
//	void	selectAudioService	(int16_t);
private:
	stateDescriptor	*theState;
	SDRunoPlugin_drmUi	*m_form;
	aacProcessor_fdk	my_aacProcessor;
	xheaacProcessor		my_xheaacProcessor;
	messageProcessor	my_messageProcessor;
	int16_t		numFrames;
	int		audioChannel;
	void	process_audio	(uint8_t *, int16_t,
	                         int16_t, int16_t, int16_t, int16_t);
};
#endif

