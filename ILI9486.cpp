/*
ILI9486.cpp
Implementation of ILI9486 class.

Copyright (C) 2024 Mateusz Bogusławski, E: mateusz.boguslawski@ibnet.pl

Based on code found at:
https://www.waveshare.com/wiki/4inch_TFT_Touch_Shield 

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
*/

#include "ILI9486.h"

ILI9486::ILI9486(Orientation orientation, uint8_t defaultBacklight, ILI9486_COLOR background):
	defaultBacklight(defaultBacklight),
	background(background)
{
	// Configure Arduino pins needed for communication
	pinMode(ILI9486_CS, OUTPUT);
	pinMode(ILI9486_BL, OUTPUT),
	pinMode(ILI9486_RST, OUTPUT);
	pinMode(ILI9486_DC, OUTPUT);

	// Initialize SPI communication
	SPI.begin();

	this->reset();
	this->turnOffBacklight();
	this->initializeRegisters();

	this->setOrientation(orientation);
	delay(200);

	this->writeRegister(0x11);
	delay(120);

	// Turn on the LCD display
	this->writeRegister(0x29);
	this->clear();
	this->setDefaultBacklight();
}

uint16_t ILI9486::getHeight() {
	return this->height;
}

uint16_t ILI9486::getWidth() {
	return this->width;
}

uint32_t ILI9486::getSize() {
	return (uint32_t)this->getHeight() * (uint32_t)this->getWidth();
}

void ILI9486::setBacklight(uint8_t value) {
	analogWrite(ILI9486_BL, value);
}

void ILI9486::changeDefaultBacklight(uint8_t value) {
	this->defaultBacklight = value;
}

void ILI9486::setDefaultBacklight() {
	this->setBacklight(this->defaultBacklight);
}

void ILI9486::turnOffBacklight() {
	this->setBacklight(0);
}

void ILI9486::changeBackground(ILI9486_COLOR color) {
	this->background = color;
}

void ILI9486::fill(uint16_t xStart, uint16_t yStart, uint16_t xEnd, uint16_t yEnd, ILI9486_COLOR color) {
	// Number of pixels inside rectangle
	uint32_t size = (uint32_t)(xEnd - xStart) * (uint32_t)(yEnd - yStart);

	// Set display area and write color
	this->openWindow(xStart, yStart, xEnd, yEnd);
	this->writeColor(color, size);
}

void ILI9486::clear(ILI9486_COLOR color) {
	fill(0, 0, this->width, this->height, color);
}

void ILI9486::clear() {
	clear(this->background);
}

void ILI9486::openWindow(uint16_t xStart, uint16_t yStart, uint16_t xEnd, uint16_t yEnd) {
	// Ensure that coordinates are given in correct order
	if (xStart > xEnd) {
		uint16_t tmp = xStart;
		xStart = xEnd;
		xEnd = tmp;
	}

	if (yStart > yEnd) {
		uint16_t tmp = yStart;
		yStart = yEnd;
		yEnd = tmp;
	}

	// Set the X coordinates
	this->writeRegister(0x2A);
	this->writeData(xStart >> 8); // Set the horizontal starting point to the high octet
	this->writeData(xStart & 0xff);	// Set the horizontal starting point to the low octet
	this->writeData((xEnd - 1) >> 8); // Set the horizontal end to the high octet
	this->writeData((xEnd - 1) & 0xff); // Set the horizontal end to the low octet

	// Set the Y coordinates
	this->writeRegister(0x2B);
	this->writeData(yStart >> 8);
	this->writeData(yStart & 0xff );
	this->writeData((yEnd - 1) >> 8);
	this->writeData((yEnd - 1) & 0xff);
	
	this->writeRegister(0x2C);
}

void ILI9486::setCursor(uint16_t x, uint16_t y) {
	this->openWindow(x, y, x, y);
}

void ILI9486::writeColor(ILI9486_COLOR color, uint32_t n) {
	digitalWrite(ILI9486_DC, 1);
	digitalWrite(ILI9486_CS, 0);

	for (uint32_t i = 0; i < n; i++) {
		SPI.transfer16(color);
	}

	digitalWrite(ILI9486_CS, 1);
}

