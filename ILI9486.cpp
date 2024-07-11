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

ILI9486::ILI9486(Orientation orientation) {
	// Configure Arduino pins needed for communication
    pinMode(LCD_CS, OUTPUT);
	pinMode(LCD_BL, OUTPUT),
    pinMode(LCD_RST, OUTPUT);
    pinMode(LCD_DC, OUTPUT);
    pinMode(TP_CS, OUTPUT);
	pinMode(SD_CS, OUTPUT);

	// Is safe to call as SPI library has protection for calling begin() method multiple times
	SPI.begin();

	// Initialize SD card
	digitalWrite(SD_CS, 1);
	digitalWrite(LCD_CS, 1);
	digitalWrite(TP_CS, 1);
	SD.begin(SD_CS);

	this->reset();

	// Set backlight to 0 
	// (LCD turned off)
	this->setBacklight(0);

    this->writeRegister(0x11);
    delay(120);

    // Turn on the LCD display
    this->writeRegister(0x29);
}

void ILI9486::setBacklight(uint8_t value) {
	this->backlight = value;
	analogWrite(LCD_BL, this->backlight);
}

void ILI9486::initializeRegisters() {
	this->writeRegister(0XF9);
    this->writeData(0x00);
    this->writeData(0x08);

    this->writeRegister(0xC0);
    this->writeData(0x19);// VREG1OUT POSITIVE
    this->writeData(0x1a);// VREG2OUT NEGATIVE

    this->writeRegister(0xC1);
    this->writeData(0x45);// VGH,VGL    VGH>=14V.
    this->writeData(0x00);

    this->writeRegister(0xC2);	// Normal mode, increase can change the display quality, while increasing power consumption
    this->writeData(0xA0);

    this->writeRegister(0XC5);
    this->writeData(0x00);
    this->writeData(0x28);// VCM_REG[7:0]. <=0X80.

    this->writeRegister(0xB1);// Sets the frame frequency of full color normal mode
    this->writeData(0x60); // Modifing this value can solve flickering strips problem 
    this->writeData(0x11);

    this->writeRegister(0xB4);
    this->writeData(0x02); // 2 DOT FRAME MODE,F<=70HZ.

    this->writeRegister(0xB6);//
    this->writeData(0x00);
    this->writeData(0x42);//0 GS SS SM ISC[3:0];
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
    delay(500);
    digitalWrite(LCD_RST, 0);
    delay(500);
    digitalWrite(LCD_RST, 1);
    delay(500);
}

void ILI9486::writeRegister(uint8_t reg) {
	digitalWrite(LCD_DC, 0);
    digitalWrite(LCD_CS, 0);
    SPI.transfer(reg);
    digitalWrite(LCD_DC, 1);
}

void ILI9486::writeData(uint8_t data) {
	digitalWrite(LCD_DC, 1);
	digitalWrite(LCD_CS, 0);
	SPI.transfer(data >> 8);
	SPI.transfer(data & 0xFF);
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
    this->writeData(0X00);
    this->writeData(DisFunReg_Data);

    this->writeRegister(0x36);
    this->writeData(MemoryAccessReg_Data);
}