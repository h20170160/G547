Project Title: Linux Device Driver for USB to LED subsystem 
Summary:  LED subsytem is employed to communicate from PC to LEDs connected AVR Atmega8 through USB.
          We have used vUSB in the firmware side.
          usb_control_msg was used to communicate echo requests for brightness.
          Additionally, we have created a trigger named, 'triggerable'.
                Which can be activated and deactivated using echo commands.
Firmware: build using Makefile given
          Commands: make clean
                    make all
                    avrdude -c usbasp -p m8 -U flash:w:main.hex
Kernel Space Drive: build using Makefile given
          Commands: make all
                    sudo insmod usbtoled.ko
                    