void ILI9486::writeBuffer(ILI9486_COLOR *buffer, uint32_t n) {
	digitalWrite(ILI9486_DC, 1);
	digitalWrite(ILI9486_CS, 0);

	for (uint32_t i = 0; i < n; i++) {
		SPI.transfer16(buffer[i]);
	}

	digitalWrite(ILI9486_CS, 1);
}

void ILI9486::setPixel(uint16_t x, uint16_t y, ILI9486_COLOR color) {
	this->setCursor(x, y);
	this->writeColor(color, 1);
}

void ILI9486::drawCircle(uint16_t x, uint16_t y, uint16_t radius, ILI9486_COLOR color, bool filled) {
	// Bresenham's Circle Algorithm
	// See: https://www.javatpoint.com/computer-graphics-bresenhams-circle-algorithm
	int32_t r = (uint32_t)radius;
	int32_t p = 0;
	int32_t q = r; 
	int32_t d = 3 - (2*r);
	int32_t x0 = (int32_t)x;
	int32_t y0 = (int32_t)y;

	while (p <= q) {
		if (d <= 0) {
			d = d + (4*p) + 6;
			p++;
		} else {
			q--;
			d = d + 4*(p - q) + 10;
			p++;
		}

		if (!filled) {
			// Use 8 point symmetry 
			this->setPixel(x0 + p, y0 + q, color);
			this->setPixel(x0 - p, y0 + q, color);
			this->setPixel(x0 + p, y0 - q, color);
			this->setPixel(x0 - p, y0 - q, color);
			this->setPixel(x0 - q, y0 + p, color);
			this->setPixel(x0 + q, y0 + p, color);
			this->setPixel(x0 - q, y0 - p, color);
			this->setPixel(x0 + q, y0 - p, color);
		} else {
			this->drawHLine(x0 - p, y0 + q, 2*p, color);
			this->drawHLine(x0 - p, y0 - q, 2*p, color);
			this->drawHLine(x0 - q, y0 + p, 2*q, color);
			this->drawHLine(x0 - q, y0 - p, 2*q, color);
		}
	}
	
	// Add 4 missing points
	if (!filled) {
		this->setPixel(x0, y0 + r, color);
		this->setPixel(x0, y0 - r, color);
		this->setPixel(x0 + r, y0, color);
		this->setPixel(x0 - r, y0, color);
	} else {
		this->drawVLine(x0, y0 - r, 2*r, color);
		this->drawHLine(x0 - r, y0, 2*r, color);
	}
}

void ILI9486::drawHLine(uint16_t x, uint16_t y, uint16_t len, ILI9486_COLOR color) {
	this->fill(x, y, x + len, y + 1, color);
}

void ILI9486::drawVLine(uint16_t x, uint16_t y, uint16_t len, ILI9486_COLOR color) {
	this->fill(x, y, x + 1, y + len, color);
}

void ILI9486::drawLine(uint16_t xStart, uint16_t yStart, uint16_t xEnd, uint16_t yEnd, ILI9486_COLOR color) {
	// Bresenham's Line Algorithm
	// See: https://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm
	int32_t dx = abs((int32_t)xEnd - (int32_t)xStart);
	int32_t dy = -1 * abs((int32_t)yEnd - (int32_t)yStart);

	int32_t sx = (xEnd > xStart) ? 1 : -1;
	int32_t sy = (yEnd > yStart) ? 1 : -1;

	int32_t error = dx + dy;
	
	int32_t x0 = (int32_t)xStart;
	int32_t y0 = (int32_t)yStart;

	while ( (x0 != xEnd) || (y0 != yEnd) ) {
		this->setPixel(x0, y0, color);

		int32_t e2 = error * 2;

		if (e2 >= dy) {
			if (x0 == xEnd) { break; }

			error += dy;
			x0 += sx;
		} 
		
		if (e2 <= dx) {
			if (y0 == yEnd) { break; }

			error += dx;
			y0 += sy;
		}
	}
}

