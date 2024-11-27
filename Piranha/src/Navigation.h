//
// Navigation.h
// Навигация (гироскоп/акселерометр)
//

#include <stdint.h>

#ifndef NAVIGATION_H_
#define NAVIGATION_H_

	class Navigation {
	private:
		bool available;
		
	protected:
		// Назначить, доступны ли данные
		void setAvailable(bool avail);
		
	public:
		// Конструктор
		Navigation();
		
		// Доступна ли
		bool isAvailable();
		
		// Инициализация периферии и выводов
		virtual void Init() = 0;
		
		// Обновить данные
		virtual void Update() = 0;
		
		// Получить курсовой угол
		virtual float getYaw() = 0;
		
		// Получить крен
		virtual float getRoll() = 0;
		
		// Получить тангаж
		virtual float getPitch() = 0;
		
		// Откалибровать устройство
		virtual void Calibrate();
		
		// Откалибровано ли устройство
		virtual bool hasCalibration();
	};

#endif /* VOLTAGE_H_ */