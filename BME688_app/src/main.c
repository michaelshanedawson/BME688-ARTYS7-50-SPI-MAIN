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
* v1.1
*/

#include "xparameters.h"
#include "xil_printf.h"
#include "xil_types.h"
#include "xspi.h"
#include <xspi_l.h>
#include "xgpio.h"
#include "stdio.h"

/*Global variables*/
uint8_t registerData = 0x00;

/*Declare prototypes*/
void select_memory_page(uint8_t page);
void spi_write(uint8_t reg, uint8_t page);
uint8_t spi_read(uint8_t reg, uint8_t page);
void delay(uint32_t value);

/*
*   Configuration information for the SPI interface for the BME688
*/
#define SPI_DEVICE_ID XPAR_AXI_QUAD_SPI_1_BASEADDR
#define BUFFER_SIZE 1

/*
*   Configuration information for the Chip Select pin.
*   We will manually control the CS pin because the XSPI driver sucks.
*/
#define CS_ADDR XPAR_XGPIO_0_BASEADDR
#define CS_CHANNEL 1
#define CS_MASK 0b0001

XGpio cs;

//typedef u8 DataBuffer[BUFFER_SIZE];

XSpi BME688; /* The instance of the SPI device */
uint8_t readBuffer[BUFFER_SIZE];
uint8_t writeBuffer[BUFFER_SIZE];

void delay(uint32_t value)
{    
    value = value * 100;
    for (uint32_t i=0; i<value; i++)
    {
        
    }
    return;
}

/* Specific function used to select the SPI memory page of the BME688 */
void select_memory_page(uint8_t page)
{
    uint8_t pageValue = 0x00 | page << 4;
    uint8_t regData = 0x73 | 0 << 7;   
    
    XGpio_DiscreteWrite(&cs, CS_CHANNEL, 0);
    XSpi_Transfer(&BME688, &regData, NULL, sizeof(regData));
    XSpi_Transfer(&BME688, &pageValue, NULL, sizeof(pageValue));
    XGpio_DiscreteWrite(&cs, CS_CHANNEL, CS_MASK);
}

/* Read from the selected register */
uint8_t spi_read(uint8_t reg, uint8_t page)
{
    uint8_t readData = 0x00;
    select_memory_page(page);
    uint8_t controlByte = reg | 1 << 7;
    
    XGpio_DiscreteWrite(&cs, CS_CHANNEL, 0);
    XSpi_Transfer(&BME688, &controlByte, NULL, sizeof(controlByte));
    XSpi_Transfer(&BME688, &readData, &readData, sizeof(controlByte));
    XGpio_DiscreteWrite(&cs, CS_CHANNEL, CS_MASK);  

    return readData;
}

int main()
{
    int Status;
    xil_printf("Entered function main\r\n\n");

    XGpio_Config *gpio_cfg;

    /* Initialize the Chip Select pin */
    gpio_cfg = XGpio_LookupConfig(CS_ADDR);
    if(gpio_cfg == NULL)
    {
        xil_printf("GPIO device not found!!!\r\n");
    }    
        
    Status = XGpio_CfgInitialize(&cs, gpio_cfg, CS_ADDR);
    if(Status != 0)
    {
        xil_printf("GPIO Init has failed with a status of: %lu\r\n",Status); //will print a 0 if the status is ok
    }

    //Set GPIO Tristate
    XGpio_SetDataDirection(&cs, CS_CHANNEL, 0);

    //Set the default output for the GPIO pin
    XGpio_DiscreteWrite(&cs, CS_CHANNEL, CS_MASK); //This sets the CS bit HIGH, idle state

    /*
    *   Configure SPI and start the driver
    */
    
	XSpi_Config *ConfigPtr;	/* Pointer to Configuration data */
    ConfigPtr = XSpi_LookupConfig(SPI_DEVICE_ID);
    if (ConfigPtr == NULL) {
		xil_printf("SPI device not found!!!\r\n");
	}
    Status = XSpi_CfgInitialize(&BME688, ConfigPtr, SPI_DEVICE_ID);
    if(Status != 0)
    {
        xil_printf("SPI Init has failed with a stauts of:  %lu\r\n",Status); //will print a 0 if the status is ok
    }
    Status = XSpi_SetOptions(&BME688, XSP_MASTER_OPTION | XSP_CLK_ACTIVE_LOW_OPTION | XSP_CR_MANUAL_SS_MASK | XSP_CLK_PHASE_1_OPTION);
    if(Status != 0)
    {
        xil_printf("SPI Set Options has failed with a status of: %lu\r\n",Status); //will print a 0 if the status is ok 
    }
    XSpi_SetSlaveSelect(&BME688, 0x1);
    Status = XSpi_Start(&BME688);
    if(Status != 0)
    {
        xil_printf("SPI Startup failed with a status of: %lu\r\n",Status); //will print a 0 if the status is ok        
    } 
    XSpi_Enable(&BME688);
    XSpi_IntrGlobalDisable(&BME688);



    /* Here we will begin our coding adventure */    

    registerData = spi_read(0x50, 0);    
    xil_printf("The chip ID is: %u \r\n", registerData);

    registerData = spi_read(0x70, 0);    
    xil_printf("The chip variant is: %u \r\n\n", registerData);

   
}
