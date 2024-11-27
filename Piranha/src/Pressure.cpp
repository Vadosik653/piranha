//
// Pressure.cpp
// Код датчика давления MS5803-30
//

#include "Pressure.h"
#include <Arduino.h>
#include <Wire.h>

#define CMD_RESET		0x1E	// ADC reset command
#define CMD_ADC_READ	0x00	// ADC read command
#define CMD_ADC_CONV	0x40	// ADC conversion command
#define CMD_ADC_D1		0x00	// ADC D1 conversion
#define CMD_ADC_D2		0x10	// ADC D2 conversion
#define CMD_ADC_256		0x00	// ADC resolution=256
#define CMD_ADC_512		0x02	// ADC resolution=512
#define CMD_ADC_1024	0x04	// ADC resolution=1024
#define CMD_ADC_2048	0x06	// ADC resolution=2048
#define CMD_ADC_4096	0x08	// ADC resolution=4096

const uint64_t POW_2_33 = 8589934592ULL;   // 2^33 = 8589934592
const uint64_t POW_2_37 = 137438953472ULL; // 2^37 = 137438953472

const SensorCoeffs MS5803_30 = {
	.Name = "MS5803_30 (30 bar)",
	.Low = {
		.TempDivider     = 8589934592ULL,   // 2^33
		.TempMultiplier  = 3,               // 3
		.Off2Coeff       = 24,              // 3/2
		.Sens2           = 5,               // 5/8
	},
	.High = {
		.TempDivider    = 137438953472ULL,  // 2^37
		.TempMultiplier = 7,                // 7
		.Off2Coeff      = 1,                // 1/16
		.Sens2          = 0,                // 0
	},
	.VeryLow = {
		.Off2Coeff      = 7,                // 1/16
		.Sens2          = 4,                // 0
	},
	
	.mbarDivider        = 8192,
	.mbarIntCoeff       = 10,
	.OffsetCoeff        = 1,
	.SensitivityCoeff   = 1,
};

const SensorCoeffs MS5803_02 = {
	.Name = "MS5803_02 (2 bar)",
	.Low = {
		.TempDivider     = 2147483648LL, // 2^31
		.TempMultiplier  = 1,            // 1
		.Off2Coeff       = 61,           // 61/16
		.Sens2           = 16,           // 2
	},
	.High = {
		.TempDivider    = 1,             // 1
		.TempMultiplier = 0,             // 0
		.Off2Coeff      = 0,             // 0
		.Sens2          = 0,             // 0
	},
	.VeryLow = {
		.Off2Coeff      = 20,            // 1/16
		.Sens2          = 12,            // 0
	},
	
	.mbarDivider        = 32768,
	.mbarIntCoeff       = 100,
	.OffsetCoeff        = 2,
	.SensitivityCoeff   = 2,
};

// Конструктор
Pressure::Pressure() : I2CDevice(0x76), PressFilter(DEPTH_FILTER_ALPHA), Table(&MS5803_02) // или бывает 0x77
{

}

// Установка типа
void Pressure::SetType(EnumSensorType Type)
{
	if(Type == ENUM_MS5803_02)
		Table = &MS5803_02;
	else
		Table = &MS5803_30;
}

// Инициализация периферии
void Pressure::Init()
{
	Serial.printf("Depth sensor type: %s\n", Table->Name);
	
	Reset();
	
	// Читаем коэффициенты сенсора
	for (int i = 0; i < 8; i++ )
	{
		bool ok;
		Coeffs[i] = ReadU16BE(0xA0 + i * 2, &ok);
	}
	
	// Последние 4 бита - контрольная сумма таблицы
	uint8_t p_crc = Coeffs[7] & 0x0F;
	// Посчитаем для проверки
	uint8_t n_crc = CalcTableCRC();

	Available = (p_crc == n_crc);	
}

