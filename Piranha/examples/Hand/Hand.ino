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
	
	// Откроем руку
	hand::Open();
	
	// Ждём 5 сек
	Wait(5);
	
	// Закроем руку
	hand::Close();
	
	// Ждём 5 сек
	Wait(5);
	
	// Плавно откроем
	for(int i = 0; i < 255; i++)
	{
		hand::Set(i);
		
		WaitMs(100);
	}
	
	// Мигнём 100 мс три раза
	headlight::Blink(100, 3);
}
