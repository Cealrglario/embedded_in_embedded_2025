/*!*********************************************************************************************************************
@file user_app1.h                                                                
@brief Header file for user_app1

----------------------------------------------------------------------------------------------------------------------
To start a new task using this user_app1 as a template:
1. Follow the instructions at the top of user_app1.c
2. Use ctrl-h to find and replace all instances of "user_app1" with "yournewtaskname"
3. Use ctrl-h to find and replace all instances of "UserApp1" with "YourNewTaskName"
4. Use ctrl-h to find and replace all instances of "USER_APP1" with "YOUR_NEW_TASK_NAME"
5. Add #include yournewtaskname.h" to configuration.h
6. Add/update any special configurations required in configuration.h (e.g. peripheral assignment and setup values)
7. Delete this text (between the dashed lines)
----------------------------------------------------------------------------------------------------------------------

**********************************************************************************************************************/

#ifndef __USER_APP1_H
#define __USER_APP1_H

/**********************************************************************************************************************
Type Definitions
**********************************************************************************************************************/

/**********************************************************************************************************************
Function Declarations
**********************************************************************************************************************/
static u8 mapInputToNote(char input) {
    switch (input) {
        case 'z': return 0;
        case 's': return 1;
        case 'x': return 2;
        case 'd': return 3;
        case 'c': return 4;
        case 'v': return 5;
        case 'g': return 6;
        case 'b': return 7;
        case 'h': return 8;
        case 'n': return 9;
        case 'j': return 10;
        case 'm': return 11;
        default: return 0; // Default for unmapped characters
    }
}

/*------------------------------------------------------------------------------------------------------------------*/
/*! @publicsection */                                                                                            
/*--------------------------------------------------------------------------------------------------------------------*/


/*------------------------------------------------------------------------------------------------------------------*/
/*! @protectedsection */                                                                                            
/*--------------------------------------------------------------------------------------------------------------------*/
void UserApp1Initialize(void);
void UserApp1RunActiveState(void);


/*------------------------------------------------------------------------------------------------------------------*/
/*! @privatesection */                                                                                            
/*--------------------------------------------------------------------------------------------------------------------*/


/***********************************************************************************************************************
State Machine Declarations
***********************************************************************************************************************/
static void UserApp1SM_Idle(void);    
static void UserApp1SM_Error(void);         



/**********************************************************************************************************************
Constants / Definitions
**********************************************************************************************************************/
#define USERAPP1_USER_INPUT_NOTE_BUFFER (u16)(DEBUG_SCANF_BUFFER_SIZE + 1) // We can only play one note at a time, but add space for excess inputs
#define U8_MIN_OCTAVE (u8)0 
#define U8_MAX_OCTAVE (u8)3 // We are starting at C3 so we can only go up 3 octaves up to B6
#define U16_NOTE_LENGTH (u16)150 // Each note on the keyboard lasts 300ms

#endif /* __USER_APP1_H */

/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File                                                                                                        */
/*--------------------------------------------------------------------------------------------------------------------*/
