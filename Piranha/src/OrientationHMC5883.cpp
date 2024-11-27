//
// OrientationHMC5883.cpp
// Датчик курсовой ориентации на магнитометре HMC5883
//

#include "OrientationHMC5883.h"
#include <Arduino.h>
#include <Wire.h>

// Адрес датчика HMC5883 на шине I2C: 0011110b
static const uint8_t hmc5883Address = 0x1E;
// Адрес регистра режима:
static const uint8_t hmc5883ModeRegister = 0x02;
// Бесконечный режим
static const uint8_t hmcContinuousMode = 0x00;
// Адрес регистра получаемых данных
static const uint8_t hmcDataOutputXMSBAddress = 0x03;

// Конструктор
OrientationHMC5883::OrientationHMC5883() : X(0), Y(0), Z(0)
{
	
}

// Инициализация периферии
void OrientationHMC5883::Init()
{
	Wire.beginTransmission(hmc5883Address);
	Wire.write(hmc5883ModeRegister);
	Wire.write(hmcContinuousMode);
	Wire.endTransmission();
}

// Обновить показания с датчика
void OrientationHMC5883::Update()
{
	uint16_t x, y, z;
	
	// Читаем данные из магнитометра
	Wire.beginTransmission(hmc5883Address);
	Wire.write(hmcDataOutputXMSBAddress); // Выбираем регистр данных
	Wire.endTransmission();
		
	Wire.requestFrom(hmc5883Address, 6u);
	if(6 <= Wire.available())
	{
		x = Wire.read()<<8; // X старший байт
		x |= Wire.read();   // X младший байт
		z = Wire.read()<<8; // Z старший байт
		z |= Wire.read();   // Z младший байт
		y = Wire.read()<<8; // Y старший байт
		y |= Wire.read();   // Y младший байт
	}
	
	X = x;
	Y = y;
	Z = z;
}

// Получить курсовой угол
// return: курсовой угол
int16_t OrientationHMC5883::getAngle()
{
	int16_t angle = atan2(-Y,X)/M_PI*180;
	if (angle < 0)
	{
		angle = angle + 360;
	}
	return angle;
}