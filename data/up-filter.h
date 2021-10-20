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
#ifndef	__UP_FILTER_H
#define	__UP_FILTER_H

#include	<complex>
#include	<vector>
#include	<math.h>


class	upFilter {
	std::vector<std::complex<float>> kernel;
	std::vector<std::complex<float>> buffer;
	int		ip;
	int		order;
	int		bufferSize;
	int		multiplier;
public:
	upFilter	(int, int, int);
	~upFilter	();
void	Filter	(std::complex<float>, std::complex<float> *);
};
#endif

