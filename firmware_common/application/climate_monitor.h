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
static void ClimateMonitorSM_VerifySHTC3(void);
static void ClimateMonitorSM_TakeMeasurementSHTC3(void);
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

/* SHTC3-relevant Constants */
#define U8_SHTC3_I2C_ADDRESS        (u8)0x70 /* I2C address of the SHTC3 sensor */


#endif /* __CLIMATE_MONITOR_H */

/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File                                                                                                        */
/*--------------------------------------------------------------------------------------------------------------------*/
