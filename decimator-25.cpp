#
/*
 *    Copyright (C) 2020
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of the SDRuno plugin for drm
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

#include	"decimator-25.h"
//
//	decimating 62500 N times results in a rate on which
//	we do the decoding
	decimator_25::decimator_25 (int amount) {
	this -> amount	= amount;
	counter	= amount;
	iValue = std::complex<DRM_FLOAT> (0, 0);
}

	decimator_25::~decimator_25 () {}

bool	decimator_25::Pass (std::complex<DRM_FLOAT> in, std::complex<DRM_FLOAT> *out) {
	counter --;
	iValue += in;
	if (counter == 0) {
	   *out = std::complex<float> (real (iValue) / amount,
	                               imag (iValue) / amount);
	   iValue = std::complex<DRM_FLOAT> (0, 0);
	   counter = amount;
	   return true;
	}
	return false;
}

