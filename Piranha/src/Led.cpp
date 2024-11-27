//
// Led.h
// Управление светодиодами
//

#include "Led.h"
#include "Pinout.h"
#include <Arduino.h>

// Конструктор
// Pin: вывод, на котором осуществляется управление светом
// Активный уровень высокий!
Led::Led(uint8_t Pin) : Pin(Pin)
{
	
}

// Инициализация периферии (выводов)
void Led::Init()
{
	pinMode(Pin, OUTPUT);
	digitalWrite(Pin, LOW);
}

// Установка состояния фонаря или светодиода
// Value: яркость
void Led::setValue(bool Value)
{
	digitalWrite(Pin, Value ? HIGH : LOW);
}
