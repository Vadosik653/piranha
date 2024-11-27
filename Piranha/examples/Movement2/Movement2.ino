#include <Piranha.h>

// Выполняется один раз при старте "Пираньи"
void DroneSetup(void)
{
	// Автозапуск
	//botsetup::Autostart();
}

// Выполняется после нажатия на кнопку "Поехали"
void DroneLoop(void)
{
	// Глубина погружения, см
	int TargetDepth = bot::GetDepth();
	
	// Cкорость: 100%
	bot::SetSpeed(100);
	// Текущее положение как стартовое
	bot::ResetCourse();
	
	// Мигнём 1 сек
	headlight::Blink(1000);
	
	// Целевая глубина
	int ErrorCounter = 0;
	// Медленно погрузимся сколько сможем
	while(true)
	{
		bot::SetDepth(TargetDepth);
		WaitMs(400);
		
		// Получим актуальную глубину
		float ActualDepth = bot::GetDepth();
		if(abs(TargetDepth - ActualDepth) > 10) 
		{
			// Если расхождение глубины более 10 см, то что-то пошло не так
			// Включим фонарь
			headlight::On();
			
			// Увеличиваем счётчик ошибок
			ErrorCounter++;
			if(ErrorCounter > 5)
			{
				// Набрали достаточное количество ошибок...
				// Считаем, мы достигли дна. Зададим глубину чуть-чуть поменьше, чтоб не прилипнуть ко дну (иначе вперёд он не поплывёт)
		    bot::SetDepth(TargetDepth - 16);
				break;
			}
		}
		else
		{
			// Выключим фонарь, всё нормально
			headlight::Off();
			ErrorCounter = 0;
		}
		
		// Добавим 2 см глубины...
		TargetDepth += 2;
	}
	
	// Мигнём 100 мс
	headlight::Blink(100);
	
	// Едем вперёд 3 сек
	bot::Forward();
	Wait(5);
	
	// Повернём на 90 градусов
	bot::Turn(90);
	Wait(2);
	
	// Едем вперёд 5 сек
	bot::Forward();
	Wait(3);
	
	// Остановка
	bot::Stop();
  
	// Медленно всплывём наверх
	for(int i = TargetDepth; i > 10; i--)
	{
		bot::SetDepth(i);
		WaitMs(100);
	}
	
	// Мигнём 100 мс три раза
	headlight::Blink(100, 3);
}
