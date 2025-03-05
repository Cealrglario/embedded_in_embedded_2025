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
void ClimateMonitorInitialize(void)
{
  /* If good initialization, set state to Idle */
  if( 1 )
  {
    ClimateMonitor_pfStateMachine = ClimateMonitorSM_Idle;
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
