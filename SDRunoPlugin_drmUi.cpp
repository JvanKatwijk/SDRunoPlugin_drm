#include <sstream>
#include <nana/gui.hpp>
#include <nana/gui/widgets/button.hpp>
#include <nana/gui/widgets/listbox.hpp>
#include <nana/gui/widgets/slider.hpp>
#include <nana/gui/widgets/label.hpp>
#include <nana/gui/timer.hpp>
#include <unoevent.h>

#include "SDRunoPlugin_drm.h"
#include "SDRunoPlugin_drmUi.h"
#include "SDRunoPlugin_drmForm.h"

// Ui constructor - load the Ui control into a thread
	SDRunoPlugin_drmUi::
	              SDRunoPlugin_drmUi(SDRunoPlugin_drm& parent,
	              IUnoPluginController& controller) :
	m_parent(parent),
	m_form(nullptr),
	m_controller(controller) {
	constellationMode	= 2;
	m_thread = std::thread (&SDRunoPlugin_drmUi::ShowUi, this);
}

// Ui destructor (the nana::API::exit_all();) is required if using Nana UI library
	SDRunoPlugin_drmUi::~SDRunoPlugin_drmUi () {	
	nana::API::exit_all();
	m_thread.join();	
}

// Show and execute the form
void	SDRunoPlugin_drmUi::ShowUi() {	
	m_lock.lock();
	m_form = std::make_shared<SDRunoPlugin_drmForm>(*this, m_controller);
	m_lock.unlock();

	m_form->Run();
}

// Load X from the ini file (if exists)
// TODO: Change Template to plugin name
int	SDRunoPlugin_drmUi::LoadX () {
	std::string tmp;
	m_controller.GetConfigurationKey("drm.X", tmp);
	if (tmp.empty()) {
	   return -1;
	}
	return stoi(tmp);
}

// Load Y from the ini file (if exists)
// TODO: Change Template to plugin name
int	SDRunoPlugin_drmUi::LoadY() {
	std::string tmp;
	m_controller.GetConfigurationKey("drm.Y", tmp);
	if (tmp.empty()) {
	   return -1;
	}
	return stoi(tmp);
}

// Handle events from SDRuno
// TODO: code what to do when receiving relevant events
void	SDRunoPlugin_drmUi::HandleEvent (const UnoEvent& ev) {
	switch (ev.GetType()) {
	   case UnoEvent::StreamingStarted:
	      break;

	   case UnoEvent::StreamingStopped:
	      break;

	   case UnoEvent::SavingWorkspace:
	      break;

	   case UnoEvent::ClosingDown:
	      FormClosed();
	      break;

	   default:
	      break;
	}
}

// Required to make sure the plugin is correctly unloaded when closed
void	SDRunoPlugin_drmUi::FormClosed () {
	m_controller.RequestUnload (&m_parent);
}

void	SDRunoPlugin_drmUi::set_timeDelayDisplay	(float f) {
	std::lock_guard<std::mutex> l (m_lock);
	if (m_form != nullptr)
	   m_form -> set_timeDelayDisplay (f);
}

void	SDRunoPlugin_drmUi::set_intOffsetDisplay	(int offset) {
	std::lock_guard<std::mutex> l (m_lock);
	if (m_form != nullptr)
	   m_form -> set_intOffsetDisplay (offset);
}

void	SDRunoPlugin_drmUi::set_countryLabel	(const std::string s) {
	std::lock_guard<std::mutex> l (m_lock);
	if (m_form != nullptr)
	   m_form -> set_countryLabel (s);
}

void	SDRunoPlugin_drmUi::set_channel_1 (const std::string s) {
	std::lock_guard<std::mutex> l (m_lock);
	if (m_form != nullptr)
	   m_form -> set_channel_1 (s);
}

void	SDRunoPlugin_drmUi::set_timeSyncLabel (bool b) {
	std::lock_guard<std::mutex> l (m_lock);
	if (m_form != nullptr)
	   m_form -> set_timeSyncLabel (b);
}

void	SDRunoPlugin_drmUi::set_timeOffsetDisplay (float f) {
	std::lock_guard<std::mutex> l (m_lock);
	if (m_form != nullptr)
	   m_form -> set_timeOffsetDisplay (f);
}

void	SDRunoPlugin_drmUi::set_smallOffsetDisplay (float f) {
	std::lock_guard<std::mutex> l (m_lock);
	if (m_form != nullptr)
	   m_form -> set_smallOffsetDisplay (f);
}

void	SDRunoPlugin_drmUi::set_timeLabel (const std::string s) {
	std::lock_guard<std::mutex> l (m_lock);
	if (m_form != nullptr)
	   m_form -> set_timeLabel (s);
}

void	SDRunoPlugin_drmUi::set_channel_2 (const std::string s) {
	std::lock_guard<std::mutex> l (m_lock);
	if (m_form != nullptr)
	   m_form -> set_channel_2 (s);
}

void	SDRunoPlugin_drmUi::set_facSyncLabel (bool b) {
	std::lock_guard<std::mutex> l (m_lock);
	if (m_form != nullptr)
	   m_form -> set_facSyncLabel (b);
}

