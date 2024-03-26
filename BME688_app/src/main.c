/*
* BME688-ARTYS7-50-SPI-MAIN
* main.c
* Michael Dawson
* michaelshanedawson@gmail.com
*
* SPI Driver and example code for the Bosch Sensortec BME688 temperature, humidity, pressure and gas sensor unit. This example uses the forced mode for measurements.
* Datasheet can be found here : https://www.bosch-sensortec.com/products/environmental-sensors/gas-sensors/bme688/#documents
*
* see README for changelog
* v0.1
*/

#include "xparameters.h"
#include "xil_printf.h"
#include "xil_types.h"
#include "xgpio.h"

/*
*   Configuration information for the GPIO interface
*/
#define GPIO_DEVICE_ID XPAR_GPIO_BASEADDR
#define GPIO_CHANNEL 1
#define GPIO_MASK 0b0001


int main()
{   
    XGpio_Config *gpio_ptr;
    XGpio gpio_device;
    
    int Status;
    xil_printf("Entered function main\r\n\n");

   

    //Initialize the GPIO port
    gpio_ptr = XGpio_LookupConfig(GPIO_DEVICE_ID);
    if(gpio_ptr == NULL) {
        xil_printf("GPIO device not found!!!\r\n");
    }    
        
    Status = XGpio_CfgInitialize(&gpio_device, gpio_ptr, GPIO_DEVICE_ID);        
    xil_printf("GPIO Init status = %lu\r\n",Status); //will print a 0 if the status is ok

    Status = XGpio_SelfTest(&gpio_device);
    xil_printf("GPIO Self Test = %lu\r\n",Status); //will print a 0 if the status is ok
    

    //Set GPIO Tristate
    XGpio_SetDataDirection(&gpio_device, GPIO_CHANNEL, 0);
    xil_printf("Setting GPIO to output.\r\n");

    //Set the default output for the GPIO pin
    XGpio_DiscreteWrite(&gpio_device, GPIO_CHANNEL, GPIO_MASK); //This sets the CS bit HIGH, idle state
    xil_printf("GPIO set to HIGH.\r\n\n");

   
}
