//
// Pressure.cpp
// Код датчика давления
// (на основе внешней библиотеки)
//

#ifndef PRESSURE_H_
#define PRESSURE_H_

	#include "Config.h"
	#include "I2CDevice.h"
	#include "Filter.h"

	typedef struct {
		uint64_t TempDivider;
		uint8_t  TempMultiplier;
		uint8_t  Off2Coeff;  // 1/16
		uint8_t  Sens2;      // 1/8
	} SensorCoeffsGroup;

	typedef struct {
		uint8_t  Off2Coeff;  // 1/16
		uint8_t  Sens2;      // 1/8
	} SensorCoeffsGroupVeryLow;

	typedef struct {
		const char *             Name;
		
		SensorCoeffsGroup        Low;
		SensorCoeffsGroup        High;
		
		SensorCoeffsGroupVeryLow VeryLow;
		
		uint16_t                 mbarDivider;
		uint8_t                  mbarIntCoeff;
		uint8_t                  OffsetCoeff;
		uint8_t                  SensitivityCoeff;
	} SensorCoeffs;

	typedef enum {
		ENUM_MS5803_02 = 0, // 2 бар
		ENUM_MS5803_30 = 1, // 30 бар
	} EnumSensorType;

	class Pressure : public I2CDevice
	{
		private:
			Filter PressFilter;
    
			// Давление, полученное из датчика, мбар
			float Press;
			// Температура, полученная из датчикаб С
			float Temp;
			// Глубина, полученная из датчика, м
			float Depth;
			
			// Доступен ли датчик
			bool Available;
			
			// Таблица коэффициентов перевода
			const SensorCoeffs * Table;
			
			// Коэффициенты датчика
			uint16_t Coeffs[8];
			
			// Сброс датчика
			void Reset();
			
			// Прочитать измеренное значение АЦП
			// CommandADC: тип измерения
			//     return: считанное значение АЦП
			uint32_t ReadADC(uint8_t CommandADC);
			
			// Расчёт CRC для таблицы поправок
			// return: рассчитанный crc
			uint8_t CalcTableCRC();
			
		public:
			// Конструктор
			Pressure();
		
			// Установка типа
			void SetType(EnumSensorType Type);
		
			// Инициализация периферии
			void Init();
			// Обновление показаний датчика
			void Update();
			
			// Получить температуру
			// return: температура, С
			float getTemp();
			// Получить давление
			// return: давление, мбар
			float getPressure();
			// Получить глубину
			// return: глубина, м
			float getDepth();
	};

#endif /* PRESSURE_H_ */
