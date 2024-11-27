//
// Debug.h
// Отладка. Отправка сообщений о состоянии по UDP
//

#include <stdint.h>
#include <WiFi.h>

#ifndef DEBUG_H_
#define DEBUG_H_

	class Debug
	{
	private:
		// Подклчюено
		bool Connected;
		
		// Время последней отправки
		uint32_t LastSended;
		
		// Обработка состояния подключения
		void ProcessConnection();

		// Получить широковещательный адрес
		IPAddress getBroadcast();
		
	protected:
		// Объект для работы с UDP
		WiFiUDP udp;
		
		// Отправка пакета
		void Send(const void * Data, uint16_t Size);

		// СОБЫТИЕ: WiFi отключён
		virtual void onDisconnected();

		// СОБЫТИЕ: WiFi подключён
		virtual void onConnected();

		// СОБЫТИЕ: Надо отправлять пакет
		virtual void onSending();

	public:
		// Конструктор
		Debug();
		
		// Инициализация
		void Init();
		
		// Основной цикл
		void Loop();
	};

	class StringDebug : public Debug
	{
	protected:
		// СОБЫТИЕ: Надо отправлять пакет
		virtual void onSending() override;
	};
	
	class PacketDebug : public Debug
	{
	protected:
		// СОБЫТИЕ: Надо отправлять пакет
		virtual void onSending() override;
	};
	
#endif /* LEDLIGHT_H_ */