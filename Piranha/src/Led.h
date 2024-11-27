//
// LedLight.h
// Управление светодиодом
//

#include <stdint.h>

#ifndef LEDLIGHT_H_
#define LEDLIGHT_H_

	class Led
	{
	private:
		// Назначенный для светодиода вывод
		uint8_t Pin;
		
	public:
		// Конструктор
		// Pin: вывод, на котором осуществляется управление светом
		Led(uint8_t Pin);

		// Инициализация периферии (выводов)
		void Init();

		// Установка состояния фонаря или светодиода
		// Value: вкл или выкл
		void setValue(bool Value);
	};

#endif /* LEDLIGHT_H_ */
