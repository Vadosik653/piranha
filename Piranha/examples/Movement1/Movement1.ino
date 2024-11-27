#include <Piranha.h>

// Выполняется один раз при старте "Пираньи"
void DroneSetup(void)
{
	
}

// Выполняется после нажатия на кнопку "Поехали"
void DroneLoop(void)
{
	// Начальная глубина
	int StartDepth = bot::GetDepth();
	
	// Глубина погружения относительно начальной, см
	int TargetDepth = StartDepth + 16;
	
	// Полная скорость: 100%
	bot::SetSpeed(100);
	
	// Мигнём 1 сек
	headlight::Blink(1000);
	
	// Медленно погрузимся на заданную глубину
	for(int i = StartDepth; i < TargetDepth; i++)
	{
		bot::SetDepth(i);
		WaitMs(300);
	}
	
	// Проплывём туда-сюда 3 раза
	for(int i = 0; i < 6; i++)
	{
		// Мигнём 100 мс
		headlight::Blink(100);
		
		// Едем вперёд 5 сек
		bot::Forward();
		Wait(3);
		
		// Остановим движение вперёд, чтобы развернуться на месте
		bot::Stop();
		
		// Повернём обратно за 4 сек
		bot::Turn(180);
		Wait(3);
	}
	
	// Мигнём 100 мс
	headlight::Blink(100);
	
	// Медленно всплывём на полметра
	for(int i = TargetDepth; i > StartDepth; i--)
	{
		bot::SetDepth(i);
		WaitMs(300);
	}
	
	// Мигнём 100 мс три раза
	headlight::Blink(100, 3);
}
