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
	// Полная скорость: 100%
	bot::SetSpeed(100);
	
	// Мигнём 1 с
	headlight::Blink(1000);
	
	// Погрузимся на 20 см примерно
	// И ждём 5 сек для погружения
	bot::SetDepth(20);
	Wait(5);
	
	// Едем вперёд 3 сек
	bot::Forward();
	Wait(3);
	
	// Едем надад 3 сек
	bot::Backward();
	Wait(3);
	
	// Всплывём
	bot::SetDepth(0);
	Wait(5);
	
	// Мигнём 100 мс три раза
	headlight::Blink(100, 3);
}