void ILI9486::drawChar(uint16_t x, uint16_t y, uint8_t character, FontSize size, ILI9486_COLOR color) {
	// Load correct font size
	sFONT *font;
	switch(size) {
		case XS: font = &Font8; break;
		case S: font = &Font12; break;
		case M: font = &Font16; break;
		case L: font = &Font20; break;
		case XL: font = &Font24; break;
	}
	
	// Modify position to top left corner of character
	x -= (font->Width / 2);
	y += (font->Height / 2);

	// Calculate character position in memory
	uint32_t pos = uint32_t(character - ' ') * font->Height;
	// Some fonts have use more then 8 bits for one pixel line
	pos *= ( font->Width / 8 + ((font->Width % 8) ? 1 : 0) );

	for (uint16_t i = 0; i < font->Height; i++) {
		for (uint16_t j = 0; j < font->Width; j++) {
			// Some fonts use more than 8 bits for one pixel line
			if ( (j % 8 == 0) && (j != 0) ) { pos++; }

			// Font is saved in FLASH memory
			if (pgm_read_byte(&font->table[pos]) & (0x80 >> (j % 8))) {
				this->setPixel(x + j, y - i, color);
			}
		}

		pos++;
	}
}

void ILI9486::drawString(uint16_t x, uint16_t y, const uint8_t *str, FontSize size, ILI9486_COLOR color) {
	for (uint16_t i = 0; str[i] != '\0'; i++) {
		this->drawChar(x, y, str[i], size, color);

		// Move x for next letter depending on font size
		switch(size) {
			case XS: x += Font8.Width; break;
			case S: x += Font12.Width; break;
			case M: x += Font16.Width; break;
			case L: x += Font20.Width; break;
			case XL: x += Font24.Width; break;
		}	
	}
}

void ILI9486::initializeRegisters() {
	this->writeRegister(0XF9);
	this->writeData(0x00);
	this->writeData(0x08);

	this->writeRegister(0xC0);
	this->writeData(0x19); // VREG1OUT POSITIVE
	this->writeData(0x1a); // VREG2OUT NEGATIVE

	this->writeRegister(0xC1);
	this->writeData(0x45); // VGH,VGL    VGH>=14V.
	this->writeData(0x00);

	this->writeRegister(0xC2); // Normal mode, increase can change the display quality, while increasing power consumption
	this->writeData(0x33);

	this->writeRegister(0XC5);
	this->writeData(0x00);
	this->writeData(0x28); // VCM_REG[7:0]. <=0X80.

	this->writeRegister(0xB1);// Sets the frame frequency of full color normal mode
	this->writeData(0x60); // Modifing this value can solve flickering strips problem 
	this->writeData(0x11);

	this->writeRegister(0xB4);
	this->writeData(0x02); // 2 DOT FRAME MODE,F<=70HZ.

	this->writeRegister(0xB6);//
	this->writeData(0x00);
	this->writeData(0x42); //0 GS SS SM ISC[3:0];
	this->writeData(0x3B);

	this->writeRegister(0xB7);
	this->writeData(0x07);

	this->writeRegister(0xE0);
	this->writeData(0x1F);
	this->writeData(0x25);
	this->writeData(0x22);
	this->writeData(0x0B);
	this->writeData(0x06);
	this->writeData(0x0A);
	this->writeData(0x4E);
	this->writeData(0xC6);
	this->writeData(0x39);
	this->writeData(0x00);
	this->writeData(0x00);
	this->writeData(0x00);
	this->writeData(0x00);
	this->writeData(0x00);
	this->writeData(0x00);

	this->writeRegister(0XE1);
	this->writeData(0x1F);
	this->writeData(0x3F);
	this->writeData(0x3F);
	this->writeData(0x0F);
	this->writeData(0x1F);
	this->writeData(0x0F);
	this->writeData(0x46);
	this->writeData(0x49);
	this->writeData(0x31);
	this->writeData(0x05);
	this->writeData(0x09);
	this->writeData(0x03);
	this->writeData(0x1C);
	this->writeData(0x1A);
	this->writeData(0x00);

	this->writeRegister(0XF1);
	this->writeData(0x36);
	this->writeData(0x04);
	this->writeData(0x00);
	this->writeData(0x3C);
	this->writeData(0x0F);
	this->writeData(0x0F);
	this->writeData(0xA4);
	this->writeData(0x02);

	this->writeRegister(0XF2);
	this->writeData(0x18);
	this->writeData(0xA3);
	this->writeData(0x12);
	this->writeData(0x02);
	this->writeData(0x32);
	this->writeData(0x12);
	this->writeData(0xFF);
	this->writeData(0x32);
	this->writeData(0x00);

	this->writeRegister(0XF4);
	this->writeData(0x40);
	this->writeData(0x00);
	this->writeData(0x08);
	this->writeData(0x91);
	this->writeData(0x04);

	this->writeRegister(0XF8);
	this->writeData(0x21);
	this->writeData(0x04);

	this->writeRegister(0X3A);	// Set Interface Pixel Format
	this->writeData(0x55);
}

