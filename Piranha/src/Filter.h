//
// Filter.h
// Фильтрация сигналов
//

#include <stdint.h>

#ifndef FILTER_H_
#define FILTER_H_

	class Filter
	{
	private:
		bool Inited;
		float Alpha;
		float OldValue;
		
	public:
		// Конструктор
		// Alpha: коэффициент фильтрации (0-1), 0 - сильно, 1 - без фильтрации
		Filter(float Alpha);

		// Обработать выборку
		//  Value: входной сэмпл
		// return: отфильтрованный семпл
		float Process(float Value);
		
	};

#endif /* LEDLIGHT_H_ */