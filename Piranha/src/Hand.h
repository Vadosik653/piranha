//
// Hand.h
// Рука-хват
//

#include <stdint.h>

#ifndef HAND_H_
#define HAND_H_

	typedef enum {
		HAND_MODE_NONE,
		HAND_MODE_OPEN,
		HAND_MODE_CLOSE	
	} enum_hand_mode;

	class Hand {
	private:
		// Текущий угол
		float Angle;
		
		// Режим руки
		enum_hand_mode Mode;
		
		// Открыть руку
		void Open();
		
		// Закрыть руку
		void Close();
		
	public:
		// Конструктор
		Hand();
		
		// Сброс
		void Reset();
		
		// Режим руки
		void SetMode(enum_hand_mode M);
		
		// Задать угол
		void SetAngle(uint8_t Angle);
		
		// Обновить положение руки
		void Update();
		
		// Получить угол
		uint8_t GetAngle();
	};


#endif /* PINOUT_H_ */
