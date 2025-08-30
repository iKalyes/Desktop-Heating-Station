#ifndef _FLASH_H_
#define _FLASH_H_ 

#include <Arduino.h>
#include <variables.h>
#include <EEPROM.h>

void WriteFlash();
void WriteHeater();
void WriteDisplayType();
void WritePID();
void ReadFlash();
void ClearFlash();

#endif