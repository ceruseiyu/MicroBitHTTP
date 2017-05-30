/*
The MIT License (MIT)

Copyright (c) 2016 British Broadcasting Corporation.
This software is provided by Lancaster University by arrangement with the BBC.

Permission is hereby granted, free of charge, to any person obtaining a
copy of this software and associated documentation files (the "Software"),
to deal in the Software without restriction, including without limitation
the rights to use, copy, modify, merge, publish, distribute, sublicense,
and/or sell copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
DEALINGS IN THE SOFTWARE.
*/

#include "MicroBit.h"

//Weather API JSON:
//bit.ly/2lMpBAd

//ID for our macro on the server side
#define WEATHER_MACRO_ID 0x01


MicroBit uBit;
MicroBitHTTPService* http;

int main() {
    uBit.init();
    //Start HTTP Service
    http = new MicroBitHTTPService(*uBit.ble);

    //Set URL for Weather JSON API for london
    http->setURL("bit.ly/2lMpBAd");


    uint8_t* data = http->requestMacroHTTP(WEATHER_MACRO_ID, "data");

    http->setURL("test.com");

    //Convert first byte into our temperature value from -64 to +64 degrees celsius
    int temperature = (int)data[0] - 64;

    //Rest of the data is the string for the type of weather taking place
    ManagedString rawString = ManagedString((char*)data);
    ManagedString weatherString = rawString.substring(1, rawString.length());

    //Display data
    uBit.display.scroll(weatherString);
    uBit.display.scroll(temperature);
    release_fiber();
}
