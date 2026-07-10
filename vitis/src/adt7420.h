#ifndef ADT7420_H
#define ADT7420_H

#include "xparameters.h"
#include "xiicps.h"
#include "xil_printf.h"
#include "sleep.h"
#include <xstatus.h>

// Default I2C address for Pmod TMP2 (JP1=JP2 open)
#define ADT7420_I2C_ADDR   0x4B

// ADT7420 Register Map
#define ADT7420_REG_TEMP_MSB   0x00
#define ADT7420_REG_TEMP_LSB   0x01
#define ADT7420_REG_CONFIG     0x03

// Timeout for I2C operations
#define ADT7420_IIC_TIMEOUT    100000


// Driver API
int ADT7420_Init(XIicPs *Iic, u32 BASEADDR);
float ADT7420_ReadTemperature(XIicPs *Iic);
void ADT7420_Print_Temp(XIicPs *Iic);




#endif