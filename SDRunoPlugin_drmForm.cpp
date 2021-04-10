#include <sstream>
#ifdef _WIN32
#include <Windows.h>
#endif

#include "SDRunoPlugin_drmForm.h"
#include "SDRunoPlugin_drmUi.h"
#include "resource.h"
#include <io.h>
#include <shlobj.h>
#include "basics.h"

// Form constructor with handles to parent and uno controller - launches form Setup
	SDRunoPlugin_drmForm::
	             SDRunoPlugin_drmForm (SDRunoPlugin_drmUi& parent,
	                                   IUnoPluginController& controller) :
	nana::form(nana::API::make_center(formWidth, formHeight), nana::appearance(false, true, false, false, true, false, false)),
	m_parent(parent),
	m_controller(controller)
{
	Setup();
}

// Form deconstructor
	SDRunoPlugin_drmForm::~SDRunoPlugin_drmForm () {
}

// Start Form and start Nana UI processing
void	SDRunoPlugin_drmForm::Run () {
	show();
	nana::exec();
}

// Create the initial plugin form
void	SDRunoPlugin_drmForm::Setup () {
	// This first section is all related to the background and border
	// it shouldn't need to be changed
	nana::paint::image img_border;
	nana::paint::image img_inner;
	HMODULE hModule = NULL;
	HRSRC rc_border = NULL;
	HRSRC rc_inner = NULL;
	HRSRC rc_close = NULL;
	HRSRC rc_close_over = NULL;
	HRSRC rc_min = NULL;
	HRSRC rc_min_over = NULL;
	HRSRC rc_bar = NULL;
	HRSRC rc_sett = NULL;
	HRSRC rc_sett_over = NULL;
	HBITMAP bm_border = NULL;
	HBITMAP bm_inner = NULL;
	HBITMAP bm_close = NULL;
	HBITMAP bm_close_over = NULL;
	HBITMAP bm_min = NULL;
	HBITMAP bm_min_over = NULL;
	HBITMAP bm_bar = NULL;
	HBITMAP bm_sett = NULL;
	HBITMAP bm_sett_over = NULL;
	BITMAPINFO bmInfo_border = { 0 };
	BITMAPINFO bmInfo_inner = { 0 };
	BITMAPINFO bmInfo_close = { 0 };
	BITMAPINFO bmInfo_close_over = { 0 };
	BITMAPINFO bmInfo_min = { 0 };
	BITMAPINFO bmInfo_min_over = { 0 };
	BITMAPINFO bmInfo_bar = { 0 };
	BITMAPINFO bmInfo_sett = { 0 };
	BITMAPINFO bmInfo_sett_over = { 0 };
	BITMAPFILEHEADER borderHeader = { 0 };
	BITMAPFILEHEADER innerHeader = { 0 };
	BITMAPFILEHEADER closeHeader = { 0 };
	BITMAPFILEHEADER closeoverHeader = { 0 };
	BITMAPFILEHEADER minHeader = { 0 };
	BITMAPFILEHEADER minoverHeader = { 0 };
	BITMAPFILEHEADER barHeader = { 0 };
	BITMAPFILEHEADER settHeader = { 0 };
	BITMAPFILEHEADER settoverHeader = { 0 };
	HDC hdc = NULL;
	BYTE* borderPixels = NULL;
	BYTE* innerPixels = NULL;
	BYTE* closePixels = NULL;
	BYTE* closeoverPixels = NULL;
	BYTE* minPixels = NULL;
	BYTE* minoverPixels = NULL;
	BYTE* barPixels = NULL;
	BYTE* barfocusedPixels = NULL;
	BYTE* settPixels = NULL;
	BYTE* settoverPixels = NULL;
	const unsigned int rawDataOffset = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFO);
	hModule = GetModuleHandle(L"SDRunoPlugin_drm");
	hdc = GetDC(NULL);
	rc_border = FindResource(hModule, MAKEINTRESOURCE(IDB_BG_BORDER), RT_BITMAP);
	rc_inner = FindResource(hModule, MAKEINTRESOURCE(IDB_BACKGROUND), RT_BITMAP);
	rc_close = FindResource(hModule, MAKEINTRESOURCE(IDB_CLOSE), RT_BITMAP);
	rc_close_over = FindResource(hModule, MAKEINTRESOURCE(IDB_CLOSE_DOWN), RT_BITMAP);
	rc_min = FindResource(hModule, MAKEINTRESOURCE(IDB_MIN), RT_BITMAP);
	rc_min_over = FindResource(hModule, MAKEINTRESOURCE(IDB_MIN_DOWN), RT_BITMAP);
	rc_bar = FindResource(hModule, MAKEINTRESOURCE(IDB_HEADER), RT_BITMAP);
	rc_sett = FindResource(hModule, MAKEINTRESOURCE(IDB_SETT), RT_BITMAP);
	rc_sett_over = FindResource(hModule, MAKEINTRESOURCE(IDB_SETT_DOWN), RT_BITMAP);
	bm_border = (HBITMAP)LoadImage(hModule, MAKEINTRESOURCE(IDB_BG_BORDER), IMAGE_BITMAP, 0, 0, LR_COPYFROMRESOURCE);
	bm_inner = (HBITMAP)LoadImage(hModule, MAKEINTRESOURCE(IDB_BACKGROUND), IMAGE_BITMAP, 0, 0, LR_COPYFROMRESOURCE);
	bm_close = (HBITMAP)LoadImage(hModule, MAKEINTRESOURCE(IDB_CLOSE), IMAGE_BITMAP, 0, 0, LR_COPYFROMRESOURCE);
	bm_close_over = (HBITMAP)LoadImage(hModule, MAKEINTRESOURCE(IDB_CLOSE_DOWN), IMAGE_BITMAP, 0, 0, LR_COPYFROMRESOURCE);
	bm_min = (HBITMAP)LoadImage(hModule, MAKEINTRESOURCE(IDB_MIN), IMAGE_BITMAP, 0, 0, LR_COPYFROMRESOURCE);
	bm_min_over = (HBITMAP)LoadImage(hModule, MAKEINTRESOURCE(IDB_MIN_DOWN), IMAGE_BITMAP, 0, 0, LR_COPYFROMRESOURCE);
	bm_bar = (HBITMAP)LoadImage(hModule, MAKEINTRESOURCE(IDB_HEADER), IMAGE_BITMAP, 0, 0, LR_COPYFROMRESOURCE);
	bm_sett = (HBITMAP)LoadImage(hModule, MAKEINTRESOURCE(IDB_SETT), IMAGE_BITMAP, 0, 0, LR_COPYFROMRESOURCE);
	bm_sett_over = (HBITMAP)LoadImage(hModule, MAKEINTRESOURCE(IDB_SETT_DOWN), IMAGE_BITMAP, 0, 0, LR_COPYFROMRESOURCE);
	bmInfo_border.bmiHeader.biSize = sizeof(bmInfo_border.bmiHeader);
	bmInfo_inner.bmiHeader.biSize = sizeof(bmInfo_inner.bmiHeader);
	bmInfo_close.bmiHeader.biSize = sizeof(bmInfo_close.bmiHeader);
	bmInfo_close_over.bmiHeader.biSize = sizeof(bmInfo_close_over.bmiHeader);
	bmInfo_min.bmiHeader.biSize = sizeof(bmInfo_min.bmiHeader);
	bmInfo_min_over.bmiHeader.biSize = sizeof(bmInfo_min_over.bmiHeader);
	bmInfo_bar.bmiHeader.biSize = sizeof(bmInfo_bar.bmiHeader);
	bmInfo_sett.bmiHeader.biSize = sizeof(bmInfo_sett.bmiHeader);
	bmInfo_sett_over.bmiHeader.biSize = sizeof(bmInfo_sett_over.bmiHeader);
	GetDIBits(hdc, bm_border, 0, 0, NULL, &bmInfo_border, DIB_RGB_COLORS);
	GetDIBits(hdc, bm_inner, 0, 0, NULL, &bmInfo_inner, DIB_RGB_COLORS);
	GetDIBits(hdc, bm_close, 0, 0, NULL, &bmInfo_close, DIB_RGB_COLORS);
	GetDIBits(hdc, bm_close_over, 0, 0, NULL, &bmInfo_close_over, DIB_RGB_COLORS);
	GetDIBits(hdc, bm_min, 0, 0, NULL, &bmInfo_min, DIB_RGB_COLORS);
	GetDIBits(hdc, bm_min_over, 0, 0, NULL, &bmInfo_min_over, DIB_RGB_COLORS);
	GetDIBits(hdc, bm_bar, 0, 0, NULL, &bmInfo_bar, DIB_RGB_COLORS);
	GetDIBits(hdc, bm_sett, 0, 0, NULL, &bmInfo_sett, DIB_RGB_COLORS);
	GetDIBits(hdc, bm_sett_over, 0, 0, NULL, &bmInfo_sett_over, DIB_RGB_COLORS);
	bmInfo_border.bmiHeader.biCompression = BI_RGB;
	bmInfo_inner.bmiHeader.biCompression = BI_RGB;
	bmInfo_close.bmiHeader.biCompression = BI_RGB;
	bmInfo_close_over.bmiHeader.biCompression = BI_RGB;
	bmInfo_min.bmiHeader.biCompression = BI_RGB;
	bmInfo_min_over.bmiHeader.biCompression = BI_RGB;
	bmInfo_bar.bmiHeader.biCompression = BI_RGB;
	bmInfo_sett.bmiHeader.biCompression = BI_RGB;
	bmInfo_sett_over.bmiHeader.biCompression = BI_RGB;
	borderHeader.bfOffBits = rawDataOffset;
	borderHeader.bfSize = bmInfo_border.bmiHeader.biSizeImage;
	borderHeader.bfType = 0x4D42;
	innerHeader.bfOffBits = rawDataOffset;
	innerHeader.bfSize = bmInfo_inner.bmiHeader.biSizeImage;
	innerHeader.bfType = 0x4D42;
	closeHeader.bfOffBits = rawDataOffset;
	closeHeader.bfSize = bmInfo_close.bmiHeader.biSizeImage;
	closeHeader.bfType = 0x4D42;
	closeoverHeader.bfOffBits = rawDataOffset;
	closeoverHeader.bfSize = bmInfo_close_over.bmiHeader.biSizeImage;
	closeoverHeader.bfType = 0x4D42;
	minHeader.bfOffBits = rawDataOffset;
	minHeader.bfSize = bmInfo_min.bmiHeader.biSizeImage;
	minHeader.bfType = 0x4D42;
	minoverHeader.bfOffBits = rawDataOffset;
	minoverHeader.bfSize = bmInfo_min_over.bmiHeader.biSizeImage;
	minoverHeader.bfType = 0x4D42;
	barHeader.bfOffBits = rawDataOffset;
	barHeader.bfSize = bmInfo_bar.bmiHeader.biSizeImage;
	barHeader.bfType = 0x4D42;
	settHeader.bfOffBits = rawDataOffset;
	settHeader.bfSize = bmInfo_sett.bmiHeader.biSizeImage;
	settHeader.bfType = 0x4D42;
	settoverHeader.bfOffBits = rawDataOffset;
	settoverHeader.bfSize = bmInfo_sett_over.bmiHeader.biSizeImage;
	settoverHeader.bfType = 0x4D42;
	borderPixels = new BYTE[bmInfo_border.bmiHeader.biSizeImage + rawDataOffset];
	innerPixels = new BYTE[bmInfo_inner.bmiHeader.biSizeImage + rawDataOffset];
	closePixels = new BYTE[bmInfo_close.bmiHeader.biSizeImage + rawDataOffset];
	closeoverPixels = new BYTE[bmInfo_close_over.bmiHeader.biSizeImage + rawDataOffset];
	minPixels = new BYTE[bmInfo_min.bmiHeader.biSizeImage + rawDataOffset];
	minoverPixels = new BYTE[bmInfo_min_over.bmiHeader.biSizeImage + rawDataOffset];
	barPixels = new BYTE[bmInfo_bar.bmiHeader.biSizeImage + rawDataOffset];
	settPixels = new BYTE[bmInfo_sett.bmiHeader.biSizeImage + rawDataOffset];
	settoverPixels = new BYTE[bmInfo_sett_over.bmiHeader.biSizeImage + rawDataOffset];
	*(BITMAPFILEHEADER*)borderPixels = borderHeader;
	*(BITMAPINFO*)(borderPixels + sizeof(BITMAPFILEHEADER)) = bmInfo_border;
	*(BITMAPFILEHEADER*)innerPixels = innerHeader;
	*(BITMAPINFO*)(innerPixels + sizeof(BITMAPFILEHEADER)) = bmInfo_inner;
	*(BITMAPFILEHEADER*)closePixels = closeHeader;
	*(BITMAPINFO*)(closePixels + sizeof(BITMAPFILEHEADER)) = bmInfo_close;
	*(BITMAPFILEHEADER*)closeoverPixels = closeoverHeader;
	*(BITMAPINFO*)(closeoverPixels + sizeof(BITMAPFILEHEADER)) = bmInfo_close_over;
	*(BITMAPFILEHEADER*)minPixels = minHeader;
	*(BITMAPINFO*)(minPixels + sizeof(BITMAPFILEHEADER)) = bmInfo_min;
	*(BITMAPFILEHEADER*)minoverPixels = minoverHeader;
	*(BITMAPINFO*)(minoverPixels + sizeof(BITMAPFILEHEADER)) = bmInfo_min_over;
	*(BITMAPFILEHEADER*)barPixels = barHeader;
	*(BITMAPINFO*)(barPixels + sizeof(BITMAPFILEHEADER)) = bmInfo_bar;
	*(BITMAPFILEHEADER*)settPixels = settHeader;
	*(BITMAPINFO*)(settPixels + sizeof(BITMAPFILEHEADER)) = bmInfo_sett;
	*(BITMAPFILEHEADER*)settoverPixels = settoverHeader;
	*(BITMAPINFO*)(settoverPixels + sizeof(BITMAPFILEHEADER)) = bmInfo_sett_over;
	GetDIBits(hdc, bm_border, 0, bmInfo_border.bmiHeader.biHeight, (LPVOID)(borderPixels + rawDataOffset), &bmInfo_border, DIB_RGB_COLORS);
	GetDIBits(hdc, bm_inner, 0, bmInfo_inner.bmiHeader.biHeight, (LPVOID)(innerPixels + rawDataOffset), &bmInfo_inner, DIB_RGB_COLORS);
	GetDIBits(hdc, bm_close, 0, bmInfo_close.bmiHeader.biHeight, (LPVOID)(closePixels + rawDataOffset), &bmInfo_close, DIB_RGB_COLORS);
	GetDIBits(hdc, bm_close_over, 0, bmInfo_close_over.bmiHeader.biHeight, (LPVOID)(closeoverPixels + rawDataOffset), &bmInfo_close_over, DIB_RGB_COLORS);
	GetDIBits(hdc, bm_min, 0, bmInfo_min.bmiHeader.biHeight, (LPVOID)(minPixels + rawDataOffset), &bmInfo_min, DIB_RGB_COLORS);
	GetDIBits(hdc, bm_min_over, 0, bmInfo_min_over.bmiHeader.biHeight, (LPVOID)(minoverPixels + rawDataOffset), &bmInfo_min_over, DIB_RGB_COLORS);
	GetDIBits(hdc, bm_bar, 0, bmInfo_bar.bmiHeader.biHeight, (LPVOID)(barPixels + rawDataOffset), &bmInfo_bar, DIB_RGB_COLORS);
	GetDIBits(hdc, bm_sett, 0, bmInfo_sett.bmiHeader.biHeight, (LPVOID)(settPixels + rawDataOffset), &bmInfo_sett, DIB_RGB_COLORS);
	GetDIBits(hdc, bm_sett_over, 0, bmInfo_sett_over.bmiHeader.biHeight, (LPVOID)(settoverPixels + rawDataOffset), &bmInfo_sett_over, DIB_RGB_COLORS);
	img_border.open(borderPixels, bmInfo_border.bmiHeader.biSizeImage);
	img_inner.open(innerPixels, bmInfo_inner.bmiHeader.biSizeImage);
	img_close_normal.open(closePixels, bmInfo_close.bmiHeader.biSizeImage);
	img_close_down.open(closeoverPixels, bmInfo_close_over.bmiHeader.biSizeImage);
	img_min_normal.open(minPixels, bmInfo_min.bmiHeader.biSizeImage);
	img_min_down.open(minoverPixels, bmInfo_min_over.bmiHeader.biSizeImage);
	img_header.open(barPixels, bmInfo_bar.bmiHeader.biSizeImage);
	img_sett_normal.open(settPixels, bmInfo_sett.bmiHeader.biSizeImage);
	img_sett_down.open(settoverPixels, bmInfo_sett_over.bmiHeader.biSizeImage);
	ReleaseDC(NULL, hdc);
	bg_border.load(img_border, nana::rectangle(0, 0, 590, 340));
	bg_border.stretchable(0, 0, 0, 0);
	bg_border.transparent(true);
	bg_inner.load(img_inner, nana::rectangle(0, 0, 582, 299));
	bg_inner.stretchable(sideBorderWidth, 0, sideBorderWidth, bottomBarHeight);
	bg_inner.transparent(false);

	// TODO: Form code starts here

	// Load X and Y location for the form from the ini file (if exists)
	int posX = m_parent.LoadX();
	int posY = m_parent.LoadY();
	move(posX, posY);

	// This code sets the plugin size, title and what to do when the X is pressed
	size(nana::size(formWidth, formHeight));
	caption ("SDRuno drm Plugin");
	events().destroy([&] { m_parent.FormClosed(); });

	//Initialize header bar
	header_bar.size(nana::size(122, 20));
	header_bar.load(img_header, nana::rectangle(0, 0, 122, 20));
	header_bar.stretchable(0, 0, 0, 0);
	header_bar.move(nana::point((formWidth / 2) - 61, 5));
	header_bar.transparent(true);

	//Initial header text 
	title_bar_label.size(nana::size(65, 12));
	title_bar_label.move(nana::point((formWidth / 2) - 5, 9));
	title_bar_label.format(true);
	title_bar_label.caption("< bold size = 6 color = 0x000000 font = \"Verdana\">TEMPLATE</>");
	title_bar_label.text_align(nana::align::center, nana::align_v::center);
	title_bar_label.fgcolor(nana::color_rgb(0x000000));
	title_bar_label.transparent(true);

	//Iniitialize drag_label
	form_drag_label.move(nana::point(0, 0));
	form_drag_label.transparent(true);

	//Initialize dragger and set target to form, and trigger to drag_label 
	form_dragger.target(*this);
	form_dragger.trigger(form_drag_label);

	//Initialise the "Minimize button"
	min_button.load(img_min_normal, nana::rectangle(0, 0, 20, 15));
	min_button.bgcolor(nana::color_rgb(0x000000));
	min_button.move(nana::point(formWidth - 51, 9));
	min_button.transparent(true);
	min_button.events().mouse_down([&] { min_button.load(img_min_down, nana::rectangle(0, 0, 20, 15)); });
	min_button.events().mouse_up([&] { min_button.load(img_min_normal, nana::rectangle(0, 0, 20, 15)); nana::API::zoom_window(this->handle(), false); });
	min_button.events().mouse_leave([&] { min_button.load(img_min_normal, nana::rectangle(0, 0, 20, 15)); });

	//Initialise the "Close button"
	close_button.load(img_close_normal, nana::rectangle(0, 0, 20, 15));
	close_button.bgcolor(nana::color_rgb(0x000000));
	close_button.move(nana::point(formWidth - 26, 9));
	close_button.transparent(true);
	close_button.events().mouse_down([&] { close_button.load(img_close_down, nana::rectangle(0, 0, 20, 15)); });
	close_button.events().mouse_up([&] { close_button.load(img_close_normal, nana::rectangle(0, 0, 20, 15)); close(); });
	close_button.events().mouse_leave([&] { close_button.load(img_close_normal, nana::rectangle(0, 0, 20, 15)); });

	//Uncomment the following block of code to Initialise the "Setting button"
	sett_button.load(img_sett_normal, nana::rectangle(0, 0, 40, 15));
	sett_button.bgcolor(nana::color_rgb(0x000000));
	sett_button.move(nana::point(10, 9));
	sett_button.events().mouse_down([&] { sett_button.load(img_sett_down, nana::rectangle(0, 0, 40, 15)); });
	sett_button.events().mouse_up([&] { sett_button.load(img_sett_normal, nana::rectangle(0, 0, 40, 15)); SettingsButton_Click(); });
	sett_button.events().mouse_leave([&] { sett_button.load(img_sett_normal, nana::rectangle(0, 0, 40, 15)); });
	sett_button.tooltip("Show settings window");
	sett_button.transparent(true);

	// TODO: Extra Form code goes here
	label_1. transparent (true);
	label_1. fgcolor (nana::colors::white);
	label_1. caption ("time sync");

	label_2. transparent (true);
	label_2. fgcolor (nana::colors::white);
	label_2. caption ("FAC sync");
	
	label_3. transparent (true);
	label_3. fgcolor (nana::colors::white);
	label_3. caption ("SDC sync");

	label_4. transparent (true);
	label_4. fgcolor (nana::colors::white);
	label_4. caption ("AAC sync");

	channel_1. events (). click ([&](){activate_channel_1 ();});
	channel_2. events (). click ([&](){activate_channel_2 ();});

	timeDelayDisplay. tooltip ("estimate of time delay");
	intOffsetDisplay. tooltip ("estimate of integer frequency offset");
	timeSyncLabel. tooltip ("time synchronized if green");
	timeSyncLabel. bgcolor (nana::colors::red);

	smallOffsetDisplay. tooltip ("estimate of fraction frequency offset");
	facSyncLabel. tooltip ("if green, FAC could be decoded");
	facSyncLabel.bgcolor(nana::colors::red);

	clockOffsetDisplay. tooltip ("estimate of clock offset");
	angleDisplay. tooltip ("estimate of error in phase angle");
	sdcSyncLabel. tooltip ("if gree, nthe SDC could be decoded");
	sdcSyncLabel.bgcolor(nana::colors::red);

	snrDisplay. tooltip ("estimate of SNR");	
	aacDataLabel. tooltip ("type  of aac");
	faadSyncLabel.  tooltip ("if green, the aac in the selected service is decoded");
	faadSyncLabel.bgcolor(nana::colors::red);

	spectrumIndicator. tooltip ("type of detected spectrum");
	modeIndicator. tooltip ("the detected mode");

	pixelStore. resize (nrRows * nrColumns);
	eqPicture = new nana::drawing (imageLabel);
	eqPicture -> draw ([&](nana::paint::graphics& graph) {
	        for (int i = 0; i <  nrRows ; i ++)
		   for (int j = 0; j < nrColumns; j++) {
	              int res =
			   pixelStore [i * nrColumns + j];
	              graph.set_pixel (j, i, res == 0 ?
	                                     nana::colors::white :
	                                     res == 100 ?
	                                     nana::colors::black:
	                                     nana::colors::red);
	           }
		});
}

