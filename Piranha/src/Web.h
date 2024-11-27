//
// Web.h
// Веб-сервер
//

#include <stdint.h>
#include <Config.h>

#ifndef WEB_H_
#define WEB_H_

#if defined(WEB_AVAILABLE) && (WEB_AVAILABLE > 0)
#include <ESPAsyncWebSrv.h>

	class Web {
	private:
		// Объект веб-сервера
		AsyncWebServer server;
		// Была ли пройдена инициализация корректно
		bool inited;
		
		// Добавить статические страницы из SPIFFS
		void AddStaticPages();
		
		// Добавить динамические страницы (AJAX)
		void AddDynamicPages();
		
	public:
		// Конструктор
		Web();
	
		// Инициализация веба
		void Init();
	};
#endif

#endif /* WEB_H_ */
