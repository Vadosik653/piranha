//
// Pid.cpp
// Пропорционально-интегрально-дифференциальный регулятор (пока не дописано)
//

#include "Pid.h"
#include <Arduino.h>

// Настройка коэффициентов
// Kp: Коэффициент для пропорционального регулятора
// Ki: Коэффициент для интегрального регулятора
// Kd: Коэффициент для дифференциального регулятора
void PID_Controller::SetK(float Kp, float Ki, float Kd)
{
	this->Kp = Kp;
	this->Ki = Ki;
	this->Kd = Kd;
}

// Расчёт воздействия
// current: текущее значение регулируемого параметра
//      dt: прошедшее время с предыдущего обновления
//  return: величина воздействия
float PID_Controller::Regulate(float current, float dt)
{
	/*
	current - current sensor value
	target - target value
	dt - sampling period
	*/
	
	error = current - target;
	proportional = error * Kp;
	integral = integral + error * dt * Ki;
	integral = constrain(integral, -100, 100);
	derivative = (error - prev_error) / dt * Kd;
	prev_error = error;

	// Расчёт output... TODO

	output = constrain(output, -100, 100);
	return output;
}

// Назначение уставки
// value: целевое значение регулируемой величины
void PID_Controller::SetTarget(float value)
{
	this->target = value;
}