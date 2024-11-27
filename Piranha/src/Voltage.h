//
// Voltage.h
// Измерение напряжений на плате
//

#include <stdint.h>

#ifndef VOLTAGE_H_
#define VOLTAGE_H_

	class Voltage {
	public:
		// Инициализация периферии и выводов
		void Init();

		// Прочитать напряжение батареи
		// return: напряжение, выраженное в 0.1В (т.е. 12В = 120)
		uint16_t getVoltage();
	};

#endif /* VOLTAGE_H_ */