//
// Piranha.h
// Код управления пираньей для автономной работы
//

#include <Arduino.h>
#include "Piranha.h"

#include "Camera.h"
#include "Config.h"
#include "Drone.h"
#include "HeadLight.h"
#include "Protocol.h"
#include "SensorRGB.h"
#include "SystemLoop.h"
#include "Voltage.h"
#include <string.h>
#include <Preferences.h>

#if defined(ENABLE_RGB) && (ENABLE_RGB > 0)
// Датчик цвета
extern SensorRGB sysrgb;
#endif

// измерение напряжения
extern Voltage voltage;

// Управление внешней камерой
extern Camera camera;

// Фонарь
extern HeadLight ledlight;

// Управление движением дрона
extern Drone drone;

// Протокол
extern Protocol proto;

// Автозапуск разрешён
extern bool AutostartEnabled;

// Подождать миллисекунды
// Milliseconds: количество миллисекунд
void WaitMs(uint16_t Milliseconds)
{
	vTaskDelay(Milliseconds / portTICK_PERIOD_MS);
}

// Подождать секунды
// Seconds: количество секунд
void Wait(uint8_t Seconds)
{
	WaitMs(Seconds * 1000);
}

// Включить фонарь
void headlight::On()
{
    ledlight.setValue(255);
}

// Выключить фонарь
void headlight::Off()
{
    ledlight.setValue(0);
}

// Задать яркость фонарей
void headlight::Set(uint8_t Value)
{
    ledlight.setValue(Value);
}

// Мигнуть фонарями с заданной задержкой
// Delay: время свечения и паузы в мс
// Count: количество миганий
void headlight::Blink(int Delay, int Count)
{
	for(int i = 0; i < Count; i++)
	{
		// Включим фары как индикацию старта
		headlight::On();
		
		// Ждём Delay мс
		WaitMs(Delay);
		
		// Выключим фары
		headlight::Off();
		
		// Ждём Delay мс
		WaitMs(Delay);
	}
}

// Открыть
void hand::Open()
{
	drone.hand.SetMode(HAND_MODE_NONE);
	drone.hand.SetAngle(180);
}

// Зарыть
void hand::Close()
{
	drone.hand.SetMode(HAND_MODE_NONE);
	drone.hand.SetAngle(0);
	
}

// Задать промежуточное положение руки-хвата (0 - закрыто, 255 - открыто)
void hand::Set(uint8_t Value)
{
	uint8_t Angle = ((uint16_t)Value * 180) / 255;
	drone.hand.SetMode(HAND_MODE_NONE);
	drone.hand.SetAngle(Angle);
}
// Остановить изменение состояния руки (закрытие и открытие)
void hand::Stop()
{
	drone.hand.SetMode(HAND_MODE_NONE);
}

// Управление направлением насоса
// output: true, если выкачивание
void payload::SetDirection(bool output)
{
	drone.pump.setDirection(output);
}

// Управление состоянием нагрузки
// state: true, если включено
void payload::SetState(bool state)
{
	drone.pump.setState(state);
}

// Включить нагрузку
void payload::On()
{
	payload::SetState(true);
}

// Выключить нагрузку
void payload::Off()
{
	payload::SetState(false);
}

// Настроить WiFi, к какой точке доступа подключаться
void botsetup::WiFi(const char * SSID, const char * Password)
{
	if((strlen(SSID) < 64) && (strlen(Password) < 64))
	{
		Preferences settings;
		settings.begin("wifi-sta", false);
		
		String oldSSID     = settings.getString("ssid", "");
		String oldPassword = settings.getString("pass", "");
		
		if(strcmp(oldSSID.c_str(), SSID))
		{
			Serial.printf("Store WiFi SSID: %s\n", SSID);
			settings.putString("ssid", SSID);
		}
		
		if(strcmp(oldPassword.c_str(), Password))
		{
			Serial.printf("Store WiFi pass: %s\n", Password);
			settings.putString("pass", Password);
		}
		
		settings.end();
	}
}

// Выбор названия робота (для беспроводной прошивкИ)
void botsetup::SetName(const char * Name)
{
	if(strlen(Name) < 32)
	{
		Preferences settings;
		settings.begin("wifi-sta", false);
	
		String oldName = settings.getString("name", "");

		if(strcmp(oldName.c_str(), Name))
		{
			Serial.printf("Set piranha name: %s\n", Name);
			settings.putString("name", Name);
		}
		
		settings.end();
	}
}
		
// Сделать автостарт скрипта (если нет джойстика)
void botsetup::Autostart()
{
	AutostartEnabled = true;
}

// Разрешён ли автостарт
bool botsetup::IsAutostartEnabled()
{
	return AutostartEnabled;
}

// Выбор модуля управления дроном
void botsetup::SetComm(enum_protocol_type type)
{
	if(type < 3)
	{
		Preferences settings;
		settings.begin("communication", false);
		enum_protocol_type old = (enum_protocol_type)settings.getUInt("type", 0);
		if(old != type)
		{
			Serial.printf("Store communication type: %d\n", type);
			settings.putUInt("type", type);
		}
		
		settings.end();
		
		proto.setType(type);
	}
}

