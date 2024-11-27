//
// Main.h
// Точка входа в программу управления дроном, настройка ОС и запуск потоков
//

#include <Arduino.h>
#include "Config.h"
#include "Piranha.h"
#include "Program.h"
#include "Pinout.h"
#include "ProtocolSlave.h"

// Размер стека
#define STACK_SIZE 8192

// Буфер под статическую задачу
static StaticTask_t xTaskBufferM;
static StaticTask_t xTaskBufferP;

// Память для стека
static StackType_t  xStackM[STACK_SIZE];
static StackType_t  xStackP[STACK_SIZE];

// Идентификатор сети
char WiFiSSID[64];

// Идентификатор сети
char WiFiPassword[64];

// Настройка системного цикла
void SystemSetup();
// Системный цикл
void SystemLoop();

// Настройка цикла обмена
void ProtocolSetup();
// Обменный цикл
void ProtocolLoop();

// Системная задача: управление моторами, опрос датчиков
// pvParameters: параметры запуска (не используются)
void SystemTask(void *pvParameters)
{
	Serial.write("Start SystemTask...\n");
	SystemSetup();
	Serial.write("SystemTask started...\n");
	program_MarkSystemStarted();
  
	while(1)
	{
		SystemLoop();
	}
}

// Системная задача: приём пакетов по UART
// pvParameters: параметры запуска (не используются)
void ProtocolTask(void *pvParameters)
{
	Serial.write("Start ProtocolTask...\n");
	ProtocolSetup();
	while(1)
	{
		ProtocolLoop();
		if (serialEventRun) serialEventRun();
	}
}

// Включить транслятор уровней 3.3В -> 5В
void init_level_shifter()
{
	pinMode(LevelSh, OUTPUT);
	digitalWrite(LevelSh, LOW); // Сперва низкий уровень, чтобы все ножки были в Hi-Z и определили направление работы
	delay(10);
	digitalWrite(LevelSh, HIGH);
	delay(100);
}

// Функция настройки от Ардуино
void setup()
{
	Serial.begin(115200);
	Serial.write("Starting piranha code...\n");
	
	// Включить транслятор уровней 3.3В -> 5В
	init_level_shifter();
	proto_Init();
	
	// Системная задача
	xTaskCreateStatic(SystemTask, "system", STACK_SIZE, NULL, 2, xStackM, &xTaskBufferM);
	xTaskCreateStatic(ProtocolTask, "protocol", STACK_SIZE, NULL, 2, xStackP, &xTaskBufferP);
}

// Функция цикла от Ардуино (не будет использоваться)
void loop()
{
	vTaskDelay(1000);
}
