//
// Drone.cpp
// Класс управления роботом
// 

#include "Drone.h"
#include "Config.h"
#include <Arduino.h>
#include <math.h>

// Конструктор
Drone::Drone() : 
	pidDepth(&depth, &depth_power, &depth_target), 
	pidYaw(&yaw_error, &yaw_power, &yaw_target), 
	pidRoll(&roll_error, &roll_power, &roll_target), 
	depth(0), 
	depth_target(0), 
	depth_power(0), 
	depth_auto(false),
	depth_available(false),
	speed(100),
	ActualYaw(0),
	ExpectedYaw(0),
	ExpectedSpeed(0),
	YawOffset(0),
	yaw_error(0),
	yaw_power(0),
	yaw_target(0),
	roll_error(0),
	roll_power(0),
	roll_target(0),
	depth_zero(0),
	course_fix(false)
{
	pidDepth.SetTunings(DEPTH_PID_P, DEPTH_PID_I, DEPTH_PID_D);
	pidDepth.SetOutputLimits(-127, 127);
	pidDepth.SetMode(QuickPID::Control::automatic);
	pidDepth.SetControllerDirection(QuickPID::Action::reverse);
	
#if defined(ENABLE_NAVIGATION) && (ENABLE_NAVIGATION > 0)
	pidYaw.SetTunings(YAW_PID_P, YAW_PID_I, YAW_PID_D);
	pidYaw.SetOutputLimits(-127, 127);
	pidYaw.SetMode(QuickPID::Control::automatic);
	pidYaw.SetControllerDirection(QuickPID::Action::reverse);
	
	pidRoll.SetTunings(ROLL_PID_P, ROLL_PID_I, ROLL_PID_D);
	pidRoll.SetOutputLimits(-127, 127);
	pidRoll.SetMode(QuickPID::Control::automatic);
	pidRoll.SetControllerDirection(QuickPID::Action::reverse);
#endif
}

// Инициализация периферии
void Drone::Init()
{
	hand.Reset();
	motors.Init();
	pump.Init();
#if defined(ENABLE_PRESSURE) && (ENABLE_PRESSURE > 0)
	pressure.Init();
#endif
#if defined(ENABLE_COMPASS) && (ENABLE_COMPASS > 0)
	ori.Init();
#endif
#if defined(ENABLE_NAVIGATION) && (ENABLE_NAVIGATION > 0)
	navigation.Init();
#endif

	LoadSettings();
	UpdateSensors();
	
	depth_target = depth;
}

// Загрузить настройки
void Drone::LoadSettings()
{
	Preferences settings;
	settings.begin("sensors", true);
	depth_zero = settings.getInt("depth", 0);
	
	int type = settings.getInt("depthsensor", 0);
	switch(type)
	{
		case 0: pressure.SetType(ENUM_MS5803_02); break;
		case 1: pressure.SetType(ENUM_MS5803_30); break;
	}
	settings.end();
}

void Drone::UpdateSensors()
{
#if defined(ENABLE_PRESSURE) && (ENABLE_PRESSURE > 0)
	pressure.Update();
#endif

#if defined(ENABLE_COMPASS) && (ENABLE_COMPASS > 0)
	ori.Update();
#endif

#if defined(ENABLE_NAVIGATION) && (ENABLE_NAVIGATION > 0)
	navigation.Update();
	
	if(navigation.isAvailable())
	{
		ActualYaw = navigation.getYaw();
		yaw_error = CalcYawError(getCourse());
		roll_error = navigation.getRoll() / 5.0f;
	}
	else
	{
		ActualYaw = 0;
		yaw_error = 0;
		roll_error = 0;
	}
#endif
  
	depth = getDepth();
	
	//Serial.printf("Depth: %f Press: %f zero: %d\n", depth, pressure.getPressure(), depth_zero);
}

void Drone::ReleaseDepth(void)
{
    depth_auto = false;
}

// Зафиксировать глубину текущую
void Drone::FixDepth(void)
{
	depth_target = depth;
	depth_available = true;
}

// Задать скорость
// Speed: скорость в процентах, 0 - 100%
void Drone::SetSpeed(uint8_t Speed)
{
	if ((Speed >= 30) && (Speed <= 100))
	{
		speed = Speed;
	}
}

// Получить скорость
// return: скорость в процентах, 0 - 100%
uint8_t Drone::getSpeed()
{
	return speed;
}

