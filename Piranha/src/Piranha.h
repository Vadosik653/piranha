//
// Piranha.h
// Код управления пираньей для автономной работы
//

#include <stdint.h>
#include <stdbool.h>

#ifndef _PIRANHA_H
#define _PIRANHA_H

	typedef enum {
		COMM_UART  = 0,
		COMM_RS485 = 1,
		COMM_HC12  = 2
	} enum_protocol_type;

	typedef enum {
		DEPTH_TYPE_2BAR  = 0,
		DEPTH_TYPE_30BAR = 1
	} enum_depth_type;
	
	namespace botsetup {
		// Сделать автостарт скрипта (если нет джойстика)
		void Autostart();
		
		// Разрешён ли автостарт
		bool IsAutostartEnabled();
		
		// Настроить WiFi, к какой точке доступа подключаться
		void WiFi(const char * SSID, const char * Password);
		
		// Выбор названия робота (для беспроводной прошивкИ)
		void SetName(const char * Name);
		
		// Выбор модуля управления дроном
		void SetComm(enum_protocol_type type);
		
		// Выбор канала
		void SetChannel(uint8_t channel);
		
		// Выбрать тип датчика глубины
		// 0: 2 бар
		// 1: 30 бар
		void SetDepthType(enum_depth_type value);
		
		// Ручная установка смещения глубины, см
		void SetDepthOffset(int32_t offset);
	};

	namespace bot {
		// Включить управление моторами (если не были включены, то будет задержка на ожидание старта моторов)
		void MotorEnable();
		
		// Выключить управление моторами
		void MotorDisable();
		
		// Установка общей скорости движения
		// Speed: скорость, в процентах, от 30 до 100
		void SetSpeed(uint8_t Speed);
		
		// Остановка всех двигателей
		void Stop();
		
		// Подъём вверх
		void Up();
		// Спуск вниз
		void Down();
		
		// Поворот вправо
		void Right();
		// Поворот влево
		void Left();
		
		// Движение вперёд
		void Forward();

		// Движение назад
		void Backward();
		
		// Зафиксировать глубину
		void FixDepth();

		// Отключить удержание глубины
		void DisableDepthControl();
		
		// Получить напряжение батареи
		// return: напряжение батареи, В
		float GetBatteryVoltage();
		
		// Получить глубину погружения
		// return: глубина, см
		float GetDepth();

		// Задать глубину погружения
		// Depth: глубина, см
		void SetDepth(float Depth);

		// Получить тукцщий курс
		// return: курс, градусы (-180 - 180)
		float GetCourse();

		// Изменить курс (относительно текущего)
		// Angle: изменение курса, градусы (-180 - 180)
		void Turn(float Angle);

		// Изменить курс вправо
		// Angle: изменение курса, градусы (0 - 180)
		void TurnRight(float Angle);
		
		// Изменить курс влево
		// Angle: изменение курса, градусы (0 - 180)
		void TurnLeft(float Angle);
		
		// Задать желаемый курс
		// Course: курс, градусы (-180 - 180)
		void SetCourse(float Course);

		// Текущий курс считать нулём
		void ResetCourse();

		// Получить температуру
		// return: температура, С
		int8_t GetTemperature();
		
		// Перекалибровать датчик ориентации
		void Calibrate();
		
		// Перекалибровать датчик глубины
		void CalibrateDepth();
	};
	
	namespace motors {
		// Управление отдельными моторами
		// При задании мощности на отдельный мотор, на ручной режим переводится
		// сразу вся пара двигателей. Оба вертикальных или оба горизонтальных.
		// Перевод в обычный режим производится обычными командами навигации
		
		// Задать мощность левого мотора вертикального движения
		// Value: от -127 до 127, 0 - стоп
		void SetVL(int8_t Value);
		// Задать мощность правого мотора вертикального движения
		// Value: от -127 до 127, 0 - стоп
		void SetVR(int8_t Value);
		// Задать мощность левого мотора горизонтального движения
		// Value: от -127 до 127, 0 - стоп
		void SetHL(int8_t Value);
		// Задать мощность правого мотора горизонтального движения
		// Value: от -127 до 127, 0 - стоп
		void SetHR(int8_t Value);
	};
	
	// Полезная нагрузка (насос)
	namespace payload {
		// Управление состоянием нагрузки
		// state: true, если включено
		void SetState(bool state);
		
		// Управление направлением насоса
		// output: true, если выкачивание
		void SetDirection(bool output);

		// Включить нагрузку
		void On();
		
		// Выключить нагрузку
		void Off();
	};
	
	// Фонарь
	namespace headlight {
		// Включить фонарь
		void On();
		// Выключить фонарь
		void Off();
		
		// Задать яркость фонарей
		void Set(uint8_t Value);
		
		// Мигнуть фонарями с заданной задержкой
		// Delay: время свечения и паузы в мс
		// Count: количество миганий
		void Blink(int Delay, int Count = 1);
	};
	
	// Рука-хват
	namespace hand {
		// Открыть руку-хват
		void Open();
		// Зарыть руку-хват
		void Close();
		// Остановить изменение состояния руки (закрытие и открытие)
		void Stop();
		
		// Задать промежуточное положение руки-хвата (0 - закрыто, 255 - открыто)
		void Set(uint8_t Value);
	};
	
	// Датчик цвета
	namespace rgb {
		// Доступен ли датчик цвета
		// return: true, если доступен
		bool IsAvailable();
		
		// Получить красную компоненту цвета от датчика
		// return: красная компонента цвета с датчика, 0-255
		uint8_t GetRed();

		// Получить зелёную компоненту цвета от датчика
		// return: зелёная компонента цвета с датчика, 0-255
		uint8_t GetGreen();

		// Получить синюю компоненту цвета от датчика
		// return: синяя компонента цвета с датчика, 0-255
		uint8_t GetBlue();

		// Получить общую яркость света от датчика
		// return: общая яркость света с датчика, 0-255
		uint8_t GetBrigthness();
	};
	
	// Полезные функции по управлению роботом
	namespace move {
		// Начало выполнения задания (сброс глубины, скорости, курса)
		void Reset();
		
		// Поворот на месте с небольшим ожиданием поворота
		// Angle: угол поворота (0-180 - направо, -180-0 - налево)
		// Delay: ожидание в секундах
		void Turn(int Angle, int Delay);
		
		// Смена глубины на месте за заданное время
		// Depth: на какую глубину переместиться
		// Delay: за какое время
		void ChangeDepth(int Depth, int Delay);
		
		// Движение вперёд без смены глубины
		// Delay: время движения в секундах
		void Forward(int Delay);

		// Движение вперёд со сменой глубины
		// Depth: на какую глубину переместиться
		// Delay: время движения в секундах
		void Forward(int Depth, int Delay);
	};
	
	// Подождать секунды
	// Seconds: количество секунд
	void Wait(uint8_t Seconds);

	// Подождать миллисекунды
	// Milliseconds: количество миллисекунд
	void WaitMs(uint16_t Milliseconds);
	
#endif
