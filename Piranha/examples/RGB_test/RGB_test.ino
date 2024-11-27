#include <Piranha.h>

// Выполняется один раз при старте "Пираньи"
void DroneSetup(void)
{
	
}

// Выполняется после нажатия на кнопку "Поехали"
void DroneLoop(void)
{
	// Полная скорость: 100%
	bot::SetSpeed(100);
	
	if(rgb::IsAvailable())
	{
		// Мигнём 1 с
		headlight::Blink(1000);
	
		for(int i = 0; i < 1000; i++)
		{
			// Если есть хоть какая-то яркость
			if(rgb::GetBrigthness() > 128)
				headlight::On();
			else
				headlight::Off();
				
			WaitMs(100);
		}
		
		// Мигнём 100 мс три раза
		headlight::Blink(100, 3);
	}
	else
	{
		// Мигнём 500 мс пять раз
		headlight::Blink(500, 3);
	}
	
}
