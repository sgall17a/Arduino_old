#include <Carrier.h>
#include <IRremote.h>

// the Carrier code generator object
Carrier carrier(MODE_auto,FAN_3,AIRFLOW_dir_1,25,STATE_on);

// the IR emitter object
IRsend irsend;

void setup()
{
    // wait 5 seconds
    delay(5000);

    // change the state
    carrier.setState(STATE_off);
    // send the code
    irsend.sendRaw(carrier.codes,CARRIER_BUFFER_SIZE,38);
	
    // wait 10 seconds
    delay(10000);

    // change the state
    carrier.setState(STATE_on);			
    // send the code
    irsend.sendRaw(carrier.codes,CARRIER_BUFFER_SIZE,38);
}

void loop() {}