// Выбрать тип датчика глубины
// 0: 2 бар
// 1: 30 бар
void botsetup::SetDepthType(enum_depth_type value)
{
	Preferences settings;
	settings.begin("sensors", false);
	
	int depthsensor = settings.getInt("depthsensor", 0);
	if(depthsensor != value)
	{
		Serial.printf("Store depth sensor type: %d\n", value);
		settings.putInt("depthsensor", value);
		
		drone.LoadSettings();
	}
	
	settings.end();
}

// Ручная установка смещения глубины, см
void botsetup::SetDepthOffset(int32_t offset)
{
	Preferences settings;
	settings.begin("sensors", false);
	
	int zero_depth = settings.getInt("depth", 0);
	if(zero_depth != offset)
	{
		Serial.printf("Store depth zero offset: %d\n", offset);
		settings.putInt("depth", offset);
		
		drone.LoadSettings();
	}
	
	settings.end();
}

// Выбор канала
void botsetup::SetChannel(uint8_t channel)
{
	if(channel <= MAX_CHANNEL)
	{
		Preferences settings;
		settings.begin("communication", false);
		uint8_t old = settings.getUInt("channel", 0);
		if(old != channel)
		{
			Serial.printf("Store communication channel: %d\n", channel);
			settings.putUInt("channel", channel);
		}
		
		settings.end();
		
		proto.setChannel(channel);
	}
}

// Зафиксировать глубину
void bot::FixDepth()
{
	drone.FixDepth();
}

// Задать глубину погружения
// Depth: глубина, см
void bot::SetDepth(float Depth)
{
   drone.setDepth(Depth);
}

// Отключить удержание глубины
void bot::DisableDepthControl()
{
	drone.ReleaseDepth();
}

// Получить тукцщий курс
// return: курс, градусы (-180 - 180)
float bot::GetCourse()
{
	return drone.getCourse();
}

// Задать желаемый курс
// Depth: курс, градусы (-180 - 180)
void bot::SetCourse(float Course)
{
	drone.setCourse(Course);
}

// Текущий курс считать нулём
void bot::ResetCourse()
{
	drone.ResetCourse();
}

// Изменить курс (относительно текущего)
// Angle: изменение курса, градусы (-180 - 180)
void bot::Turn(float Angle)
{
	float Course = drone.getExpectedCourse() + Angle;
	if(Course > 180) Course -= 360;
	if(Course < -180) Course += 360;
	
	drone.setCourse(Course);
}

// Изменить курс вправо
// Angle: изменение курса, градусы (0 - 180)
void bot::TurnRight(float Angle)
{
	if((Angle >= 0) && (Angle <= 180))
		bot::Turn(Angle);
}

// Изменить курс влево
// Angle: изменение курса, градусы (0 - 180)
void bot::TurnLeft(float Angle)
{
	if((Angle >= 0) && (Angle <= 180))
		bot::Turn(-Angle);
}

// Включить управление моторами (если не были включены, то будет задержка на ожидание старта моторов)
void bot::MotorEnable()
{
	if(drone.IsMotorLocked())
	{
		drone.UpdateLock(false);
		WaitMs(MOTOR_WAIT_TIME_MS);
	}
}

// Выключить управление моторами
void bot::MotorDisable()
{
	drone.UpdateLock(true);
}

// Установка общей скорости движения
// Speed: скорость, в процентах, от 30 до 100
void bot::SetSpeed(uint8_t Speed)
{
	drone.SetSpeed(Speed);
}

// Остановка двигателей
void bot::Stop()
{
	drone.DisableOverrideH();
	drone.DisableOverrideV();
	drone.UpdateJoystick(0, 0, 0, 0);
}
		
// Подъём вверх
void bot::Up()
{
	drone.DisableOverrideV();
	drone.UpdateJoystickV(-127, 0);
}

// Спуск вниз
void bot::Down()
{
	drone.DisableOverrideV();
	drone.UpdateJoystickV(127, 0);
}
		
// Поворот вправо
void bot::Right()
{
	drone.DisableOverrideH();
	drone.UpdateJoystickH(0, 127);
}

// Поворот влево
void bot::Left()
{
	drone.DisableOverrideH();
	drone.UpdateJoystickH(0, -127);
}
		
// Движение вперёд
void bot::Forward()
{
	drone.DisableOverrideH();
	drone.UpdateJoystickH(-127, 0);
}

// Движение назад
void bot::Backward()
{
	drone.DisableOverrideH();
	drone.UpdateJoystickH(127, 0);
}

// Задать мощность левого мотора вертикального движения
// Value: от -127 до 127, 0 - стоп
void motors::SetVL(int8_t Value)
{
	drone.EnableOverrideVL(Value);
}

