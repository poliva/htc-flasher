# Linux ROM Kitchen #

Linux ROM kitchen is a tool that allows you to cook your own Windows Mobile ROMs for HTC Windows Mobile devices. This means you can extract the contents of a Shipped or Dumped ROM, and add/remove files or Packages into the ROM.

It consists of several tools, most of them can be run separately from command line, altough they are all nicely put together in a GUI.

Basic knowledge of ROM cooking is expected, it helps if you have played with similar kitchens available for windows before, and know a bit of the internals of WinCE OS.

Use it with CARE. If you don't know what you're doing you can brick your device. USE IT AT YOUR OWN RISK.


# Command Line Tools #

```
 lrk-extract: Exctract ROM packages from Shipped OS.nb or Dumped ROM (Part0x.raw)
 lrk-build: Create os-new.nb from kitchen folder (SYS/OEM + ROM + Packages)
 sofbuild: Generate packages.sof
 dsminfo: Show information from a *.dsm file
 dsmbuild: Create a *.dsm file from a package folder
 nbimg: Create a BMP file from a NB splashscreen, or create a NB file from a BMP
```


# Screencast #

To show the basic usage of LRK, I have created a SWF video showing how to cook a ROM, it is available here:

> http://www.eslack.org/pof/LRK/


# Linux ROM Kitchen Screenshots #

![http://pof.eslack.org/HTC/HTCFlasher/Screenshot-LRK1.png](http://pof.eslack.org/HTC/HTCFlasher/Screenshot-LRK1.png)