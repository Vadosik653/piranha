//
// Protocol.cpp
// Протокол связи
//

#include "Protocol.h"
#include "Pinout.h"
#include "Config.h"
		
// Конструктор
Protocol::Protocol() : stream(0), type(0), channel(0), index(0), state(PARSE_STATE_WAIT_STX1)
{
		
}

// Начать обмен
void Protocol::begin(Stream * stream)
{
	this->stream = stream;
}

// Режим передачи
void Protocol::mode_transmit()
{
	if(type == 1)
	{
		// Отправка
		digitalWrite(Dirpin, HIGH);
		delay(3);
	}
}

// Режим приёма
void Protocol::mode_receive()
{
	if(type == 1)
	{
		// Приём
		delay(1);
		digitalWrite(Dirpin, LOW);
	}
}

// Сброс приёмника
void Protocol::reset()
{
	index = 0;
	state = PARSE_STATE_WAIT_STX1;
}

// Корректный ли код пакета
bool Protocol::isCorrectCode(uint8_t code)
{
	switch(code)
	{
	case PACKET_CODE_CONTROL_JOYSTICK:
	case PACKET_CODE_CONTROL_PROGRAM:
	case PACKET_CODE_STATUS_MAIN:
	case PACKET_CODE_CONTROL_MOTORS:
		return true;
	default:
		return false;
	}
}

// Поулчить размер пакета
uint8_t Protocol::getPacketSize(uint8_t code)
{
	switch(code)
	{
	case PACKET_CODE_CONTROL_JOYSTICK: return sizeof(str_packet_ctrl_joystick);
	case PACKET_CODE_CONTROL_PROGRAM:  return sizeof(str_packet_ctrl_program);
	case PACKET_CODE_STATUS_MAIN:      return sizeof(str_packet_status_main);
	case PACKET_CODE_CONTROL_MOTORS:   return sizeof(str_packet_status_motors);
	default:
		return 0;
	}
}

// Посчитать контрольную сумму
uint8_t Protocol::calcChecksum(const str_packet * packet)
{
	uint8_t CS = 0;
	for(int i = 0; i < packet->Size; i++)
		CS ^= packet->Data[i];

	return CS;
}

// Выбор типа передатчика
// Type: тип передатчика
void Protocol::setType(uint8_t Type)
{
	if(type != Type)
	{
		type = Type;
		
		if(type == 0)
		{
			Serial.write("Set communication mode to UART\n");
		}
		if(type == 1)
		{
			Serial.write("Set communication mode to RS485\n");
			
			// Приём данных
			pinMode(Dirpin, OUTPUT);
			digitalWrite(Dirpin, LOW);
		}
		else if(type == 2)
		{
			Serial.write("Set communication mode to HC12\n");
			
			// Приём данных
			pinMode(Dirpin, OUTPUT);
			digitalWrite(Dirpin, HIGH);
			
			channel = 255;
		}
	}
}

// Выбор радиоканала
// Channel: номер канала
void Protocol::setChannel(uint8_t Channel)
{
	if(channel != Channel)
	{
		if(type == 2)
		{
			uint8_t RFChannel = 1 + Channel * 6;
	  
			Serial.printf("Set radiochannel to %d (HC12 channel = %d)\n", Channel, RFChannel);
			vTaskDelay(3);
			digitalWrite(Dirpin, LOW);
			vTaskDelay(90);
	  
			stream->write("AT+C");
			if(RFChannel < 10)
				stream->write("00");
			else if(RFChannel < 100)
				stream->write("0");
		  
			stream->print(RFChannel);
			// Приём
			vTaskDelay(60);
			digitalWrite(Dirpin, HIGH);
			vTaskDelay(3);
		}
		
		channel = Channel;
	}
}

// Обработать байт
// return: true, если в буфере собран пакет
bool Protocol::process(uint8_t Byte)
{
  //Serial.write("DATA ");
  //Serial.print(Byte, HEX);
  //Serial.println();

	switch(state)
	{
	case PARSE_STATE_WAIT_STX1:
		if(Byte == 0x06)
		{
			index = 0;
			buffer[index++] = Byte;
			state = PARSE_STATE_WAIT_STX2;
		}
		else
			reset();
		break;
	case PARSE_STATE_WAIT_STX2:
		if(Byte == 0x86)
		{
			buffer[index++] = Byte;
			state = PARSE_STATE_WAIT_CODE;
		}
		else
			reset();
		break;
	case PARSE_STATE_WAIT_CODE:
		if(isCorrectCode(Byte))
		{
			buffer[index++] = Byte;
			state = PARSE_STATE_WAIT_SIZE;
		}
		else
			reset();
		break;
	case PARSE_STATE_WAIT_SIZE:
		if(getPacketSize(buffer[2]) == Byte)
		{
			buffer[index++] = Byte;
			state = PARSE_STATE_WAIT_DATA;
		}
		else
			reset();
		break;
	case PARSE_STATE_WAIT_DATA:
		if(index < 4 + buffer[3])
		{
			buffer[index++] = Byte;
			if(index == 4 + buffer[3])
				state = PARSE_STATE_WAIT_CHECK;
		}
		else
			reset();
		break;
	case PARSE_STATE_WAIT_CHECK:
		buffer[index++] = Byte;
		
		if(Byte == calcChecksum((const str_packet *)buffer))
			return true;
		else
			reset();
    break;
	}
	return false;
}

// Получить данные
// code: куда записать код команды
// data: буфер под данные пакета
// size: куда записать длину данных. На вход - длина буфера
bool Protocol::receive(enum_packet_code * code, void * data, uint8_t * size)
{
	int16_t available = stream->available();

	if(available > 0)
	{
		int16_t ch = stream->read();
		if(ch >= 0)
		{
			if(process(ch))
			{
				str_packet * packet = (str_packet *)buffer;
				if(packet->Size < *size)
				{
					memcpy(data, packet->Data, packet->Size);
					
					*code = (enum_packet_code)packet->Code;
					*size = packet->Size;
				}
				
				reset();
				return true;
			}
		}
	}
	return false;
}

// Отправить данные
// code: код команды
// data: данные пакета
// size: длина данных
void Protocol::send(enum_packet_code code, const void * data, uint8_t size)
{
	if(stream)
	{
		uint8_t CS = 0;
		const uint8_t * raw = (const uint8_t*)data;
		
		mode_transmit();
		stream->write(0x06);
		stream->write(0x86);
		stream->write(code);
		stream->write(size);
		for(int i = 0; i < size; i++)
		{
			CS ^= raw[i];
			stream->write(raw[i]);
		}
		stream->write(CS);
		stream->flush();
		
		mode_receive();
	}
}
