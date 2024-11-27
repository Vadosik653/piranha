//
// Navigation_mpu6050.cpp
// Навигация (гироскоп/акселерометр)
//

#include "Navigation_mpu6050.h"
#include <Arduino.h>

NavigationMPU6050::NavigationMPU6050() : Navigation()
{
	
}

// Инициализация периферии и выводов
void NavigationMPU6050::Init()
{
	sensor.initialize();
	
	if(sensor.testConnection())
	{
		uint8_t devStatus = sensor.dmpInitialize();
		if (devStatus == 0)
		{
			if(hasCalibration())
			//	Calibrate();
			//else
				Serial.println("MPU6050 calibration is available");
			
			// Настройки
			Preferences settings;
		
			settings.begin("nav-mpu6050", true);
			int gyrox  = settings.getInt("gyrox",  0);
			int gyroy  = settings.getInt("gyroy",  0);
			int gyroz  = settings.getInt("gyroz",  0);
			int accelx = settings.getInt("accelx", 0);
			int accely = settings.getInt("accely", 0);
			int accelz = settings.getInt("accelz", 0);
			
			sensor.setXGyroOffset(gyrox);
			sensor.setYGyroOffset(gyroy);
			sensor.setZGyroOffset(gyroz);
			sensor.setXAccelOffset(accelx);
			sensor.setYAccelOffset(accely);
			sensor.setZAccelOffset(accelz);
			
			settings.end();
			
			
			sensor.setDMPEnabled(true);
		}
		else
			Serial.println("MPU6050 DMP initialization failed");
	}
	else
		Serial.println("MPU6050 connection failed");
	
}

// Обновить данные
void NavigationMPU6050::Update()
{
	uint8_t fifoBuffer[64];
	if (sensor.dmpGetCurrentFIFOPacket(fifoBuffer))
	{
		// Квантерион
		Quaternion q;
		
		sensor.dmpGetQuaternion(&q, fifoBuffer);
		sensor.dmpGetEuler(euler, &q);
		setAvailable(true);
	}
}

// Откалибровать устройство
void NavigationMPU6050::Calibrate()
{
	bool isDMPEnabled = sensor.getDMPEnabled();
	
	if(isDMPEnabled) sensor.setDMPEnabled(false);
	
	Serial.println("Calibration.");
	sensor.CalibrateAccel(12);
	sensor.CalibrateGyro(12);
	int16_t * offsets = sensor.GetActiveOffsets();
	
	Preferences settings;
	settings.begin("nav-mpu6050", false);
	settings.putInt("calibrated", 1);
	settings.putInt("accelx", offsets[0]);
	settings.putInt("accely", offsets[1]);
	settings.putInt("accelz", offsets[2]);
	settings.putInt("gyrox",  offsets[3]);
	settings.putInt("gyroy",  offsets[4]);
	settings.putInt("gyroz",  offsets[5]);
	settings.end();
	
	settings.begin("navigation-mpu6050", true);
	settings.end();
	
	Serial.println("Calibration ok.");
	if(isDMPEnabled) sensor.setDMPEnabled(true);
}

// Откалибровано ли устройство
bool NavigationMPU6050::hasCalibration()
{
	Preferences settings;
	bool calibrated;
	
	settings.begin("nav-mpu6050", true);
	calibrated = settings.isKey("calibrated");
	settings.end();
	
	return calibrated;
}

// Получить курсовой угол
float NavigationMPU6050::getYaw()
{
	return euler[0] * 180 / M_PI;
}

// Получить крен
float NavigationMPU6050::getRoll()
{
	return euler[2] * 180 / M_PI;
}

// Получить тангаж
float NavigationMPU6050::getPitch()
{
	return euler[1] * 180 / M_PI;
}
