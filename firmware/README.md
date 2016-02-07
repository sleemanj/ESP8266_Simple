# Firmware For ESP8266

The firmwares in this folder were found at http://wiki.aprbrother.com/wiki/Firmware_For_ESP8266

And have been recorded here for posterity because for some reason the firmware binaries for these devices seem to "dissapear" from the internet, frankly google is full of dead links.

*** If anybody has a copy of the 0.9.2.4 firmware, I'd really love to get that here as well, that is what the library was originally developed for. ***

## Uploading Firmware

To upload these firmwares you can use [esptool](https://github.com/themadinventor/esptool) with the commands `write_flash 0x0 [bin file]`

Example on my Linux box with the board connected to USB1

    ./esptool.py --port /dev/ttyUSB1 --baud 9600 write_flash 0x0 ai-thinker-0.9.5.2-9600.bin 

You must pull GPIO0 to ground and reset the module before trying to upload the flash.  Remember to let GPIO go and reset again before you try and use it after uploading.


### v0.9.5.2

* [ai-thinker-0.9.5.2-9600.bin](https://cdn.rawgit.com/sleemanj/ESP8266_Simple/master/firmware/ai-thinker-0.9.5.2-9600.bin) - The default baud rate is 9600
* [ai-thinker-0.9.5.2-115200.bin](https://cdn.rawgit.com/sleemanj/ESP8266_Simple/master/firmware/ai-thinker-0.9.5.2-115200.bin) - The default baud rate is 115200


### v1.1.1

** NOT TESTED - CAUTION - YOU MUST READ THIS **

I believe that some ESP modules do not have a large enough flash chip for versions 1.0 and greater.  I think it requires at least 8 Megabit flash (1 Megabyte).  Before you try flashing this you should confirm your ESP has 8 Megabit installed. Refer: http://bbs.ai-thinker.com/forum.php?mod=viewthread&tid=984&extra=page%3D1

* [ai-thinker-v1.1.1-115200.bin](https://cdn.rawgit.com/sleemanj/ESP8266_Simple/master/firmware/ai-thinker-v1.1.1-115200.bin) - the default baud rate is 115200