// Нормализация угла
// Angle: угол
// return: нормализованный угол (от -180 до 180)
float Drone::NormalizeAngle(float Angle)
{
	while(Angle > 180)  Angle -= 360;
	while(Angle < -180) Angle += 360;
	
	return Angle;
}

// Получить курс (если включён модуль ориентации)
// return: курс, градусы (-180, 180)
float Drone::getCourse()
{
	return NormalizeAngle(ActualYaw - YawOffset);
}

// Получить желаемый курс (если включён модуль ориентации)
// return: курс, градусы (-180, 180)
float Drone::getExpectedCourse()
{
	return ExpectedYaw;
}

// Получить крен
// return: крен, градусы
float Drone::getRoll()
{
#if defined(ENABLE_NAVIGATION) && (ENABLE_NAVIGATION > 0)
	if(navigation.isAvailable())
		return navigation.getRoll();
	else
#endif
		return 0;
}

// Получить тангаж
// return: тангаж, градусы
float Drone::getPitch()
{
#if defined(ENABLE_NAVIGATION) && (ENABLE_NAVIGATION > 0)
	if(navigation.isAvailable())
		return navigation.getPitch();
	else
#endif
		return 0;
}

// Текущий курс задать как 0
void Drone::ResetCourse()
{
	YawOffset = ActualYaw;
	ExpectedYaw = 0;
}

// Задать курс (если включён модуль ориентации)
// Depth: курс, градусы (-180, 180)
void Drone::setCourse(float Course)
{
	ExpectedYaw = NormalizeAngle(Course);
}

// Задать глубину в см
// Depth: глубина, см
void Drone::setDepth(float Depth)
{
	Serial.printf("Depth stabilization is enabled to depth: %f\n", Depth);
	depth_target = Depth;
	depth_available = true;
	depth_auto = true;
}

// Задать угол поворота камеры
// CameraPitch: угол (0-180)
void Drone::UpdateCamera(uint8_t CameraPitch)
{
	CamPitch = CameraPitch;
}

// Обновить блокировку моторов
// Lock: true, если заблокировано
void Drone::UpdateLock(bool Lock)
{
	bool Enabled = motors.GetState();
	bool ReqEnabled = !Lock;
	
	if(Enabled != ReqEnabled)
	{
		ResetCourse();
		motors.SetState(ReqEnabled);
		
		depth_target = getDepth();

		motors.SetVL(90);
		motors.SetVR(90);
		motors.SetHL(90);
		motors.SetHR(90);
		motors.SetCam(90);
		motors.SetHand(90);
	}
}

// Установлена ли блокировка мотора
// return: true, если заблокировано
bool Drone::IsMotorLocked()
{
	return !motors.GetState();
}

// Пересчёт управляющего сигнала с учётом внесёноой зоны безопасности
//  Value: входной сигнал с джойстика
// return: исправленный сигнал джойстика (часть зоны управления отключена и смещена)
uint8_t Drone::SafeZone(int8_t Value)
{
	if(Value > JOYSTICK_SAFE_ZONE)
		return 0x80 + (int16_t)(Value - JOYSTICK_SAFE_ZONE) * 0x80 / (0x80 - JOYSTICK_SAFE_ZONE);
	else if(Value < -JOYSTICK_SAFE_ZONE)
		return 0x80 + (int16_t)(Value + JOYSTICK_SAFE_ZONE) * 0x80 / (0x80 - JOYSTICK_SAFE_ZONE);
	else
		return 0x80;
}

// Обновить параметры джойстика (верх-0,низ-255,лево-0,право-255)
//       UpVal: Значения с джойстика для смены глубины (правый джойстик, вверх-вниз)
// RollLeftVal: Значения с джойстика для смены угла крена (правый джойстик, лево-право)
//  ForwardVal: Значения с джойстика для движения вперёд-назад (левый джойстик, вверх-вниз)
//  YawLeftVal: Значения с джойстика для горизонтельного поворота (левый джойстик, лево-право)
void Drone::UpdateJoystick(int8_t UpVal, int8_t RollLeftVal, int8_t ForwardVal, int8_t YawLeftVal)
{
	UpdateJoystickH(ForwardVal, YawLeftVal);
	UpdateJoystickV(UpVal, RollLeftVal);
}

