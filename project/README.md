Project Title: Linux Device Driver for USB to LED subsystem 

Summary:  LED subsytem is employed to communicate from PC to LEDs connected with AVR Atmega8 through USB.
          We have used vUSB in the firmware side.
          usb_control_msg was used to communicate echo commands.
          Additionally, we have created a trigger named, 'triggerable'.
                  Which can be activated and deactivated using echo commands.
                
Firmware: build using Makefile given
          Commands:
                      make clean
                  	  make all
                    	avrdude -c usbasp -p m8 -U flash:w:main.hex

Kernel Space Drive: build using Makefile given
          Commands:
                      make all
                      sudo insmod usbtoled.ko

Trigger: this module is able to add a trigger for all LEDs
          Commands:
                      make all
                    	sudo insmod usbtoled.ko
                    

To run LED project:
          Goto /sys/Class/leds/<led file name>
          Commands: cat trigger                             //will show all triggers available along with the trigger added before
                    sudo chmod 777 brightness               //makes brightness file editable
                    echo 1 > brighntess                     //LED on the hardware turns ON
                    echo 0 > brighntess                     //LED on the hardware turns OFF
                    sudo chmod 777 trigger                  //makes trigger file editable
                    echo triggerable > trigger              //makes the LED sensitive to 'triggerable' trigger
                                                            //LEDs blink in a patter on the hardware
                    
For more reference on trigger: https://elixir.bootlin.com/linux/v4.5/source/drivers/leds/trigger/ledtrig-heartbeat.c
