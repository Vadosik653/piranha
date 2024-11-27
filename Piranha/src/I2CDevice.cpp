//
// SensorRGB.cpp
// Датчик цвета и освещённости
//

#include "I2CDevice.h"
#include <Arduino.h>
#include <Wire.h>

// Конструктор
// address: адрес устройства (7 бит)
I2CDevice::I2CDevice(uint8_t address) : address(address)
{
	
}

// Запись в регистр
// RegId: идентификатор регистра
// Value: значение
void I2CDevice::WriteU8(uint8_t RegId, uint8_t Value)
{
	Wire.beginTransmission(address);
	Wire.write(RegId);
	Wire.write(Value);
	Wire.endTransmission();
}

// Запись в регистр  (Big Endian)
// RegId: идентификатор регистра
// Value: значение
void I2CDevice::WriteU16BE(uint8_t RegId, uint16_t Value)
{
	Wire.beginTransmission(address);
	Wire.write(RegId);
	Wire.write((Value >> 8) & 0xFF);
	Wire.write(Value & 0xFF);
	Wire.endTransmission();
}

// Запись данных
// RegId: стартовый номер регистра
//  Data: данные для записи
//  Size: размер данных
void I2CDevice::WriteData(uint8_t RegId, const uint8_t * Data, uint8_t Size)
{
	Wire.beginTransmission(address);
	Wire.write(RegId);
	for(int i = 0; i < Size; i++)
	{
		Wire.write(Data[i]);
	}
	Wire.endTransmission();
}

// Запись бита в регистр
// RegId: идентификатор регистра
// BitId: номер бита
// Value: значение
void I2CDevice::WriteBit(uint8_t RegId, uint8_t BitId, bool Value)
{
	bool Success = false;
	uint8_t RegValue = ReadU8(RegId, &Success);
	if(Success)
	{
		if(Value)
			RegValue |= (1 << BitId);
		else
			RegValue &= ~(1 << BitId);
		
		WriteU8(RegId, RegValue);
	}
}

// Запись нескольких бит в регистр
//    RegId: идентификатор регистра
//    BitId: номер бита
// BitCount: номер бита
//    Value: значение
void I2CDevice::WriteBits(uint8_t RegId, uint8_t BitId, uint8_t BitCount, uint8_t Value)
{
	bool Success = false;
	uint8_t RegValue = ReadU8(RegId, &Success);
	if(Success)
	{
		uint8_t Shift = BitId - BitCount + 1;
		
		uint8_t Mask = ((1 << BitCount) - 1) << Shift;
		RegValue = RegValue & ~Mask | (Value << Shift);
	
		WriteU8(RegId, RegValue);
	}
}

// Чтение данных
// RegId: стартовый номер регистра
//  Data: данные для записи
//  Size: размер данных
bool I2CDevice::ReadData(uint8_t RegId, uint8_t * Data, uint8_t Size)
{
	Wire.beginTransmission(address);
	Wire.write(RegId);
	Wire.endTransmission(false);
	Wire.requestFrom(address, Size, true);

	uint8_t avail = Wire.available();
	if(avail == Size)
	{
		for(int i = 0; i < Size; i++)
			Data[i] = Wire.read();
		
		return true;
	}
	
	return false;
}

// Прочитать бит регистра
//   RegId: идентификатор регистра
//   BitId: номер бита
// Success: успешно ли прочиталось
//  return: результат чтения, если Success = true
bool I2CDevice::ReadBit(uint8_t RegId, uint8_t BitId, bool * Success)
{
	uint8_t Value = ReadU8(RegId, Success);
	
	if(*Success)
		return (Value & (1 << BitId)) != 0;
	else
		return false;
}

// Прочитать биты регистра
//    RegId: идентификатор регистра
//    BitId: номер первого сташего бита
// BitCount: количество бит
// Success: успешно ли прочиталось
//   return: результат чтения, если Success = true
uint8_t I2CDevice::ReadBits(uint8_t RegId, uint8_t BitId, uint8_t BitCount, bool * Success)
{
	uint8_t Mask = (1 << BitCount) - 1;
	uint8_t Shift = BitId - BitCount + 1;
	uint8_t Value = ReadU8(RegId, Success);
	
	if(*Success)
		return (Value >> Shift) & Mask;
	else
		return 0;
}

// Прочитать регистр
//   RegId: идентификатор регистра
// Success: успешно ли прочиталось
//  return: результат чтения, если Success = true
uint8_t I2CDevice::ReadU8(uint8_t RegId, bool * Success)
{
	Wire.beginTransmission(address);
	Wire.write(RegId);
	Wire.endTransmission(false);
	Wire.requestFrom(address, 1u, true);
	uint8_t avail = Wire.available();
	if(1 <= avail)
	{
		uint8_t res = Wire.read();
		*Success = true;
		
		return res;
	}
	else
	{
		*Success = false;
		return 0;
	}
}

// Прочитать регистр (Little Endian)
//   RegId: идентификатор регистра
// Success: успешно ли прочиталось
//  return: результат чтения, если Success = true
uint16_t I2CDevice::ReadU16LE(uint8_t RegId, bool * Success)
{
	Wire.beginTransmission(address);
	Wire.write(RegId);
	Wire.endTransmission(false);
	Wire.requestFrom(address, 2u, true);
	uint8_t avail = Wire.available();
	if(2 <= avail)
	{
		uint16_t res = Wire.read() | ((uint16_t)Wire.read() << 8);
		*Success = true;
		
		return res;
	}
	else
	{
		*Success = false;
		return 0;
	}
}

// Прочитать регистр (Big Endian)
//   RegId: идентификатор регистра
// Success: успешно ли прочиталось
//  return: результат чтения, если Success = true
uint16_t I2CDevice::ReadU16BE(uint8_t RegId, bool * Success)
{
	Wire.beginTransmission(address);
	Wire.write(RegId);
	Wire.endTransmission(false);
	Wire.requestFrom(address, 2u, true);
	uint8_t avail = Wire.available();
	if(2 <= avail)
	{
		uint16_t res = ((uint16_t)Wire.read() << 8) | Wire.read();
		*Success = true;
		
		return res;
	}
	else
	{
		*Success = false;
		return 0;
	}
}

// Прочитать регистр (Big Endian)
//   RegId: идентификатор регистра
// Success: успешно ли прочиталось
//  return: результат чтения, если Success = true
uint32_t I2CDevice::ReadU24BE(uint8_t RegId, bool * Success)
{
	Wire.beginTransmission(address);
	Wire.write(RegId);
	Wire.endTransmission(false);
	Wire.requestFrom(address, 3u, true);
	uint8_t avail = Wire.available();
	if(3 <= avail)
	{
		uint32_t res = ((uint32_t)Wire.read() << 16) | ((uint32_t)Wire.read() << 8) | Wire.read();
		*Success = true;
		
		return res;
	}
	else
	{
		*Success = false;
		return 0;
	}
}