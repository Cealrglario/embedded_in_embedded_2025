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
static u32 ClimateMonitor_u32Timer;                             /*!< @brief Timer used for wait periods across states */
static int ClimateMonitor_u32SHTC3TempReading;                  /*!< @brief Temperature reading in Celsius obtained from SHTC3 */
static int ClimateMonitor_u32SHTC3FahrenheitReading;            /*!< @brief Temperature reading in Fahrenheit obtained from SHTC3 */
static u32 ClimateMonitor_u32SHTC3HumidityReading;              /*!< @brief Humidity reading in RH% obtained from SHTC3 */
static bool isFahrenheit = FALSE;                               /*!< @brief Flag to display temp in fahrenheit */
static u8 au8SHTC3VerifyResponse[U8_SHTC3_VERIFY_BYTES];        /*!< @brief Response from SHTC3 ID Register */
static u8 au8SHTC3Measurement[U8_SHTC3_MEASURE_BYTES];          /*!< @brief Response from SHTC3 Measurements */


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

    LcdClearScreen();

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

/* The following sleep and wake states are just to put the sensor into a predictable state before measurement looping begins */

static void ClimateMonitorSM_SleepSHTC3(void) {
    static u8 au8SHTC3_Sleep[] = {U8_SHTC3_SLEEP_MSB, U8_SHTC3_SLEEP_LSB};

    TwiWriteData(U8_SHTC3_I2C_ADDRESS, U8_SHTC3_COMMAND_LENGTH, au8SHTC3_Sleep, TWI_STOP);

    ClimateMonitor_pfStateMachine = ClimateMonitorSM_WakeSHTC3;
}

static void ClimateMonitorSM_WakeSHTC3(void) {
    static u8 au8SHTC3_Wake[] = {U8_SHTC3_WAKEUP_MSB, U8_SHTC3_WAKEUP_LSB};

    TwiWriteData(U8_SHTC3_I2C_ADDRESS, U8_SHTC3_COMMAND_LENGTH, au8SHTC3_Wake, TWI_STOP);

    ClimateMonitor_pfStateMachine = ClimateMonitorSM_VerifySHTC3;
}

static void ClimateMonitorSM_VerifySHTC3(void) {
    static u8 au8VerifySHTC3[] = {U8_SHTC3_READ_ID_MSB, U8_SHTC3_READ_ID_LSB};

    TwiWriteData(U8_SHTC3_I2C_ADDRESS, U8_SHTC3_COMMAND_LENGTH, au8VerifySHTC3, TWI_STOP);
    TwiReadData(U8_SHTC3_I2C_ADDRESS, au8SHTC3VerifyResponse, U8_SHTC3_VERIFY_BYTES);

    ClimateMonitor_u32Timer = 0;
    ClimateMonitor_pfStateMachine = ClimateMonitorSM_WaitVerifySHTC3;
}

static void ClimateMonitorSM_WaitVerifySHTC3(void) {
    ClimateMonitor_u32Timer++;

    if (ClimateMonitor_u32Timer == U32_SHTC3_TX_WAIT_MS) {
      ClimateMonitor_pfStateMachine = ClimateMonitorSM_PrintVerifySHTC3;
    }
}

static void ClimateMonitorSM_PrintVerifySHTC3(void) {
    DebugLineFeed();
    DebugPrintf("Verification ID (one byte per line, values in decimal): ");
    DebugLineFeed();

    for (u8 i = 0; i < (sizeof(au8SHTC3VerifyResponse) / sizeof(u8)); i++) {
      DebugPrintNumber(au8SHTC3VerifyResponse[i]);
      DebugLineFeed();
    }

    if (au8SHTC3VerifyResponse[0] == U8_SHTC3_VERIFY_ID_MSB && au8SHTC3VerifyResponse[1] == U8_SHTC3_VERIFY_ID_LSB) {
      ClimateMonitor_pfStateMachine = ClimateMonitorSM_TakeMeasurementSHTC3;
    } else {
      ClimateMonitor_pfStateMachine = ClimateMonitorSM_Error;
    }
}

