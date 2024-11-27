//
// SensorRGB.h
// Датчик цвета и освещённости
//

#include <stdint.h>

#include "I2CDevice.h"

#ifndef SENSOR_RGB_H_
#define SENSOR_RGB_H_

	typedef enum {
		TCS34725_GAIN_1X = 0x00,  /**<  No gain  */
		TCS34725_GAIN_4X = 0x01,  /**<  4x gain  */
		TCS34725_GAIN_16X = 0x02, /**<  16x gain */
		TCS34725_GAIN_60X = 0x03  /**<  60x gain */
	} enum_tcs3472_gain;

	#define TCS34725_INTEGRATIONTIME_154MS (0xC0) /** < 153.6ms - 64 cycles - Max Count: 65535 */
	#define TCS34725_INTEGRATIONTIME_614MS (0x00) /** < 614.4ms - 256 cycles - Max Count: 65535 */


	class SensorRGB : public I2CDevice
	{
	private:
		// Красная компонента цвета
		uint8_t R;
		// Зелёная компонента цвета
		uint8_t G;
		// Синяя компонента цвета
		uint8_t B;
		// Общая освещённость
		uint8_t A;
		
		// Счётчик для прореживания запросов к датчику
		uint8_t cycles;
		
		// Найден ли на шине датчик цвета
		bool              available;

		// Режим захвата обновлён
		bool              updated;
    
		// Текущий режим усиления
		uint8_t           mode;
		
		// Усиление
		enum_tcs3472_gain gain;
		
		// Время интегрирования
		uint8_t           inttime;
		
	private:
		// Задать время интегрирования
		// value: время
		void SetIntegrationTime(uint8_t Value);
	
		// Задать режим усиления
		// value: усиление
		void SetGain(enum_tcs3472_gain Value);
	
		// Включить работу
		void Enable();
	
		// Проверка идентификатора
		// return: если это действительно датчик цвета
		bool CheckId();
	
	public:
		SensorRGB();
 
		// Инициализация периферии
		void Init();
		// Обновление показаний датчика
		void Update();
		
		// Получить красную компоненту цвета
		// return: красная компонента цвета
		uint8_t getR();
		// Получить зелёную компоненту цвета
		// return: зелёная компонента цвета
		uint8_t getG();
		// Получить синюю компоненту цвета
		// return: синяя компонента цвета
		uint8_t getB();
		// Получить общую освещённость
		// return: общая освещённость
		uint8_t getA();
		
		// Доступен ли датчик цвета
		// return: true, если доступно
		bool isAvailable();
	};
	
#endif /* SENSOR_RGB_H_ */
