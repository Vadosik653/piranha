//
// Temperature.cpp
// Датчик температуры платы типа TMP3X
//

#include "Temperature.h"
#include "Pinout.h"
#include <Arduino.h>

// Инициализация периферии
void Temperature::Init()
{
	
}

// Прочитать температуру
// return: температура, С
int8_t TemperatureTMP36::getTemperature()
{
	Raw = analogRead(Temppin);
	Value = (Raw * 50 * 5 / 1024); // Формула нужнается в проверке и уточнении
	
	return Value;
}

// Прочитать температуру
// return: температура, С
int8_t TemperatureTMP37::getTemperature()
{
	uint16_t Raw = analogRead(Temppin);
	Value = (Raw * 50 * 5 / 1024); // Формула нужнается в проверке и уточнении
	
	return Value;
}