static void ClimateMonitorSM_TakeMeasurementSHTC3(void) {
  static u8 au8TakeMeasurementSHTC3[] = {U8_SHTC3_MEASURE_MSB, U8_SHTC3_MEASURE_LSB};

  TwiWriteData(U8_SHTC3_I2C_ADDRESS, U8_SHTC3_COMMAND_LENGTH, au8TakeMeasurementSHTC3, TWI_STOP);
  TwiReadData(U8_SHTC3_I2C_ADDRESS, au8SHTC3Measurement, U8_SHTC3_MEASURE_BYTES);

  ClimateMonitor_u32Timer = 0;
  ClimateMonitor_pfStateMachine = ClimateMonitorSM_WaitMeasurementSHTC3;
}

static void ClimateMonitorSM_WaitMeasurementSHTC3(void) {
  ClimateMonitor_u32Timer++;

  if (ClimateMonitor_u32Timer == U32_SHTC3_TX_WAIT_MS) {
    ClimateMonitor_pfStateMachine = ClimateMonitorSM_PrintMeasurementSHTC3;
  }
}

static void ClimateMonitorSM_PrintMeasurementSHTC3(void) {
  u16 u16SHTC3BinaryTemp = 0000;
  u16 u16SHTC3BinaryHumidity = 0000;

  DebugLineFeed();
  DebugPrintf("Measurement (one byte per line, values in decimal): ");
  DebugLineFeed();

  for (int i = 0; i < (sizeof(au8SHTC3Measurement) / sizeof(u8)); i++) {
    DebugPrintNumber(au8SHTC3Measurement[i]);
    DebugLineFeed();
  }

  /* Store the MSB and LSB of the measurements in order */
  u16SHTC3BinaryTemp = ((u16)au8SHTC3Measurement[U8_SHTC3_TEMP_MSB_INDEX] << 8) | (u16)au8SHTC3Measurement[U8_SHTC3_TEMP_LSB_INDEX];
  u16SHTC3BinaryHumidity = ((u16)au8SHTC3Measurement[U8_SHTC3_HUMIDITY_MSB_INDEX] << 8) | (u16)au8SHTC3Measurement[U8_SHTC3_HUMIDITY_LSB_INDEX];

  /* Convert binary measurements to real values */
  ClimateMonitor_u32SHTC3TempReading = (int)(-45 + 175 * ((float)u16SHTC3BinaryTemp / 65536));
  ClimateMonitor_u32SHTC3HumidityReading = (int)(100 * ((float)u16SHTC3BinaryHumidity / 65536));
  ClimateMonitor_u32SHTC3FahrenheitReading = ClimateMonitor_u32SHTC3TempReading * 9/5 + 32;

  ClimateMonitor_u32Timer = 0;
  ClimateMonitor_pfStateMachine = ClimateMonitorSM_DisplayInfo;
}

