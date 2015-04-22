# HTCFlasher #

HTCFlasher is a free (GPL'd) tool to work with HTC based bootloaders (SPLs).

Use it with CARE. If you don't know what you're doing you can brick your  device. Keep in mind that future / untested bootloaders can break backward compatibility. USE IT AT YOUR OWN RISK.

# Supported Devices #

Currently most new HTC devices are supported, and the basic set of functions to work with every HTC bootloader has been implemented, so adding support for new bootloader versions or new devices should be quite easy to do, if not working out of the box.

For an incomplete list, see [SupportedDevices](SupportedDevices.md).


# Features #

  * Flash NBH files: replaces the Windows Rom Upgrade Utility (RUU)
  * Serial prompt: replaces mtty / minicom
  * Easy to use Gtk GUI

# Comand Line Mode #

```
=== HTCflasher v3.1
=== Open source RUU for HTC devices
=== (c) 2007-2008 Pau Oliva Fora

Usage: HTCFlasher [options]

 options:
    -F [filename]    flash NBH file
    -D [device]      use another serial device (default: /dev/ttyUSB0)
    -i               get device info
    -p               enter serial prompt (minicom/mtty like)
    -v               verbose mode
    -h               shows this help message
```



# HTCFlasherGUI Screenshots #

**Main Window**

![http://pof.eslack.org/HTC/HTCFlasher/Screenshot-HTCFlasher-1.png](http://pof.eslack.org/HTC/HTCFlasher/Screenshot-HTCFlasher-1.png)

**Extract NBH file**

![http://pof.eslack.org/HTC/HTCFlasher/Screenshot-HTCFlasher-2.png](http://pof.eslack.org/HTC/HTCFlasher/Screenshot-HTCFlasher-2.png)

**Flash NBH file**

![http://pof.eslack.org/HTC/HTCFlasher/Screenshot-HTCFlasher.png](http://pof.eslack.org/HTC/HTCFlasher/Screenshot-HTCFlasher.png)