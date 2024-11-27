//
// Pinout.h
// Список используемых выводов
//

#include <stdint.h>

#ifndef PINOUT_H_
#define PINOUT_H_

	// Номер вывода для красного светодиода
	extern const uint8_t RedLEDpin;
	// Номер вывода для управления фонарями
	extern const uint8_t HeadLts;

	// Номер вывода с резистивным делителем для проверки напряжения батареи
	extern const uint8_t Voltpin;
	// Номер вывода, подключённого к датчику температры платы TMP36
	extern const uint8_t Temppin;
	// Номер вывода, подключённого к насосу
	extern const uint8_t Pumppin1;
	// Номер вывода, подключённого к насосу
	extern const uint8_t Pumppin2;
	// Номер вывода для управления приёмопередатчиками (HC-12/RS485)
	extern const uint8_t Dirpin;
	
	// Номер вывода для управления преобразователем уровней
	extern const uint8_t LevelSh;



#endif /* PINOUT_H_ */
