//
// SensorRGB.h
// Датчик цвета и освещённости
//

#include <stdint.h>

#ifndef I2C_DEVICE_H_
#define I2C_DEVICE_H_

	class I2CDevice
	{
	protected:
		// Адрес датчика
		uint16_t address;
			
	public:
		// Конструктор
		// address: адрес устройства (7 бит)
		I2CDevice(uint8_t address);
 
		// Запись данных
		// RegId: стартовый номер регистра
		//  Data: данные для записи
		//  Size: размер данных
		void WriteData(uint8_t RegId, const uint8_t * Data, uint8_t Size);
		
		// Запись в регистр
		// RegId: идентификатор регистра
		// Value: значение
		void WriteU8(uint8_t RegId, uint8_t Value);
		
		// Запись в регистр  (Big Endian)
		// RegId: идентификатор регистра
		// Value: значение
		void WriteU16BE(uint8_t RegId, uint16_t Value);
		
		// Запись бита в регистр
		// RegId: идентификатор регистра
		// BitId: номер бита
		// Value: значение
		void WriteBit(uint8_t RegId, uint8_t BitId, bool Value);
		
		// Запись нескольких бит в регистр
		//    RegId: идентификатор регистра
		//    BitId: номер первого сташего бита
		// BitCount: номер бита
		//    Value: значение
		void WriteBits(uint8_t RegId, uint8_t BitId, uint8_t BitCount, uint8_t Value);
		
		// Чтение данных
		// RegId: стартовый номер регистра
		//  Data: данные для записи
		//  Size: размер данных
		bool ReadData(uint8_t RegId, uint8_t * Data, uint8_t Size);

		// Прочитать бит регистра
		//   RegId: идентификатор регистра
		//   BitId: номер бита
		// Success: успешно ли прочиталось
		//  return: результат чтения, если Success = true
		bool ReadBit(uint8_t RegId, uint8_t BitId, bool * Success);

		// Прочитать биты регистра
		//    RegId: идентификатор регистра
		//    BitId: номер первого сташего бита
		// BitCount: количество бит
		//  Success: успешно ли прочиталось
		//   return: результат чтения, если Success = true
		uint8_t ReadBits(uint8_t RegId, uint8_t BitId, uint8_t BitCount, bool * Success);

		// Прочитать регистр
		//   RegId: идентификатор регистра
		// Success: успешно ли прочиталось
		//  return: результат чтения, если Success = true
		uint8_t ReadU8(uint8_t RegId, bool * Success);
		
		// Прочитать регистр (Little Endian)
		//   RegId: идентификатор регистра
		// Success: успешно ли прочиталось
		//  return: результат чтения, если Success = true
		uint16_t ReadU16LE(uint8_t RegId, bool * Success);
		
		// Прочитать регистр (Big Endian)
		//   RegId: идентификатор регистра
		// Success: успешно ли прочиталось
		//  return: результат чтения, если Success = true
		uint16_t ReadU16BE(uint8_t RegId, bool * Success);
		
		// Прочитать регистр (Big Endian)
		//   RegId: идентификатор регистра
		// Success: успешно ли прочиталось
		//  return: результат чтения, если Success = true
		uint32_t ReadU24BE(uint8_t RegId, bool * Success);
	};
	
#endif /* SENSOR_RGB_H_ */
