#
/*
 *    Copyright (C) 2020
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of the SDRunoPlugin_drm
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

#ifndef	__WORD_COLLECTOR__
#define	__WORD_COLLECTOR__

#include	"..\basics.h"
#include	<stdio.h>
#include	<stdlib.h>
#include	<cstring>
#include	<math.h>
#include	"..\fftw3.h"
#include	"shifter.h"

class	Reader;
//class	simpleBuf;
class	SDRunoPlugin_drmUi;

class	wordCollector {
public:
			wordCollector 		(SDRunoPlugin_drmUi *,
	                                         Reader *,
	                                         smodeInfo *,
	                                         int32_t);
			~wordCollector 		();
	void		getWord			(std::complex<float> *,
	                                         int32_t,
	                                         float);
	void		getWord			(std::complex<float> *,
	                                         int32_t,
	                                         bool,
	                                         float,
	                                         float,
	                                         float);
private:
	shifter		theShifter;

	smodeInfo	*modeInf;
	uint32_t	bufMask;
	void		fft_and_extract		(std::complex<float> *,
	                                         std::complex<float> *);
	Reader		* buffer;
	int32_t		sampleRate;
	uint8_t		Mode;
	uint8_t		Spectrum;
	SDRunoPlugin_drmUi *m_form;
	float		theAngle;
	float		sampleclockOffset;
	int16_t		Tu;
	int16_t		Ts;
	int16_t		Tg;
	int16_t		K_min;
	int16_t		K_max;
	int16_t		displayCount;
	std::complex<float>	*fft_vector;
	fftwf_plan	hetPlan;
	float		get_timeOffset		(int, int);
	int		get_intOffset		(int base,
                                                 int nrSymbols, int range);
	double		compute_mmse		(int, int);
};

#endif

