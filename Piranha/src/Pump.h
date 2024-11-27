//
// Pump.h
// Управление насосом
//

#include <stdint.h>

#ifndef PUMP_H_
#define PUMP_H_

	class Pump {
	public:
		// Инициализация периферии и выводов
		void Init();

		// Задать направление
		// Output: выкачиваем
		void setDirection(bool Output);

		// Задать состояние
		void setState(bool Enabled);
	};

#endif /* VOLTAGE_H_ */
