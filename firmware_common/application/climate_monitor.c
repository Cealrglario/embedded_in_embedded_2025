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
static int ClimateMonitor_u32SHTC3TempReading = 20;              /*!< @brief Temperature reading in Celsius obtained from SHTC3 */
static int ClimateMonitor_u32SHTC3FahrenheitReading = 52;        /*!< @brief Temperature reading in Fahrenheit obtained from SHTC3 */
static u32 ClimateMonitor_u32SHTC3HumidityReading = 30;          /*!< @brief Humidity reading in RH% obtained from SHTC3 */
static bool isFahrenheit = FALSE;                                /*!< @brief Flag to display temp in fahrenheit */


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

static void ClimateMonitorSM_SleepSHTC3(void) {
    static u8 au8SHTC3_Sleep[] = {U8_SHTC3_SLEEP_MSB, U8_SHTC3_SLEEP_LSB};

    TwiWriteData(U8_SHTC3_I2C_ADDRESS, 2, au8SHTC3_Sleep, TWI_STOP);

    ClimateMonitor_pfStateMachine = ClimateMonitorSM_WakeSHTC3;
}

static void ClimateMonitorSM_WakeSHTC3(void) {
    static u8 au8SHTC3_Wake[] = {U8_SHTC3_WAKEUP_MSB, U8_SHTC3_WAKEUP_LSB};

    TwiWriteData(U8_SHTC3_I2C_ADDRESS, 2, au8SHTC3_Wake, TWI_STOP);

    ClimateMonitor_pfStateMachine = ClimateMonitorSM_VerifySHTC3;
}

static void ClimateMonitorSM_VerifySHTC3(void) {
    static u8 au8VerifySHTC3[] = {U8_SHTC3_READ_ID_MSB, U8_SHTC3_READ_ID_LSB};
    static u8 au8SHTC3_Response[3] = {0, 0, 0};

    TwiWriteData(U8_SHTC3_I2C_ADDRESS, 2, au8VerifySHTC3, TWI_NO_STOP);
    TwiReadData(U8_SHTC3_I2C_ADDRESS, au8SHTC3_Response, 3);

    DebugPrintf("Response (each line is one byte): ");
    DebugLineFeed();

    for (int i = 0; i < (sizeof(au8SHTC3_Response) / sizeof(u8)); i++) {
      DebugPrintNumber(au8SHTC3_Response[i]);
      DebugLineFeed();
    }

    ClimateMonitor_pfStateMachine = ClimateMonitorSM_TakeMeasurementSHTC3;
}

static void ClimateMonitorSM_TakeMeasurementSHTC3(void) {
  static u8 au8TakeMeasurementSHTC3[] = {U8_SHTC3_MEASURE_MSB, U8_SHTC3_MEASURE_LSB};
  static u8 au8SHTC3Measurement[6] = {0, 0, 0, 0, 0, 0};

  TwiWriteData(U8_SHTC3_I2C_ADDRESS, 2, au8TakeMeasurementSHTC3, TWI_NO_STOP);
  TwiReadData(U8_SHTC3_I2C_ADDRESS, au8SHTC3Measurement, 6);

  DebugPrintf("Measurement (each line is one byte): ");
  DebugLineFeed();

  for (int i = 0; i < (sizeof(au8SHTC3Measurement) / sizeof(u8)); i++) {
    DebugPrintNumber(au8SHTC3Measurement[i]);
    DebugLineFeed();
  }

  /* Convert binary measurements to real values */
  // ClimateMonitor_u32SHTC3TempReading = (-45 + 175 * (au8SHTC3Measurement[U8_SHTC3_TEMP_BYTE_INDEX] / 65536));
  // ClimateMonitor_u32SHTC3HumidityReading = (100 * (au8SHTC3Measurement[U8_SHTC3_HUMIDITY_BYTE_INDEX] / 65536));
  // ClimateMonitor_u32SHTC3FahrenheitReading = ClimateMonitor_u32SHTC3TempReading * 9/5 + 32;

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
  u8 au8TempReading[20];
  u8 au8HumidityReading[20];
  u8 au8ClimateInfo[20];
  u8 au8Recommedation[20];

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

  sprintf(au8HumidityReading, "Humidity: %d%%", ClimateMonitor_u32SHTC3HumidityReading);

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

  ClimateMonitor_pfStateMachine = ClimateMonitorSM_Idle;
}

/* What does this state do? */
static void ClimateMonitorSM_Idle(void)
{
  ClimateMonitor_u32Timer++;

  if (WasButtonPressed(BUTTON0)) {
    ButtonAcknowledge(BUTTON0);

    isFahrenheit = FALSE;
  }

  if (WasButtonPressed(BUTTON1)) {
    ButtonAcknowledge(BUTTON1);

    isFahrenheit = TRUE;
  }
  
  if (ClimateMonitor_u32Timer == U32_SHTC3_MEASURE_PERIOD_MS) {
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
