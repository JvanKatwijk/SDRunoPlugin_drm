#
/*
 *    Copyright (C) 2014
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of the SDRuno rtty decoder
 *
 *    rtty decoder is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 *
 *    rtty decoder is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with rtty decoder; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include	"utilities.h"
#include	<cstring>

float	decayingAverage (float old, float input, float weight) {
	if (weight <= 1)
	   return input;
	return (float)(input * (1.0 / weight) + old * (1.0 - (1.0 / weight)));
}

float	clamp (float X, float Min, float Max) {
	if (X > Max)
	   return Max;
	if (X < Min)
	   return Min;
	return X;
}

	rttyAverage::rttyAverage (int16_t s) {
	size	= s;
	vec	= new float [s];
	filp	= 0;

	memset (vec, 0, s * sizeof (float));
}

	rttyAverage::~rttyAverage () {
	delete[]	vec;
}

float	rttyAverage::filter (float e) {
float	out	= 0.0;

	vec [filp] = e;
	filp = (filp + 1) % size;

	for (int i = 0; i < size; i ++)
	   out += vec [i];

	return out / size;
}

void	rttyAverage::clear (float c ) {
int16_t	i;

	for (i = 0; i < size; i ++)
	   vec [i] = c;
	filp = 0;
}

