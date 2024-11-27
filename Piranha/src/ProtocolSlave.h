//
// ProtocolSlave.h
// Протокол связи для ведомого блока
//

#include <stdint.h>
#include <stdbool.h>
#include <Arduino.h>

#ifndef _PROTOCOL_SLAVE_H
#define _PROTOCOL_SLAVE_H

	// Инициализация протокола
	void proto_Init();

	// Главный цикл
	void proto_Main();

#endif
