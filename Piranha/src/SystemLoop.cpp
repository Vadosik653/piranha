//
// SystemLoop.cpp
// Системный цикл управления пираньей (обработка датчиков, управление моторами и т.д.)
//
#include "SystemLoop.h"
#include <Arduino.h>
#include <Wire.h>

#include "Pinout.h"
#include "Debug.h"
#include "Program.h"
#include "Drone.h"
#include "Led.h"
#include "HeadLight.h"
#include "Voltage.h"
#include "Camera.h"
#include "SensorRGB.h"
#include "Temperature.h"
#include "FirmwareUpdate.h"
#include "Web.h"
#include "ProtocolSlave.h"

// Фонарь
HeadLight        ledlight(HeadLts);
// Светодиод индикаторный
Led              led(RedLEDpin);
// Управление движением дрона
Drone            drone;
// Управление внешней камерой
Camera           camera;
// измерение напряжения
Voltage          voltage;
// Датчик температуры
TemperatureTMP36 temp;
#if defined(ENABLE_RGB) && (ENABLE_RGB > 0)
// Датчик цвета
SensorRGB        sysrgb;
#endif

#if defined(WEB_AVAILABLE) && (WEB_AVAILABLE > 0)
Web              web;
#endif
#if defined(ENABLE_UDP_DEBUG) && (ENABLE_UDP_DEBUG > 0)
PacketDebug      debug;
#endif

// Последнее состояние сигнала на запись видео
bool CamRecd = false;
// Последнее состояние сигнала на съёмку фото
bool CamPhoto = false;

// Состояние фонарей
bool LedState = false;

// Настройка периферии и инициализация переменных
void SystemSetup()
{
	// Инициализация индикаторного светодиода и его включение
	led.Init();
	led.setValue(true);
  
	Wire.begin();
	Wire.setClock(400000); 
	Wire.setTimeout(500);
  
	// Выключим светодиод
	led.setValue(false);

	// Инициализация всех подсистем
	program_Init();
	camera.Init();
	drone.Init();
	ledlight.Init();
	voltage.Init();
	temp.Init();
#if defined(ENABLE_RGB) && (ENABLE_RGB > 0)
	sysrgb.Init();
#endif
#if defined(ENABLE_UDP_DEBUG) && (ENABLE_UDP_DEBUG > 0)
	debug.Init();
#endif
	fwu_Init();
#if defined(WEB_AVAILABLE) && (WEB_AVAILABLE > 0)
	web.Init();
#endif
		
	// Ожидание запуска буя
	for(int i = 0; i < 2; i++)
	{
		LedState = !LedState;
		led.setValue(LedState);
		delay(500);
	}

	// В оригинале тут останавливалась запись, случайно стартующая при старте робота
	// (неизвестно, нужна ли эта команда)
	camera.TriggerVideo();
	
	// Отметить, что системный поток стартовал
	program_MarkSystemStarted();
}

// Главный цикл опроса датчиков, получения и отправки пакетов и т.д.
void SystemLoop()
{
//	Serial.write("SystemLoop...\n");
	// Переключаем светодиод для контроля частоты выполнения этой функции
	led.setValue(LedState);
	LedState = !LedState;
	
	// Обновим состояние датчиков
	camera.Update();
	drone.UpdateSensors();
	drone.SendToMotors();
#if defined(ENABLE_RGB) && (ENABLE_RGB > 0)
	sysrgb.Update();
#endif
#if defined(ENABLE_UDP_DEBUG) && (ENABLE_UDP_DEBUG > 0)
	debug.Loop();
#endif
	fwu_Main();

	vTaskDelay(2);
}


void ProtocolSetup()
{
	
}

void ProtocolLoop()
{
	proto_Main();
	
	vTaskDelay(1);
}
