#
/*
 *    Copyright (C) 2020
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of the SDRuno drm plugin
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
#include	"up-filter.h"
#define  _USE_MATH_DEFINES
#include <math.h>

static inline
std::complex<float> cmul (std::complex<float> in, float y) {
	return std::complex<float> (real (in) * y,
	                            imag (in) * y);
}

static inline 
std::complex<float> cdiv (std::complex<float> in, float y) {
	return std::complex<float> (real (in) / y,
	                            imag (in) / y);
}

	upFilter::upFilter (int bufferSize, int  inRate, int outRate):
	                                    kernel (bufferSize * (outRate / inRate)),
	                                    buffer (bufferSize) {
float	* tmp = (float *)_malloca (bufferSize * (outRate / inRate) * sizeof (float));
float	f	= (float)(inRate / 2) / outRate;
float	sum	= 0;

	this	-> multiplier	= outRate / inRate;
	this	-> order	= bufferSize * multiplier;
	this	-> ip		= 0;
	this	-> bufferSize	= bufferSize;

	for (int i = 0; i < bufferSize; i ++)
	   buffer [i] = std::complex<float> (0, 0);

	for (int i = 0; i < order; i ++) {
	   if (i == order / 2)
	      tmp [i] = 2 * M_PI * f;
	   else 
	      tmp [i] = sin (2 * M_PI * f * (i - order / 2))/ (i - order /2);
//
//	Blackman window
	   tmp [i]  *= (0.42 -
		    0.5 * cos (2 * M_PI * (float)i / (float) order) +
		    0.08 * cos (4 * M_PI * (float)i / (float) order));
	   sum += tmp [i];
	}

	for (int i = 0; i < order; i ++)
	   kernel [i] = std::complex<float> (tmp [i] / sum, 0);
}

	upFilter::~upFilter	() {
}
//
static bool first_time = false;
void	upFilter::Filter (std::complex<float> in, std::complex<float> *res) {

	buffer [ip] = in;
	for (int i = 0; i < multiplier; i ++) {
	   res [i] = std::complex<float> (0, 0);
	   for (int j = 0; j < bufferSize; j ++) {
	      int index = ip - j;
	      if (index < 0)
	         index += bufferSize;
	      res [i] += buffer [index] *
	                   kernel [i + j * multiplier];
	   }
	}
	ip = (ip + 1) % bufferSize;
}

