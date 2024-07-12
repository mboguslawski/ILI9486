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
#define LCD_CS 10
#define LCD_BL 9
#define LCD_RST 8
#define LCD_DC 7
#define TP_CS 4

// Dimensions of LCD panel in pixels
#define LONG_SIDE 480
#define SHORT_SIDE 320

#define COLOR uint16_t
#define BLACK 0x0000
#define WHITE 0xFFFF

class ILI9486 {
public:
	
	// Order in which GRAM is scanned
	enum Orientation {
		L2R_U2D,	// The display interface is displayed left to right, up to down
		L2R_D2U,
		R2L_U2D,
		R2L_D2U,
		U2D_L2R,
		U2D_R2L,
		D2U_L2R,
		D2U_R2L
	};
	
	ILI9486(Orientation orientation, COLOR background = BLACK); // ILI9486 driver initialization, takes about 1 second to execute

	void setBacklight(uint8_t value); // Set LCD backlight value, from 0(min) to 255(max)
	void changeDefaultBacklight(uint8_t value); // Set LCD default backlight value, from 0(min) to 255(max), 255 after init
	void setDefaultBacklight(); // Set LCD brightness to default value
	void turnOffBacklight(); // Set LCD panel brightness to 0

	void changeBackground(COLOR color); // Change default color to display on clear screen

	void fill(uint16_t xStart, uint16_t yStart, uint16_t xEnd, uint16_t yEnd, COLOR color); // Fill area with given color
	void clear(COLOR color); // Fill entire screen with given color
	void clear(); // Fill entire screen with background color
	void openWindow(uint16_t xStart, uint16_t yStart, uint16_t xEnd, uint16_t yEnd); // Set display area
	void setCursor(uint16_t x, uint16_t y); // Set cursor to given position
	
	void writeColor(COLOR color, uint32_t n); // Write given colors n times
	void writeBuffer(COLOR *buffer, uint32_t n); // Write buffer to screen
	void setPixel(uint16_t x, uint16_t y, COLOR color); // Set cursor to given position and write color

private:
	void reset(); // Hardware reset
	void initializeRegisters(); // Write inital values to registers
	void writeRegister(uint8_t reg); // Write register address  
	void writeData(uint8_t data); // Write data to register
	void setScanOrder(Orientation orientation); // Set order in which GRAM is scanned

	uint8_t defaultBacklight; // LCD panel default brightness, 0 for turned off, 255 for maximum brightness
	uint16_t width; // [px]
	uint16_t height; // [px]
	COLOR background; // Default color to display on clear screen
};
