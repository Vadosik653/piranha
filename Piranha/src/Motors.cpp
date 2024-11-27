//
// Motors.cpp
// Управление моторами (ESP32)
//

#include "Motors.h"
#include "Config.h"
#include <Arduino.h>
#include <esp32-hal-ledc.h>

// Количество выдаваемых сигналов управления двигателем
#define PIN_COUNT            7

// Частота обновления, Гц
#define REFRESH_FREQ         50

// Период ШИМ, мкс
#define REFRESH_USEC         (1000000L / REFRESH_FREQ)

// Размерность счётчика таймера, бит
#define TIMER_WIDTH          16

// Количество тиков таймера
#define TIMER_WIDTH_TICKS    (1UL << TIMER_WIDTH)

// Описание управляемого вывода
typedef struct {
	// Нужно ли управляеть этим выводом
	bool               Avail;
	// Вывод
	int                Pin;
} TMotorPin;

// Выполняется ли сейчас генерация импульса
static bool    Busy;
// Разрешена ли генерация импульсов на выводах
static bool    GenerationEnabled;

// Таблица выводов
static TMotorPin Pins[PIN_COUNT] = {
  { true,  26 }, // GPIO26 (лев. верт)
  { true,  33 }, // GPIO33 (прав. верт)
  { true,  25 }, // GPIO25 (лев. гор)
  { true,  32 }, // GPIO32 (прав. гор)
  { true,  12 }, // GPIO12 (лев. ххх)
  { true,  14 }, // GPIO14 (прав. ххх)
  { true,  5  }  // GPIO5  (рука)
};

// Инициализация периферии
void Motors::Init()
{
	GenerationEnabled = false;
  
	for(int i = 0; i < PIN_COUNT; i++)
	{
		ledcSetup(i, REFRESH_FREQ, TIMER_WIDTH);
		ledcAttachPin(Pins[i].Pin, i);   
	}
 
	// Все сервоприводы в нейтральное положение (остановка)
	SetVL(90);
	SetVR(90);
	SetHL(90);
	SetHR(90);
	SetXL(90);
	SetXR(90);
	SetHand(90);
}

// Пересчёт угла поворота сервопривода в эквивалентную длину импульса
//  Angle: угол поворота поворотного сервопривода
// return: длина импульса, мкс
uint16_t Motors::CalcTime(uint8_t Angle)
{
	return ESC_MIDDLE + (int16_t)ESC_DISTANCE * (Angle - 90) / 90;
}

// Задать длину импульса для левого двигателя вертикальной ориентации
// Angle: угол поворота поворотного сервопривода (как эквивалент ширины импульса)
void Motors::SetVL(uint8_t Angle)
{
	VL = Angle;
	
	SetAngle(0, Angle);
}

// Задать длину импульса для правого двигателя вертикальной ориентации
// Angle: угол поворота поворотного сервопривода (как эквивалент ширины импульса)
void Motors::SetVR(uint8_t Angle)
{
	VR = Angle;
	
	SetAngle(1, Angle);
}

// Задать длину импульса для левого двигателя горизонтальной ориентации
// Angle: угол поворота поворотного сервопривода (как эквивалент ширины импульса)
void Motors::SetHL(uint8_t Angle)
{
	HL = Angle;
	
	SetAngle(2, Angle);
}

// Задать длину импульса для правого двигателя горизонтальной ориентации
// Angle: угол поворота поворотного сервопривода (как эквивалент ширины импульса)
void Motors::SetHR(uint8_t Angle)
{
	HR = Angle;
	
	SetAngle(3, Angle);
}

// Задать длину импульса для левого двигателя доп. ориентации
// Angle: угол поворота поворотного сервопривода (как эквивалент ширины импульса)
void Motors::SetXL(uint8_t Angle)
{
	XL = Angle;
	
	SetAngle(4, Angle);
}

// Задать длину импульса для правого двигателя доп. ориентации
// Angle: угол поворота поворотного сервопривода (как эквивалент ширины импульса)
void Motors::SetXR(uint8_t Angle)
{
	XR = Angle;
	
	SetAngle(5, Angle);
}

// Задать длину импульса для двигателя руки-хвата
// Angle: угол поворота поворотного сервопривода (как эквивалент ширины импульса)
void Motors::SetHand(uint8_t Angle)
{
	Hand = Angle;
	
	SetAngle(6, Angle);
}

// Задать угол поворота камеры (пока что не работает)
// Angle: угол поворота поворотного сервопривода
void Motors::SetCam(uint8_t Angle) { }

// Задать скважность ШИМ
// Index: номер канала серво
// Angle: угол поворота поворотного сервопривода
void Motors::SetAngle(uint8_t Index, uint8_t Angle)
{
	if(Index < PIN_COUNT)
	{
#if defined(ENABLE_MOTOR_ALWAYS_ENABLES) && (ENABLE_MOTOR_ALWAYS_ENABLES != 0)
		uint16_t Time = CalcTime(GenerationEnabled ? Angle : 90);
		uint16_t Ticks = (Time * TIMER_WIDTH_TICKS) / REFRESH_USEC;
		
		ledcWrite(Index, Ticks);
#else
		if(GenerationEnabled)
		{
			uint16_t Time = CalcTime(Angle);
			uint16_t Ticks = (Time * TIMER_WIDTH_TICKS) / REFRESH_USEC;
			
			ledcWrite(Index, Ticks);
		}
		else
			ledcWrite(Index, 0);
#endif
	}
}

// Задать угол для всех моторов
// Angle: угол поворота поворотного сервопривода
void Motors::SetAll(uint8_t Angle)
{
	for(int i = 0; i < PIN_COUNT - 1; i++)
		SetAngle(i, Angle);
}

// Процедура включения всех моторов
void Motors::Startup()
{
	SetAll(90);
	vTaskDelay(500);
	SetAll(180);
	vTaskDelay(500);
	SetAll(0);
	vTaskDelay(500);
	SetAll(90);
	vTaskDelay(500);
}

// Разрешить или запретить генерацию испульсов
// Enabled: true, если генерация разрешена
void Motors::SetState(bool Enabled)
{
	if(Enabled != GenerationEnabled)
	{
		GenerationEnabled = Enabled;
		
		if(Enabled)
		{
			Startup();
			
			SetVL(VL);
			SetVR(VR);
			SetHL(HL);
			SetHR(HR);
			SetXL(XL);
			SetXR(XR);
			SetHand(Hand);
		}
	}
}

// Значение угла левого двигателя вертикального перемещения
uint8_t Motors::GetVL()
{
	return VL;
}

// Значение угла правого двигателя вертикального перемещения
uint8_t Motors::GetVR()
{
	return VR;
}

// Значение угла левого двигателя горизонтального перемещения
uint8_t Motors::GetHL()
{
	return HL;
}

// Значение угла правого двигателя горизонтального перемещения
uint8_t Motors::GetHR()
{
	return HR;
}

// Значение угла левого двигателя доп. перемещения
uint8_t Motors::GetXL()
{
	return XL;
}

// Значение угла правого двигателя доп. перемещения
uint8_t Motors::GetXR()
{
	return XR;
}

// Значение угла руки-хвата
uint8_t Motors::GetHand()
{
	return Hand;
}

// Разрешена ли генерация ШИМ
// return: true, если генерация разрешена
bool Motors::GetState()
{
	return GenerationEnabled;
}
