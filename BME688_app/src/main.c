/*
* BME688-ARTYS7-50-SPI-MAIN
* main.c
* Michael Dawson
* michaelshanedawson@gmail.com
*
* SPI Driver and example code for the Bosch Sensortec BME688 temperature, humidity, pressure and gas sensor unit. This example uses the forced mode for measurements.
* Datasheet can be found here : https://www.bosch-sensortec.com/products/environmental-sensors/gas-sensors/bme688/#documents
*
* see README.md for changelog
* v2.0
*/

#include "xparameters.h"
#include "xil_printf.h"
#include "xil_types.h"
#include "xspi.h"
#include <xspi_l.h>
#include "xgpio.h"
#include "stdio.h"



/* Declare prototypes */
void select_memory_page(uint8_t page);
void spi_write(uint8_t reg, uint8_t data, uint8_t page);
uint8_t spi_read(uint8_t reg, uint8_t page);
void delay(uint32_t value);
void take_reading();
void print_float(float input);

/*
*   Configuration information for the SPI interface for the BME688
*/
#define SPI_DEVICE_ID XPAR_AXI_QUAD_SPI_1_BASEADDR
#define BUFFER_SIZE 1
XSpi BME688; // The instance of the SPI device

/*
*   Configuration information for the Chip Select pin.
*   We will manually control the CS pin because the XSPI driver sucks.
*/
#define CS_ADDR XPAR_XGPIO_0_BASEADDR
#define CS_CHANNEL 1
#define CS_MASK 0b0001
XGpio cs; //CS GPIO instance

/* BME688 Configuration Variables */
#define HEATER_TARGET_TEMP 300.00 //Target temperature for the gas sensor heater, usually between 200°C and 400°C. Floating point value.
#define AMBIENT_TEMP 22.0 //General ambient room temperature, used to set the heater resistance. 
#define TEMP_OFFSET 0.0 //Calibration temperature offset. Floating point value.
#define PRESSURE_OFFSET 0.0 //Calibration pressure offset. Floating point value.

/* Global variables */
uint8_t registerData = 0x00;
double currentTemperature = 0.0; //The temperature data is used in multiple functions of the device

/*Temperature sensor calibration data. This data will be used to perform the math on the polled ADC data during each read.*/
double parT1 = 0x00; //Two bytes at LSB 0xE9 and MSB 0xEA, page 0
double parT2 = 0x00; //Two bytes at LSB 0x8A and MSB 0x8B, page 0
double parT3 = 0x00; //Single byte at 0x8C, page 0

/*Gas sensor calibration data. This data will be used to perform the math on the polled ADC data during each read.*/
double parG1 = 0x00; //Single byte at 0xED, page 0
double parG2 = 0x00; //Two bytes at LSB 0xEB and MSB 0xEC, page 0
double parG3 = 0x00; //Single byte at 0xEE, page 0
int resHeatValue = 0x00; //Single signed value at 0x00, page 0
uint8_t resHeatRange = 0x00; //Heater range at 0x02 <5:4>, page 0

/*Pressure sensor calibration data. This data will be used to perform the math on the polled ADC data during each read.*/
double parP1 = 0x00; //Two bytes at LSB 0x8E and MSB 0x8F, page 0
double parP2 = 0x00; //Two bytes at LSB 0x90 and MSB 0x91, page 0
double parP3 = 0x00; //Single byte at 0x92, page 0
double parP4 = 0x00; //Two bytes at LSB 0x94 and MSB 0x95, page 0
double parP5 = 0x00; //Two bytes at LSB 0x96 and MSB 0x97, page 0
double parP6 = 0x00; //Single byte at 0x99, page 0
double parP7 = 0x00; //Single byte at 0x98, page 0
double parP8 = 0x00; //Two bytes at LSB 0x9C and MSB 0x9D, page 0
double parP9 = 0x00; //Two bytes at LSB 0x9E and MSB 0x9F, page 0
double parP10 = 0x00; //Single byte at 0xA0, page 0

