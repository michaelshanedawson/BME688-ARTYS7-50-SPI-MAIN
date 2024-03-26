
#include <stdio.h>
#include "xparameters.h"
#include "xil_printf.h"

#include "xintc.h"
#include "intc_header.h"

#include "xspi.h"
#include "spi_header.h"

#include "xuartlite.h"
#include "uartlite_header.h"
int main ()
{

    static XIntc microblaze_0_axi_intc;
    static XSpi axi_quad_spi_0;
    static XSpi axi_quad_spi_1;
    static XUartLite axi_uartlite_0;

    print("---Entering main---\n\r");

    {
        int status;
        print("\r\nRunning IntcSelfTestExample for microblaze_0_axi_intc ... \r\n");
        status = IntcSelfTestExample(XPAR_MICROBLAZE_0_AXI_INTC_BASEADDR);
        if (status == 0) {
            print("IntcSelfTestExample PASSED \r\n");
        } else {
            print("IntcSelfTestExample FAILED \r\n");
        }
    }

    {
        int status;
        print("\r\nRunning SpiSelfTestExample for axi_quad_spi_0 ... \r\n");
        status = SpiSelfTestExample(XPAR_AXI_QUAD_SPI_0_BASEADDR);
        if (status == 0) {
            print("SpiSelfTestExample PASSED \r\n");
        } else {
            print("SpiSelfTestExample FAILED \r\n");
        }
    }

    {
        int status;
        print("\r\nRunning SpiSelfTestExample for axi_quad_spi_1 ... \r\n");
        status = SpiSelfTestExample(XPAR_AXI_QUAD_SPI_1_BASEADDR);
        if (status == 0) {
            print("SpiSelfTestExample PASSED \r\n");
        } else {
            print("SpiSelfTestExample FAILED \r\n");
        }
    }

    {
        int status;
        print("\r\nRunning UartLiteSelfTestExample for axi_uartlite_0 ... \r\n");
        status = UartLiteSelfTestExample(XPAR_AXI_UARTLITE_0_BASEADDR);
        if (status == 0) {
            print("UartLiteSelfTestExample PASSED \r\n");
        } else {
            print("UartLiteSelfTestExample FAILED \r\n");
        }
    }

    print("---Exiting main---");
    return 0;
}
