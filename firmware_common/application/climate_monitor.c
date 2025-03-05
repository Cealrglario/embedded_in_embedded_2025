/*!*********************************************************************************************************************
@file climate_monitor.c                                                                
@brief Application that handles the logic related to retrieving, processing, and displaying the temperature and humidity
sensor data from an SHTC3 sensor for implementation of this development board as a climate monitor.
------------------------------------------------------------------------------------------------------------------------
GLOBALS
- NONE

CONSTANTS
- NONE

TYPES
- NONE

PUBLIC FUNCTIONS
- NONE

PROTECTED FUNCTIONS
- void ClimateMonitorInitialize(void)
- void ClimateMonitorRunActiveState(void)


**********************************************************************************************************************/

#include "configuration.h"

/***********************************************************************************************************************
Global variable definitions with scope across entire project.
All Global variable names shall start with "G_<type>ClimateMonitor"
***********************************************************************************************************************/
/* New variables */
volatile u32 G_u32ClimateMonitorFlags;                          /*!< @brief Global state flags */


/*--------------------------------------------------------------------------------------------------------------------*/
/* Existing variables (defined in other files -- should all contain the "extern" keyword) */
extern volatile u32 G_u32SystemTime1ms;                   /*!< @brief From main.c */
extern volatile u32 G_u32SystemTime1s;                    /*!< @brief From main.c */
extern volatile u32 G_u32SystemFlags;                     /*!< @brief From main.c */
extern volatile u32 G_u32ApplicationFlags;                /*!< @brief From main.c */


/***********************************************************************************************************************
Global variable definitions with scope limited to this local application.
Variable names shall start with "ClimateMonitor_<type>" and be declared as static.
***********************************************************************************************************************/
static fnCode_type ClimateMonitor_pfStateMachine;               /*!< @brief The state machine function pointer */
//static u32 ClimateMonitor_u32Timeout;                           /*!< @brief Timeout counter used across states */


/**********************************************************************************************************************
Function Definitions
**********************************************************************************************************************/

/*--------------------------------------------------------------------------------------------------------------------*/
/*! @publicsection */                                                                                            
/*--------------------------------------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------------------------------*/
/*! @protectedsection */                                                                                            
/*--------------------------------------------------------------------------------------------------------------------*/

/*!--------------------------------------------------------------------------------------------------------------------
@fn void ClimateMonitorInitialize(void)

@brief
Initializes the State Machine and its variables.

Should only be called once in main init section.

Requires:
- NONE

Promises:
- NONE

*/
void ClimateMonitorInitialize(void) {
    ErrorStatusType eErrorStatus = SUCCESS;

    /* Initialize I2C pins */
    eErrorStatus += BladeRequestPin(BLADE_PIN8, PERIPHERAL);
    eErrorStatus += BladeRequestPin(BLADE_PIN9, PERIPHERAL);

    /* If good initialization, set state to Idle */
    if(eErrorStatus == SUCCESS)
    {
      ClimateMonitor_pfStateMachine = ClimateMonitorSM_SleepSHTC3;
    }
    else
    {
      /* The task isn't properly initialized, so shut it down and don't run */
      ClimateMonitor_pfStateMachine = ClimateMonitorSM_Error;
    }

} /* end ClimateMonitorInitialize() */

  
/*!----------------------------------------------------------------------------------------------------------------------
@fn void ClimateMonitorRunActiveState(void)

@brief Selects and runs one iteration of the current state in the state machine.

All state machines have a TOTAL of 1ms to execute, so on average n state machines
may take 1ms / n to execute.

Requires:
- State machine function pointer points at current state

Promises:
- Calls the function to pointed by the state machine function pointer

*/
void ClimateMonitorRunActiveState(void)
{
  ClimateMonitor_pfStateMachine();

} /* end ClimateMonitorRunActiveState */


/*------------------------------------------------------------------------------------------------------------------*/
/*! @privatesection */                                                                                            
/*--------------------------------------------------------------------------------------------------------------------*/


/**********************************************************************************************************************
State Machine Function Definitions
**********************************************************************************************************************/
/*-------------------------------------------------------------------------------------------------------------------*/

static void ClimateMonitorSM_SleepSHTC3(void) {
    u8 au8SHTC3_Sleep[] = {U8_SHTC3_SLEEP_MSB, U8_SHTC3_SLEEP_LSB};

    TwiWriteData(U8_SHTC3_I2C_ADDRESS, 2, au8SHTC3_Sleep, TWI_STOP);

    ClimateMonitor_pfStateMachine = ClimateMonitorSM_WakeSHTC3;
}

static void ClimateMonitorSM_WakeSHTC3(void) {
    u8 au8SHTC3_Wake[] = {U8_SHTC3_WAKEUP_MSB, U8_SHTC3_WAKEUP_LSB};

    TwiWriteData(U8_SHTC3_I2C_ADDRESS, 2, au8SHTC3_Wake, TWI_STOP);

    ClimateMonitor_pfStateMachine = ClimateMonitorSM_VerifySHTC3;
}

static void ClimateMonitorSM_VerifySHTC3(void) {
    u8 au8VerifySHTC3[] = {U8_SHTC3_READ_ID_MSB, U8_SHTC3_READ_ID_LSB};
    u8 au8SHTC3_Response[3] = {0, 0, 0};

    TwiWriteData(U8_SHTC3_I2C_ADDRESS, 2, au8VerifySHTC3, TWI_NO_STOP);
    TwiReadData(U8_SHTC3_I2C_ADDRESS, au8SHTC3_Response, 3);

    DebugPrintf("Response (each line is one byte): ");
    DebugLineFeed();

    for (int i = 0; i < (sizeof(au8SHTC3_Response) / sizeof(u8)); i++) {
      DebugPrintNumber(au8SHTC3_Response[i]);
      DebugLineFeed();
    }

    ClimateMonitor_pfStateMachine = ClimateMonitorSM_Idle;
}

static void ClimateMonitorSM_TakeMeasurementSHTC3(void) {
  u8 au8TakeMeasurementSHTC3[] = {U8_SHTC3_MEASURE_MSB, U8_SHTC3_MEASURE_LSB};
  u8 au8SHTC3Measurement[6] = {0, 0, 0, 0, 0, 0};

  TwiWriteData(U8_SHTC3_I2C_ADDRESS, 2, au8TakeMeasurementSHTC3, TWI_NO_STOP);
  TwiReadData(U8_SHTC3_I2C_ADDRESS, au8SHTC3Measurement, 6);

  DebugPrintf("Measurement (each line is one byte): ");
  DebugLineFeed();

  for (int i = 0; i < (sizeof(au8SHTC3Measurement) / sizeof(u8)); i++) {
    DebugPrintNumber(au8SHTC3Measurement[i]);
    DebugLineFeed();
  }

  ClimateMonitor_pfStateMachine = ClimateMonitorSM_Idle;

}

/* What does this state do? */
static void ClimateMonitorSM_Idle(void)
{
     
} /* end ClimateMonitorSM_Idle() */
     

/*-------------------------------------------------------------------------------------------------------------------*/
/* Handle an error */
static void ClimateMonitorSM_Error(void)          
{
  
} /* end ClimateMonitorSM_Error() */




/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File                                                                                                        */
/*--------------------------------------------------------------------------------------------------------------------*/
