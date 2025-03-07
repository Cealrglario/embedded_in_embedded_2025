/*!*********************************************************************************************************************
@file climate_monitor.h                                                                
@brief Header file for climate_monitor

**********************************************************************************************************************/

#ifndef __CLIMATE_MONITOR_H
#define __CLIMATE_MONITOR_H

/**********************************************************************************************************************
Type Definitions
**********************************************************************************************************************/


/**********************************************************************************************************************
Function Declarations
**********************************************************************************************************************/

/*------------------------------------------------------------------------------------------------------------------*/
/*! @publicsection */                                                                                            
/*--------------------------------------------------------------------------------------------------------------------*/


/*------------------------------------------------------------------------------------------------------------------*/
/*! @protectedsection */                                                                                            
/*--------------------------------------------------------------------------------------------------------------------*/
void ClimateMonitorInitialize(void);
void ClimateMonitorRunActiveState(void);


/*------------------------------------------------------------------------------------------------------------------*/
/*! @privatesection */                                                                                            
/*--------------------------------------------------------------------------------------------------------------------*/


/***********************************************************************************************************************
State Machine Declarations
***********************************************************************************************************************/
static void ClimateMonitorSM_Idle(void);    
static void ClimateMonitorSM_Wait(void);
static void ClimateMonitorSM_SleepSHTC3(void);
static void ClimateMonitorSM_WakeSHTC3(void);
static void ClimateMonitorSM_VerifySHTC3(void);
static void ClimateMonitorSM_WaitVerifySHTC3(void);
static void ClimateMonitorSM_PrintVerifySHTC3(void);
static void ClimateMonitorSM_TakeMeasurementSHTC3(void);
static void ClimateMonitorSM_WaitMeasurementSHTC3(void);
static void ClimateMonitorSM_PrintMeasurementSHTC3(void);
static void ClimateMonitorSM_DisplayInfo(void);
static void ClimateMonitorSM_Error(void);         



/**********************************************************************************************************************
Constants / Definitions
**********************************************************************************************************************/
/* SHTC3-relevant Commands */
#define U8_SHTC3_SLEEP_MSB          (u8)0xB0 /* MSB of the command to tell the SHTC3 sensor to go to sleep */
#define U8_SHTC3_SLEEP_LSB          (u8)0x98 /* LSB of the command to tell the SHTC3 sensor to go to sleep */
#define U8_SHTC3_WAKEUP_MSB         (u8)0x35 /* MSB of the command to tell the SHTC3 sensor to wake up */
#define U8_SHTC3_WAKEUP_LSB         (u8)0x17 /* LSB of the command to tell the SHTC3 sensor to wake up */
#define U8_SHTC3_MEASURE_MSB        (u8)0x7C /* MSB of the command to tell the SHTC3 sensor to begin a measurement */
#define U8_SHTC3_MEASURE_LSB        (u8)0xA2 /* LSB of the command to tell the SHTC3 sensor to begin a measurement */
#define U8_SHTC3_READ_ID_MSB        (u8)0xEF /* MSB of the command to read the ID register of the SHTC3 sensor */
#define U8_SHTC3_READ_ID_LSB        (u8)0xC8 /* LSB of the command to read the ID register of the SHTC3 sensor */
#define U8_SHTC3_RESET_MSB          (u8)0x80 /* MSB of the command to soft reset the SHTC3 sensor */
#define U8_SHTC3_RESET_LSB          (u8)0x5D /* LSB of the command to soft reset the SHTC3 sensor */

/* SHTC3-relevant Constants */
#define U8_SHTC3_I2C_ADDRESS            (u8)0x70    /* I2C address of the SHTC3 sensor */
#define U32_SHTC3_MEASURE_PERIOD_MS     (u32)3000   /* How often the SHTC3 sensor takes a measurement, in ms */
#define U8_SHTC3_TEMP_BYTE_INDEX        (u8)0       /* Index where the temperature reading byte is stored after reading measurement */
#define U8_SHTC3_HUMIDITY_BYTE_INDEX    (u8)3       /* Index where the humidity reading byte is stored after reading measurement */
#define U32_SHTC3_TX_WAIT_MS            (u32)500    /* Wait period in ms to print out the data received from the SHTC3 sensor */


#endif /* __CLIMATE_MONITOR_H */

/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File                                                                                                        */
/*--------------------------------------------------------------------------------------------------------------------*/