// Обновить параметры джойстика (верх-0,низ-255,лево-0,право-255)
//  ForwardVal: Значения с джойстика для движения вперёд-назад (левый джойстик, вверх-вниз)
//  YawLeftVal: Значения с джойстика для горизонтельного поворота (левый джойстик, лево-право)
void Drone::UpdateJoystickH(int8_t ForwardVal, int8_t YawLeftVal)
{
	uint8_t sForw = SafeZone(constrain((int16_t)ForwardVal * speed / 100, -127, 127));
	uint8_t sYaw = SafeZone(constrain((int16_t)YawLeftVal * speed / 100, -127, 127));
	
	// ForwardVal  => правый джойстик: вверх 0, вниз  255
	// YawLeftVal  => правый джойстик: лево  0, право 255

	HorisontalMove(sForw, sYaw);
}

// Обновить параметры джойстика только по вертикали (верх-0,низ-255,лево-0,право-255)
//       UpVal: Значения с джойстика для смены глубины (правый джойстик, вверх-вниз)
// RollLeftVal: Значения с джойстика для смены угла крена (правый джойстик, лево-право)
void Drone::UpdateJoystickV(int8_t UpVal, int8_t RollLeftVal)
{
	uint8_t sUp = SafeZone(constrain((int16_t)UpVal * speed / 100, -127, 127));
	uint8_t sRoll = SafeZone(constrain((int16_t)RollLeftVal * speed / 100, -127, 127));
	
	// UpVal       => левый  джойстик: вверх 0, вниз  255
	// RollLeftVal => левый  джойстик: лево  0, право 255

	VerticalMove(sUp, sRoll);
}

// Разрешить перезапись управления горизонтальными моторами
// Value: значение сигнала управления для левого двигателя горизонтальной ориентации
void Drone::EnableOverrideHL(int8_t Value)
{
	if(!OverrideHorisontal)
		OverrideHR = HRraw;
	
	OverrideHorisontal = true;
	OverrideHL = Value;
	HLraw = Value;
}

// Разрешить перезапись управления горизонтальными моторами
// Value: значение сигнала управления для правого двигателя горизонтальной ориентации
void Drone::EnableOverrideHR(int8_t Value)
{
	if(!OverrideHorisontal)
		OverrideHL = HLraw;
	
	OverrideHorisontal = true;
	OverrideHR = Value;
	HRraw = Value;
}

// Разрешить перезапись управления вертикальными моторами
// Value: значение сигнала управления для левого двигателя вертикальной ориентации
void Drone::EnableOverrideVL(int8_t Value)
{
	if(!OverrideVertical)
		OverrideVR = upRraw;
	
	OverrideVertical = true;
	OverrideVL = Value;
	upLraw = Value;
}

// Разрешить перезапись управления вертикальными моторами
// Value: значение сигнала управления для правого двигателя вертикальной ориентации
void Drone::EnableOverrideVR(int8_t Value)
{
	if(!OverrideVertical)
		OverrideVL = upLraw;
	
	OverrideVertical = true;
	OverrideVR = Value;
	upRraw = Value;
}

// Запретить перезапись управления вертикальными моторами
void Drone::DisableOverrideV()
{
	OverrideVertical = false;
}

// Запретить перезапись управления вертикальными моторами
void Drone::DisableOverrideH()
{
	OverrideHorisontal = false;
}

// Задать стабилизацию курса
void Drone::SetCourseStab(bool Stab)
{
	course_fix = Stab;
}

// Задать стабилизацию глубины
void Drone::SetDepthStab(bool Stab)
{
	if(depth_auto != Stab)
	{
		if(Stab)
		{
			Serial.println("Depth stabilization is enabled");
			FixDepth();
		}
		else
		{
			Serial.println("Depth stabilization is disabled");
			ReleaseDepth();
		}
		
		depth_auto = Stab;
	}
}

// Перекалибровать датчики
void Drone::Calibrate()
{
#if defined(ENABLE_NAVIGATION) && (ENABLE_NAVIGATION > 0)
	if(navigation.isAvailable())
	{
		navigation.Calibrate();
	}
#endif
}

