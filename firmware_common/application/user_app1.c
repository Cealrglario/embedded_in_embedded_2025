/*!*********************************************************************************************************************
@file user_app1.c                                                                
@brief User's tasks / applications are written here.  This description
should be replaced by something specific to the task.

----------------------------------------------------------------------------------------------------------------------
To start a new task using this user_app1 as a template:
 1. Copy both user_app1.c and user_app1.h to the Application directory
 2. Rename the files yournewtaskname.c and yournewtaskname.h
 3. Add yournewtaskname.c and yournewtaskname.h to the Application Include and Source groups in the IAR project
 4. Use ctrl-h (make sure "Match Case" is checked) to find and replace all instances of "user_app1" with "yournewtaskname"
 5. Use ctrl-h to find and replace all instances of "UserApp1" with "YourNewTaskName"
 6. Use ctrl-h to find and replace all instances of "USER_APP1" with "YOUR_NEW_TASK_NAME"
 7. Add a call to YourNewTaskNameInitialize() in the init section of main
 8. Add a call to YourNewTaskNameRunActiveState() in the Super Loop section of main
 9. Update yournewtaskname.h per the instructions at the top of yournewtaskname.h
10. Delete this text (between the dashed lines) and update the Description below to describe your task
----------------------------------------------------------------------------------------------------------------------

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
- void UserApp1Initialize(void)
- void UserApp1RunActiveState(void)


**********************************************************************************************************************/

#include "configuration.h"
#include <stdio.h>

/***********************************************************************************************************************
Global variable definitions with scope across entire project.
All Global variable names shall start with "G_<type>UserApp1"
***********************************************************************************************************************/
/* New variables */
volatile u32 G_u32UserApp1Flags;                          /*!< @brief Global state flags */


/*--------------------------------------------------------------------------------------------------------------------*/
/* Existing variables (defined in other files -- should all contain the "extern" keyword) */
extern volatile u32 G_u32SystemTime1ms;                   /*!< @brief From main.c */
extern volatile u32 G_u32SystemTime1s;                    /*!< @brief From main.c */
extern volatile u32 G_u32SystemFlags;                     /*!< @brief From main.c */
extern volatile u32 G_u32ApplicationFlags;                /*!< @brief From main.c */


/***********************************************************************************************************************
Global variable definitions with scope limited to this local application.
Variable names shall start with "UserApp1_<type>" and be declared as static.
***********************************************************************************************************************/
static fnCode_type UserApp1_pfStateMachine;               /*!< @brief The state machine function pointer */
//static u32 UserApp1_u32Timeout;                           /*!< @brief Timeout counter used across states */


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
@fn void UserApp1Initialize(void)

@brief
Initializes the State Machine and its variables.

Should only be called once in main init section.

Requires:
- NONE

Promises:
- NONE

*/
void UserApp1Initialize(void)
{
  // Turn off all LEDs on the board by for-loop
  for(u8 i = 0; i < U8_TOTAL_LEDS - 1; i++) {
    LedOff((LedNameType)i);
  }

  /* If good initialization, set state to Idle */
  if( 1 )
  {
    UserApp1_pfStateMachine = UserApp1SM_Idle;
  }
  else
  {
    /* The task isn't properly initialized, so shut it down and don't run */
    UserApp1_pfStateMachine = UserApp1SM_Error;
  }

} /* end UserApp1Initialize() */

  
/*!----------------------------------------------------------------------------------------------------------------------
@fn void UserApp1RunActiveState(void)

@brief Selects and runs one iteration of the current state in the state machine.

All state machines have a TOTAL of 1ms to execute, so on average n state machines
may take 1ms / n to execute.

Requires:
- State machine function pointer points at current state

Promises:
- Calls the function to pointed by the state machine function pointer

*/
void UserApp1RunActiveState(void)
{
  UserApp1_pfStateMachine();

} /* end UserApp1RunActiveState */


/*------------------------------------------------------------------------------------------------------------------*/
/*! @privatesection */                                                                                            
/*--------------------------------------------------------------------------------------------------------------------*/


/**********************************************************************************************************************
State Machine Function Definitions
**********************************************************************************************************************/
/*-------------------------------------------------------------------------------------------------------------------*/
/* What does this state do? */
static void UserApp1SM_Idle(void)
{
  static u16 u16BlinkCount = 0;
  static u8 u8Counter = 0;
  static u8 u8ColourCounter = 0;
  // This will be used to "select" the LED we are changing based on a match with the masked bits
  u8 u8Offset = 0; 

  /* We will need a colour order to cycle through, which we can make Red, Yellow, Green, Cyan, Blue, Purple, White, which we
  can cycle through with an array of arrays of size 3. */

  static u8 u8ColourArray[][3] = {{RED0, 0xff, 0xff}, 
                                  {RED0, GREEN0, 0xff}, 
                                  {0xff, GREEN0, 0xff}, 
                                  {0xff, GREEN0, BLUE0}, 
                                  {0xff, 0xff, BLUE0}, 
                                  {RED0, 0xff, BLUE0}, 
                                  {RED0, GREEN0, BLUE0}};

  u16BlinkCount++;

  if(u16BlinkCount == u16BlinkTime) {
    u16BlinkCount = 0;
    u8Counter++;

    if(u8Counter == 16) {
      u8Counter = 0;
    }

    // Turn off all twelve LEDs (13-1 since we don't care about the backlight) in the dot matrix board by loop
    for(u8 i = 0; i < (U8_TOTAL_LEDS - 1); i++) {
      LedOff((LedNameType)i);
    }

    if(u8Counter & 0x01) {
      /* This is the LSB and thus the rightmost LED, so we must select the rightmost LED by setting the LED offset to 3.
      this will make more sense when you read the following code */
      u8Offset = 3;
      for(u8 j = 0; j < 3; j++) {
        // We don't want to turn the LED on if it's meant to be off
        if(u8ColourArray[u8ColourCounter][j] != 0xff) {
          LedOn(u8ColourArray[u8ColourCounter][j] + u8Offset);
        }
      }
    }

    if(u8Counter & 0x02) {
      /* This is to the left of the LSB and thus we must select the corresponding LED by setting the LED offset to 2. 
      This pattern will repeat for the rest of the LEDs. */
      u8Offset = 2;
      for(u8 j = 0; j < 3; j++) {
        if(u8ColourArray[u8ColourCounter][j] != 0xff) {
          LedOn(u8ColourArray[u8ColourCounter][j] + u8Offset);
        }
      }
    }

    if(u8Counter & 0x04) {
      u8Offset = 1;
      for(u8 j = 0; j < 3; j++) {
        if(u8ColourArray[u8ColourCounter][j] != 0xff) {
          LedOn(u8ColourArray[u8ColourCounter][j] + u8Offset);
        }
      }
    }

    if(u8Counter & 0x08) {
      u8Offset = 0;
      for(u8 j = 0; j < 3; j++) {
        if(u8ColourArray[u8ColourCounter][j] != 0xff) {
          LedOn(u8ColourArray[u8ColourCounter][j] + u8Offset);
        }
      }
    }

    u8ColourCounter++;

    if(u8ColourCounter == 7) {
      u8ColourCounter = 0;
    }

  }
     
} /* end UserApp1SM_Idle() */
     

/*-------------------------------------------------------------------------------------------------------------------*/
/* Handle an error */
static void UserApp1SM_Error(void)          
{
  printf("There was an error.");
} /* end UserApp1SM_Error() */




/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File                                                                                                        */
/*--------------------------------------------------------------------------------------------------------------------*/
