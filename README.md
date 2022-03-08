# epd_demo
Super simple example to push pixels to waveshare epd display

## Usage
See main/main/epd_demo.c for main usage

In short: feclare an `epd_t` and at least set the `.pins` to match where you want to
wire things up. The values there now work for me.


* `createEpd()` to set things up, this does all the esp32-specific setup, but
   doesnt talk to the display itself.
* `initEpd()` does a reset of the display and wait for it to be ready.


Then, each time you want to update the display:

* `epdBegin()` gets the display into a state where it is ready to receive pixel
  data
* `epdPush()` call repeatedly in a loop until exactly 600 * 448 / 2 = 134400
  bytes have been transferred. If you have the whole buffer at once then this
  might be just once, with all the data. I like to do it line-by line to keep
  peak memory requirments down. Or you can push to the display as each chunk of
  a download completes, etc.
  Note: you MUST send the correct number of bytes. I have no idea what happens
  if you send too little or to much.
* `epdEnd()` This finialises the display, beginning the update and waiting for
  it to happen. Note: this may take many seconds (say, upto 30)

## Data format
The buffer you push is 4-bits per pixel, packed tightly into neighbouring
nybbles of a byte. So for the a 600 * 448 display we send half that many bytes.

0x0: Black
0x1: White
0x2: Green
0x3: Blue
0x4: Red
0x5: Yellow
0x6: Orange
0x7: not defined, and priobably best avoided!

Internally, `epdClear()` just sends 0x11 bytes to make it all white.