// Управление вертикальным движением
//       UpVal: Значения с джойстика для смены глубины (правый джойстик, вверх-вниз)
// RollLeftVal: Значения с джойстика для смены угла крена (правый джойстик, лево-право)
void Drone::VerticalMove(uint8_t UpVal, uint8_t RollLeftVal)
{
	// UpVal       => вверх 0, вниз  255
	// RollLeftVal => лево  0, право 255
	if(!IsInRange(UpVal, 128-UPDOWN_JOYSTICK_NOISE_HISTERESIS, 128+UPDOWN_JOYSTICK_NOISE_HISTERESIS))
	{
		if(depth_auto)
		{
			float depth_change = DEPTH_CHANGE_COEFF * (128 - (int16_t)UpVal) / 128;
			
			depth_target -= depth_change;
			if(depth_target > depth)
			{
				if(depth_target - depth > 50)
					depth_target = depth + 50;
			}
			else
			{
				if(depth - depth_target > 50)
					depth_target = depth - 50;
			}
			//Serial.printf("Depth: %f => %f\n", depth, depth_target);
		}
		else
		{
			upLraw = (128 - (int16_t)UpVal) - (128 - (int16_t)RollLeftVal) / ROLL_COEFF;
			upRraw = (128 - (int16_t)UpVal) + (128 - (int16_t)RollLeftVal) / ROLL_COEFF;
			//Serial.printf("Updown: %d, %d\n", upLraw, upRraw);
		}
	}
	else
	{
		upLraw = 0;
		upRraw = 0;
	}
	
	// upLraw => левый двигатель вертикальной ориентации
	// upRraw => правый двигатель вертикальной ориентации
}

// Управление горизонтальным движением
//  ForwardVal: Значения с джойстика для движения вперёд-назад (левый джойстик, вверх-вниз)
//  YawLeftVal: Значения с джойстика для горизонтельного поворота (левый джойстик, лево-право)
void Drone::HorisontalMove(uint8_t ForwardVal, uint8_t YawLeftVal)
{
	// ForwardVal  => вверх 0, вниз  255
	// YawLeftVal  => лево  0, право 255
	if(course_fix)
		YawLeftVal = 0x80;
	
#if defined(ENABLE_NAVIGATION) && (ENABLE_NAVIGATION > 0)
	if(navigation.isAvailable())
	{
		int8_t Rel = 0x80 - YawLeftVal;
		int16_t Temp = ExpectedYaw - (YAW_CHANGE_COEFF * (int16_t)Rel) / 128;
		while(Temp < -180) Temp += 360;
		while(Temp > 180) Temp -= 360;
		
		ExpectedYaw = Temp;
		ExpectedSpeed = 0x80 - ForwardVal;
		//Serial.printf("  Yaw: %f => %f\n", ActualYaw, ExpectedYaw);
	}
	else
#endif
	{
		HLraw = -(128-(int16_t)ForwardVal) + ((128-(int16_t)YawLeftVal) / YAW_COEFF);
		HRraw = -(128-(int16_t)ForwardVal) - ((128-(int16_t)YawLeftVal) / YAW_COEFF);
	}
}

// Расчёт вертикальной силы для удержаия глубины
void Drone::AutoDepth()
{
	int8_t r_power = 0;
	if(!depth_available)
		FixDepth();
	
	pidDepth.Compute();
#if defined(ENABLE_NAVIGATION) && (ENABLE_NAVIGATION > 0)
	if(navigation.isAvailable())
	{
		pidRoll.Compute();
		r_power = constrain((int16_t)roll_power, -ROLL_MAX_POWER, ROLL_MAX_POWER);
	}
#endif

	// Работа по PID
	if(depth_available)
	{
		int8_t power = constrain((int16_t)depth_power, -DEPTH_MAX_POWER, DEPTH_MAX_POWER);

		upLraw = power - r_power;
		upRraw = power + r_power;
		//Serial.printf("Autodepth power: %d\n", power);
	}
	else
	{
		//Serial.println("No depth");
		upLraw = 0;
		upRraw = 0;
	}
}

// Получить линейную скорость
// return: текущая линейная скорость
int16_t Drone::GetLinearSpeed()
{
	return ExpectedSpeed;
}

// Расчёт повротов для удержания курса
void Drone::AutoYaw()
{
	pidYaw.Compute();
	
	int16_t power = constrain((int16_t)yaw_power, -127, 127);
	
	HLraw = -ExpectedSpeed - (power / YAW_COEFF);
	HRraw = -ExpectedSpeed + (power / YAW_COEFF);
	
	//Serial.printf("Yaw power %d (%02f => %02f error: %02f)\n", power, ActualYaw, ExpectedYaw, yaw_error);
}

