//
// OrientationHMC5883.h
// Датчик курсовой ориентации на магнитометре HMC5883
//

#include "Orientation.h"

#ifndef ORIENTATIONHMC5883_H_
#define ORIENTATIONHMC5883_H_

	class OrientationHMC5883 : public Orientation
	{
	private:
		// Показания магнитометра по оси Х
		int16_t X;
		// Показания магнитометра по оси Y
		int16_t Y;
		// Показания магнитометра по оси Z
		int16_t Z;
			
	public:
		// Конструктор
		OrientationHMC5883();

		// Получить курсовой угол
		// return: курсовой угол
		virtual int16_t getAngle() override;

		// Инициализация периферии
		virtual void Init() override;
		// Обновить показания с датчика
		virtual void Update() override;
	};

#endif /* ORIENTATIONHMC5883_H_ */