#include <Piranha.h>

// Выполняется один раз при старте "Пираньи"
void DroneSetup(void)
{
	// Автозапуск
	botsetup::Autostart();
}

// Выполняется после нажатия на кнопку "Поехали"
void DroneLoop(void)
{
	// Мигнём 1 с 
	headlight::Blink(1000);
	
	// Перекалибровать датчик ориентации
	bot::Calibrate();
	
	// Мигнём 100 мс три раза
	headlight::Blink(100, 3);
}
