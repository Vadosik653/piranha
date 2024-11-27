//
// SensorRGB.cpp
// Датчик цвета и освещённости
//

#include "SensorRGB.h"
#include <Arduino.h>
#include <Wire.h>

typedef struct {
	// Режим усиления
	enum_tcs3472_gain Gain;
	// Время интегрирования
	uint8_t           Time;
	// Минимальное значение яркости
	uint16_t          MinValue;
	// Максимальное значение яркости
	uint16_t          MaxValue;
} SensorRGBMode;

// Бит команды, нужен для доступа к регистрам
#define TCS34725_COMMAND_BIT (0x80)

// Регистры датчика
#define TCS34725_ENABLE  (0x00)
#define TCS34725_ATIME   (0x01)
#define TCS34725_CONTROL (0x0F)
#define TCS34725_ID      (0x12)
#define TCS34725_CDATAL  (0x14)
#define TCS34725_CDATAH  (0x15)
#define TCS34725_RDATAL  (0x16)
#define TCS34725_RDATAH  (0x17)
#define TCS34725_GDATAL  (0x18)
#define TCS34725_GDATAH  (0x19)
#define TCS34725_BDATAL  (0x1A)
#define TCS34725_BDATAH  (0x1B)

#define TCS34725_ENABLE_PON  (0x01)
#define TCS34725_ENABLE_AEN  (0x02)

#define MODE_COUNT 5
static const SensorRGBMode RgbModes[MODE_COUNT] = {
	{ TCS34725_GAIN_60X, TCS34725_INTEGRATIONTIME_614MS,     0, 20000 },
	{ TCS34725_GAIN_60X, TCS34725_INTEGRATIONTIME_154MS,  4990, 63000 },
	{ TCS34725_GAIN_16X, TCS34725_INTEGRATIONTIME_154MS, 16790, 63000 },
	{ TCS34725_GAIN_4X,  TCS34725_INTEGRATIONTIME_154MS, 15740, 63000 },
	{ TCS34725_GAIN_1X,  TCS34725_INTEGRATIONTIME_154MS, 15740, 0     }
};

SensorRGB::SensorRGB() : I2CDevice(0x29), R(0), G(0), B(0), A(0), cycles(0), mode(0), available(false), updated(true), gain(TCS34725_GAIN_60X), inttime(TCS34725_INTEGRATIONTIME_614MS)
{

}

// Инициализация периферии
void SensorRGB::Init()
{
	if(CheckId())
	{
		SetIntegrationTime(RgbModes[mode].Time);
		SetGain(RgbModes[mode].Gain);
		Enable();
		available = true;
	}
}

// Задать время интегрирования
// Value: время интегрирования
void SensorRGB::SetIntegrationTime(uint8_t Value)
{
	WriteU8(TCS34725_COMMAND_BIT | TCS34725_ATIME, Value);
	
	inttime = Value;
}

// Задать режим усиления
// value: усиление
void SensorRGB::SetGain(enum_tcs3472_gain Value)
{
	WriteU8(TCS34725_COMMAND_BIT | TCS34725_CONTROL, Value);
	
	gain = Value;
}

void SensorRGB::Enable()
{
	WriteU8(TCS34725_COMMAND_BIT | TCS34725_ENABLE, TCS34725_ENABLE_PON);
	delay(3);
	WriteU8(TCS34725_COMMAND_BIT | TCS34725_ENABLE, TCS34725_ENABLE_PON | TCS34725_ENABLE_AEN);
}

// Проверка идентификатора
// return: если это действительно датчик цвета
bool SensorRGB::CheckId()
{
	bool ok;
	uint8_t id = ReadU8(TCS34725_COMMAND_BIT | TCS34725_ID, &ok);
	if(ok)
	{
		if((id == 0x4d) || (id == 0x44) || (id == 0x10))
			return true;
	}
	
	return false;
}

// Обновление показаний датчика
void SensorRGB::Update()
{
	if(cycles < 20)
	{
		cycles++;
	}
	else
	{
		cycles = 0;
		if(available)
		{
			bool ok;
			// Сразу как 8 байт оно не читает почему-то.
			uint16_t c = ReadU16LE(TCS34725_COMMAND_BIT | TCS34725_CDATAL | (1 << 5), &ok);
			uint16_t r = ReadU16LE(TCS34725_COMMAND_BIT | TCS34725_RDATAL | (1 << 5), &ok);
			uint16_t g = ReadU16LE(TCS34725_COMMAND_BIT | TCS34725_GDATAL | (1 << 5), &ok);
			uint16_t b = ReadU16LE(TCS34725_COMMAND_BIT | TCS34725_BDATAL | (1 << 5), &ok);
			
			R = 255ul * r / c;
			G = 255ul * g / c;
			B = 255ul * b / c;
			A = c >> 8;
			
			// Проверим режим и перенастроим...
			bool updated = false;
			if(RgbModes[mode].MaxValue && (c > RgbModes[mode].MaxValue))
			{
				mode++;
				updated = true;
			}
			else if (RgbModes[mode].MinValue && (c < RgbModes[mode].MinValue))
			{
				mode--;
				updated = true;
			}
			
			if(updated)
			{
				SetGain(RgbModes[mode].Gain);
				SetIntegrationTime(RgbModes[mode].Time);
				delay(10);
			}
		}
		else
		{
			R = 0;
			G = 0;
			B = 0;
			A = 0;
		}
	}
}

// Получить красную компоненту цвета
// return: красная компонента цвета
uint8_t SensorRGB::getR()
{
	return R;
}

// Получить зелёную компоненту цвета
// return: зелёная компонента цвета
uint8_t SensorRGB::getG()
{
	return G;
}

// Получить синюю компоненту цвета
// return: синяя компонента цвета
uint8_t SensorRGB::getB()
{
	return B;
}

// Получить общую освещённость
// return: общая освещённость
uint8_t SensorRGB::getA()
{
	return A;
}

// Доступен ли датчик цвета
// return: true, если доступно
bool SensorRGB::isAvailable()
{
	return available;
}
