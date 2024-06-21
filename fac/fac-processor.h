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
#
#ifndef	__FAC_PROCESSOR__
#define	__FAC_PROCESSOR__

#include	"..\basics.h"
#include	"..\support\mapper.h"
#include	"..\support\qam4-metrics.h"
#include	"..\support\prbs.h"
#include	"..\support\checkcrc.h"
#include	"..\support\viterbi-drm.h"
#include	"..\support\my-array.h"
#include	"..\SDRunoPlugin_drmUi.h"

class	stateDescriptor;

class	facProcessor  {
public:
	                facProcessor	(SDRunoPlugin_drmUi *, smodeInfo *);
			~facProcessor	();
	bool		processFAC	(myArray<theSignal> *outbank,
                                         stateDescriptor *theState);
private:
	SDRunoPlugin_drmUi *m_form;
	smodeInfo	*modeInf;
	uint8_t		Mode;
	uint8_t		Spectrum;
	struct facElement	*facTable;
	Mapper		myMapper;
	prbs		thePRBS;
	checkCRC	theCRC;
	qam4_metrics	myMetrics;
	viterbi_drm	deconvolver;
	void		fromSamplestoBits	(theSignal *, uint8_t *);
	void		interpretFac		(uint8_t *, stateDescriptor *);
	void		set_serviceParameters	(uint8_t *, stateDescriptor *);
	void		set_channelParameters	(uint8_t *, stateDescriptor *);
	void		set_serviceLanguage	(int, uint8_t *, stateDescriptor *);
	void		set_serviceDescriptor	(int, uint8_t *, stateDescriptor *);
	int16_t		mscCells		(uint8_t, uint8_t);
};

#endif

