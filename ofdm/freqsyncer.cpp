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
#include	<stdio.h>
#include	<stdlib.h>
#define  _USE_MATH_DEFINES
#include	<math.h>
#include	<windows.h>
#include	"freqsyncer.h"
#include	"reader.h"
#include	"../SDRunoPlugin_drmUi.h"

//
struct testCells {
	int	index;
	int	phase;
};

struct testCells testCellsforModeA [] = {
        {18, 205},
        {54, 836},
        {72, 215},
        {-1, -1}
};

struct testCells testCellsforModeB [] = {
        {16, 331},
        {48, 651},
        {64, 555},
        {-1, -1}
};

struct testCells testCellsforModeC [] = {
        {11, 214},
        {33, 392},
        {44, 242},
        {-1, -1}
};

struct testCells testCellsforModeD [] = {
        { 7, 788},
        {21, 1014},
        {28, 332},
        {-1, -1}
};

static inline
std::complex<float> cmul(std::complex<float> x, float y) {
	return std::complex<float>(real(x) * y, imag(x) * y);
}
//	The frequency shifter is in steps of 0.01 Hz
	freqSyncer::freqSyncer (Reader		*b,
	                        smodeInfo	*m,
	                        int32_t		sampleRate,	
	                        SDRunoPlugin_drmUi *m_form):
	                                 theShifter (100 * sampleRate) {
	this	-> buffer	= b;
	this	-> Mode		= m -> Mode;
	this	-> Spectrum	= m -> Spectrum;
	this	-> sampleRate	= sampleRate;
	this	-> m_form	= m_form;
	this	-> theAngle	= 0;
	this	-> Tu		= Tu_of (Mode);
	this	-> Ts		= Ts_of (Mode);
	this	-> Tg		= Tg_of (Mode);
	this	-> nrSymbols	= symbolsperFrame (Mode);
	this	-> displayCount	= 0;

//	for detecting pilots:
	int16_t cnt	= 0;

	struct testCells *base =
	            Mode == Mode_A ? testCellsforModeA :
	            Mode == Mode_B ? testCellsforModeB :
	            Mode == Mode_C ? testCellsforModeC : testCellsforModeD;

	this	-> k_pilot1 = base [0]. index;
	this	-> k_pilot2 = base [1]. index;
	this	-> k_pilot3 = base [2]. index;

	this	-> symbolBuffer	= new std::complex<float> *[nrSymbols];
	for (int i = 0; i < nrSymbols; i ++)
	   symbolBuffer [i] = new std::complex<float> [Tu];
	fft_vector = (std::complex<float> *)
	                               fftwf_malloc (Tu *
	                                            sizeof (fftwf_complex));
	hetPlan			= fftwf_plan_dft_1d (Tu,
	                    reinterpret_cast <fftwf_complex *>(fft_vector),
	                    reinterpret_cast <fftwf_complex *>(fft_vector),
	                    FFTW_FORWARD, FFTW_ESTIMATE);
}

		freqSyncer::~freqSyncer (void) {
	fftwf_free (fft_vector);
	fftwf_destroy_plan (hetPlan);
	for (int i = 0; i < nrSymbols; i ++)
	   delete[]  symbolBuffer [i];
	delete[] symbolBuffer;
}

