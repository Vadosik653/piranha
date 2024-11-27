#include <Piranha.h>

// Упражнение "Маяк"

// Выполняется один раз при старте "Пираньи"
void DroneSetup(void)
{
	
}

// Выполняется после нажатия на кнопку "Поехали"
void DroneLoop(void)
{
	// Начальная глубина
	int StartDepth = bot::GetDepth();
	
	// Глубина погружения, см
	int TargetDepth = StartDepth + 30;
	
	// Половинная скорость: 50%
	bot::SetSpeed(50);
	// Текущее положение как стартовое
	bot::ResetCourse();
	
	// Мигнём 1 сек
	headlight::Blink(1000);
	
	// Медленно погрузимся
	for(int i = StartDepth; i <  TargetDepth; i++)
	{
		bot::SetDepth(i);
		WaitMs(300);
	}
	Wait(3);
	
	// Покрутимся
	for(int i = 0; i < 8; i++)
	{
		// Повернём на 45 градусов
		bot::Turn(45);
		Wait(2);
		
		// Моргнём 5 раз
		headlight::Blink(100, 5);
	}
	
	// Мигнём 100 мс
	headlight::Blink(100);
	
	// Медленно всплывём наверх
	for(int i = TargetDepth; i > StartDepth; i--)
	{
		bot::SetDepth(i);
		WaitMs(300);
	}
	
	headlight::Blink(100, 3);
}
