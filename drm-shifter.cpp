#
/*
 *    Copyright (C) 2020
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of the SDRuno drm decoder
 *
 *    drm decoder is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 *
 *    drm decoder is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with drm decoder; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include	"drm-shifter.h"
#define _USE_MATH_DEFINES
#include	<math.h>

	drmShifter::drmShifter	(int32_t size) {
int i;
	this	->tableSize	= size;
	phase			= 0;
	table			= new std::complex<float> [size];
	for (i = 0; i < size; i ++)
	   table [i] = std::complex<float>
	                       ((float)cos ((float) i / (float)size * 2 * M_PI),
	                        (float)sin ((float) i / (float)size * 2 * M_PI));
}

	drmShifter::~drmShifter	(void) {
	delete[] table;
}

void	drmShifter::do_shift	(std::complex<float>	*buffer,
	                         int32_t	bufLen,
	                         int32_t	freq) {
	for (int i = 0; i < bufLen; i ++) {
	   buffer [i] *= table [phase];
	   phase	-= freq;
	   if (phase < 0)
	      phase += tableSize;
	   if (phase >= tableSize)
	      phase -= tableSize;
	}
}

std::complex<float>
	drmShifter::do_shift	(std::complex<float> z, int32_t freq) {
std::complex<float> res	= z * table [phase];
	phase	-= freq;
        if (phase < 0)
           phase += tableSize;
        if (phase >= tableSize)
           phase -= tableSize;
	return res;
}

