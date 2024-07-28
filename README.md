# ILI9486 class
Represent display with ILI9486 driver used with Arduino board. \
Suitable for elegant style coding due to class implementation.
___

## Getting Started

Example showing simple usage of this this class is available in `examples/` directory.
For further help read below documentation.
- #### Class constructor
> ILI9486(uint8_t CS, uint8_t BL, uint8_t RST, uint8_t DC, Orientation orientation, uint8_t defaultBacklight, ILI9486_COLOR background = ILI9486_BLACK)

Takes arduino pins numbers connected to ILI9486, orientation of screen and default background color, which will be displayed after initialization.

Inside constructor ILI9486 driver registers are initialized with initial values and `SPI.begin()` is called to start SPI communication.
Initializing takes about 1 second.

- #### Getting dimensions
> uint16_t getWidth(); \
    uint16_t getHeight();

Note that dimensions can vary depending on screen orientation, so use above methods rather then constant values.

> uint32_t getSize()

Use above method to get number of pixels (width x height)

- #### Setting backlight
> void setBacklight(uint8_t value)

Set backlight (screen brightness) to desired value using Arduino PWM pin.
Values range from 0 to 255.

> void turnOffBacklight()

Set backlight to 0. Screen would seem to be turned off.

> uint16_t getDefaultBacklight()

Get current default backlight value

- #### Default backlight feature
Default backlight value can be used to return to chosen brightness for example after setting backlight to 0.
> void changeDefaultBacklight(uint8_t value)

Use above method to change default backlight value, after initialization default backlight is set to 255 (maximum value).

> void setDefaultBacklight()

Use above method to set brightness to chosen default value.

- #### Clearing screen
> void clear()

Above method clears screen by filling entire screen with background color passed in constructor or changed by `void changeBackground(ILI9486_COLOR color)` method.

> void clear(ILI9486_COLOR color);

Above method can be used if you wish to fill screen with other color without changing default background color.

- #### Writing text
After program upload fonts are stored in Arduino FLASH memory to save up RAM memory.
Five font sizes are available, they can be passed to methods using `ILI9486::FontSize` enum. Unused font sizes will not be loaded int Arduino FLASH memory, which is useful if your project uses Arduino with small amount of RAM.

> void drawChar(uint16_t x, uint16_t y, uint8_t character, FontSize size, ILI9486_COLOR color) \
void drawString(uint16_t x, uint16_t y, const uint8_t *str, FontSize size, ILI9486_COLOR color)

Text display is done with two above methods. note that (x, y) position is position of the center of character (or center of the first character). String passed in `drawString` method must be terminated with `'\0'` character (strings passed as const char[] such as `"example"` are terminated with `'\0'`).

- #### Drawing shapes
> void drawCircle(uint16_t x, uint16_t y, uint16_t radius, ILI9486_COLOR color, bool filled = false)

Above method prints circle with center in (x, y) point, circled can be filled by passing true as last argument.

> void fill(uint16_t xStart, uint16_t yStart, uint16_t xEnd, uint16_t yEnd, ILI9486_COLOR color)

Use above method for drawing rectangles. Unstable for drawing lines (thickness one). For those see other methods listed below.

> void drawHLine(uint16_t x, uint16_t y, uint16_t len, ILI9486_COLOR color) \
	void drawVLine(uint16_t x, uint16_t y, uint16_t len, ILI9486_COLOR color)

Above methods draw straight line starting from (x, y) point and incrementing x coordinate in horizontal line or y coordinate in vertical lines.

> void drawLine(uint16_t xStart, uint16_t yStart, uint16_t xEnd, uint16_t yEnd, ILI9486_COLOR color)

Above method is used to draw line between any two points (might be not straight).

- #### Changing orientation
>void setOrientation(Orientation orientation)

Use above method to change the order in which GRAM is scanned

- #### Custom drawing on screen
> void setPixel(uint16_t x, uint16_t y, ILI9486_COLOR color)

Above method sets desired pixel color. This method is slow, so for writing buffers (eg. uploading image) see methods listed below.

> void setCursor(uint16_t x, uint16_t y)

Above method set cursor to desired position cursor is moved automatically after data write in direction depending on orientation.

> void openWindow(uint16_t xStart, uint16_t yStart, uint16_t xEnd, uint16_t yEnd)

Above method creates display area, useful if you wish to draw only on some (rectangular) part of the screen.

> void writeColor(ILI9486_COLOR color, uint32_t n)

Above method writes color to the screen n times, starting from the point set by `setCursor` or `openWindow` methods.

> void writeBuffer(ILI9486_COLOR *buffer, uint32_t n)

Above method writes buffer, starting from the point set by `setCursor` or `openWindow` methods.
___
### Supported hardware
This class was developed using
- Arduino Pro Mini 5V (https://docs.arduino.cc/retired/boards/arduino-pro-mini/)
- LCD screen base on ILI9486 driver (https://www.waveshare.com/4inch-tft-touch-shield.htm)

Should work with other Arduino compatible boards, which support SPI interface, and at least one PWM pin.
___
### Author
#### Mateusz Bogusławski (mateusz.boguslawski@ibnet.pl) 
___

Based on code available at https://www.waveshare.com/wiki/4inch_TFT_Touch_Shield \
Fonts author: MCD Application Team 