// Задать мощность правого мотора вертикального движения
// Value: от -127 до 127, 0 - стоп
void motors::SetVR(int8_t Value)
{
	drone.EnableOverrideVR(Value);
}

// Задать мощность левого мотора горизонтального движения
// Value: от -127 до 127, 0 - стоп
void motors::SetHL(int8_t Value)
{
	drone.EnableOverrideHL(Value);
}

// Задать мощность правого мотора горизонтального движения
// Value: от -127 до 127, 0 - стоп
void motors::SetHR(int8_t Value)
{
	drone.EnableOverrideHR(Value);
}

// Доступен ли датчик цвета
// return: true, если доступен
bool rgb::IsAvailable()
{
#if defined(ENABLE_RGB) && (ENABLE_RGB > 0)
	return sysrgb.isAvailable();
#else
	return false;
#endif
}

// Получить красную компоненту цвета от датчика
// return: красная компонента цвета с датчика, 0-255
uint8_t rgb::GetRed()
{
#if defined(ENABLE_RGB) && (ENABLE_RGB > 0)
	return sysrgb.getR();
#else
	return 0;
#endif
}

// Получить зелёную компоненту цвета от датчика
// return: зелёная компонента цвета с датчика, 0-255
uint8_t rgb::GetGreen()
{
#if defined(ENABLE_RGB) && (ENABLE_RGB > 0)
	return sysrgb.getG();
#else
	return 0;
#endif
}

// Получить синюю компоненту цвета от датчика
// return: синяя компонента цвета с датчика, 0-255
uint8_t rgb::GetBlue()
{
#if defined(ENABLE_RGB) && (ENABLE_RGB > 0)
	return sysrgb.getB();
#else
	return 0;
#endif
}

// Получить общую яркость света от датчика
// return: общая яркость света с датчика, 0-255
uint8_t rgb::GetBrigthness()
{
#if defined(ENABLE_RGB) && (ENABLE_RGB > 0)
	return sysrgb.getA();
#else
	return 0;
#endif
}

// Получить напряжение батареи
// return: напряжение батареи, В
float bot::GetBatteryVoltage()
{
	return voltage.getVoltage() * 0.1f;
}

// Получить глубину погружения
// return: глубина, см
float bot::GetDepth()
{
	return drone.getDepth();
}

// Получить температуру
// return: температура, С
int8_t bot::GetTemperature()
{
	return drone.getTemp();
}

// Перекалибровать датчик ориентации
void bot::Calibrate()
{
	drone.Calibrate();
}

// Перекалибровать датчик глубины
void bot::CalibrateDepth()
{
	int depth = drone.getDepth();
	
	Preferences settings;
	settings.begin("sensors", false);
	
	int zero_depth = settings.getInt("depth", 0);
	
	int fixed_offset = zero_depth + depth;
	if(zero_depth != fixed_offset)
	{
		Serial.printf("Store depth zero offset: %d\n", fixed_offset);
		settings.putInt("depth", fixed_offset);
		
		drone.LoadSettings();
	}
	
	settings.end();
}

// Начало выполнения задания (сброс глубины, скорости, курса)
void move::Reset()
{
	// Текущее положение как стартовое
	bot::ResetCourse();
	
	// Установка глубины как нулевой
	bot::CalibrateDepth();
	
	// Полная скорость: 100%
	bot::SetSpeed(100);
}

// Поворот на месте с небольшим ожиданием поворота
// Angle: угол поворота (0-180 - направо, -180-0 - налево)
// Delay: ожидание поворота в секундах
void move::Turn(int Angle, int Delay)
{
	// Сперва остановимся, если куда-то двигались
	bot::Stop();
	
	// Повернём
	bot::Turn(Angle);
	
	// Ждём
	Wait(Delay);
}

// Смена глубины на месте за заданное время
// Depth: на какую глубину переместиться
// Delay: за какое время
void move::ChangeDepth(int Depth, int Delay)
{
	int MoveTime = Delay * 1000;
	int TimeStep = 200;
	
	// Начальная глубина, от которой экстраполируем
	int StartDepth = drone.getExpectedDepth();
	
	for(int Time = 0; Time < MoveTime; Time += TimeStep)
	{
		int StepDepth = StartDepth + (Depth - StartDepth) * Time / MoveTime;
		drone.setDepth(StepDepth);
		
		// Ждём
		WaitMs(TimeStep);
	}
	
	drone.setDepth(Depth);
}
	
// Движение вперёд без смены глубины
// Delay: время движения в секундах
void move::Forward(int Delay)
{
	// Включаем движение вперёд
	bot::Forward();
	
	// Ждём
	Wait(Delay);
	
	// Останавливаемся!
	bot::Stop();
}

// Движение вперёд со сменой глубины
// Depth: на какую глубину переместиться
// Delay: время движения в секундах
void move::Forward(int Depth, int Delay)
{
	// Включаем движение вперёд
	bot::Forward();
	
	// Плавно меняем глубину
	move::ChangeDepth(Depth, Delay);
	
	// Останавливаемся!
	bot::Stop();
}