void	SDRunoPlugin_drmForm::SettingsButton_Click () {
	// TODO: Insert code here to show settings panel
}

void	SDRunoPlugin_drmForm::set_timeDelayDisplay (float f) {
	timeDelayDisplay. caption (std::to_string (f));
}

void	SDRunoPlugin_drmForm::set_intOffsetDisplay (int offset) {
	intOffsetDisplay. caption (std::to_string (offset));
}

void	SDRunoPlugin_drmForm::set_countryLabel (const std::string s) {
	countryLabel. caption (s);
}

void	SDRunoPlugin_drmForm::hide_channel_1 () {
	channel_1. caption ("");
}

void	SDRunoPlugin_drmForm::set_channel_1 (const std::string s) {
	channel_1. caption (s);
}

void	SDRunoPlugin_drmForm::activate_channel_1 () {
	m_parent, activate_channel_1 ();
}

void	SDRunoPlugin_drmForm::hide_channel_2	() {
	channel_2. caption ("");
}

void	SDRunoPlugin_drmForm::set_channel_2 (const std::string s) {
	channel_2. caption (s);
}

void	SDRunoPlugin_drmForm::activate_channel_2 () {
	m_parent, activate_channel_2 ();
}

void	SDRunoPlugin_drmForm::set_channel_3 (const std::string s) {
	channel_3. caption (s);
}

