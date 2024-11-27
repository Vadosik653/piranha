//
// OrientationQMC5883.h
// Датчик курсовой ориентации на магнитометре QMC5883
//

#include "Orientation.h"

#ifndef ORIENTATIONQMC5883_H_
#define ORIENTATIONQMC5883_H_

	class OrientationQMC5883 : public Orientation
	{
	private:
		// Показания магнитометра по оси Х
		int16_t X;
		// Показания магнитометра по оси Y
		int16_t Y;
		// Показания магнитометра по оси Z
		int16_t Z;
		// Предрассчитанный угол
		int16_t Angle;
	
	public:
		// Конструктор
		OrientationQMC5883();

		// Получить курсовой угол
		// return: курсовой угол
		virtual int16_t getAngle() override;

		// Инициализация периферии
		virtual void Init() override;
		// Обновить показания с датчика
		virtual void Update() override;
	};

#endif /* ORIENTATIONQMC5883_H_ */