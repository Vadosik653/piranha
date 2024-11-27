//
// Web.cpp
// Веб-сервер
//

#include "Web.h"
#include <WiFi.h>

#if defined(WEB_AVAILABLE) && (WEB_AVAILABLE > 0)

#include <AsyncJson.h>
#include <FS.h>
#include <SPIFFS.h>

Web::Web() : server(80), inited(false)
{

}

// Инициализация периферии и выводов
void Web::Init()
{
	//if(!SPIFFS.begin()) return;
	
	
	
	//server.begin();
	inited = true;
}

// Добавить статические страницы из SPIFFS
void Web::AddStaticPages()
{
	
}

// Добавить динамические страницы (AJAX)
void Web::AddDynamicPages()
{
	
}

#endif
