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
#include	<Windows.h>
#include	<math.h>
#include	"..\SDRunoPlugin_drmUi.h"
#include	"..\basics.h"
#include	"word-collector.h"
#include	"reader.h"

//	The wordCollector will handle segments of a given size,
//	do all kinds of frequency correction (timecorrection
//	was done in the syncer) and map them onto ofdm words.
//	
//	The caller just calls upon "getWord" to get a new ofdm word

static inline 
std::complex<float> cmul (std::complex<float> x, float y) {
	return std::complex<float> (real (x) * y, imag (x) * y);
}

//	The frequency shifter is in steps of 0.01 Hz
	wordCollector::wordCollector (SDRunoPlugin_drmUi *m_form,
	                              Reader	*b,
	                              smodeInfo	*modeInf,
	                              int sampleRate):
	                                 theShifter (100 * sampleRate) {
	this	-> m_form	= m_form;
	this	-> buffer	= b;
	this	-> bufMask	= buffer -> bufSize - 1;
	this	-> sampleRate	= sampleRate;
	this	-> modeInf	= modeInf;
	this	-> Mode		= modeInf -> Mode;
	this	-> Spectrum	= modeInf -> Spectrum;
	this	-> theAngle	= 0;
	this	-> Tu		= Tu_of (Mode);
	this	-> Ts		= Ts_of (Mode);
	this	-> Tg		= Tg_of (Mode);
	this	-> K_min	= Kmin	(Mode, Spectrum);
	this	-> K_max	= Kmax	(Mode, Spectrum);
	this	-> displayCount	= 0;
	fft_vector		= (std::complex<float> *)
	                               fftwf_malloc (Tu *
	                                            sizeof (fftwf_complex));
	hetPlan			= fftwf_plan_dft_1d (Tu,
	                    reinterpret_cast <fftwf_complex *>(fft_vector),
	                    reinterpret_cast <fftwf_complex *>(fft_vector),
	                    FFTW_FORWARD, FFTW_ESTIMATE);
}

		wordCollector::~wordCollector (void) {
	fftwf_free (fft_vector);
	fftwf_destroy_plan (hetPlan);
}

//	when starting up, we "borrow" the precomputed frequency offset
//	and start building up the spectrumbuffer.
//	
void	wordCollector::getWord (std::complex<float>	*out,
	                        int32_t		initialFreq,
	                        float		offsetFractional) {
std::complex<float> *temp  =
	  (std::complex<float> *)_malloca (Ts * sizeof (std::complex<float>));
std::complex<float>	angle	= std::complex<float> (0, 0);
float		offset	= 0;
float	timeOffsetFractional;

	buffer		-> waitfor (Ts + Ts / 2);
	float timedelay	= get_timeOffset (16, 6);
//	float timedelay	= offsetFractional;
	int d		= floor (timedelay + 0.5);
	timeOffsetFractional	= timedelay - d;

	int f = (int)(floor (buffer -> currentIndex + d)) & bufMask;
//	correction of the time offset by interpolation
	for (int i = 0; i < Ts; i ++) {
	   std::complex<float> one = buffer -> data [(f + i) & bufMask];
	   std::complex<float> two = buffer -> data [(f + i + 1) & bufMask];
	   temp [i] = cmul (one, 1 - timeOffsetFractional) +
	                            cmul (two, timeOffsetFractional);
	   temp [i] = cmul (one, 1 - offsetFractional) +
	                            cmul (two, offsetFractional);
	}

//	And we shift the bufferpointer here
	buffer -> currentIndex = (f + Ts) & bufMask;

//	Now: determine the fine grain offset.
	for (int i = 0; i < Tg; i ++)
	   angle += conj (temp [Tu + i]) * temp [i];
//	simple averaging
	theAngle	= 0.9 * theAngle + 0.1 * arg (angle);
//
//	offset  (and shift) in Hz / 100
	offset		= theAngle / (2 * M_PI) * 100 * sampleRate / Tu;
	if (offset != -offset)	// precaution to handle undefines
	   theShifter. do_shift (temp, Ts,
	                            100 * modeInf -> freqOffset_integer - offset);

	if (++displayCount > 20) {
	   displayCount = 0;
	   m_form -> set_intOffsetDisplay	(initialFreq);
	   m_form -> set_smallOffsetDisplay	(- offset / 100);
	   m_form -> set_angleDisplay		(arg (angle));
	   m_form -> set_timeDelayDisplay	(offsetFractional);
	}

	fft_and_extract (&temp [Tg], out);
}
//
//	The getWord as below is used in the main loop, to obtain
//	a next ofdm word
void	wordCollector::getWord (std::complex<float>	*out,
	                        int32_t		initialFreq,
	                        bool		firstTime,
	                        float		offsetFractional,
	                        float		angle,
	                        float		clockOffset) {
std::complex<float>* temp =
	(std::complex<float> *)_malloca  (Ts * sizeof (std::complex<float>));

	buffer		-> waitfor (Ts + Ts / 2);

	float tt		= get_timeOffset (24, 8);
	int timeOffsetInteger	= floor (tt + 0.5);
	offsetFractional	= tt - timeOffsetInteger;
	int f	= (int)(floor (buffer -> currentIndex + 
	                              timeOffsetInteger)) & bufMask;
//	just linear interpolation
	for (int i = 0; i < Ts; i ++) {
	   std::complex<float> one = buffer -> data [(f + i) & bufMask];
	   std::complex<float> two = buffer -> data [(f + i + 1) & bufMask];
	   temp [i] = cmul (one, 1 - offsetFractional) +
	              cmul (two, offsetFractional);
	}

//	And we adjust the bufferpointer here
	buffer -> currentIndex = (f + Ts) & bufMask;

//	correct the phase
	theAngle	= theAngle - 0.1 * angle;
//	offset in 0.01 * Hz
	float offset          = theAngle / (2 * M_PI) * 100 * sampleRate / Tu;
	if (offset != -offset) { // precaution to handle undefines
	   if (offset > 200 * sampleRate / Tu) {
	      modeInf -> freqOffset_integer +=  sampleRate / Tu;
	      offset -= 200 * sampleRate / Tu;
	   }
	   if (offset < -200 * sampleRate / Tu) {
	      modeInf -> freqOffset_integer -= sampleRate / Tu / 2;
	      offset += 200 * sampleRate / Tu;
	   }

	   theShifter. do_shift (temp, Ts,
	                        100 * modeInf -> freqOffset_integer - offset);
	}

	if (++displayCount > 20) {
	   displayCount = 0;
	   m_form -> set_intOffsetDisplay	(initialFreq);
	   m_form -> set_smallOffsetDisplay	(- offset / 100);
	   m_form -> set_angleDisplay		(angle);
	   m_form -> set_timeOffsetDisplay	(offsetFractional);
	   m_form -> set_timeDelayDisplay		(timeOffsetInteger);
	   m_form -> set_clockOffsetDisplay	(Ts * clockOffset);
	}

	fft_and_extract (&temp [Tg], out);
}