bool freqSyncer::frequencySync (smodeInfo *m) {
int16_t	i;
int32_t	localIndex	= 0;
float	occupancyIndicator [6];
uint8_t	spectrum;

	buffer	-> waitfor (nrSymbols * Ts + Ts);

//	first, load spectra for the first nrSymbol symbols
//	into the (circular) buffer "symbolBuffer",
//	However: do not move the "currentIndex" in the Reader,
//	i.e. do not consume the words from the input
	for (i = 0; i < nrSymbols; i ++) {
	   getWord (buffer,
	            localIndex,
	            i,
	            m -> timeOffset_fractional);
	   localIndex += Ts;
	}

//	our version of get_zeroBin returns the "bin" number
//	of the bin with the highest energy level.
	int32_t	binNumber = get_zeroBin (0);
	m -> freqOffset_integer	= binNumber * sampleRate / Tu;

//	binNumber = binNumber < 0 ? binNumber + Tu : binNumber;
	for (i = 0; i <= 3; i ++) 
	   occupancyIndicator [i] = get_spectrumOccupancy (i, binNumber);

	float tmp1	= 0.0;
	m	-> Spectrum = 3;
	for (spectrum = 0; spectrum <= 3; spectrum ++) {	
	   if (occupancyIndicator [spectrum] >= tmp1) {
	      tmp1 = occupancyIndicator [spectrum];
	      m -> Spectrum = spectrum;
	   }
	}
	return true;
}

//
//	get_zeroBin cooperates with the local version of getWord.
//	It uses the two dimensional array "symbolBuffer" with
//	in the rows the computed spectra of the last N_symbols ofdm words,
//	starting at start (i.e. circular)

float	square (std::complex<float> v) {
	return real (v * conj (v));
}

int32_t	freqSyncer::get_zeroBin (int16_t start) {
std::complex<float>* correlationSum =
	    (std::complex<float> *)_malloca  (Tu * sizeof(std::complex<float>));
float	*abs_sum = (float *) _malloca (Tu * sizeof (float));
float	*squares = (float *) _malloca (Tu * sizeof (float));
//
	memset (correlationSum, 0, Tu * sizeof (std::complex<float>));
	memset (abs_sum, 0, Tu * sizeof (float));
	memset (squares, 0, Tu * sizeof (float));

//	accumulate phase diffs of all carriers in subsequent symbols
	for (int j = start + 1; j < start + nrSymbols; j++) {
	   int16_t jmin1 	= (j - 1) % nrSymbols;
	   int16_t jj		= j % nrSymbols;
	   for (int i = 0; i < Tu; i++) {
	      std::complex<float> tmp1 = symbolBuffer [jmin1][i] *
	                                        conj (symbolBuffer [jj][i]);
	      correlationSum [i] += tmp1;
	      squares [i] += square (symbolBuffer [jmin1][i]) +
	                                        square (symbolBuffer [jj][i]);
	   }
	}
//
	for (int i = 0; i < Tu; i++) 
	   abs_sum [i] = abs (squares [i] - 2 * abs (correlationSum [i]));

	float	lowest		= 1.0E20;
	int	dcOffset	= 0;
//
//	recall that the pilots are relative to -Tu / 2
	for (int i = - Tu / 10; i < Tu / 10; i ++) {
	   float sum = abs_sum [Tu / 2 + k_pilot1 + i] +
	               abs_sum [Tu / 2 + k_pilot2 + i] +
	               abs_sum [Tu / 2 + k_pilot3 + i];
	   if (sum < lowest) {
	      dcOffset = i;
	      lowest = sum;
	   }
	}

	return dcOffset;
}

