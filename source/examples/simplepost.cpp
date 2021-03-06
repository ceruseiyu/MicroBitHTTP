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

//Griffito post.php
//griffito.info/microbit/post.php
//bit.ly/2nKyGtT

MicroBit uBit;
MicroBitHTTPService* http;

int main() {
    // Initialise the micro:bit runtime.
    uBit.init();

    // Start the HTTP service
    http = new MicroBitHTTPService(*uBit.ble);

    // Set the URL to make the HTTP request to
    http->setURL("bit.ly/2nKyGtT");

    // Set the POST data to write with the request
    http->writePostData("data=Test");

    // Make the POST request
    uint8_t* data = http->requestHTTP(HTTP_POST, "data");

    // Convert the data to a string and display it
    ManagedString string = ManagedString((char*)data);
    uBit.display.scroll(string);
    // If main exits, there may still be other fibers running or registered event handlers etc.
    // Simply release this fiber, which will mean we enter the scheduler. Worse case, we then
    // sit in the idle task forever, in a power efficient sleep.
    release_fiber();
}
