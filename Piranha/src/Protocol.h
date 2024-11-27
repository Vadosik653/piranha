//
// Protocol.h
// Протокол связи
//

#include <stdint.h>
#include <stdbool.h>
#include <Arduino.h>

#ifndef _PROTOCOL_H
#define _PROTOCOL_H

  #define PACKED __attribute__((__packed__))
  
	#define PROTO_BUFFER_SIZE 32

	typedef enum {
		PARSE_STATE_WAIT_STX1,
		PARSE_STATE_WAIT_STX2,
		PARSE_STATE_WAIT_CODE,
		PARSE_STATE_WAIT_SIZE,
		PARSE_STATE_WAIT_DATA,
		PARSE_STATE_WAIT_CHECK
	} enum_parse_state;

	typedef enum {
		PACKET_CODE_UNKNOWN             = 0x00,
		PACKET_CODE_CONTROL_JOYSTICK    = 0x01,
		PACKET_CODE_CONTROL_PROGRAM     = 0x02,
		PACKET_CODE_CONTROL_ORIENTATION = 0x03,
		PACKET_CODE_STATUS_MAIN         = 0x81, 
		PACKET_CODE_CONTROL_MOTORS      = 0x82
	} enum_packet_code;
	
	#define CONTROL_FLAG_SEND_STATUS (1 << 0)
	#define CONTROL_FLAG_HAND_OPEN   (1 << 1)
	#define CONTROL_FLAG_HAND_CLOSE  (1 << 2)
  
	#define CONTROL_FLAG_COURSE_STAB (1 << 6)
	#define CONTROL_FLAG_DEPTH_STAB  (1 << 7)
	
	typedef struct {
		uint8_t STX1;   // 0x06
		uint8_t STX2;   // 0x86
		uint8_t Code;   // код типа пакета
		uint8_t Size;   // длина полезных данных в байтах
		uint8_t Data[]; // полезные данные
	} str_packet PACKED;

	typedef struct {
		uint8_t ForwardVal;   // Сигнал джойстика для управления движением вперёд-назад
		uint8_t YawLeftVal;   // Сигнал джойстика для управления поворотами влево-вправо
		uint8_t UpVal;        // Сигнал джойстика для управления глубиной
		uint8_t RollLeftVal;  // Сигнал джойстика для управления креном
		uint8_t Speed;        // Скорость (0-100), бит 7 -- фиксация курса
		uint8_t Led;          // Фонари (0-255)
		uint8_t Lock;         // Блок моторов: 1 - блок, 0 - работа
		uint8_t Flags;        // Флаги (бит 0 - слать ответ, бит 1 - открывать руку, бит 2 - закрывать руку, бит 6 - стабилизация курса, бит 7 - стаб глубины) 
	} str_packet_ctrl_joystick PACKED;

	typedef struct {
		uint8_t Program; // Номер программы
		uint8_t Flags;   // Флаги (бит 0 - слать ответ) 
	} str_packet_ctrl_program PACKED;
	
	typedef struct {
		uint16_t Depth;       // Желаемая глубина
		int16_t  Yaw;         // Ориентация (-180 - 180)
		uint8_t  Speed;       // Скорость вперёд
		uint8_t  Led;         // Фонари (0-255)
		uint8_t  Hand;        // Положение руки (0-закрыто, 255-открыто)
		uint8_t  Flags;       // Флаги (бит 0 - слать ответ)
	} str_packet_ctrl_orientation PACKED;

	typedef struct {
		uint8_t BattVolt;  // Напряжение питания дрона, 0.1В
		uint8_t ROVTemp;   // Температура
		int16_t ROVDepth;  // Глубина в см
		int16_t Yaw;       // Курсовой угол
		int16_t Roll;      // Тангаж
		int16_t Pitch;     // Крен
		int8_t  WaterTemp; // Температура воды
		uint8_t Speed;     // Скорость (0-3)
		uint8_t Led;       // Фонарь (0-3)
		uint8_t Prog;      // Номер программы текущей
		uint8_t Flags;      // Флаги статуса
		uint8_t Flags2;    // Флаги статуса
	} str_packet_status_main PACKED;

	typedef struct {
		uint8_t LV;    // Мощность на левом моторе верт.ориентации
		uint8_t RV;    // Мощность на правом моторе верт.ориентации
		uint8_t LH;    // Мощность на левом моторе гор.ориентации
		uint8_t RH;    // Мощность на правом моторе гор.ориентации
		uint8_t LX;    // Мощность на левом доп. моторе
		uint8_t RX;    // Мощность на правом доп. моторе
		uint8_t Hand1; // Мощность на руке 1
		uint8_t Hand2; // Мощность на руке 2
	} str_packet_status_motors PACKED;

	class Protocol {
	private:
		Stream *         stream;
		uint8_t          buffer[PROTO_BUFFER_SIZE];
		uint8_t          index;
		enum_parse_state state;
		uint8_t          channel;
		uint8_t          type;
		
		// Режим передачи
		void mode_transmit();
		// Режим приёма
		void mode_receive();
		
		// Сброс приёмника
		void reset();

		// Корректный ли код пакета
		bool isCorrectCode(uint8_t code);

		// Поулчить размер пакета
		uint8_t getPacketSize(uint8_t code);

		// Посчитать контрольную сумму
		uint8_t calcChecksum(const str_packet * packet);

		// Обработать байт
		// return: true, если в буфере собран пакет
		bool process(uint8_t Byte); 
		
	public:
		// Конструктор
		Protocol();
	
		// Начать обмен
		void begin(Stream * stream);
		
		// Выбор радиоканала
		// Channel: номер канала
		void setChannel(uint8_t Channel);

		// Выбор типа передатчика
		// Type: тип передатчика
		void setType(uint8_t Type);

		// Получить данные
		//   code: куда записать код команды
		//   data: буфер под данные пакета
		//   size: куда записать длину данных. На вход - длина буфера
		// return: true, если получен пакет
		bool receive(enum_packet_code * code, void * data, uint8_t * size);
		
		// Отправить данные
		// code: код команды
		// data: данные пакета
		// size: длина данных
		void send(enum_packet_code code, const void * data, uint8_t size);
	};

#endif