// Расчёт CRC для таблицы поправок
// return: рассчитанный crc
uint8_t Pressure::CalcTableCRC()
{
    uint16_t n_rem;	   // остаток
    uint16_t crc_read; // исходное значение crc

    n_rem = 0x00;
    crc_read = Coeffs[7];
    Coeffs[7] = (0xFF00 & (Coeffs[7]));
    for (uint8_t i = 0; i < 16; i++)
    {
	    if (i % 2 == 1) {
		    n_rem ^= (uint16_t)((Coeffs[i >> 1]) & 0x00FF);
	    }
	    else {
		    n_rem ^= (uint16_t)(Coeffs[i >> 1] >> 8);
	    }
	    for (uint8_t n_bit = 8; n_bit > 0; n_bit--)
	    {
		    if (n_rem & (0x8000))
			    n_rem = (n_rem << 1) ^ 0x3000;
		    else
			    n_rem = (n_rem << 1);
	    }
    }
    n_rem = (0x000F & (n_rem >> 12));
    Coeffs[7] = crc_read;
    return (n_rem ^ 0x00);
}

// Прочитать измеренное значение АЦП
// CommandADC: тип измерения
//     return: считанное значение АЦП
uint32_t Pressure::ReadADC(uint8_t CommandADC)
{
	Wire.beginTransmission(address);
	Wire.write(CMD_ADC_CONV + CommandADC);
	Wire.endTransmission();
	
	delay(3);
	
	bool ok;
	uint32_t Res = ReadU24BE(CMD_ADC_READ, &ok);
	return Res;
}

// Обновление показаний датчика
void Pressure::Update()
{
	// Обновляем данные из датчика
	uint32_t rawPressure = ReadADC(CMD_ADC_D1 + CMD_ADC_512);
	uint32_t rawTemp     = ReadADC(CMD_ADC_D2 + CMD_ADC_512);
	
	int32_t dT = (int32_t)rawTemp - ((int32_t)Coeffs[5] * 256 );
	int32_t TEMP = 2000 + ((int64_t)dT * Coeffs[6]) / 8388608LL;
	TEMP = (int32_t)TEMP;
	
	int32_t	T2 = 0;
	int64_t	OFF2 = 0;
	int64_t	Sens2 = 0;
	if (TEMP < 2000)
	{
		uint32_t TempCoeff = TEMP - 2000;
		TempCoeff = TempCoeff * TempCoeff;
		T2 = Table->Low.TempMultiplier * ((int64_t)dT * dT) / Table->Low.TempDivider;
		OFF2 = Table->Low.Off2Coeff * TempCoeff / 16;
		Sens2 = Table->Low.Sens2 * TempCoeff / 8;
	}
	else
	{
		uint32_t TempCoeff = TEMP - 2000;
		TempCoeff = TempCoeff * TempCoeff;
		
		T2 = Table->High.TempMultiplier * ((int64_t)dT * dT) / Table->High.TempDivider;
		OFF2 = Table->High.Off2Coeff * TempCoeff / 16;
		Sens2 = Table->High.Sens2 * TempCoeff / 8;
	}
	if (TEMP < -1500) {
		uint32_t TempCoeff = TEMP + 1500;
		TempCoeff = TempCoeff * TempCoeff;
		
		OFF2 = OFF2 + Table->VeryLow.Off2Coeff * TempCoeff;
		Sens2 = Sens2 + Table->VeryLow.Sens2 * TempCoeff;
	}
	int64_t Offset = (int64_t)Coeffs[2] * Table->OffsetCoeff * 65536 + (Coeffs[4] * (int64_t)dT * Table->OffsetCoeff) / 128;
	int64_t Sensitivity = (int64_t)Coeffs[1] * 32768 * Table->SensitivityCoeff + (Coeffs[3] * (int64_t)dT  * Table->SensitivityCoeff) / 256;
	
    TEMP = TEMP - T2;
    Offset = Offset - OFF2;
    Sensitivity = Sensitivity - Sens2;
	
	int32_t mbarInt = ((rawPressure * Sensitivity) / 2097152 - Offset) / Table->mbarDivider;
	
	// Читаем вычисленные давление и температуру
	Press = PressFilter.Process((float)mbarInt / Table->mbarIntCoeff);
	Temp = (float)TEMP / 100; 
	
	if(Press == 0)
		Depth = 0;
	else
		Depth = (Press - 1013) * 1.01974428892211f / 10;
}

// Сброс датчика
void Pressure::Reset()
{
	Wire.beginTransmission(address);
	Wire.write(CMD_RESET);
	Wire.endTransmission();
	delay(10);
}

// Получить температуру
// return: температура, С
float Pressure::getTemp() { return Temp; }

// Получить давление
// return: давление, мбар
float Pressure::getPressure() { return Press; }

// Получить глубину
// return: глубина, м
float Pressure::getDepth() { return Depth; }
