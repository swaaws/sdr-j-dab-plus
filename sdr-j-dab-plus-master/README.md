PLEASE READ

This directory contains the implementation of a -relatively simple -
dab/dab+ receiver.
The receiver supports terrestrial DAB reception with as input either
the stream from a dabstick or a (prerecorded) file,
and it will output through the default soundcard.


Building:
As usual, a pre-compiled version for 64 bits Windows exists, and for Linux
one has to collect libraries and do the building oneself.

Libraries that are needed are

qwt		(6.2)
qt		(4.8 or more)
usbx		(1.0.14 used)
portaudio	0.19
fftw3f
rtlsdr
faad
sndfile
samplerate

Two possibilities for building the software are there: the Qt qmake tools
or the CMake tools. Both have been tested on Fedora 19 and Fedora 20

QMake:
The dabreceiver.pro file assumes the availability of all libraries.

When using fedora (or Ubuntu), just install the required packages!!!

Running:

Normal way of running is through
  ./sdr-j-dabreceiver-0.97

(Note that running under Windows requires a Zadiq-set usb driver.)

good luck

jan

Manual and License

http://www.sdr-j.tk/sdr-j-dab-manual-0.98.pdf
