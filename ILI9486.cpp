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

ILI9486::ILI9486(Orientation orientation, COLOR background):
        defaultBacklight(UINT8_MAX),
    background(background)
{
	// Configure Arduino pins needed for communication
    pinMode(LCD_CS, OUTPUT);
	pinMode(LCD_BL, OUTPUT),
    pinMode(LCD_RST, OUTPUT);
    pinMode(LCD_DC, OUTPUT);

	// Is safe to call as SPI library has protection for calling begin() method multiple times
	SPI.setDataMode(SPI_MODE0);
    SPI.setBitOrder(MSBFIRST);
    SPI.setClockDivider(SPI_CLOCK_DIV2);
    SPI.begin();

	this->reset();
	this->turnOffBacklight();
    this->initializeRegisters();

    this->setScanOrder(orientation);
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
    return this->width();
}

void ILI9486::setBacklight(uint8_t value) {
	analogWrite(LCD_BL, value);
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

void ILI9486::changeBackground(COLOR color) {
    this->background = color;
}

void ILI9486::fill(uint16_t xStart, uint16_t yStart, uint16_t xEnd, uint16_t yEnd, COLOR color) {
    uint32_t size = (uint32_t)(xEnd - xStart) * (uint32_t)(yEnd - yStart);

    this->openWindow(xStart, yStart, xEnd, yEnd);
    this->writeColor(color, size);
}

void ILI9486::clear(COLOR color) {
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

void ILI9486::writeColor(COLOR color, uint32_t n) {
    digitalWrite(LCD_DC, 1);
    digitalWrite(LCD_CS, 0);

    for (uint32_t i = 0; i < n; i++) {
        SPI.transfer16(color);
    }

    digitalWrite(LCD_CS, 1);
}

void ILI9486::writeBuffer(COLOR *buffer, uint32_t n) {
    digitalWrite(LCD_DC, 1);
    digitalWrite(LCD_CS, 0);

    for (uint32_t i = 0; i < n; i++) {
        SPI.transfer16(buffer[i]);
    }

    digitalWrite(LCD_CS, 1);
}

void ILI9486::setPixel(uint16_t x, uint16_t y, COLOR color) {
    this->setCursor(x, y);
    this->writeColor(color, 1);
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
	digitalWrite(LCD_RST, 1);
    delay(100);
    digitalWrite(LCD_RST, 0);
    delay(100);
    digitalWrite(LCD_RST, 1);
    delay(100);
}

void ILI9486::writeRegister(uint8_t reg) {
	digitalWrite(LCD_DC, 0);
    digitalWrite(LCD_CS, 0);
    SPI.transfer(reg);
    digitalWrite(LCD_CS, 1);
}

void ILI9486::writeData(uint8_t data) {
	digitalWrite(LCD_DC, 1);
	digitalWrite(LCD_CS, 0);
	SPI.transfer16(data);
	digitalWrite(LCD_CS, 1);
}

void ILI9486::setScanOrder(Orientation orientation) {
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
        this->width = SHORT_SIDE;
        this->height = LONG_SIDE;
    } else {
        this->width = LONG_SIDE;
        this->height = SHORT_SIDE;
    }

    // Set the read / write scan direction of the frame memory
    this->writeRegister(0xB6);
    this->writeData(0x00);
    this->writeData(DisFunReg_Data);

    this->writeRegister(0x36);
    this->writeData(MemoryAccessReg_Data);
}
