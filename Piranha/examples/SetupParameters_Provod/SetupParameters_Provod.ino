#include <Piranha.h>

// Выполняется один раз при старте "Пираньи"
void DroneSetup(void)
{
	// Настроим параметры Wi-Fi, куда подключаться
	botsetup::WiFi("FilF", "murmansk13");
	// Задаим имя устройства (для обновления по Wi-Fi)
	botsetup::SetName("Piranha");
	
	// Выберем модуль HC-12 и канал 3
	// Как вариант там COMM_RS485 или COMM_HC12
	botsetup::SetComm(COMM_RS485);
	// Номер канала для HC-12
	//botsetup::SetChannel(3);
	
	// Выбор датчика глубины: DEPTH_TYPE_2BAR и DEPTH_TYPE_30BAR
	botsetup::SetDepthType(DEPTH_TYPE_2BAR);
	// Смещение нуля сбрасываем
	botsetup::SetDepthOffset(0);
}

// Выполняется после нажатия на кнопку "Поехали"
void DroneLoop(void)
{
	Wait(1);
}
