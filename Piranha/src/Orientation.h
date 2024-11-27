//
// Orientation.h
// Базовый класс для датчика курсовой ориентации
//

#include <stdint.h>

#ifndef ORIENTATION_H_
#define ORIENTATION_H_

	class Orientation {
	public:
		// Получить курсовой угол
		// return: курсовой угол
		virtual int16_t getAngle() = 0;
		
		// Инициализация
		virtual void Init() = 0;
		// Обновить показания с датчика
		virtual void Update() = 0;
	};

#endif /* ORIENTATION_H_ */