------------------------------------------------------------------------------	
SDRunoPlugin_drm
------------------------------------------------------------------------------

The drm plugin for SDRuno is - as the name suggests - a plugin for 
decoding DRM (Digital Radio Mondiale) signals transmitted on shortwave.

![overview](/drm-decoder.png?raw=true)

-----------------------------------------------------------------------------
Installing  and starting the plugin:  READ THIS FIRST
-----------------------------------------------------------------------------

The plugin is - as other plugins - developed using MSVC.
The DRM signal has a spectrum with a width of 10 KHz, the SDRplay shows
a much wider spectrum. On a 2 MHz wide spectrum 10 KHz takes 1/200-th part
and is therefore not (hardly) visible.

It is therefore wise to select a smaller samplerate.
**On the main widget select samplerate 2000000, and a suitablee
decimation factor **. (I use a decimation factor 32, leading to a
spectrum with a width of 63 KHz)

![overview](/drm-main-widget.png?raw=true)

To use the plugin, select on the main receiver widget a bandwidth of 12k,
note that - as can be seen on the picture above - the filtered 12 k
band should cover the drm signal, **the marker is on the left of the signal **.
The small spectrum widget shows the spectrum of the filtered signal in the
range 0 .. 12 k.

![overview](/drm-small-spectrum.png?raw=true)

Note that the plugin uses some **additional libraries** (dll's) for AAC and xHE-AAC decoding.

   * libfdk-aac-2.dll, for the decoding of the AAC data is the main one.

However, running this dll requires two additional dll's to be installed

   * libgcc_s_dw-1.dll
   * libwinpthread-1.dll

These libraries are needed for convertings the AAC (or xHE-AAC) blocks
 to PCM samples. The latter are in the end converted to sound.

** If (one of) these libraries is NOT installed, the plugin will work with limited functionality: no sound will be decoded**.
In that case the plugin will
show a message to invite you to install the library (libraries)

![overview](/lib-not-found.png?raw=true)

**Copies of these dll's are in the "the-dll" folder** 
Ne aware that the folder for the community plugins is NOT standard in the
Windows searchpath. Being a simple Linux user, I have no idea how to show or alter
this searchpath, however, the folder C:\Program Files (x86)\SDRplay\SDRuno is.


------------------------------------------------------------------------
About the Plugin
------------------------------------------------------------------------

![overview](/drm-decoder-widget.png?raw=true)

As soon as the plugin is activated it will start reading in samples
and trying to synchronize and decode.

If decoding is possible, the name of the service (services) will appear
on the widget (as can be seen, the picture on top shows a decoder widget with
two names on it, the transmission received there contains two channels).

If the transmission contains a single audio component, that component
will just be made audible. If the transmission contains two audio
components, by default the top one is selected for further decoding.
Clicking with the mouse on any of the two will select that component.

If the transmission contains two components, a message will tell which
of the two components is the selected one.

As can be seen on the widget, some transmissions carry - next to audio -
some additional textual information.

A DRM signal is in one of 4 modes, Mode B is here the one being decoded here.
The spectrum is "3", telling that the aspectrum width is 10 KHz, symmetrically
around 0, i.e. 5 KHz on each side.

On the top left, one sees the frequency offset in tuning, The software
is able to correct frequency offsets up to a few hundreds of Hz,
The offset is shown as two figures, one so-called coarse offset,
relatively stable, and one so-called fine offset, which will take values
from app -23 .. + 23 Hz, and will most of the time continuously change a
little.

To the right of the frequency offset specification, sometimes one
may see the time, in UTC, when transmitted.

DRM is a digital transmission, as can be expected on shortwavesm sometimes
fading is such that decoding is impossible. A quality indication of the
signal is displayed on the right side of the widget, there are 4 labels.
Since the transmission can be decoded they are all green.
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

The decoding complexity increases from Time synchronization to AAC decoding.

-----------------------------------------------------------------------
About DRM
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

