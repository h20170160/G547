#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>

#include "usbdrv.h"

#define F_CPU 16000000L
#include <util/delay.h>

#define USB_LED_OFF 0
#define USB_LED_ON  1
#define USB_LED_OFF1 2
#define USB_LED_ON1  3

//////////// Connect a LED at pin 14 i.e. PB0////////////////////////

USB_PUBLIC uchar usbFunctionSetup(uchar data[8]) {
    usbRequest_t *rq = (void *)data; // cast data to correct type
        
    switch(rq->bRequest) { // custom command is in the bRequest field
    case USB_LED_ON:
        PORTB |= 1; // turn LED on
        return 0;
    case USB_LED_OFF: 
        PORTB &= ~(1); // turn LED off  //pin 15
        return 0;
	case USB_LED_ON1:
        PORTB |= 2; // turn LED on
        return 0;
    case USB_LED_OFF1: 
        PORTB &= ~(2); // turn LED off  //pin 15
        return 0;
    }

    return 0; // should not get here
}

///////////////////////////////////////////////////////////////////////

int main() {
        uchar i;
		DDRB =3;

    wdt_enable(WDTO_1S); // enable 1s watchdog timer

    usbInit();
        
    usbDeviceDisconnect(); // enforce re-enumeration
    for(i = 0; i<250; i++) { // wait 500 ms
        wdt_reset(); 
        _delay_ms(2);
    }
    usbDeviceConnect();
        
    sei(); // Enable interrupts after re-enumeration
        
    while(1) {
        wdt_reset(); 
        usbPoll();
    }
        
    return 0;
}