static void ClimateMonitorSM_DisplayInfo(void) {
  static PixelAddressType sReadingLocation;
  static PixelAddressType sClimateInfoLocation;
  static PixelAddressType sRecommendationLocation;
  extern PixelBlockType G_sLcdClearLine1;
  extern PixelBlockType G_sLcdClearLine2;
  extern PixelBlockType G_sLcdClearLine4;
  extern PixelBlockType G_sLcdClearLine5;
  u8 au8TempReading[U8_SMALL_FONT_STRING_MAX_LEN];
  u8 au8HumidityReading[U8_SMALL_FONT_STRING_MAX_LEN];
  u8 au8ClimateInfo[U8_SMALL_FONT_STRING_MAX_LEN];
  u8 au8Recommedation[U8_SMALL_FONT_STRING_MAX_LEN];

  /* Select output climate info message based on data */
  if (ClimateMonitor_u32SHTC3TempReading <= -20) {
    // Frigid!
    strcpy((char*)au8ClimateInfo, "Extremely cold.");
    strcpy((char*)au8Recommedation, "Wear a heavy coat!");
}

  else if (ClimateMonitor_u32SHTC3TempReading <= 0 && ClimateMonitor_u32SHTC3HumidityReading >= (u32)60) {
    // Cold and chilly!
    strcpy((char*)au8ClimateInfo, "Cold and humid.");
    strcpy((char*)au8Recommedation, "Breathable coat!");
  }

  else if (ClimateMonitor_u32SHTC3TempReading <= 0) {
    // Cool, crisp fresh air
    strcpy((char*)au8ClimateInfo, "Cold and crisp.");
    strcpy((char*)au8Recommedation, "Wear some layers!");
  }

  else if (ClimateMonitor_u32SHTC3TempReading <= 10 && ClimateMonitor_u32SHTC3HumidityReading >= (u32)60) {
    // Mild, but very humid.
    strcpy((char*)au8ClimateInfo, "Mild and humid.");
    strcpy((char*)au8Recommedation, "Breathable layers!");
  }

  else if (ClimateMonitor_u32SHTC3TempReading <= 10) {
    // Mild, wear a light jacket
    strcpy((char*)au8ClimateInfo, "Mild, cool air.");
    strcpy((char*)au8Recommedation, "Wear a light coat!");
  }

  else if (ClimateMonitor_u32SHTC3TempReading <= 15 && ClimateMonitor_u32SHTC3HumidityReading >= (u32)60) {
    // Warm but very humid! Breathable clothing.
    strcpy((char*)au8ClimateInfo, "Warm but humid.");
    strcpy((char*)au8Recommedation, "Breathable clothing!");
  }

  else if (ClimateMonitor_u32SHTC3TempReading <= 15) {
    // Warm, enjoy the weather, wear a sweater!
    strcpy((char*)au8ClimateInfo, "Warm and crisp.");
    strcpy((char*)au8Recommedation, "Wear a sweater!");
  }

  else if (ClimateMonitor_u32SHTC3TempReading <= 25 && ClimateMonitor_u32SHTC3HumidityReading >= (u32)60) {
    // Almost perfect, but too humid. Breathable clothing.
    strcpy((char*)au8ClimateInfo, "Ideal but humid.");
    strcpy((char*)au8Recommedation, "Airy clothing!");
  }

  else if (ClimateMonitor_u32SHTC3TempReading <= 25) {
    // Perfect temperature! Go outside!
    strcpy((char*)au8ClimateInfo, "Perfect conditions.");
    strcpy((char*)au8Recommedation, "Go outside!");
  }

  else if (ClimateMonitor_u32SHTC3TempReading <= 35 && ClimateMonitor_u32SHTC3HumidityReading >= (u32)65) {
    // Hot and humid! Stay in A/C room.
    strcpy((char*)au8ClimateInfo, "Hot and humid.");
    strcpy((char*)au8Recommedation, "Stay dry and cool!");
  }

  else if (ClimateMonitor_u32SHTC3TempReading <= 35) {
    // Hot, bring cold drinks
    strcpy((char*)au8ClimateInfo, "Very hot.");
    strcpy((char*)au8Recommedation, "Bring some water!");
  }

  else if (ClimateMonitor_u32SHTC3TempReading > 35) {
    // Stay indoors! Too hot!
    strcpy((char*)au8ClimateInfo, "Dangerously hot.");
    strcpy((char*)au8Recommedation, "Stay indoors!");
  }

  else {
    // Error
    strcpy((char*)au8ClimateInfo, "Unknown conditions");
    strcpy((char*)au8Recommedation, "N/A");
  }

  /* Parse each of the bytes sent by SHTC3 so we can display it in ASCII on the LCD */
  if (isFahrenheit) {
    sprintf(au8TempReading, "Temperature: %dF", ClimateMonitor_u32SHTC3FahrenheitReading);
  } else {
    sprintf(au8TempReading, "Temperature: %dC", ClimateMonitor_u32SHTC3TempReading);
  }

  sprintf(au8HumidityReading, "Humidity: %ld%%", ClimateMonitor_u32SHTC3HumidityReading);

  /* Print messages to the center of the LCD */
  sReadingLocation.u16PixelColumnAddress =
  U16_LCD_CENTER_COLUMN - (strlen((char const*)au8TempReading) * (U8_LCD_SMALL_FONT_COLUMNS + U8_LCD_SMALL_FONT_SPACE) / 2); 

  sReadingLocation.u16PixelRowAddress = U8_LCD_SMALL_FONT_LINE1;
  LcdClearPixels(&G_sLcdClearLine1);

  LcdLoadString(au8TempReading, LCD_FONT_SMALL, &sReadingLocation);

  sReadingLocation.u16PixelColumnAddress =
  U16_LCD_CENTER_COLUMN - (strlen((char const*)au8HumidityReading) * (U8_LCD_SMALL_FONT_COLUMNS + U8_LCD_SMALL_FONT_SPACE) / 2); 

  sReadingLocation.u16PixelRowAddress = U8_LCD_SMALL_FONT_LINE2;
  LcdClearPixels(&G_sLcdClearLine2);

  LcdLoadString(au8HumidityReading, LCD_FONT_SMALL, &sReadingLocation);

  sClimateInfoLocation.u16PixelColumnAddress =
  U16_LCD_CENTER_COLUMN - (strlen((char const*)au8ClimateInfo) * (U8_LCD_SMALL_FONT_COLUMNS + U8_LCD_SMALL_FONT_SPACE) / 2); 

  sClimateInfoLocation.u16PixelRowAddress = U8_LCD_SMALL_FONT_LINE4;
  LcdClearPixels(&G_sLcdClearLine4);

  LcdLoadString(au8ClimateInfo, LCD_FONT_SMALL, &sClimateInfoLocation);

  sRecommendationLocation.u16PixelColumnAddress =
  U16_LCD_CENTER_COLUMN - (strlen((char const*)au8Recommedation) * (U8_LCD_SMALL_FONT_COLUMNS + U8_LCD_SMALL_FONT_SPACE) / 2); 

  sRecommendationLocation.u16PixelRowAddress = U8_LCD_SMALL_FONT_LINE5;
  LcdClearPixels(&G_sLcdClearLine5);

  LcdLoadString(au8Recommedation, LCD_FONT_SMALL, &sRecommendationLocation);

  ClimateMonitor_pfStateMachine = ClimateMonitorSM_SelectLED;
}