void	SDRunoPlugin_drmForm::set_channel_4 (const std::string s) {
	channel_4. caption (s);
}

void	SDRunoPlugin_drmForm::set_timeSyncLabel (bool b) {
	if (b)
	   timeSyncLabel. bgcolor (nana::colors::green);
	else
	   timeSyncLabel. bgcolor (nana::colors::red);
}

void	SDRunoPlugin_drmForm::set_timeOffsetDisplay (float f) {
	timeOffsetDisplay. caption (std::to_string (f));
}

void	SDRunoPlugin_drmForm::set_smallOffsetDisplay (float f) {
	smallOffsetDisplay. caption (std::to_string (f));
}

void	SDRunoPlugin_drmForm::set_timeLabel (const std::string s) {
	timeLabel. caption (s);
}

void	SDRunoPlugin_drmForm::set_facSyncLabel (bool b) {
	if (b) 
	   facSyncLabel. bgcolor (nana::colors::green);
        else
           facSyncLabel. bgcolor (nana::colors::red);
}

void	SDRunoPlugin_drmForm::set_sdcSyncLabel (bool b) {
	if (b)
	   sdcSyncLabel. bgcolor (nana::colors::green);
    else
       sdcSyncLabel. bgcolor (nana::colors::red);
}

void	SDRunoPlugin_drmForm::set_clockOffsetDisplay (float f) {
	clockOffsetDisplay. caption (std::to_string (f));
}

