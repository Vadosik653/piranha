//
// Hand.cpp
// Рука-хват
//

#include "Hand.h"
#include "Pinout.h"
#include "Config.h"
#include <Arduino.h>

// Конструктор
Hand::Hand() : Angle(90), Mode(HAND_MODE_NONE)
{
	
}

// Сброс
void Hand::Reset()
{
	Angle = 90;
}

// Режим руки
void Hand::SetMode(enum_hand_mode M)
{
	Mode = M;
}

// Открыть руку
void Hand::Open()
{
	if((Angle + HAND_STEP) < HAND_MAX_POS)
		Angle += HAND_STEP;
	else
		Angle = HAND_MAX_POS;
}

// Закрыть руку
void Hand::Close()
{
	if((Angle - HAND_STEP) > HAND_MIN_POS)
		Angle -= HAND_STEP;
	else
		Angle = HAND_MIN_POS;
}

// Задать угол
void Hand::SetAngle(uint8_t Angle)
{
	uint8_t Scaled = map(Angle, 0, 180, HAND_MIN_POS, HAND_MAX_POS);
	if(Scaled <= 180)
	{
		this->Angle = Scaled;
	}
}

// Обновить положение
void Hand::Update()
{
	switch(Mode)
	{
	case HAND_MODE_OPEN: Open(); break;
	case HAND_MODE_CLOSE: Close(); break;
	}
}

// Получить угол
uint8_t Hand::GetAngle()
{
	return Angle;
}