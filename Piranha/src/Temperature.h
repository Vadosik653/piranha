//
// Temperature.h
// Датчик температуры платы типа TMP3X
//

#include <stdint.h>

#ifndef TEMPERATURE_H_
#define TEMPERATURE_H_

	// Абстрактный класс датичка температуры платы (TMP3X)
	class Temperature
	{
	protected:
		// Сырые показания датчика
		uint16_t Raw;
		// Температура платы в градусах
		int8_t Value;
		
	public:
		// Инициализация периферии
		void Init();
		
		// Прочитать температуру
		// return: температура, С
		virtual int8_t getTemperature() = 0;
	};
	
	// Класс для датчика температуры TMP36
	class TemperatureTMP36 : public Temperature {
	
	public:
		// Прочитать температуру
		// return: температура, С
		virtual int8_t getTemperature() override;
	};

	// Класс для датчика температуры TMP36
	class TemperatureTMP37 : public Temperature {
		
	public:
		// Прочитать температуру
		// return: температура, С
		virtual int8_t getTemperature() override;
	};
	
#endif /* TEMPERATURE_H_ */