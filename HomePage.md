# HTCFlasher #

**HTCFlasher** -formerly known as HERMflasher- is an open source tool which allows you to flash ROMs on most current HTC devices. It has some extra features that the original HTC RUU doesn't have, like for example it can present a serial prompt to the bootloader (replacing mtty).

  * [HTCFlasher wiki page](HTCFlasher.md)


# YANG #

**YANG** is Yet Another NBH Generator. A tool that allows you to generate custom NBH files out of ROM parts (.nb) files. You can choose the CID, ModelID, Language, etc. and also adjust the NBH signature size, thus making valid NBHs for all HTC devices. You need a way to flash custom/cooked ROMs (ie: HardSPL, USPL, SSPL, JumpSPL, etc...) to flash NBH files generated with YANG. YANG can also dump NBH file contents (.nb ROM parts), this is a command line replacement for windows tools like nbhextract or htcrt.

  * [YANG wiki page](YANG.md)


# Linux ROM Kitchen #

**Linux ROM Kitchen** is a tool that allows you to cook your own Windows Mobile ROMs for HTC Windows Mobile devices. This means you can extract the contents of a Shipped or Dumped ROM, and add/remove files or Packages into the ROM.

  * [Linux ROM Kitchen wiki page](Linux_ROM_Kitchen.md)


# NBIMG #

**nbimg** is a tool to convert NB splash screens to BMP images, it can also do the reverse, convert BMP images to NB splash screens. It can work with any resolution, and supports HTC PocketPC and Smartphone splash screen formats. This tool is command line only, I have not written a GUI for it yet.

  * [NBIMG wiki page](NBIMG.md)