float	freqSyncer::get_spectrumOccupancy (uint8_t spectrum,
	                                   int16_t baseBin) {
int16_t	i, j;
int16_t K_min_ = Kmin (Mode, spectrum);
int16_t K_max_ = Kmax (Mode, spectrum);

	if (K_min_ == K_max_) 	// should not happen
	   return 0;

//	now we go

//	The indices of the "lowest" and the "highest" carrier.
	int K_min_indx		= (Tu / 2 + baseBin + K_min_) % (Tu);
	int K_max_indx		= (Tu / 2 + baseBin + K_max_) % (Tu);

	float tmp3	= 0;
	float tmp4	= 0;
	float tmp5	= 0;
	float tmp6	= 0;

	for (i = 0; i < nrSymbols; i ++) {
//	near the carrier with the lowest index
	   for (j = 0; j < 15; j ++) {
	      int ind1 = (K_min_indx - 2 - j) % Tu;
	      int ind2 = (K_min_indx + 2 + j) % Tu;
	      tmp3 += real (symbolBuffer [i][ind1] *
	                                  conj (symbolBuffer [i][ind1]));
	      tmp4 += real (symbolBuffer [i][ind2] *
	                                  conj (symbolBuffer [i][ind2]));
	   }
//	near the carrier with the highest index
	   for (j = 0; j < 25; j ++) {
	      int ind1 = (K_max_indx - 2 - j) % Tu;
	      int ind2 = (K_max_indx + 2 + j) % Tu;
	      tmp5 += real (symbolBuffer [i][ind1] *
	                                     conj (symbolBuffer [i][ind1]));
	      tmp6 += real (symbolBuffer [i][ind2] *
	                                     conj (symbolBuffer [i][ind2]));
	   }
	}

	float energy_ratio_K_min_to_K_min_p4;
	float energy_ratio_K_max_to_K_max_p4;
//
//	some safety measure, we assume that the data that is to
//	supposed to be in the carriers with energy has substantially
//	higher value that the "outside" data
	energy_ratio_K_min_to_K_min_p4 = tmp4 / tmp3;
	energy_ratio_K_max_to_K_max_p4 = tmp5 / tmp6;
	return energy_ratio_K_max_to_K_max_p4 +
	       energy_ratio_K_min_to_K_min_p4;
}
//
//	In this local version of getWord, the input buffer is only looked
//	at, not read!! It is called by the frequency synchronizer
//	No reduction of the output to the Kmin .. Kmax useful
//	carriers is made, but the order of the low-high freqencies
//	is change to reflect the "lower .. higher" frequencies in order.
void	freqSyncer::getWord (Reader	*buffer,
	                     int32_t	localIndex,
	                     int32_t	wordNumber,
	                     float	offsetFractional) {
std::complex<float> *temp  =
	   (std::complex<float> *)_malloca (Ts * sizeof (std::complex<float>));
std::complex<float> angle	= std::complex<float> (0, 0);

//	To take into account the fractional timing difference,
//	we do some interpolation between samples in the time domain
	int f	= (buffer -> currentIndex + localIndex) % buffer -> bufSize;
	if (offsetFractional < 0) {
	   offsetFractional = 1 + offsetFractional;
	   f -= 1;
	}

	for (int i = 0; i < Ts; i ++) {
	   std::complex<float> een = buffer -> data  [(f + i) & 
	                                              buffer -> bufMask];
	   std::complex<float> twee = buffer -> data  [(f + i + 1) &
	                                              buffer -> bufMask];
	   temp [i] = cmul (een, 1 - offsetFractional) +
	              cmul (twee, offsetFractional);
	}

//	Now: estimate the fine grain offset.
	for (int i = 0; i < Tg; i ++)
	   angle += conj (temp [Tu + i]) * temp [i];
//	simple averaging:
	theAngle	= 0.9 * theAngle + 0.1 * arg (angle);

//	offset in Hz / 100
	float offset	= theAngle / (2 * M_PI) * 100 * sampleRate / Tu;
	if (++displayCount >= 10) {
	   displayCount = 0;
	   m_form -> set_smallOffsetDisplay	(- offset / 100.0);
	   m_form -> set_angleDisplay		(arg (angle));
	   m_form -> set_timeDelayDisplay	(offsetFractional);
	}

	if (!isnan<float>(offset)) 	// shouldn't happen
	   theShifter. do_shift (temp, Ts, -offset);
	else
	   theAngle = 0;
//	and extract the Tu set of samples for fft processsing
	memcpy (fft_vector, &temp [Tg], Tu * sizeof (std::complex<float>));
	
	fftwf_execute (hetPlan);
	memcpy (symbolBuffer [wordNumber],
	        &fft_vector [Tu / 2], Tu / 2 * sizeof (std::complex<float>));
	memcpy (&symbolBuffer [wordNumber] [Tu / 2],
	        fft_vector , Tu / 2 * sizeof (std::complex<float>));
}

