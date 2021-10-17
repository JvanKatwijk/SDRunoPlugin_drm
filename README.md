------------------------------------------------------------------------------	
SDRunoPlugin_drm
------------------------------------------------------------------------------

The drm plugin for SDRuno is - as the name suggests - a plugin for 
decoding drm signals transmitted on shortwave.

![overview](/drm-decoder.png?raw=true)

-----------------------------------------------------------------------------
Installing the plugin. READ THIS FIRST
-----------------------------------------------------------------------------

The plugin is - as other plugins - developed under MSVC.
Since DRM is a small band signal (up to 10 KHz in this plugin),
the effective samplerate, used as input for the plugin 
is 62500 samples/second.

**On the main widget select samplerate 2000000, and decimation factor 32**.

Note that the plugin uses some additional libraries (dll's) for AAC and xHE-AAC decoding.

   * libfdk-aac-2.dll, for the decoding of the AAC data is the main one.

However, running this dll requires two additional dll's to be installed

   * libgcc_s_dw-1.dll
   * libwinpthread-1.dll

These libraries are needed for the final converting of AAC (or xHE-AAC) to PCM
samples, that in the end are converted to sound.

**Note that,if (one of) these libraries is NOT installed, the plugin will work with limited functionality: no sound will be decoded**.

In case the libfdk-aac-2.dll or one of the other two could not be installed, the plugin will
show a message to invite you to install the library (libraries)

![overview](/lib-not-found.png?raw=true)

**Copies of these dll's are in the "the-dll" folder** 

------------------------------------------------------------------------
Over the Plugin
------------------------------------------------------------------------

![overview](/drm-widget.png?raw=true)

There are no controls on the widget, as soon as the plugin is activated
it will start reading in samples and trying to decode DRM.

A DRM signal is in one of 4 modes, Mode B is the one being decoded here.
The spectrum is "3", telling that the width is 10 KHz, symmetrically
around 0, i.e. 5 KHz on each side.

The different labels in the
widget give information on the reception.
The name of the receiver service is "SNR Tiganesti E1".

On the right hand side of the widget, there are four labels, 
since the transmission can be decoder they are all green.
From top to bottom

   * the time sync label. DRM is transmitted using an (C)OFDM technique, what is important is that there is a time synchronization so that the receiver "knows"
where segments in the incoming sample stream start;

   * the FAC label. A DRM transmission contains data in the so-called FAC, i.e. Fast Access Channel. The FAC data is encoded as 4QAM, which is relatively easy
to decode (given that the time synchronizaarion is OK). FAC data is concerned
with structuring the data;

   * the SDC label. The SDC, Service Description Channel, contains - as the
name suggests - the description of the service, its name, how it is encoded, etc.

   * the AAC label. The service is encoded in the Main Service Channel, and
usually encoded as an AAC stream. 

For each of these labels it holds that "green"  means that processing is 
without errors.

The bottom line of the widget gives some quality indicators in a more
quantitative way. From left to right:

   * the quality of the FAC signal, expressed as number where higher is better;

   * the quality of the SDC signal, expressed as number, higher is better;

   * the quality of the AAC decoder, expressed as number, higher is better.

On the second to bottom line, we see the spectrum (3), the Mode (B),
the encoding of the MSC (QAM16) and the way the audio is encoded (AAC).

The remaining labels give an indication of the frequency and time offset
of the received signals. All labels are equipped with a tooltip telling
the function.

-----------------------------------------------------------------------
Over DRM
-----------------------------------------------------------------------

DRM, Digital Radio Mondiale, is a form of digital radio. Transmissions
are relatively scarce here in Western Europe, but is seems that elsewhere
there is an increasing interest.

For DRM signals there are different Modes and Spectra, here is Western
Europe transmissions can be received in Mode B, with a spectrum of 10 KHz.
Other spectra are 4.5, 5, 9, 18 and 20 KHz.

Different Modes have different measures against fading in the signal.

The picture shows  a reasonable signal on 9760 KHz, a transmission of the
Radio Rumenia International, in Mode B, with a spectrum of 10 KHz.
The waterfall clearly shows the effect of fading on the signal,
the signal can be decoded however.

The picture shows that the receiver is tuned to 9760 KHz, that
the samplerate is already reduced to 62.5 KHz (the decoder will do the
further filtering an reduction to 12 KHz). 

The decoder is - apart from GUI issues - a mirror of the decoder of the drm-receiver I
developed under (and for) Linux.

