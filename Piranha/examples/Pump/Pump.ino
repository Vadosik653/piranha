#include <Piranha.h>

// Выполняется один раз при старте "Пираньи"
void DroneSetup(void)
{
	// Назначим автостарт
	botsetup::Autostart();
}

// Выполняется после нажатия на кнопку "Поехали"
void DroneLoop(void)
{
	// Мигнём 1 с
	headlight::Blink(1000);
	
	// Включим нагрузку
	payload::On();
	
	// Ждём 5 сек
	Wait(5);
	
	// Выключим нагрузку
	payload::Off();
	
	// Мигнём 100 мс три раза
	headlight::Blink(100, 3);
}
