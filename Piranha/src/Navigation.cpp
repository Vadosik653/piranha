//
// Navigation.cpp
// Навигация (гироскоп/акселерометр)
//

#include "Navigation.h"
#include <Arduino.h>

// Конструктор
Navigation::Navigation() : available(false)
{
	
}

// Назначить, доступно ли устройство
void Navigation::setAvailable(bool avail)
{
	available = avail;
}

// Доступна ли
bool Navigation::isAvailable()
{
	return available;
}

// Откалибровать устройство
void Navigation::Calibrate()
{
	Serial.println("No calibration");
}

// Откалибровано ли устройство
bool Navigation::hasCalibration()
{
	return false;
}