void ILI9486::reset() {
	digitalWrite(ILI9486_RST, 1);
	delay(100);
	digitalWrite(ILI9486_RST, 0);
	delay(100);
	digitalWrite(ILI9486_RST, 1);
	delay(100);
}

void ILI9486::writeRegister(uint8_t reg) {
	digitalWrite(ILI9486_DC, 0);
	digitalWrite(ILI9486_CS, 0);
	SPI.transfer(reg);
	digitalWrite(ILI9486_CS, 1);
}

void ILI9486::writeData(uint8_t data) {
	digitalWrite(ILI9486_DC, 1);
	digitalWrite(ILI9486_CS, 0);
	SPI.transfer16(data);
	digitalWrite(ILI9486_CS, 1);
}

void ILI9486::setOrientation(Orientation orientation) {
	uint16_t MemoryAccessReg_Data = 0; //addr:0x36
	uint16_t DisFunReg_Data = 0; //addr:0xB6

	// Gets the scan direction of GRAM
	switch (orientation) {
	case L2R_U2D:
		MemoryAccessReg_Data = 0x08; // 0x08 | 0X8
		DisFunReg_Data = 0x22;
		break;
	case L2R_D2U:
		MemoryAccessReg_Data = 0x08;
		DisFunReg_Data = 0x62;
		break;
	case R2L_U2D: // 0X4
		MemoryAccessReg_Data = 0x08;
		DisFunReg_Data = 0x02;
		break;
	case R2L_D2U: // 0XC
		MemoryAccessReg_Data = 0x08;
		DisFunReg_Data = 0x42;
		break;
	case U2D_L2R: // 0X2
		MemoryAccessReg_Data = 0x28;
		DisFunReg_Data = 0x22;
		break;
	case U2D_R2L: // 0X6
		MemoryAccessReg_Data = 0x28;
		DisFunReg_Data = 0x02;
		break;
	case D2U_L2R: // 0XA
		MemoryAccessReg_Data = 0x28;
		DisFunReg_Data = 0x62;
		break;
	case D2U_R2L: // 0XE
		MemoryAccessReg_Data = 0x28;
		DisFunReg_Data = 0x42;
		break;
	}

	// Get GRAM and LCD width and height
	if (orientation == L2R_U2D || orientation == L2R_D2U || orientation == R2L_U2D || orientation == R2L_D2U) {
		this->width = ILI9486_SHORT_SIDE;
		this->height = ILI9486_LONG_SIDE;
	} else {
		this->width = ILI9486_LONG_SIDE;
		this->height = ILI9486_SHORT_SIDE;
	}

	// Set the read / write scan direction of the frame memory
	this->writeRegister(0xB6);
	this->writeData(0x00);
	this->writeData(DisFunReg_Data);

	this->writeRegister(0x36);
	this->writeData(MemoryAccessReg_Data);
}
