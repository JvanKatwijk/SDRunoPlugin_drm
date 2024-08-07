#pragma once

#include	<nana/gui.hpp>
#include	<nana/gui/widgets/button.hpp>
#include	<nana/gui/widgets/listbox.hpp>
#include	<nana/gui/widgets/slider.hpp>
#include	<nana/gui/widgets/label.hpp>
#include	<nana/gui/widgets/combox.hpp>
#include	<nana/gui/widgets/spinbox.hpp>
#include	<nana/gui/timer.hpp>
#include	<nana/gui/widgets/picture.hpp>
#include	<nana/gui/filebox.hpp>
#include	<nana/gui/dragger.hpp>
#include	<nana/gui.hpp>
#include	<iostream>
#include	<iomanip>
#include	<sstream>

#include	<iunoplugincontroller.h>
#include	<complex>

using namespace nana;

// Shouldn't need to change these
#define topBarHeight (27)
#define bottomBarHeight (8)
#define sideBorderWidth (8)

// TODO: Change these numbers to the height and width of your form
#define formWidth (710)
#define formHeight (300)
#define	nrColumns (formWidth - 60)
#define	nrRows (200)

class SDRunoPlugin_drmUi;

class SDRunoPlugin_drmForm : public nana::form {
public:

	SDRunoPlugin_drmForm (SDRunoPlugin_drmUi& parent,
	                      IUnoPluginController& controller);		
	~SDRunoPlugin_drmForm();
	void	Run	();

	void	initDisplay	();
	void	addPixel	(std::complex<float> pix);
	void	drawConstellation ();
	void	setConstellation	(const std::string &);

	void	set_timeDelayDisplay (float f);
	void	set_intOffsetDisplay (int offset);
	void	set_countryLabel (const std::string s);
	void	set_channel_1 (const std::string s);
	void	set_timeSyncLabel (bool b);
	void	set_timeOffsetDisplay (float f);
	void	set_smallOffsetDisplay (float f);
	void	set_timeLabel (const std::string s);
	void	set_channel_2 (const std::string s);
	void	set_facSyncLabel (bool b);
	void	set_clockOffsetDisplay (float f);
	void	set_angleDisplay (float f);
	void	set_programTypeLabel (const std::string s);
	void	set_sdcSyncLabel (bool b);
	void	set_aacDataLabel (const std::string s);
	void	set_channel_4 (const std::string s);
	void	set_faadSyncLabel (bool b);
	void	set_spectrumIndicator (int s);
	void	set_modeIndicator (int m);
	void	set_datacoding (const std::string s);
	void	set_audioModeLabel (const std::string s);
	void	set_messageLabel (const std::string s);
	void	showLines		(std::vector<std::complex<float>> &);
	void	clearScreen		();

	void	show_fac_mer		(float);
	void	show_sdc_mer		(float);
	void	show_msc_mer		(float);

	void	set_Mode		(const std::string &);
	void	show_quality		(float);
	void	hide_channel_1		();
	void	hide_channel_2		();
	void	activate_channel_1	();
	void	activate_channel_2	();
	void	set_audioRate		(int);
	void	showService(const std::string&);
private:

	void Setup();

	// The following is to set up the panel graphic to look like a standard SDRuno panel
	nana::picture bg_border{ *this, nana::rectangle(0, 0, formWidth, formHeight) };
	nana::picture bg_inner{ bg_border, nana::rectangle(sideBorderWidth, topBarHeight, formWidth - (2 * sideBorderWidth), formHeight - topBarHeight - bottomBarHeight) };
	nana::picture header_bar{ *this, true };
	nana::label title_bar_label{ *this, true };
	nana::dragger form_dragger;
	nana::label form_drag_label{ *this, nana::rectangle(0, 0, formWidth, formHeight) };
	nana::paint::image img_min_normal;
	nana::paint::image img_min_down;
	nana::paint::image img_close_normal;
	nana::paint::image img_close_down;
	nana::paint::image img_header;
	nana::picture close_button{ *this, nana::rectangle(0, 0, 20, 15) };
	nana::picture min_button{ *this, nana::rectangle(0, 0, 20, 15) };

	// Uncomment the following 4 lines if you want a SETT button and panel
	nana::paint::image img_sett_normal;
	nana::paint::image img_sett_down;
	nana::picture sett_button{ *this, nana::rectangle(0, 0, 40, 15) };
	void SettingsButton_Click();

	// TODO: Now add your UI controls here
//
//	row 1
	nana::label	intOffsetDisplay 
	                          {*this, nana::rectangle (30, 30, 60, 20)};
	nana::label	smallOffsetDisplay
	                          {*this, nana::rectangle (100, 30, 60, 20)};
	nana::label	timeLabel
	                          {*this, nana::rectangle (170, 30, 60, 20)};
	nana::label	copyRightLabel
	                          {*this, nana::rectangle (250, 30, 40, 20)};
	nana::label	label_1
	                          {*this, nana::rectangle (350, 30, 60, 20)};
	nana::label	timeSyncLabel
	                          {*this, nana::rectangle (420, 30, 20, 20)};



	nana::label	label_2
	                          {*this, nana::rectangle (350, 60, 60, 20)};
	nana::label	facSyncLabel
	                          {*this, nana::rectangle (420, 60, 20, 20)};
//
	nana::label	label_3
	                          {*this, nana::rectangle (350, 90, 60, 20)};
	nana::label	sdcSyncLabel
	                          {*this, nana::rectangle (420, 90, 20, 20)};
//
	
	nana::label	label_4
	                          {*this, nana::rectangle (350, 120, 60, 20)};
	nana::label	faadSyncLabel
	                          {*this, nana::rectangle (420, 120, 20, 20)};


	nana::label	channel_1
	                  {*this, nana::rectangle (30, 60, 300, 30)};
	nana::label	channel_2
	                  {*this, nana::rectangle (30, 100, 300, 30), true };
	nana::label selectedService
		              { *this, nana::rectangle(30, 140, 300, 40), true };

//
//	row 5
	nana::label	spectrumIndicator
	                          {*this, nana::rectangle (30, 170, 40, 20)};
	nana::label	modeIndicator
	                          {*this, nana::rectangle (80, 170, 40, 20)};
	nana::label	datacoding
	                          {*this, nana::rectangle (130, 170, 40, 20)};
	nana::label	audioModeLabel
	                          {*this, nana::rectangle (180, 170, 60, 20)};
	nana::label	aacDataLabel
	                          {*this, nana::rectangle (250, 170, 50, 20) };
	nana::label	qualityIndicator
	                          {*this, nana::rectangle (310, 170, 60, 20) };
	nana::label	audioRateLabel
	                          {*this, nana::rectangle (380, 170, 50, 20) };

// row 6
	nana::label	messageLabel
	                          {*this, nana::rectangle (30, 200, 400, 20)};
	nana::combox	constellationSelector 
	                          {*this, nana::rectangle (30, 240, 80, 20)};
//
//

	nana::label	displayLabel
	                          {*this, nana::rectangle (445, 30, 256, 256)};
	drawing		*theDisplay;
//
	SDRunoPlugin_drmUi & m_parent;
	IUnoPluginController & m_controller;
};