/*Humidity sensor calibration data. This data will be used to perform the math on the polled ADC data during each read.*/
double parH1 = 0x00; //Two bytes at LSB 0xE2<3:0> and MSB 0xE3, page 0
double parH2 = 0x00; //Two bytes at LSB 0xE2<7:4> and MSB 0xE1, page 0
double parH3 = 0x00; //Single byte at 0xE4, page 0
double parH4 = 0x00; //Single byte at 0xE5, page 0
double parH5 = 0x00; //Single byte at 0xE6, page 0
double parH6 = 0x00; //Single byte at 0xE7, page 0
double parH7 = 0x00; //Single byte at 0xE8, page 0


/* Generic delay function that just idles time away */
void delay(uint32_t value)
{    
    value = value * 1000;
    for (uint32_t i=0; i<value; i++)
    {        
    }
    return;
}

/* Hack to allow xil_printf to output a floating point value */
void print_float(float input)
{
      /*
       * cast input and remove floating part
     */
    long int fix_part = (long int) input;

    /*
     * remove integer part, multiply by 1000 to adjust to 3 decimal points then cast to integer
     */
    long int frac_part = (long int) (input*1000.0 - fix_part*1000);
    xil_printf("%d", fix_part);
    xil_printf(".%d", frac_part);
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
    uint8_t controlByte = reg | 1 << 7; //Sets bit position 7 as 1 for read access
    select_memory_page(page);    
    XGpio_DiscreteWrite(&cs, CS_CHANNEL, 0);
    XSpi_Transfer(&BME688, &controlByte, NULL, sizeof(controlByte));
    XSpi_Transfer(&BME688, &readData, &readData, sizeof(controlByte));
    XGpio_DiscreteWrite(&cs, CS_CHANNEL, CS_MASK);  

    return readData;
}

/* Function to write register data to the BME688 */
void spi_write(uint8_t reg, uint8_t data, uint8_t page)
{
    select_memory_page(page);
    uint8_t byteData = reg | 0 << 7; //Sets bit position 7 as 0 for write access
    XGpio_DiscreteWrite(&cs, CS_CHANNEL, 0);
    XSpi_Transfer(&BME688, &byteData, NULL, sizeof(byteData));
    XSpi_Transfer(&BME688, &data, NULL, sizeof(data));
    XGpio_DiscreteWrite(&cs, CS_CHANNEL, CS_MASK);

}


