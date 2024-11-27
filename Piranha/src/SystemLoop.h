//
// SystemLoop.h
// Системный цикл управления пираньей (обработка датчиков, управление моторами и т.д.)
//

#include <stdint.h>
#include <stdbool.h>

#ifndef _SYSTEMLOOP_H
#define _SYSTEMLOOP_H
  #define PACKED __attribute__((__packed__))

	// Формат пакета принимаемых данных
	struct RECEIVE_DATA_STRUCTURE {
		// Сигнал джойстика для управления движением вперёд-назад
		uint8_t         ForwardVal;
		// Сигнал джойстика для управления поворотами влево-вправо
		uint8_t         YawLeftVal;
		// Сигнал джойстика для управления глубиной
		uint8_t         UpVal;
		// Сигнал джойстика для управления креном
		uint8_t         RollLeftVal;

		// Угол поворота камеры
		uint8_t         CamPitch;

		// Поле для передачи сигнала о необходимости запустить пользовательскую программу (при смене номера)
		uint8_t         RunProgram;
    
		// TODO: перевести на флаги?..

		// Надо ли сделать снимок
		volatile bool CamPhotoShot;
		// Надо ли переключить съёмку видео (вкл/выкл)
		volatile bool CamRec;
		// Состояние фонарей
		volatile bool LEDHdlts;
		// Блокировка моторов
		volatile bool MotorLock; // Motor lock
	} PACKED;

	// Формат пакета отправляемых данных
	struct SEND_DATA_STRUCTURE {
		// Напряжение батареи, в 0.1В (12В = 120)
		uint8_t  BattVolt;
		// Температура внешней среды в градусах
		int8_t   ROVTemp;
		// Глубина в сантиметрах
		int16_t  ROVDepth;
		// Курсовой угол (не работает)
		int16_t  ROVHDG;
		// Температура воды
		uint8_t  WaterTemp;
	} PACKED;

#endif