void	SDRunoPlugin_drmUi::set_clockOffsetDisplay (float f) {
	std::lock_guard<std::mutex> l (m_lock);
	if (m_form != nullptr)
	   m_form -> set_clockOffsetDisplay (f);
}

void	SDRunoPlugin_drmUi::set_angleDisplay (float f) {
	std::lock_guard<std::mutex> l (m_lock);
	if (m_form != nullptr)
	   m_form -> set_angleDisplay (f);
}

void	SDRunoPlugin_drmUi::set_programTypeLabel (const std::string s) {
	std::lock_guard<std::mutex> l (m_lock);
	if (m_form != nullptr)
	   m_form -> set_programTypeLabel (s);
}

void	SDRunoPlugin_drmUi::set_sdcSyncLabel (bool b) {
	std::lock_guard<std::mutex> l (m_lock);
	if (m_form != nullptr)
	   m_form -> set_sdcSyncLabel (b);
}

void	SDRunoPlugin_drmUi::set_aacDataLabel (const std::string s) {
	std::lock_guard<std::mutex> l (m_lock);
	if (m_form != nullptr)
	   m_form -> set_aacDataLabel (s);
}

void	SDRunoPlugin_drmUi::set_channel_4 (const std::string s) {
	std::lock_guard<std::mutex> l (m_lock);
	if (m_form != nullptr)
	   m_form -> set_channel_4 (s);
}
	
void	SDRunoPlugin_drmUi::set_faadSyncLabel (bool b) {
	std::lock_guard<std::mutex> l (m_lock);
	if (m_form != nullptr)
	   m_form -> set_faadSyncLabel (b);
}

void	SDRunoPlugin_drmUi::set_spectrumIndicator (int s) {
	std::lock_guard<std::mutex> l (m_lock);
	if (m_form != nullptr)
	   m_form -> set_spectrumIndicator (s);
}

void	SDRunoPlugin_drmUi::set_modeIndicator	(int m) {
	std::lock_guard<std::mutex> l (m_lock);
	if (m_form != nullptr)
	   m_form -> set_modeIndicator (m);
}

void	SDRunoPlugin_drmUi::set_datacoding	(const std::string s) {
	std::lock_guard<std::mutex> l (m_lock);
	if (m_form != nullptr)
	   m_form -> set_datacoding (s);
}

void	SDRunoPlugin_drmUi::set_audioModeLabel	(const std::string s) {
	std::lock_guard<std::mutex> l (m_lock);
	if (m_form != nullptr)
	   m_form -> set_audioModeLabel (s);
}

void	SDRunoPlugin_drmUi::set_messageLabel (const std::string s) {
	std::lock_guard<std::mutex> l (m_lock);
	if (m_form != nullptr)
	   m_form -> set_messageLabel (s);
}

void	SDRunoPlugin_drmUi::show_fac_mer	(float v) {
	std::lock_guard<std::mutex> l (m_lock);
        if (m_form != nullptr)
           m_form -> show_fac_mer (v);
}

void	SDRunoPlugin_drmUi::show_sdc_mer	(float v) {
	std::lock_guard<std::mutex> l (m_lock);
        if (m_form != nullptr)
           m_form -> show_sdc_mer (v);
}

void	SDRunoPlugin_drmUi::show_msc_mer	(float v) {
	std::lock_guard<std::mutex> l (m_lock);
        if (m_form != nullptr)
           m_form -> show_msc_mer (v);
}

void	SDRunoPlugin_drmUi::show_quality	(float v) {
	std::lock_guard<std::mutex> l (m_lock);
        if (m_form != nullptr)
           m_form -> show_quality (v);
}

void	SDRunoPlugin_drmUi::hide_channel_1	() {
	std::lock_guard<std::mutex> l (m_lock);
	if (m_form !=  nullptr)
	   m_form	-> hide_channel_1 ();
}

void	SDRunoPlugin_drmUi::hide_channel_2	() {
	std::lock_guard<std::mutex> l (m_lock);
	if (m_form !=  nullptr)
	   m_form	-> hide_channel_2 ();
}

void	SDRunoPlugin_drmUi::showService	(const std::string &s) {
	std::lock_guard<std::mutex> l (m_lock);
        if (m_form !=  nullptr)
           m_form       -> showService (s);
}


void	SDRunoPlugin_drmUi::activate_channel_1	() {
	m_parent. activate_channel_1 ();
}

void	SDRunoPlugin_drmUi::activate_channel_2	() {
	m_parent. activate_channel_2 ();
}

void	SDRunoPlugin_drmUi::set_audioRate	(int v) {
	std::lock_guard<std::mutex> l (m_lock);
        if (m_form !=  nullptr)
           m_form       -> set_audioRate (v);

}

void	SDRunoPlugin_drmUi::setConstellation(int c) {
	constellationMode = c;
}

void	SDRunoPlugin_drmUi::addPixel	(std::complex<float> v) {
	std::lock_guard<std::mutex> l (m_lock);
        if (m_form !=  nullptr)
           m_form       -> addPixel (v);
}

