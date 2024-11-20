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
extern volatile u32 G_u32ApplicationFlags;               /*!< @brief From main.c */

extern u8 G_au8DebugScanfBuffer[DEBUG_SCANF_BUFFER_SIZE]; // From debug.c
extern u8 G_u8DebugScanfCharCount;                        // From debug.c


/***********************************************************************************************************************
Global variable definitions with scope limited to this local application.
Variable names shall start with "UserApp1_<type>" and be declared as static.
***********************************************************************************************************************/

/* Reminder that a "buffer" is simply a term for a storage container containing something for use later on.
This can only hold characters so we must use ASCII characters in u8 */
static u8 UserApp1_au8UserInputBuffer[U16_USER1_INPUT_BUFFER_SIZE]; // Specifically for user inputs to UserApp1 rather than the debug.
// We are declaring this globally so any function in UserApp1 can access the buffer, obviously.

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
  /* u8 u8String[] = "A string to print that returns cursor to start of the next line.\n\r";
  u8 u8String2[] = "Here's a number: ";
  u8 u8String3[] = " The 'cursor' was here.";
  u32 u32Number = 1234567;

  DebugPrintf(u8String);
  DebugPrintf(u8String2);
  DebugPrintNumber(u32Number);
  DebugPrintf(u8String3);
  DebugLineFeed();
  DebugPrintf(u8String3);
  DebugLineFeed(); */

  // Initialize the input buffer unique to UserApp1
  for(u8 i = 0; i < U16_USER1_INPUT_BUFFER_SIZE; i++)
  {
    UserApp1_au8UserInputBuffer[i] = '\0'; 
    /* We want everything to be null terminators (!!! USE SINGLE QUOTES !!!, or else it will be read as a string literal, or in 
    other words, a value of NULL (especially if placed in the middle of an already existing non-null string) rather
    than simply a null terminator) so that no matter the length of the string in the buffer, as long as it's within the buffer size, 
    it will end with a proper null terminator (this is assuming that we send information to the buffer by replacing these null 
    terminators with the information incrementally) */
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
  static u8 au8NumCharsMessage[] = "\n\rNumber of characters in buffer: "; // \r returns the cursor to the left after the newline
  static u8 au8BufferMessage[] = "\n\rBuffer Contents:\n\r";
  u8 u8CharCount;

  if(WasButtonPressed(BUTTON0)) 
  {
    ButtonAcknowledge(BUTTON0);

    DebugPrintf(au8NumCharsMessage);
    DebugPrintNumber(G_u8DebugScanfCharCount); // Make sure you use DebugPrintNUMBER as we need to convert it from a number first
    DebugLineFeed(); // Sets the cursor on a new line with the cursor to the left
  }

  if(WasButtonPressed(BUTTON1))
  {
    ButtonAcknowledge(BUTTON1);

    // Read the buffer and add a null terminator
    u8CharCount = DebugScanf(UserApp1_au8UserInputBuffer);
    UserApp1_au8UserInputBuffer[u8CharCount] = '\0'; // Use single quotes, it matters.

    DebugPrintf(au8BufferMessage);
    DebugPrintf(UserApp1_au8UserInputBuffer);
    DebugLineFeed();
  }
     
} /* end UserApp1SM_Idle() */
     

/*-------------------------------------------------------------------------------------------------------------------*/
/* Handle an error */
static void UserApp1SM_Error(void)          
{
  
} /* end UserApp1SM_Error() */




/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File                                                                                                        */
/*--------------------------------------------------------------------------------------------------------------------*/
