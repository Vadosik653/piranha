//
// Navigation_mpu6050.h
// Навигация (гироскоп/акселерометр)
//

#include <stdint.h>

#ifndef NAVIGATION_MPU6050_H_
#define NAVIGATION_MPU6050_H_

	#include <Preferences.h>
	#include "Navigation.h"
	#include "MPU6050_6Axis_MotionApps612.h"

	class NavigationMPU6050 : public Navigation {
	private:
		// Датчик
		MPU6050_6Axis_MotionApps612 sensor;
		
		// Эйлеровские углы
		float euler[3];
		
	public:
		NavigationMPU6050();
	
		// Инициализация периферии и выводов
		virtual void Init() override;
		
		// Обновить данные
		virtual void Update() override;
		
		// Получить курсовой угол
		virtual float getYaw() override;
		
		// Получить крен
		virtual float getRoll() override;
		
		// Получить тангаж
		virtual float getPitch() override;
		
		// Откалибровать устройство
		virtual void Calibrate() override;
		
		// Откалибровано ли устройство
		virtual bool hasCalibration() override;
	};

#endif /* VOLTAGE_H_ */