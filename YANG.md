# YANG: Yet Another NBH Generator #

Yang is a tool that allows to create custom NBH files from NB ROM parts.  You can combine all the NB ROM parts and create a single NBH file ready to flash. The NBH files created using yang are not signed with the HTC certificate, so you’ll need a method to bypass signature checking on your device bootloader. This is typically done using methods such as HardSPL, JumpSPL, USPL, SSPL, etc. as published in [xda-developers.com forum](http://forum.xda-developers.com).

Yang can also be used to extract the contents (ROM parts) found inside a NBH file. If the NBH contains a NB splash screen, it will also be converted to BMP format.


# Comand Line Mode #

```
=== yang v1.1: Yet Another NBH Generator
=== (c) 2008 Pau Oliva Fora - pof @ XDA-Developers

Usage: yang [options]

 Exctract NBH options:
    -X [filename]       Extract NBH file contents

 Generate NBH options:
    -F [filename]       Output NBH filename
    -f [file1,file2...] Comma separated list of input NB files
    -t [type1,type2...] Comma separated list of input NB types
    -s [chunksize]      SignMaxChunkSize (64 or 1024)
    -d [device]         Device name (Model ID)
    -c [CID]            CID field
    -v [version]        Version number
    -l [language]       Language
```


# yangGUI Screenshots #

**Generate NBH file (1/2)**

![http://pof.eslack.org/HTC/HTCFlasher/Screenshot-YANG.png](http://pof.eslack.org/HTC/HTCFlasher/Screenshot-YANG.png)

**Generate NBH file (2/2)**

![http://pof.eslack.org/HTC/HTCFlasher/Screenshot-YANG-1.png](http://pof.eslack.org/HTC/HTCFlasher/Screenshot-YANG-1.png)