/*
example.cpp
Simple example usage of ILI9486 class.

Copyright (C) 2024 Mateusz Bogusławski, E: mateusz.boguslawski@ibnet.pl

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.
This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
You should have received a copy of the GNU General Public License
along with this program.  If not, see https://www.gnu.org/licenses/.

///////////////////////////////////////////////////////////////////
EXAMPLE SETUP:
Arduino <-> LCD
10 <-> CS
9(PWM pin) <-> BL
8 <-> RST
7 <-> DC

If you wish to change any of above pins, modify defines in ILI9486.h file.

Connect SPI pins from arduino to according pins in LCD
See: https://www.arduino.cc/reference/en/language/functions/communication/spi/
*/

#include "ILI9486.h"

void setup() {
	constexpr COLOR red = 0xF000;
	constexpr COLOR green = 0x0F00;
	constexpr COLOR blue = 0x00F0;

	// Orientation can be easily changed, see Orientation enum in ILI9486 class
	ILI9486 display(ILI9486::L2R_U2D);

	// Turn on backlight
	display.setDefaultBacklight();

	// Clear screen, and fill with blue color
	display.clear(blue);

	// Draw red square
	display.fill(10, 10, 100, 100, red);

	// Draw red green black square
	display.openWindow(150, 150, 300, 300);
	constexpr uint16_t a = (300 - 150);
	for (uint16_t i = 0; i < a; i++) {
		switch(i % 3) {
		case 0: 
			display.writeColor(red, a);
			break;
		case 1:
			display.writeColor(green, a);
			break;
		case 2:
			display.writeColor(BLACK, a);
			break;
		}
	}

	// Draw line
	for (int i = 0; i < 320; i++) {
		display.setPixel(i, i, WHITE);
	}

	// Draw circles
	display.drawCircle(50, 200, 49, WHITE, true);
	display.drawCircle(50, 200, 20, BLACK);
}

void loop() {

}
