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
/*
 *	shield for mscProcessor 
 */
#include	"msc-processor.h"
#include	"..\SDRunoPlugin_drmUi.h"
#include	"..\parameters\state-descriptor.h"
#include	"backend-controller.h"

	backendController::
	           backendController	(SDRunoPlugin_drmUi *m_form,
	                                 int8_t		qam64Roulette,
	                                 RingBuffer<std::complex<float>> *b) {
	this	-> m_form		= m_form;
	this	-> qam64Roulette	= qam64Roulette;
	this	-> audioBuffer		= b;

	theWorker	= nullptr;
	mscMode		= 0;
	protLevelA	= 0;
	protLevelB	= 0;
	numofStreams	= 0;
	QAMMode		= 0;
}

	backendController::~backendController	() {
	if (theWorker != nullptr)
	   delete theWorker;
}
//
//	Reset is called whenever we have the start of a new stream of
//	superframe data
void	backendController::reset	(stateDescriptor *theState) {
	if (theWorker != nullptr)
	   delete theWorker;
	theWorker	= new mscProcessor (theState, m_form, 4, audioBuffer);
}

void	backendController::newFrame	(stateDescriptor *theState) {
	if (theWorker == nullptr) 
	   theWorker = new mscProcessor (theState, m_form, 6, audioBuffer);
	theWorker	-> newFrame (theState);
}

void	backendController::addtoMux	(int16_t blockno,
	                                 int cnt, theSignal v) {
	if (theWorker == nullptr)	// should not/cannot happen
	   return;
	theWorker	-> addtoMux (blockno, cnt, v);
}

void	backendController::endofFrame		() {
	if (theWorker == nullptr)	// shoulod not/cannot happen
	   return;
	theWorker	-> endofFrame ();
}