void	SDRunoPlugin_drmForm::set_angleDisplay (float f) {
	angleDisplay. caption  (std::to_string (f));
}

void	SDRunoPlugin_drmForm::set_programTypeLabel (const std::string s) {
	programTypeLabel. caption (s);
}


void	SDRunoPlugin_drmForm::set_phaseOffsetDisplay (float f) {
	phaseOffsetDisplay. caption (std::to_string (f));
}

void	SDRunoPlugin_drmForm::set_snrDisplay (float f) {
	snrDisplay. caption (std::to_string (f));
}

void	SDRunoPlugin_drmForm::set_aacDataLabel (const std::string s) {
	aacDataLabel. caption (s);
}


void	SDRunoPlugin_drmForm::set_faadSyncLabel (bool b) {
	if (b)
	   faadSyncLabel. bgcolor (nana::colors::green);
        else
           faadSyncLabel. bgcolor (nana::colors::red);
}

void	SDRunoPlugin_drmForm::set_spectrumIndicator (int s) {
	spectrumIndicator. caption (std::to_string (s));
}

static std::string modes [] = {"A", "B", "C", "D"};

void	SDRunoPlugin_drmForm::set_modeIndicator (int m) {
	if ((m < 0) || (m > 3))
	   return;
	modeIndicator. caption (modes [m - 1]);
}

