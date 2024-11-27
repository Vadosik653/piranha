//
// ProtocolSlave.cpp
// Протокол связи для ведомого блока
//

#include "ProtocolSlave.h"
#include "Protocol.h"
#include "Program.h"
#include "HeadLight.h"
#include "Hand.h"
#include "Voltage.h"
#include "Temperature.h"
#include "Drone.h"
#include "Config.h"
#include <Preferences.h>

// Управление движением дрона
extern Drone            drone;

// Фонарь
extern HeadLight        ledlight;

// измерение напряжения
extern Voltage          voltage;

// Датчик температуры
extern TemperatureTMP36 temp;

// Протокол
Protocol proto;

// Время последнего пакета управления
uint32_t last_packet_time = 0;

// Инициализация протокола
void proto_Init()
{
	Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);
	
	proto.begin(&Serial2);

	Preferences settings;
	settings.begin("communication", true);
	uint8_t channel = settings.getUInt("channel", 0);
	uint8_t type = settings.getUInt("type", TRANSMITTER);
	settings.end();
	
	proto.setType(type);
	proto.setChannel(channel);
}

static void SendStatus(void)
{
	str_packet_status_main status;
	
	// Заполняем данные для отправки в буй
	status.BattVolt = voltage.getVoltage(); // Напряжение батареи
	status.ROVTemp = temp.getTemperature(); // Температура дрона
	status.ROVDepth = drone.getDepth();     // Глубина
	status.WaterTemp = drone.getTemp();     // Температура воды

	status.Yaw   = drone.getCourse();      // Курсовой угол
	status.Roll  = drone.getRoll();        // Крен
	status.Pitch = drone.getPitch();       // Тангаж
	
	status.Speed = drone.getSpeed();
	status.Led   = ledlight.getValue();
	status.Prog  = program_GetId();
	status.Flags = 0;
	
	proto.send(PACKET_CODE_STATUS_MAIN, &status, sizeof(status));
}

// Пересчёт управляющего сигнала в знаковый
//  Value: входной сигнал с джойстика
// return: исправленный сигнал джойстика
int8_t ConvertToSigned(uint8_t Value)
{
	return (int16_t)Value - 0x80;
}

// Контрольный пакет
static void onControlPacket(str_packet_ctrl_joystick * packet)
{
	if(!program_IsLocalControl() && program_IsSystemStarted())
	{
		drone.UpdateLock(packet->Lock == 1);
		drone.UpdateJoystick(ConvertToSigned(packet->UpVal), ConvertToSigned(packet->RollLeftVal), ConvertToSigned(packet->ForwardVal), ConvertToSigned(packet->YawLeftVal));
		drone.SetSpeed(packet->Speed);
	
		drone.SetDepthStab((packet->Flags & CONTROL_FLAG_DEPTH_STAB) != 0);
		drone.SetCourseStab((packet->Flags & CONTROL_FLAG_COURSE_STAB) != 0);
	
		if (packet->Flags & CONTROL_FLAG_HAND_OPEN)
		{
			drone.pump.setDirection(true);
			drone.pump.setState(true);
			drone.hand.SetMode(HAND_MODE_OPEN);
		}
		else if (packet->Flags & CONTROL_FLAG_HAND_CLOSE)
		{
			drone.pump.setDirection(false);
			drone.pump.setState(true);
			drone.hand.SetMode(HAND_MODE_CLOSE);
		}
		else
		{
			drone.pump.setState(false);
			drone.hand.SetMode(HAND_MODE_NONE);
		}
	
		ledlight.setValue(packet->Led);
	}
	
	if(packet->Flags & CONTROL_FLAG_SEND_STATUS)
		SendStatus();
	
	//Serial.write("Received Control!..\n");
}

// Тип управления через прямые параметры
static void onOrientationPacket(str_packet_ctrl_orientation * packet)
{
	if(!program_IsLocalControl() && program_IsSystemStarted())
	{
		uint8_t HandAngle = ((uint16_t)packet->Hand * 180) / 255;
		
		//Serial.write("Received Orientation!..\n");
		drone.SetDepthStab(true);
		drone.SetCourseStab(true);
		drone.SetSpeed(100);
		
		drone.setCourse(packet->Yaw);
		drone.setDepth(packet->Depth);
		ledlight.setValue(packet->Led);
		drone.hand.SetMode(HAND_MODE_NONE);
		drone.hand.SetAngle(HandAngle);
	}
}

// Контрольный пакет
static void onProgramPacket(str_packet_ctrl_program * packet)
{
	//Serial.write("Received Program!..\n");
	if(program_IsSystemStarted())
		program_Select(packet->Program);
}

// Главный цикл
void proto_Main()
{
	uint8_t buffer[PROTO_BUFFER_SIZE];
	uint8_t size = PROTO_BUFFER_SIZE;
	enum_packet_code code = PACKET_CODE_UNKNOWN;
	
	if(proto.receive(&code, &buffer[0], &size))
	{
		last_packet_time = millis();
	
		//Serial.write("received...\n");
		switch(code)
		{
		case PACKET_CODE_CONTROL_JOYSTICK:
			onControlPacket((str_packet_ctrl_joystick *)&buffer[0]);
			break;
		case PACKET_CODE_CONTROL_PROGRAM:
			onProgramPacket((str_packet_ctrl_program *)&buffer[0]);
			break;
		case PACKET_CODE_CONTROL_ORIENTATION:
			onOrientationPacket((str_packet_ctrl_orientation *)&buffer[0]);
			break;
		}
	}
	
	if(!program_IsLocalControl())
	{
		if(last_packet_time > 0)
		{
			uint32_t now = millis();
			uint32_t diff = now - last_packet_time;
			if(diff > JOYSTICK_TIMEOUT)
			{
				// Потеря связи. Выключаем всё
				Serial.write("Joystick is lost!\n");
				last_packet_time = 0;
				
				drone.UpdateLock(true);
				drone.UpdateJoystick(0, 0, 0, 0);
				ledlight.setValue(0);
				drone.hand.SetMode(HAND_MODE_NONE);
			}
		}
	}
}
