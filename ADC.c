/*
 * ADC.c
 *
 *  Created on: Jul 8, 2017
 *      Author: jdpvi
 */

/* DriverLib Includes */
#include "driverlib.h"

/* Standard Includes */
#include <stdio.h>
#include <stdint.h>

#include "msp.h"

#include "ADC.h"

void initAdc(void)
{
    /* Initializing ADC (MCLK/1/4) */
    MAP_ADC14_enableModule();
    MAP_ADC14_initModule(ADC_CLOCKSOURCE_MCLK, ADC_PREDIVIDER_1, ADC_DIVIDER_4,
                         0);

    /* Configuring GPIOs (5.5 A0) */
    MAP_GPIO_setAsPeripheralModuleFunctionInputPin(
            GPIO_PORT_P5, GPIO_PIN5, GPIO_TERTIARY_MODULE_FUNCTION);

    /* Configuring ADC Memory */
    MAP_ADC14_configureSingleSampleMode(ADC_MEM0, false);
    MAP_ADC14_configureConversionMemory(ADC_MEM0, ADC_VREFPOS_AVCC_VREFNEG_VSS,
                                        ADC_INPUT_A0, false);

    /* Configuring Sample Timer */
    MAP_ADC14_enableSampleTimer(ADC_MANUAL_ITERATION);

    /* Enabling/Toggling Conversion */
    MAP_ADC14_enableConversion();
    MAP_ADC14_toggleConversionTrigger();
}

uint16_t readAdc(void)
{
    return MAP_ADC14_getResult(ADC_MEM0);
}
