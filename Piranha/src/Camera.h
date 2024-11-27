//
// Camera.h
// Класс для управления камерой
//

#include <stdint.h>

#ifndef CAMERA_H_
#define CAMERA_H_

	class Camera {
	private:
		// Время начала импульса управления фото, если не 0
		uint32_t TriggerHoldPhoto = 0;
		// Время начала импульса управления видео, если не 0
		uint32_t TriggerHoldVideo = 0;

	public:
		// Инициализация периферии
		void Init();

		// Функция контроля временных параметров импульсов управления (вызывается из главного цикла регулярно)
		void Update();
		
		// Сделать фото
		void TriggerPhoto();

		// Включить или выключить запись видео
		void TriggerVideo();
	};

#endif /* CAMERA_H_ */