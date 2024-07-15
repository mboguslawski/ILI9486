/*
ILI9486.h
Class ILI9486 represent display with
ILI9486 driver used with Arduino board.

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

#pragma once

#include <Arduino.h>
#include <SPI.h>
#include <SD.h>

// Below configuration can be adjusted according to wiring
// Note that LCD_BL pin on Arduino must be capable of generating PWM output
#define ILI9486_CS 10
#define ILI9486_BL 9
#define ILI9486_RST 8
#define ILI9486_DC 7

// Dimensions of LCD panel in pixels
#define ILI9486_LONG_SIDE 480
#define ILI9486_SHORT_SIDE 320

#define ILI9486_COLOR uint16_t
#define ILI9486_BLACK 0x0000
#define ILI9486_WHITE 0xFFFF
#define ILI9486_RED 0xF000
#define ILI9486_GREEN 0x0F00
#define ILI9486_BLUE 0x00F0

class ILI9486 {
public:
	
	// Order in which GRAM is scanned
	enum Orientation {
		L2R_U2D, // The display interface is displayed left to right, up to down
		L2R_D2U,
		R2L_U2D,
		R2L_D2U,
		U2D_L2R,
		U2D_R2L,
		D2U_L2R,
		D2U_R2L
	};
	
	ILI9486(Orientation orientation, ILI9486_COLOR background = ILI9486_BLACK); // ILI9486 driver initialization, takes about 1 second to execute

	uint16_t getWidth();
	uint16_t getHeight();

	void setBacklight(uint8_t value); // Set LCD backlight value, from 0(min) to 255(max)
	void changeDefaultBacklight(uint8_t value); // Set LCD default backlight value, from 0(min) to 255(max), 255 after init
	void setDefaultBacklight(); // Set LCD brightness to default value
	void turnOffBacklight(); // Set LCD panel brightness to 0

	void changeBackground(ILI9486_COLOR color); // Change default color to display on clear screen

	void fill(uint16_t xStart, uint16_t yStart, uint16_t xEnd, uint16_t yEnd, ILI9486_COLOR color); // Fill area with given color
	void clear(ILI9486_COLOR color); // Fill entire screen with given color
	void clear(); // Fill entire screen with background color
	void openWindow(uint16_t xStart, uint16_t yStart, uint16_t xEnd, uint16_t yEnd); // Set display area
	void setCursor(uint16_t x, uint16_t y); // Set cursor to given position
	
	void writeColor(ILI9486_COLOR color, uint32_t n); // Write given colors n times
	void writeBuffer(ILI9486_COLOR *buffer, uint32_t n); // Write buffer to screen
	void setPixel(uint16_t x, uint16_t y, ILI9486_COLOR color); // Set cursor to given position and write color, slow due to setting cursor every pixel

	void drawCircle(uint16_t x, uint16_t y, uint16_t radius, ILI9486_COLOR color, bool filled = false); // Draw circle with center at (x, y) using Bresenham's Circle Algorithm
	void drawHLine(uint16_t x, uint16_t y, uint16_t len, ILI9486_COLOR color); // Draw horizontal line starting at point (x, y), incrementing x coordinate
	void drawVLine(uint16_t x, uint16_t y, uint16_t len, ILI9486_COLOR color); // Draw vertical line starting at point (x, y), incrementing y coordinate
	
private:
	void reset(); // Hardware reset
	void initializeRegisters(); // Write inital values to registers
	void writeRegister(uint8_t reg); // Write register address  
	void writeData(uint8_t data); // Write data to register
	void setScanOrder(Orientation orientation); // Set order in which GRAM is scanned

	uint8_t defaultBacklight; // LCD panel default brightness, 0 for turned off, 255 for maximum brightness
	uint16_t width; // [px]
	uint16_t height; // [px]
	ILI9486_COLOR background; // Default color to display on clear screen
};
