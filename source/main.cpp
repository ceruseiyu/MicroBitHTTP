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

    /*
     * Single JSON field request
     */

    // Set the URL to make the HTTP request to
    http->setURL("bit.ly/2nKjgGg");

    // Make the HTTP request for the value of "field"
    uint8_t* singleFieldData = http->requestHTTP(HTTP_GET, "field");

    // Convert the data to a string and display it
    ManagedString singleFieldString = ManagedString((char*)singleFieldData);
    uBit.display.scroll(singleFieldString);

    fiber_wait_for_event(MICROBIT_ID_BUTTON_B, MICROBIT_BUTTON_EVT_CLICK);

    /*
     * Nested array request
     */

    http->setURL("bit.ly/2nKjgGg");

    // Retrieve the data from an array nested inisde an object
    uint8_t* nestedData = http->requestHTTP(HTTP_GET, "object.array[4]");

    // Convert the received data to a string and display it
    ManagedString nestedString = ManagedString((char*)nestedData);
    uBit.display.scroll(nestedString);

    fiber_wait_for_event(MICROBIT_ID_BUTTON_B, MICROBIT_BUTTON_EVT_CLICK);

    /*
     * POST request
     */

    http->setURL("bit.ly/2nKyGtT");

    // Set the POST data to write with the request
    http->writePostData("data=Test");

    // Make the POST request
    uint8_t* postData = http->requestHTTP(HTTP_POST, "data");

    // Convert the data to a string and display it
    ManagedString postString = ManagedString((char*)postData);
    uBit.display.scroll(postString);

    fiber_wait_for_event(MICROBIT_ID_BUTTON_B, MICROBIT_BUTTON_EVT_CLICK);

    /*
     * Weather macro request
     */

    //Set URL for Weather JSON API for london
    http->setURL("bit.ly/2lMpBAd");

    //Make request
    uint8_t* weatherData = http->requestMacroHTTP(WEATHER_MACRO_ID, "data");

    //Convert first byte into our temperature value from -64 to +64 degrees celsius
    int temperature = (int)weatherData[0] - 64;

    //Rest of the data is the string for the type of weather taking place
    ManagedString rawWeatherString = ManagedString((char*)weatherData);
    ManagedString weatherString = rawWeatherString.substring(1, rawWeatherString.length());

    //Display data
    uBit.display.scroll(weatherString);
    uBit.display.scroll(temperature);

    release_fiber();
}