// Пересчитать управляющие значения на значения, используемые в моторах и применить их
void Drone::SendToMotors()
{
	if(depth_auto)
		AutoDepth();
	//else
	//	Serial.printf("Autodepth disabled\n");

#if defined(ENABLE_NAVIGATION) && (ENABLE_NAVIGATION > 0)
	if(navigation.isAvailable())
		AutoYaw();
#endif

	int16_t upLs = OverrideVertical ? constrain((int16_t)OverrideVL * 2, -256, 256) : upLraw;
	int16_t upLr = (UL_DIR < 0) ? -upLs : upLs;
 
	int16_t upRs = OverrideVertical ? constrain((int16_t)OverrideVR * 2, -256, 256) : upRraw;
	int16_t upRr = (UR_DIR < 0) ? -upRs : upRs;
 
	int16_t HLs = OverrideHorisontal ? constrain((int16_t)OverrideHL * 2, -256, 256) : HLraw;
	int16_t HLr = (HL_DIR < 0) ? -HLs : HLs;
  
	int16_t HRs = OverrideHorisontal ? constrain((int16_t)OverrideHR * 2, -256, 256) : HRraw;
	int16_t HRr = (HR_DIR < 0) ? -HRs : HRs;

	// Пересчитываем все значения в угол поворота серво (хотя, конечно, двигатель будет вращаться, это просто компактная форма записи длительности сигнала)
	uint8_t upL = map((int)(upLr / UL_COEFF),-128 - (128/ROLL_COEFF),128+(128/ROLL_COEFF),0,179);
	uint8_t upR = map((int)(upRr / UR_COEFF),-128 - (128/ROLL_COEFF),128+(128/ROLL_COEFF),0,179);
	uint8_t HL = map((int)(HLr / HL_COEFF),-128 - (128/YAW_COEFF),128+(128/YAW_COEFF),0,179);
	uint8_t HR = map((int)(HRr / HR_COEFF),-128 - (128/YAW_COEFF),128+(128/YAW_COEFF),0,179);
  
	uint8_t CP = map(CamPitch,0,255,0,179);
  
	hand.Update();
	if(!IsMotorLocked())
	{
		motors.SetVL(upL);
		motors.SetVR(upR);
		motors.SetHL(HL);
		motors.SetHR(HR);
		motors.SetCam(CP);
		motors.SetHand(hand.GetAngle());
	}
	//Serial.printf("Hand angle %d\n", hand.GetAngle());
}

// Расчёт ошибки угла поворота
float Drone::CalcYawError(float Yaw)
{
#if defined(ENABLE_NAVIGATION) && (ENABLE_NAVIGATION > 0)
	float diff = ExpectedYaw - Yaw;
	int16_t error = 0;
	
	if((diff <= 180) && (diff >= -180))
		error = diff;
	else if(diff > 180)
		error = diff - 360;
	else
		error = diff + 360;
	
	// error: в диапазоне от -180 до 180 на входе
	return constrain(error, -MAX_YAW_ERROR, MAX_YAW_ERROR);
#else
	return 0;
#endif
}

// Находится ли значение в указанном диапазоне (не включая границы)
//  Value: проверяемое значение
//    Min: нижняя граница диапазона
//    Max: верхняя граница диапазона
// return: true, если попадает
bool Drone::IsInRange(uint8_t Value, uint8_t Min, uint8_t Max)
{
	return (Value > Min) && (Value < Max);
}

// Получить ожидаемую глубину (в некоторых единицах)
// return: глубина
float Drone::getExpectedDepth()
{
	return depth_target;
}

// Получить глубину (в некоторых единицах)
// return: глубина
float Drone::getDepth()
{
#if defined(ENABLE_PRESSURE) && (ENABLE_PRESSURE > 0)
	return (pressure.getDepth() * 10) - depth_zero;
#else
	return 0;
#endif
}

// Получить курсовой угол (в случае работы с компасом)
// return: угол (!!!некорректный, нужна привязка к акселерометру или работа от гироскопа)
int16_t Drone::getAngle()
{
#if defined(ENABLE_COMPASS) && (ENABLE_COMPASS > 0)
	return ori.getAngle();
#else
	return 0;
#endif
}

// Получить температуру воды (в случае работы с датчиком давления)
// return: температура воды, в градусах
int8_t Drone::getTemp()
{
#if defined(ENABLE_PRESSURE) && (ENABLE_PRESSURE > 0)
	return pressure.getTemp();
#else
	return 0;
#endif
}
