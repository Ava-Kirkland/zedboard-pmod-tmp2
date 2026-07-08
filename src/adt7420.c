#include "adt7420.h"


int ADT7420_Init(XIicPs *Iic, u32 BASEADDR){
     XIicPs_Config *Config;
    int Status;

    //Looks if there is a configuration of IIC availiable
    Config = XIicPs_LookupConfig(BASEADDR);
    if (Config == NULL) {
        xil_printf("I2C LookupConfig failed!\r\n");
        return XST_FAILURE;
    }
//Connect instance of driver and the configuration together
    Status = XIicPs_CfgInitialize(Iic, Config, Config->BaseAddress);
    if (Status != XST_SUCCESS) {
        xil_printf("I2C CfgInitialize failed: %d\r\n", Status);
        return XST_FAILURE;
    }
//Setting the settings, clk for IIC communication
    Status = XIicPs_SetSClk(Iic, 100000);  // 100 kHz
    if (Status != XST_SUCCESS) {
        xil_printf("Set clock failed\r\n");
        return XST_FAILURE;
    }

    xil_printf("I2C initialized successfully\r\n");
    return XST_SUCCESS;
}

float ADT7420_ReadTemperature(XIicPs *Iic){
        u8 reg = 0x00;  // Temperature MSB register
    u8 data[2] = {0};
    int Status;
    int timeout = ADT7420_IIC_TIMEOUT;

    // Write register pointer
    Status = XIicPs_MasterSendPolled(Iic, &reg, 1, ADT7420_I2C_ADDR);
    if (Status != XST_SUCCESS) {
        xil_printf("Send failed: %d\r\n", Status);
        return -999.0f;
    }
    //Max Timeout time 1 sec
    while (XIicPs_BusIsBusy(Iic) && timeout > 0){
        timeout--;
    }
    if(timeout <= 0) return -997.0f; //XST_TIMEOUT;
    
    timeout = ADT7420_IIC_TIMEOUT;
    // Read 2 bytes - default if read 2 bytes will get temp output
    Status = XIicPs_MasterRecvPolled(Iic, data, 2, ADT7420_I2C_ADDR);
    if (Status != XST_SUCCESS) {
        xil_printf("Recv failed: %d\r\n", Status);
        return -999.0f;
    }
    while (XIicPs_BusIsBusy(Iic) && timeout > 0){
        timeout--;
    }
    if(timeout <= 0) return -997.0f; //XST_TIMEOUT;

    //combine MSB and LSM
    s16 raw = (data[0] << 8) | data[1];
    //Remove last 3 bits - not used
    raw >>= 3;                    // 13-bit resolution (default)

    //The 0.0625f is the resolution between 1C and a change in a next value in binary from 0 to 1
    float temp_c = raw * 0.0625f;

    return temp_c;
}

void ADT7420_Print_Temp(XIicPs *Iic){
    

        float t = ADT7420_ReadTemperature(Iic);
        int c_temp_int = 0;
        int c_temp_frac = 0;
        float f_temp = 0;
        int f_temp_int =0;
        int f_temp_frac = 0;
        if (t > -500.0f) {  // Valid reading // might need check documents for correct value
        //only works on positive temperatures
            c_temp_int = (int)t;
            c_temp_frac = (int)((t - c_temp_int) * 100);   // two decimal places
            f_temp = (t * (9.0f / 5.0f)) +32.0f;
            f_temp_int = (int)f_temp;
            f_temp_frac = (int)((f_temp - f_temp_int) * 100);
            xil_printf("Temperature: %d.%02d C\t %d.%02d F\r\n", c_temp_int, c_temp_frac, f_temp_int,f_temp_frac);
        } else {
            xil_printf("Read error - check pull-ups, address, wiring\r\n");
        }
}