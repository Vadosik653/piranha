//
// HeadLight.h
// Управление фонарями
//

#include <stdint.h>

#ifndef HEADLIGHT_H_
#define HEADLIGHT_H_

	class HeadLight
	{
	private:
		// Назначенный для фонаря или светодиода вывод
		uint8_t Pin;
		
		// Текущая яркость
		uint8_t Value;
		
	public:
		// Конструктор
		// Pin: вывод, на котором осуществляется управление светом
		// Активный уровень высокий!
		HeadLight(uint8_t Pin);

		// Инициализация периферии (выводов)
		void Init();

		// Установка состояния фонаря
		// Value: яркость
		void setValue(uint8_t Value);
		
		// Получить состояние фонаря
		// return: яркость
		uint8_t getValue();
	};

#endif /* LEDLIGHT_H_ */
