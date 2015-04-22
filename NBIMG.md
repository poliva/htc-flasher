# NBIMG: HTC splash screen tool #

**nbimg** is a command line tool which allows to convert HTC Splash Screen images from NB to BMP and create NB splash screens from BMP format. Any splash screen size is supported (yes, it works for Diamond or Athena at 640x480 resolution too).


# Comand Line Mode #

```
=== nbimg v1.1
=== Convert NB <--> BMP splash screens
=== (c)2008 Pau Oliva - pof @ xda-developers

Usage: nbimg -F file.[nb|bmp]

Mandatory arguments:
   -F <filename>    Filename to convert.
                    If the extension is BMP it will be converted to NB.
                    If the extension is NB it will be converted to BMP.

Optional arguments:
   -w <width>       Image width in pixels. If not specified will be autodetected.
   -h <height>      Image height in pixels. If not specified will be autodetected.
   -t <pattern>     Manually specify the padding pattern (usually 0 or 255).
   -p <size>        Manually specify the padding size.
   -n               Do not add HTC splash signature to NB file.
   -s               Output smartphone format.

NBH arguments:      (only when converting from BMP to NBH)
   -D <model_id>    Generate NBH with specified Model ID (mandatory)
   -S <chunksize>   NBH SignMaxChunkSize (64 or 1024)
   -T <type>        NBH header type, this is typically 0x600 or 0x601
```


# More info #

XDA-Developers forum post:

http://forum.xda-developers.com/showthread.php?t=405118