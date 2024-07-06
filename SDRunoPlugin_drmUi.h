#pragma once

#include <nana/gui.hpp>
#include <nana/gui/widgets/button.hpp>
#include <nana/gui/widgets/label.hpp>
#include <nana/gui/timer.hpp>
#include <iunoplugin.h>
#include <iostream>
#include <iomanip>
#include <sstream>

#include <iunoplugincontroller.h>
#include "SDRunoPlugin_drmForm.h"

// Forward reference
class SDRunoPlugin_drm;

class SDRunoPlugin_drmUi {
public:

	SDRunoPlugin_drmUi (SDRunoPlugin_drm & parent,
	IUnoPluginController& controller);
	~SDRunoPlugin_drmUi	();

	void	HandleEvent	(const UnoEvent& evt);
	void	FormClosed	();
	void	ShowUi	        ();
	int	LoadX           ();
	int	LoadY		();
	void	setConstellation	(int);
	void	set_timeDelayDisplay	(float f);
	void	set_intOffsetDisplay	(int offset);
	void	set_countryLabel (const std::string s);
	void	set_channel_1 (const std::string s);
	void	set_channel_2 (const std::string s);
	void	set_timeSyncLabel (bool b);
	void	set_timeOffsetDisplay (float f);
	void	set_smallOffsetDisplay (float f);
	void	set_timeLabel (const std::string s);
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
	void	showService	(const std::string &);

	void	show_fac_mer		(float);
	void	show_sdc_mer		(float);
	void	show_msc_mer		(float);

	void	show_quality		(float);
	void	hide_channel_1		();
	void	hide_channel_2		();
	void	activate_channel_1	();
	void	activate_channel_2	();
	void	set_audioRate		(int);
	void	addPixel		(std::complex<float>);
	int		constellationMode;
private:
	SDRunoPlugin_drm & m_parent;
	std::thread m_thread;
	std::shared_ptr<SDRunoPlugin_drmForm> m_form;

	bool m_started;

	std::mutex m_lock;

	IUnoPluginController & m_controller;
};
