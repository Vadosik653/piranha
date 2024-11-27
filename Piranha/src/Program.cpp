//
// Program.cpp
// Управление программами
//

#include "Program.h"
#include "Piranha.h"
#include <Arduino.h>

// Размер стека
#define STACK_SIZE 4096

// Буфер под статическую задачу
static StaticTask_t xTaskBuffer;

// Память для стека
static StackType_t  xStack[STACK_SIZE];

// Идентификатор задачи
static TaskHandle_t xHandle = NULL;

// Главный цикл начался
static bool SystemStarted = false;

// Флаг на запуск пользовательской программы (обучающей программы)
static bool DroneStarted = false;

// Выполняется ли управление из пользовательской программы (TODO)
static bool LocalControl = false;

// Номер текущей программы
static uint8_t  SelectedProgramId = 0;

// Номер текущей программы
static uint8_t  ProgramId = 0;

// Автозапуск разрешён
bool AutostartEnabled = false;

// Настройка пользовательской программы
void DroneSetup();
// Выполнение одного цикла пользовательской программы
void DroneLoop();

// Нужно ли выполнить эту программу сразу после старта дрона?..
bool IsAutostart()
{
	return AutostartEnabled; // true - выполняем, false - только по нажатию кнопки
}

// Пользовательская задача: команды для автоматического управления роботом
// pvParameters: параметры запуска (не используются)
void DroneTask(void *pvParameters)
{
	Serial.write("Script started...\n");
	LocalControl = true;
  
	bot::MotorEnable();
	DroneLoop();
	bot::Stop();
	
	LocalControl = false;
	DroneStarted = false;
	ProgramId = 0;

	Serial.write("Script stopped...\n");
	vTaskDelete( NULL );
}

// Сообщить, что системная задача стартовала
void program_MarkSystemStarted()
{
	SystemStarted = true;
	
	if(IsAutostart())
	{
		Serial.write("Script autostart enabled...\n");
		
		program_Select(1);
	}
}

// Стартовала ли системная задача 
bool program_IsSystemStarted()
{
	return SystemStarted;
}

// Инициализация протокола
void program_Init()
{
	AutostartEnabled = false;
	DroneSetup();
}

// Управляется ли локально
bool program_IsLocalControl()
{
	return LocalControl;
}

// Получить номер текущей задачи
uint8_t program_GetId()
{
	return ProgramId;
}

// Запуск
static void program_Run(uint8_t Index)
{
	if(!DroneStarted)
	{
		DroneStarted = true;
		Serial.write("Start DroneTask...\n");
		
		// Задача для дрона
		xHandle = xTaskCreateStatic(DroneTask, "drone", STACK_SIZE, NULL, 1, xStack, &xTaskBuffer);
	}
}

// Остановить
static void program_Stop()
{
	if(DroneStarted)
	{
		bot::Stop();
		
		vTaskDelete(xHandle);
		
		DroneStarted = false;
		LocalControl = false;
		xHandle = NULL;
	}
}

// Выбор программы
void program_Select(uint8_t Index)
{
	if(!SystemStarted) return;
  
	if(SelectedProgramId != Index)
	{
		SelectedProgramId = Index;
		ProgramId = Index;
		
		if(Index)
			program_Run(Index);
		else
			program_Stop();
	}
}
