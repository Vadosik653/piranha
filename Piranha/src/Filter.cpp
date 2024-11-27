//
// Filter.cpp
// Фильтрация сигналов
//

#include "Filter.h"

// Конструктор
// Alpha: коэффициент фильтрации (0-1), 0 - сильно, 1 - без фильтрации
Filter::Filter(float Alpha) : Alpha(Alpha), Inited(false), OldValue(0)
{
	
}

// Обработать выборку
//  Value: входной сэмпл
// return: отфильтрованный семпл
float Filter::Process(float Value)
{
	if(Inited)
	{
		OldValue = OldValue * (1 - Alpha) + Value * Alpha;
	}
	else
	{
		OldValue = Value;
		Inited = true;
	}
	
	return OldValue;
}
