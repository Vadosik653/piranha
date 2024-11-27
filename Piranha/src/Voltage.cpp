//
// Voltage.cpp
// Измерение напряжений на плате
//

#include "Voltage.h"
#include "Pinout.h"
#include <Arduino.h>

// Опорное напряжения дл Arduino Nano: 5В
const float RefVolts = 3.3f;
// Вычисляется как 4095.0*(R2/(R1 + R2)), где R1=3600 Ом, а R2=1000 Ом для максимального измеряемого напряжения в 15В
const float ResistFactor = 842.7f;

// Инициализация периферии и выводов
void Voltage::Init()
{
	
}

// Прочитать напряжение батареи
// return: напряжение, выраженное в 0.1В (т.е. 12В = 120)
uint16_t Voltage::getVoltage()
{
	float raw = analogRead(Voltpin);
	uint16_t value = raw * RefVolts * 10 / ResistFactor;
	
	return value;
}