void	wordCollector::fft_and_extract (std::complex<float> *in,
	                                std::complex<float> *out) {
//	and extract the Tu set of samples for fft processsing
	memcpy (fft_vector, in, Tu * sizeof (std::complex<float>));

	fftwf_execute (hetPlan);
//	extract the "useful" data
	if (K_min < 0) {
	   memcpy (out,
	           &fft_vector [Tu + K_min],
	           - K_min * sizeof (std::complex<float>));
	   memcpy (&out [- K_min],
	           &fft_vector [0], (K_max + 1) * sizeof (std::complex<float>));
	}
	else
	   memcpy (out,
	           &fft_vector [K_min],
	           (K_max - K_min + 1) * sizeof (std::complex<float>));
}

float	wordCollector::get_timeOffset	(int nrSymbols, int range) {
int	*b = (int *)_malloca (nrSymbols * sizeof (int));

	buffer -> waitfor (2 * nrSymbols * Ts + Ts);
	for (int i = 0; i < nrSymbols; i ++)
	   b [i] = get_intOffset (i * Ts, nrSymbols, range);

	float   sumx    = 0.0;
        float   sumy    = 0.0;
        float   sumxx   = 0.0;
        float   sumxy   = 0.0;

        for (int i = 0; i < nrSymbols; i++) {
           sumx += (float) i;
           sumy += (float) b [i];
           sumxx += (float) i * (float) i;
           sumxy += (float) i * (float) b [i];
        }

        float boffs;
        boffs = (float) ((sumy * sumxx - sumx * sumxy) /
                         ((nrSymbols - 1) * sumxx - sumx * sumx));

	return boffs;
}

int	wordCollector::get_intOffset	(int base,
	                                 int nrSymbols, int range) {
int	bestIndex = -1;
double	min_mmse = 10E20;

	for (int i = - range / 2; i < range / 2; i ++) {
	   int index = buffer -> currentIndex + base + i;
	   double mmse = compute_mmse (index, nrSymbols);
	   if (mmse < min_mmse) {
	      min_mmse = mmse;
	      bestIndex = i;
	   }
	}
	
	return bestIndex;
}

double	wordCollector::compute_mmse (int starter, int nrSymbols) {
std::complex<float> gamma = std::complex<float> (0, 0);
double	squares = 0;
int32_t		bufMask	= buffer -> bufSize - 1;

	buffer -> waitfor (nrSymbols * Ts + Ts);
	for (int i = 0; i < nrSymbols; i ++) {
	   int startSample = starter + i * Ts;
	   for (int j = 0; j < Tg; j ++) {
	      std::complex<float> f1 =
	             buffer -> data [(startSample + j) & bufMask];
	      std::complex<float> f2 =
	             buffer -> data [(startSample + Tu + j) & bufMask];
	      gamma	+= f1 * conj (f2);
	      squares	+= real (f1 * conj (f1)) + real (f2 * conj (f2));
	   }
	}
	return abs (squares - 2 * abs (gamma));
}
