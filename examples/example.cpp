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

---

EXAMPLE SETUP:
Arduino <-> LCD
10 <-> CS
9(PWM pin) <-> BL
8 <-> RST
7 <-> DC

If you wish to change any of above pins, modify defines in ILI9486.h file.

Connect SPI pins from arduino to according pins in LCD
See: https://www.arduino.cc/reference/en/language/functions/communication/spi/

---

Expected behaviour:
Four lines, two squares and circle with smaller circles inside should be visible on screen.
Screen should go smoothly from maximum brightness to minimum brightness and back again in indefinite loop.
*/

#include "ILI9486.h"

ILI9486 *display;

void setup() {
	// Orientation can be easily changed, see Orientation enum in ILI9486 class
	display = new ILI9486(ILI9486::L2R_U2D);

	// Turn on backlight
	display->setDefaultBacklight();

	// Clear screen, and fill with blue color
	display->clear(ILI9486_BLUE);

	// Draw red square
	display->fill(10, 10, 100, 100, ILI9486_RED);

	// Draw red green black square
	display->openWindow(150, 150, 300, 300);
	constexpr uint16_t a = (300 - 150);
	for (uint16_t i = 0; i < a; i++) {
		switch(i % 3) {
		case 0: 
			display->writeColor(ILI9486_RED, a);
			break;
		case 1:
			display->writeColor(ILI9486_GREEN, a);
			break;
		case 2:
			display->writeColor(ILI9486_BLACK, a);
			break;
		}
	}

	// Draw lines
	display->drawLine(0, 0, display->getWidth(), display->getWidth(), ILI9486_WHITE);
	display->drawHLine(0, 10, display->getWidth(), ILI9486_WHITE);
	display->drawVLine(10, 0, display->getHeight(), ILI9486_WHITE);
	display->drawLine(display->getWidth(), 10, 10, display->getHeight(), ILI9486_WHITE);


	// Draw circles
	display->drawCircle(70, 225, 49, ILI9486_WHITE, true);
	display->drawCircle(70, 225, 20, ILI9486_BLACK, true);
	
	// Same as
	// display.drawCircle(70, 225, 30, ILI9486_BLACK, false);
	display->drawCircle(70, 225, 30, ILI9486_BLACK);
	
	display->drawCircle(70, 225, 10, ILI9486_GREEN, true);
}

void loop() {
	// Go smoothly from maximum brightness to minimum brightness
	for (int i = 255; i > -1; i--) {
		display->setBacklight(i);
		delay(10);
	}

	// Go smoothly from minimum brightness to maximum brightness
	for (int i = 0; i < 255; i++) {
		display->setBacklight(i);
		delay(10);
	}
}
