//
// Pump.cpp
// Управление насосом
//

#include "Pump.h"
#include "Pinout.h"
#include <Arduino.h>

// Направление насоса
bool OutputDirection = false;

// Инициализация периферии и выводов
void Pump::Init()
{
	pinMode(Pumppin1, OUTPUT);
	pinMode(Pumppin2, OUTPUT);
	digitalWrite(Pumppin1, LOW);
	digitalWrite(Pumppin2, LOW);
	
	OutputDirection = false;
}

// Задать состояние
void Pump::setState(bool Enabled)
{
	if(Enabled)
	{
		digitalWrite(Pumppin1, OutputDirection ? HIGH : LOW);
		digitalWrite(Pumppin2, OutputDirection ? LOW : HIGH);
	}
	else
	{
		digitalWrite(Pumppin1, LOW);
		digitalWrite(Pumppin2, LOW);
	}
}

// Задать направление
// Output: выкачиваем
void Pump::setDirection(bool Output)
{
	OutputDirection = Output;
}
