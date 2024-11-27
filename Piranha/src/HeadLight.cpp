//
// HeadLight.h
// Управление фонарями
//

#include "HeadLight.h"
#include "Pinout.h"
#include <Arduino.h>
#include <esp32-hal-ledc.h>

// Частота обновления, Гц
#define REFRESH_FREQ         5000

// Период ШИМ, мкс
#define REFRESH_USEC         (1000000L / REFRESH_FREQ)

// Размерность счётчика таймера, бит
#define TIMER_WIDTH          8

// Количество тиков таймера
#define TIMER_WIDTH_TICKS    (1UL << TIMER_WIDTH)

// Номер шим-генератора
#define LED_PWM_INDEX        8 // 0-6 -- моторы

// Конструктор
// Pin: вывод, на котором осуществляется управление светом
// Активный уровень высокий!
HeadLight::HeadLight(uint8_t Pin) : Pin(Pin), Value(0)
{
	
}

// Инициализация периферии (выводов)
void HeadLight::Init()
{
	ledcSetup(LED_PWM_INDEX, REFRESH_FREQ, TIMER_WIDTH);
	ledcAttachPin(Pin, LED_PWM_INDEX); 
}

// Установка состояния фонаря
// Value: яркость
void HeadLight::setValue(uint8_t Value)
{
	//uint32_t Ticks = (Value * TIMER_WIDTH_TICKS) / 0xFF;
	this->Value = Value;
	
	ledcWrite(LED_PWM_INDEX, Value);	
}

// Получить состояние фонаря
// return: яркость
uint8_t HeadLight::getValue()
{
	return Value;
}
