/*
 * Carrier
 * Version 0.03 April, 2013
 * Copyright 2011 Bob Fisch
 *
 * Library to generate IR codes for a Carrier air conditionner.
 *
 * For details, see http://arduino.fisch.lu
 */

#include "Carrier.h"

/**
 * set the given temperature and updates the codes
 * @param   _value  the new value [17..32]
 */
void Carrier::setTemeprature(uint8_t _value)
{
    

    temperature=_value;

    if (temperature & 32) 
        codes[39]=CODE_high;
    else 
        codes[39]=CODE_low;
    
    if (temperature & 16) 
        codes[41]=CODE_high;
    else 
        codes[41]=CODE_low;
    
    if (temperature & 8)
         codes[43]=CODE_high;
    else 
        codes[43]=CODE_low;
    
    if (temperature & 4) 
        codes[45]=CODE_high;
    else 
        codes[45]=CODE_low;
    
    if (temperature & 2) 
        codes[47]=CODE_high;
    else 
        codes[47]=CODE_low;
    
    if (temperature & 1) 
        codes[49]=CODE_high;
    else 
        codes[49]=CODE_low;

    setChecksum();
}

/**
 * set the given mode and updates the codes
 * @param   _value  the new mode {MODE_auto,MODE_cold,MODE_warm,MODE_wind,MODE_rain}
 */
void Carrier::setMode(uint8_t _value)
{
    mode=_value;

    if (mode & 4) 
        codes[19]=CODE_high;
    else 
        codes[19]=CODE_low;
    
    if (mode & 2) 
        codes[21]=CODE_high;
    else 
        codes[21]=CODE_low;
    
    if (mode & 1) 
        codes[23]=CODE_high;
    else 
        codes[23]=CODE_low;

    setChecksum();
}

/**
 * set the given fan and updates the codes
 * @param   _value  the new fan speed {FAN_1,FAN_2,FAN_3,FAN_4}
 */
void Carrier::setFan(uint8_t _value)
{
    fan=_value;

    if (fan & 2) codes[25]=CODE_high;
    else codes[25]=CODE_low;
    if (fan & 1) codes[27]=CODE_high;
    else codes[27]=CODE_low;

    setChecksum();
}

/**
 * set the given air flow and updates the codes
 * @param   _value  the new air flow direction {AIRFLOW_dir_1,AIRFLOW_dir_2,AIRFLOW_dir_3,AIRFLOW_dir_4,AIRFLOW_dir_5,AIRFLOW_dir_6,AIRFLOW_change,AIRFLOW_open}
 */
void Carrier::setAirFlow(uint8_t _value)
{
    airFlow=_value;

    if (airFlow & 4) codes[29]=CODE_high;
    else codes[29]=CODE_low;
    if (airFlow & 2) codes[31]=CODE_high;
    else codes[31]=CODE_low;
    if (airFlow & 1) codes[33]=CODE_high;
    else codes[33]=CODE_low;

    setChecksum();
}

/**
 * set the given state and updates the codes
 * @param   _value  the new state {STATE_on,STATE_off}
 */
void Carrier::setState(uint8_t _value)
{
    state=_value;
    
    if (state & 1) codes[51]=CODE_high;
    else codes[51]=CODE_low;
    
    setChecksum();
}

//      //mode bits 19-23  // fan 25-27//flow 29-33//   //temp 39-49  state 51


uint8_t Carrier::getTemeprature()
{
    return temperature;
}

uint8_t Carrier::getMode()
{
    return mode;
}

uint8_t Carrier::getAirFlow()
{
    return airFlow;
}

uint8_t Carrier::getState()
{
    return state;
}

uint8_t Carrier::getFan()
{
    return fan;
}



/**
 * increments the counter, calculates the checksum and updates the codes
 */
void Carrier::setChecksum()
{
    // first do the counter part
    counter = (counter+1) % 4;
    if (counter & 2) 
        codes[7]=CODE_high;
    else 
        codes[7]=CODE_low;
    
    if (counter & 1)
         codes[9]=CODE_high;
    else 
        codes[9]=CODE_low;
    
    // next generate the checksum
    uint16_t crc = 0;
    
    for(uint8_t b=0;b<4;b++)
    {
        uint8_t block = 0;
        for(uint8_t i=3+b*16;i<=17+b*16;i=i+2)
        {
            if(codes[i] > CODE_threshold) 
                block++;
            if (i<=15+b*16) 
                block <<= 1;
        }
        crc = (crc + block) % 256;
    }
    
    if (crc & 128) codes[67]=CODE_high;
    else codes[67]=CODE_low;
    if (crc & 64) codes[69]=CODE_high;
    else codes[69]=CODE_low;
    if (crc & 32) codes[71]=CODE_high;
    else codes[71]=CODE_low;
    if (crc & 16) codes[73]=CODE_high;
    else codes[73]=CODE_low;
    if (crc & 8) codes[75]=CODE_high;
    else codes[75]=CODE_low;
    if (crc & 4) codes[77]=CODE_high;
    else codes[77]=CODE_low;
    if (crc & 2) codes[79]=CODE_high;
    else codes[79]=CODE_low;
    if (crc & 1) codes[81]=CODE_high;
    else codes[81]=CODE_low;
}

/**
 * initialiases a new code object
 */
Carrier::Carrier(uint8_t _mode, 
                 uint8_t _fan, 
                 uint8_t _airFlow,
                 uint8_t _temperature,
                 uint8_t _state)
{
    // init the codes with the fillers and default values
    for(uint8_t i=0;i<CARRIER_BUFFER_SIZE;i++)
    {
        if (i % 2 == 0)
            codes[i]=CODE_filler;
        else 
            codes[i]=CODE_low;
    }
    // set the two first ones
    codes[0] = CODE_first;
    codes[1] = CODE_second;
    // set the "always high" fields
    codes[3] = CODE_high;
    codes[5] = CODE_high;
    codes[53] = CODE_high;
    
    // init the counter
    counter = 0;

    // set fields
    setMode(_mode);
    setFan(_fan);
    setAirFlow(_airFlow);
    setTemeprature(_temperature);
    setState(_state);
}

/**
 * gives me whatever debug output I needed during development
 */
void Carrier::debug()
{
    setChecksum();
    
    for(uint8_t i=0;i<CARRIER_BUFFER_SIZE;i++)
    {
        if (i % 2 == 1)
        {
            /**/
            //Serial.print(i,DEC);
            //Serial.print(" = ");
            Serial.print(codes[i],DEC);
            //Serial.println("");
            /*
            if(codes[i]>CODE_threshold)
                Serial.print(1);
            else
                Serial.print(0);
            /**/
            Serial.print(",");
        }
    }
    //Serial.println("");
}   

/**
 * prints the actul state of the object on the serial line
 */
void Carrier::print()
{
    Serial.print("State = ");
    if (state==STATE_on) Serial.println("on");
    else Serial.println("off");
    
    Serial.print("Mode = ");
    if (mode==MODE_auto) Serial.println("auto");
    else if (mode==MODE_rain) Serial.println("rain");
    else if (mode==MODE_cold) Serial.println("cold");
    else if (mode==MODE_wind) Serial.println("wind");
    else Serial.println("warm");

    Serial.print("Temperature = ");
    Serial.println(temperature,DEC);
    
    Serial.print("Fan = ");
    Serial.println(fan,DEC);
    
    Serial.print("Air-flow = ");
    Serial.println(airFlow,DEC);
}
