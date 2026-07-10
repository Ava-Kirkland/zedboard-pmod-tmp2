#include "adt7420.h"
#define BASEADDR XPAR_XIICPS_0_BASEADDR
XIicPs Iic;

int main()
{


    xil_printf("=== ADT7420 Driver Test ===\r\n");

    if (ADT7420_Init(&Iic, BASEADDR) != XST_SUCCESS) {
        xil_printf("I2C init failed\r\n");
        return -1;
    }

    while (1) {
        ADT7420_Print_Temp(&Iic);
        sleep(1);
    }
}