void	SDRunoPlugin_drmForm::set_datacoding (const std::string s) {
	datacoding. caption (s);
}

void	SDRunoPlugin_drmForm::set_audioModeLabel (const std::string s) {
	audioModeLabel. caption (s);
}

void	SDRunoPlugin_drmForm::set_messageLabel (const std::string s) {
	messageLabel. caption (s);
}

void	SDRunoPlugin_drmForm::showLines	(std::vector<std::complex<float>> &v) {
std::vector<float> phasesR (nrColumns);
std::vector<float> amplitudesR (nrColumns);
float	phaseScaler	= 0;
float	amplitudeScaler	= 0;
int factor	= v. size () / nrColumns;

	for (int i = 0; i < nrColumns; i ++) {
	   phasesR [i] = 0;
	   amplitudesR [i] = 0;
	   for (int j = 0; j < factor; j ++) {
	      phasesR [i] += arg (v [i + factor + j]);
	      amplitudesR [i] += abs (v [i * factor + j]);
	   }
	   phaseScaler += phasesR [i];
	   amplitudeScaler += amplitudesR [i]; 
	}

	phaseScaler /= nrColumns;
	amplitudeScaler /= nrColumns;

	for (int i = 0; i < nrRows; i ++)
	   for (int j = 0; j < nrColumns; j ++)
	      pixelStore [i * nrColumns + j] = 0;

	for (int i = 0; i < nrColumns; i ++) {
	   int scaledPhase = phasesR [i] / phaseScaler * 30  + 150;
	   int scaledAmplitude = amplitudesR [i] / amplitudeScaler * 10 + 50;
	   if (scaledPhase < 0)
	      scaledPhase = 0;
	   if (scaledPhase >= 200)
	      scaledPhase = 199;
	   if (scaledAmplitude < 0)
	     scaledAmplitude = 0;
	   if (scaledAmplitude >= 200)
	      scaledAmplitude = 199;
	   pixelStore [scaledPhase * nrColumns + i] = 100;
	   pixelStore [scaledAmplitude * nrColumns + i] = 200;
	}
	eqPicture -> update ();
}

void	SDRunoPlugin_drmForm::clearScreen	() {}

