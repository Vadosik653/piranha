//
// OrientationQMC5883.cpp
// Датчик курсовой ориентации на магнитометре QMC5883
//

#include "OrientationQMC5883.h"
#include <Arduino.h>
#include <Wire.h>

// Адрес датчика QMC5883
const uint8_t qmc5883Address = 0x0D;
// Адрес регистра управления 1
const uint8_t qmc5883RegControl1 = 0x09;
// Адрес регистра управления 2
const uint8_t qmc5883RegControl2 = 0x0A;
// Адрес регистра настройки периода
const uint8_t qmc5883RegPeriod = 0x0B;

// Конструктор
OrientationQMC5883::OrientationQMC5883() : X(0), Y(0), Z(0)
{
	
}

// Инициализация периферии
void OrientationQMC5883::Init()
{
	// Настроим регистр контроля 1
	Wire.beginTransmission(qmc5883Address);
	Wire.write(qmc5883RegControl1);
	Wire.write(0x51); //0b01010001
	Wire.endTransmission();

	// Настроим регистр контроля 2
	Wire.beginTransmission(qmc5883Address);
	Wire.write(qmc5883RegControl2);
	Wire.write(0x40); //0b01000000
	Wire.endTransmission();

	// Настроим регистр периода
	Wire.beginTransmission(qmc5883Address);
	Wire.write(qmc5883RegPeriod);
	Wire.write(0x01);
	Wire.endTransmission();
}

// Обновить показания с датчика
void OrientationQMC5883::Update()
{
	uint16_t x, y, z;
	
	Wire.beginTransmission(qmc5883Address);
	Wire.write(0x00);
	Wire.endTransmission();
	
	// Прочитаем данные из датчика
	Wire.requestFrom(qmc5883Address, 6u);
	if(6 <= Wire.available())
	{
		x = Wire.read() << 8; // X старший байт
		x |= Wire.read();     // X младший байт
		z = Wire.read() << 8; // Z старший байт
		z |= Wire.read();     // Z младший байт
		y = Wire.read() << 8; // Y старший байт
		y |= Wire.read();     // Y младший байт
	}
	
	X = x;
	Y = y;
	Z = z;
	
	// Пересчёт в угол
	int16_t angle = atan2(-Y,X)/M_PI*180;
	if (angle < 0)
	{
		angle = angle + 360;
	}
	Angle = angle;
}

// Получить курсовой угол
// return: курсовой угол
int16_t OrientationQMC5883::getAngle()
{
	return Angle;
}