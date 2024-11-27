//
// Debug.cpp
// Отладка. Отправка сообщений о состоянии по UDP
//

#include "Debug.h"
#include "Drone.h"
#include "HeadLight.h"
#include "Program.h"
#include "Config.h"
#include <Arduino.h>
#include <stdio.h>
#include <string.h>

// Управление движением дрона
extern Drone     drone;
extern HeadLight ledlight;

// Конструктор
Debug::Debug() : Connected(false)
{
	LastSended = millis();
}

// Инициализация
void Debug::Init()
{
	
}

// Обработка состояния подключения
void Debug::ProcessConnection()
{
	if(Connected)
	{
		if(WiFi.status() != WL_CONNECTED)
			onDisconnected();
		else
			onSending();
	}
	else
	{
		if(WiFi.status() == WL_CONNECTED)
			onConnected();
	}
}

// Обработать выборку
void Debug::Loop()
{
	uint32_t Time = millis();
	int32_t Diff = Time - LastSended;
	
	if(Diff > DEBUG_UDP_PERIOD)
	{
		LastSended = Time;
		ProcessConnection();
	}
}

// Получить широковещательный адрес
IPAddress Debug::getBroadcast()
{
	IPAddress ip = WiFi.localIP();
	IPAddress subnet = WiFi.subnetMask();
	
	for(int i = 0; i < 4; i++)
	{
		ip[i] |= ~subnet[i];
	}
	
	return ip;
}

// СОБЫТИЕ: WiFi отключён
void Debug::onDisconnected()
{
	Connected = false;
	
	udp.stop();
}

// СОБЫТИЕ: WiFi подключён
void Debug::onConnected()
{
	Connected = true;
	
	udp.begin(DEBUG_UDP_PORT);
}

// Отправка пакета
void Debug::Send(const void * Data, uint16_t Size)
{
	udp.beginPacket(getBroadcast(), DEBUG_UDP_PORT);
	udp.write((const uint8_t*)Data, Size);
	udp.endPacket();
}

// СОБЫТИЕ: Надо отправлять пакет
void Debug::onSending()
{
	
}

// СОБЫТИЕ: Надо отправлять пакет
void StringDebug::onSending()
{
	int depth = drone.getDepth();
	int course = drone.getCourse();
	int roll = drone.getRoll();
	int pitch = drone.getPitch();
	int speed = drone.getSpeed();
	
	char Temp[1000];
	sprintf(&Temp[0], "STATUS.BEGIN\nDEPTH=%d\nCOURSE=%d\nROLL=%d\nPITCH=%d\nSPEED=%d\nSTATUS.END\n", 
	         depth,
			 course,
			 roll,
			 pitch,
			 speed);
			 
	Send(&Temp[0], strlen(Temp));
}

typedef struct {
	uint8_t  code;
	uint8_t  flags;
	uint16_t size;
} str_debug_status_header;

typedef struct {
	int16_t depth;
	int16_t course;
	int16_t roll;
	int16_t pitch;
	
	int16_t expected_depth;
	int16_t expected_course;
	int16_t expected_speed;
	
	uint8_t speed;
	uint8_t light;
	
	uint8_t program;
	uint8_t motor_status;
	
    // Значение угла левого двигателя вертикального перемещения
    uint8_t VL;
    // Значение угла правого двигателя вертикального перемещения
    uint8_t VR;
    // Значение угла левого двигателя горизонтального перемещения
	uint8_t HL;
	// Значение угла правого двигателя горизонтального перемещения
	uint8_t HR;
    // Значение угла левого двигателя доп. перемещения
	uint8_t XL;
	// Значение угла правого двигателя доп. перемещения
	uint8_t XR;
	// Значение угла руки-хвата
	uint8_t Hand;
} str_debug_status_packet;

typedef struct {
	str_debug_status_header header;
	union {
		str_debug_status_packet status;
		
	};
} str_debug_packet;

// СОБЫТИЕ: Надо отправлять пакет
void PacketDebug::onSending()
{
	str_debug_packet packet = {
		.header = {
			.code = 1,
			.flags = 0,
			.size = sizeof(str_debug_status_packet)
		},
		.status = 
		{
			.depth = drone.getDepth(),
			.course = drone.getCourse(),
			.roll = drone.getRoll(),
			.pitch = drone.getPitch(),
			
			.expected_depth = drone.getExpectedDepth(),
			.expected_course = drone.getExpectedCourse(),
			.expected_speed = drone.GetLinearSpeed(),
			
			.speed = drone.getSpeed(),
			.light = ledlight.getValue(),
			
			.program = program_GetId(),
			.motor_status = drone.motors.GetState(),
			
			.VL = drone.motors.GetVL(),
			.VR = drone.motors.GetVR(),
			.HL = drone.motors.GetHL(),
			.HR = drone.motors.GetHR(),
			.XL = drone.motors.GetXL(),
			.XR = drone.motors.GetXR(),
			.Hand = drone.motors.GetHand()
		}
	};
	
	Send(&packet, sizeof(str_debug_status_packet) + sizeof(str_debug_status_header));
}