static void ClimateMonitorSM_SelectLED(void) {
  /* Turn off all LEDs except the LCD backlight */
  for (int i = 0;  i < U8_TOTAL_LEDS - 1; i++) {
    LedOff((LedNameType)i);
  }

  if (ClimateMonitor_u32SHTC3TempReading <= -20) {
    /* Super cold, purple LED*/
    LedOn(BLUE0);
    LedPWM(RED0, LED_PWM_50);
  } else if (ClimateMonitor_u32SHTC3TempReading <= 0) {
    /* Cold, cyan LED */
    LedOn(BLUE0);
    LedOn(GREEN0);
  } else if (ClimateMonitor_u32SHTC3TempReading <= 10) {
    /* Mild, light green/cyan LED*/
    LedOn(GREEN1);
    LedPWM(RED1, LED_PWM_25);
    LedPWM(BLUE1, LED_PWM_10);
  } else if (ClimateMonitor_u32SHTC3TempReading <= 15) {
    /* Warmer, yellow LED*/
    LedOn(GREEN1);
    LedOn(RED1);
  } else if (ClimateMonitor_u32SHTC3TempReading <= 25) {
    /* Perfect, golden LED*/
    LedOn(RED2);
    LedPWM(GREEN2, LED_PWM_35);
  } else if (ClimateMonitor_u32SHTC3TempReading <= 35) {
    /* Hot, orange LED*/
    LedOn(RED3);
    LedPWM(GREEN3, LED_PWM_10);
  } else if (ClimateMonitor_u32SHTC3TempReading > 35) {
    /* Too hot, red LED*/
    LedOn(RED3);
  } else { 
    /* Error blinking */
    LedBlink(RED0, LED_1HZ);
    LedBlink(RED1, LED_1HZ);
    LedBlink(RED2, LED_1HZ);
    LedBlink(RED3, LED_1HZ);
  }

  ClimateMonitor_pfStateMachine = ClimateMonitorSM_Idle;

}

/* What does this state do? */
static void ClimateMonitorSM_Idle(void)
{
  ClimateMonitor_u32Timer++;

  if (WasButtonPressed(BUTTON0)) {
    ButtonAcknowledge(BUTTON0);

    isFahrenheit = FALSE;
    ClimateMonitor_pfStateMachine = ClimateMonitorSM_TakeMeasurementSHTC3;
  }

  else if (WasButtonPressed(BUTTON1)) {
    ButtonAcknowledge(BUTTON1);

    isFahrenheit = TRUE;
    ClimateMonitor_pfStateMachine = ClimateMonitorSM_TakeMeasurementSHTC3;
  }
  
  else if (ClimateMonitor_u32Timer == U32_SHTC3_MEASURE_PERIOD_MS) {
    ClimateMonitor_pfStateMachine = ClimateMonitorSM_TakeMeasurementSHTC3;
  }

} /* end ClimateMonitorSM_Idle() */
     

/*-------------------------------------------------------------------------------------------------------------------*/
/* Handle an error */
static void ClimateMonitorSM_Error(void)          
{
  
} /* end ClimateMonitorSM_Error() */




/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File                                                                                                        */
/*--------------------------------------------------------------------------------------------------------------------*/
