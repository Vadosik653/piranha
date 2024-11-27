//
// Program.h
// Управление программами
//

#include <stdint.h>
#include <stdbool.h>

#ifndef _PROGRAM_H
#define _PROGRAM_H

	// Инициализация протокола
	void program_Init();

	// Сообщить, что системная задача стартовала
	void program_MarkSystemStarted();

	// Стартовала ли системная задача 
	bool program_IsSystemStarted();
	
	// Управляется ли локально
	bool program_IsLocalControl();

	// Получить номер текущей задачи
	uint8_t program_GetId();

	// Выбор программы
	void program_Select(uint8_t Index);

#endif