void take_reading()
{
    uint8_t operationByte = spi_read(0x74, 1); //Gets the current byte data for the Ctrl_meas register.
    operationByte = operationByte | 1 << 0; //Sets the mode bit to take a reading. We don't want to change anything else.
    spi_write( 0x74, operationByte, 1); //Writes the updated register value. the mode bits <1:0> reset to 0 once the reading takes place.
    delay(100); // Perform a slight delay to allow settling of the reading

   /*Perform register data pulling and mathematics to calculate the data*/

   /*Temperature reading*/
   double tempADC = spi_read(0x22, 1) << 12 | spi_read(0x23, 1) << 4; //This gets the raw temperature ADC value from the BME688

   /*Performs the temperature conversion calculations on the raw ADC value for floating point result, result is in Celsius*/
   double temp1 = ((tempADC / 16384.0) - (parT1 / 1024)) * parT2;
   double temp2 = (((tempADC / 131072.0) - (parT1 / 8192.0)) * ((tempADC / 131072.0) - (parT1 / 8192))) * (parT3 * 16.0);
   double tFine = temp1 + temp2;   
   double tempComp = tFine / 5120.0; //tempComp is the computed result of the ADC raw data, will be the returned value down the road
   if (tempComp > TEMP_OFFSET)
    {  
        tempComp = tempComp - TEMP_OFFSET;
    } 
   else if (tempComp < TEMP_OFFSET)
    {
        tempComp = tempComp + TEMP_OFFSET;
    } 
   xil_printf("Current temperature is= ");
   print_float(tempComp);
   xil_printf("°C \r\n");

   /*Pressure reading*/
   double pressureADC = spi_read(0x1F, 1) << 12 | spi_read(0x20, 1) << 4; //This gets the raw pressure ADC value

   /*Performs the pressure conversion calculation on the raw ADC value for floating point result, result is in Pascal.
   * Please note, the datasheet for the BME688 has some of the formula variables wrong. There are entries for var1_p, var2_p, var3_p.
   * These should just be var1, var2 and var3, confirmed to past product datasheet such as the BME680 which uses the same formulas to calculate with.
   * To make things easier, added conversion to inHg prior to printing. May add an offset for calibration later, needs further testing to see if
   * sensor requires proper burn-in to function properly.
   */
   double pressure1 = (tFine / 2.0) - 64000.0;
   double pressure2 = pressure1 * pressure1 * (parP6 / 131072.0);
   pressure2 = pressure2 + (pressure1 * parP5 * 2.0);
   pressure2 = (pressure2 / 4.0) + (parP4 * 65536.0);
   pressure1 = (((parP3 * pressure1 * pressure1) / 16384.0) + (parP2 * pressure1)) / 524288.0;
   pressure1 = (1.0 + (pressure1 / 32768.0)) * parP1;
   double pressureComp = 1048576.0 - pressureADC;
   pressureComp = ((pressureComp - (pressure2 / 4096.0)) * 6250.0) / pressure1;
   pressure1 = (parP9 * pressureComp * pressureComp) / 2147483648.0;
   pressure2 = pressureComp * (parP8 / 32768.0);
   double pressure3 = (pressureComp / 256.0) * (pressureComp / 256.0) * (pressureComp / 256.0) * (parP10 / 131072.0);
   pressureComp = pressureComp + (pressure1 + pressure2 + pressure3 + (parP7 * 128.0)) / 16.0;

   pressureComp = pressureComp * 0.000295; //The value is in Pascal, convert it to inHg.

   /*Offset calculation for the pressure*/
   if (pressureComp > PRESSURE_OFFSET)
    {
        pressureComp = pressureComp - PRESSURE_OFFSET;
    } 
   else if (pressureComp < PRESSURE_OFFSET)
    {
        pressureComp = pressureComp + PRESSURE_OFFSET;
    }

   xil_printf("Current pressure is= ");
   print_float(pressureComp);
   xil_printf("inHg \r\n");

   /*Humidity reading*/
   double humidityADC = spi_read(0x25, 1) << 8 | spi_read(0x26, 1); //This gets the raw humidity ADC value

   /*Performs the humidity conversion calculation on the raw ADC value for the floating point result.*/
   double humidity1 = humidityADC - ((parH1 * 16.0) + ((parH3 / 2.0) * tempComp));
   double humidity2 = humidity1 * ((parH2 / 262144.0) * (1.0 + ((parH4 / 16384.0)* tempComp) + ((parH5 / 1048576.0) * tempComp * tempComp)));
   double humidity3 = parH6 / 16384.0;
   double humidity4 = parH7 / 2097152.0;
   double humidityComp = humidity2 + ((humidity3 + (humidity4 * tempComp)) * humidity2 * humidity2);

   xil_printf("Current humidity is= ");
   print_float(humidityComp);
   xil_printf("%% \r\n");

   /*Gas sensor reading*/
   uint8_t tempData = spi_read(0x2D, 1);
   uint32_t gasRange = (tempData & 0x0F);
   int32_t gasADC = spi_read(0x2C, 1) << 2 | (tempData & 0xC0) >> 1;

   uint32_t gas1 = UINT32_C(262144) >> gasRange;
   int32_t gas2 = gasADC - INT32_C(512);
   gas2 *= INT32_C(3);
   gas2 = INT32_C(4096) + gas2;
   double gasResistance = 1000000.0f * (float)gas1 / (float)gas2;

   xil_printf("Gas plate resistance is= ");
   print_float(gasResistance);
   xil_printf("Ω \r\n");
   xil_printf("------------------------------------ \r\n");
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



    /* Here we will begin working with the BME688 */    

    registerData = spi_read(0x50, 0);    
    xil_printf("The chip ID is: %u \r\n", registerData);

    registerData = spi_read(0x70, 0);    
    xil_printf("The chip variant is: %u \r\n\n", registerData);

     /*
    * Begin BME688 configuration

    * ---Forced Mode---
    * We will need calibration data to perform the value calculations, this is stored in registers.
    * 
    * 
    * Set humidity oversampling value
    * Set temperature oversampling value
    * Set pressure oversampling value
    * 
    * Set the gas wait value for heating up of the plate
    * Set the heater step for the heater resistance value
    * Set the nb_conv to select the prior defined heater settings
    * Set run_gas_h to 1 to enable gas measurements
    * 
    * Trigger a single reading
    */

   /*Get the calibration data first and store it in the global variables*/
   uint8_t tempData = 0x00; //Generic temporary data store

   /*Calibration data for the temperature sensor*/   
   parT1 = spi_read(0xEA, 0) << 8 | spi_read(0xE9, 0);
   parT2 = spi_read(0x8B, 0) << 8 | spi_read(0x8A, 0);
   parT3 = spi_read(0x8C, 0);

   /*Calibration data for the gas sensor heating and measurement*/
   parG1 = spi_read(0xED, 0);   
   parG2 = spi_read(0XEC, 0) << 8 | spi_read(0XEB, 0); 
   parG3 = spi_read(0xEE, 0);
   resHeatValue = spi_read(0x00, 0);
   tempData = spi_read(0x02, 0);
   resHeatRange = (tempData & 0x3);

   /*Calibration data for the pressure sensor*/
   parP1 = spi_read(0x8F, 0) << 8 | spi_read(0x8E, 0);
   parP2 = spi_read(0x91, 0) << 8 | spi_read(0x90, 0);
   parP3 = spi_read(0x92, 0);
   parP4 = spi_read(0x95, 0) << 8 | spi_read(0x94, 0);
   parP5 = spi_read(0x97, 0) << 8 | spi_read(0x96, 0);
   parP6 = spi_read(0x99, 0);
   parP7 = spi_read(0x98, 0);
   parP8 = spi_read(0x9D, 0) << 8 | spi_read(0x9C, 0);
   parP9 = spi_read(0x9F, 0) << 8 | spi_read(0x9E, 0);
   parP10 = spi_read(0xA0, 0);

   /*Calibration data for the humidity sensor*/
   tempData = spi_read(0xE2, 0);
   parH1 = spi_read(0xE3, 0) << 4 | (tempData & 0x0F);
   parH2 = spi_read(0xE1, 0) << 4 | (tempData & 0xF0);
   parH3 = spi_read(0xE4, 0);
   parH4 = spi_read(0xE5, 0);
   parH5 = spi_read(0xE6, 0);
   parH6 = spi_read(0xE7, 0);
   parH7 = spi_read(0xE8, 0);

   /*Begin configuration of the BME688 registers*/
   tempData = 0x01;
   spi_write(0x72, tempData, 1); //Configure the humidity oversampling value
   tempData = 0x00 | 0x02 << 5 | 0x05 << 2;
   spi_write(0x74, tempData, 1); //Configure the temperature and pressure oversampling values
   tempData = 0x19 | 0b01 << 6;
   spi_write(0x64, tempData, 1); //Configures the gas heating wait time. Uses a base value and a multiplier value

   //Need to calculate the heater resistance to send to the BME688, this uses some of the calibration data stored.
   double var1 = (parG1 / 16.0) + 49.0;
   double var2 = ((parG2 / 32768.0) * 0.0005) + 0.00235;
   double var3 = (parG3 / 1024);
   double var4 = var1 * (1.0 + (var2 * (double)HEATER_TARGET_TEMP));
   double var5 = var4 + (var3 * (double)AMBIENT_TEMP);
   uint8_t resHeatX = (uint8_t)(3.4 * (( var5 * (4.0 / (4.0 + (double)resHeatRange)) * (1.0/(1.0 + ((double)resHeatValue * 0.002)))) -25));
   spi_write(0x5A, resHeatX, 1); //Sends the resistance value for the gas sensor heater

   tempData = 0x00 | 1 << 5;
   spi_write(0x71, tempData, 1); //Selects the previously defined heater settings for gas measurements

   //take_reading();

   while(1)
   {
       /* Here we will take a reading and print out the info */
       take_reading();
       delay(20000); //A slight delay to not overwhelm the UART
   }

   
}
