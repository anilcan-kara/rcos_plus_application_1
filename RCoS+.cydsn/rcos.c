/** @file       rcos.c
 *  @brief      RCoS+ main file
 *  @copyright  (c) 2021-Arcelik - All Rights Reserved
 *              Permission to use, reproduce, copy, prepare derivative works,
 *              modify, distribute, perform, display or sell this software and/or
 *              its documentation for any purpose is prohibited without the express
 *              written consent of Arcelik A.S.
 *  @author     Emre Karabek
 *  @date       1.06.2021
 */
#define FILE_RCOS_C

#include "rcos.h"
#include "dev/psoc4/gpio.h"
#include "dev/psoc4/capsense.h"
#include "dev/psoc4/pwm.h"
#include "mw/buttons.h"
#include "system.h"
#include "dev/encoder.h"
#include "dev/iocomb.h"
#include "libs/libs.h"
#include "dev/sevensegmentdisplay.h"
#include "dev/pattern.h"
#include "app/encoderapp.h"
#include "dev/psoc4/uart.h"

#define DEBUG_FILE_NAME "rcos"

// #define DEBUG_PORT_
// DEBUG_START(0)

CORE_EVENTQUEUE_SIZE(1024)
// CORE_DEBUG_DEV(_devName)
CORE_TICK_DEFAULT(0)
CORE_WDT_DEFAULT(0)

#define CAPSENSE_SCAN_TIME (2)      ///< 2 miliseconds
#define HIGH (0)     
#define LOW (1)
#define ms500 (500)

DEV_IO_CAPSENSE_CREATE(ioCapsense, cyCapsense, CAPSENSE_SCAN_TIME)

// buzzer IO calls #include "dev\psoc4\pwm.h"
DEV_CPX_PWM_CORE_CREATE(pwmCoreBuzzer, cyBuzzerPWM, 1e6, 0, SOUND_DUTY_HIGH)
DEV_IO_PWM_FREQ_CREATE(buzzer, pwmCoreBuzzer)
DEV_IO_PWM_DUTY_CREATE(buzzerDuty, pwmCoreBuzzer)

// create button process  #include "mw\buttons.h"
#define BUTON_P46 BIT(2)
#define BUTON_P70 BIT(1)
#define BUTON_P71 BIT(0)

PROCESS_BUTTONS_CREATE(processButton,
                       ioCapsense,
                       eProcessEncoderApp,
                       50,
                       BUTTONS_ITEM(BUTON_P46, p46Pressed, p46Released),
                       BUTTONS_ITEM(BUTON_P70, p70Pressed, p70Released),
                       BUTTONS_ITEM(BUTON_P71, p71Pressed, p71Released))

// Leds IO calls
DEV_IO_GPIO_CREATE(ledP43, cyLed_p43, 1) //active high :1 active low : 0
DEV_IO_GPIO_CREATE(ledP44, cyLed_p44, 1) //active high :1 active low : 0
DEV_IO_GPIO_CREATE(ledP45, cyLed_p45, 1) //active high :1 active low : 0

DEV_IO_IOCOMB_CREATE(ledGui,    IOCOMB_ITEM(ledP43, 1), 
                                IOCOMB_ITEM(ledP44, 1), 
                                IOCOMB_ITEM(ledP45, 1)) 

// seven segment IO calls //#include "dev/sevensegmentdisplay.h"
DEV_IO_GPIO_CREATE(segA, cySegA_p20, 1)
DEV_IO_GPIO_CREATE(segB, cySegB_p21, 1)
DEV_IO_GPIO_CREATE(segC, cySegC_p22, 1)
DEV_IO_GPIO_CREATE(segD, cySegD_p23, 1)
DEV_IO_GPIO_CREATE(segE, cySegE_p24, 1)
DEV_IO_GPIO_CREATE(segF, cySegF_p25, 1)
DEV_IO_GPIO_CREATE(segG, cySegG_p26, 1)
DEV_IO_GPIO_CREATE(digit1, cyDigit1_p34, 0)
DEV_IO_GPIO_CREATE(digit2, cyDigit2_p35, 0)

DEV_IO_IOCOMB_CREATE(sevenSegment,  IOCOMB_ITEM(segA, 1),
                                    IOCOMB_ITEM(segB, 1),
                                    IOCOMB_ITEM(segC, 1),
                                    IOCOMB_ITEM(segD, 1),
                                    IOCOMB_ITEM(segE, 1),
                                    IOCOMB_ITEM(segF, 1),
                                    IOCOMB_ITEM(segG, 1))

DEV_IO_SEVENSEGMENTDISPLAY_CREATE(sevenSegmentDisplay, sevenSegment, digit1, digit2 ) 

// encoder IO calls //#include "dev\encoder.h"
DEV_IO_GPIO_CREATE(encCol1, cyCol1_p03, 0)
DEV_IO_GPIO_CREATE(encCol2, cyCol2_p02, 0)
DEV_IO_GPIO_CREATE(encCol3, cyCol3_p01, 0)
DEV_IO_GPIO_CREATE(encCol4, cyCol4_p00, 0)

DEV_IO_ENCODER_CREATE(encoder,&encCol1,&encCol2,&encCol3,&encCol4)

DEV_IO_PATTERN_CREATE(patternBuzzer, buzzer, 0)

DEV_COM_UART_CREATE(devUart, cyUart_p50_p51)

// Encoder process create macro
PROCESS_ENCODERAPP_CREATE(encoderApp, eProcessEncoderApp, ledP43, ledP44, ledP45, sevenSegmentDisplay, encoder, patternBuzzer , devUart)

unsigned char LedBlinkCounter;

unsigned char stateOut = 1; 

TIMER_CALLBACK_FUNC(LedBlinkTimerFunc) 
{ 
    static char sira = 0;
    
    sira++; 
    if (sira == 1) 
    { 
        devIoPut(&ledP43, HIGH); 
        devIoPut(&ledP44, LOW); 
        devIoPut(&ledP45, HIGH); 
    } 
    if (sira == 2) 
    { 
        devIoPut(&ledP43, HIGH); 
        devIoPut(&ledP44, HIGH); 
        devIoPut(&ledP45, LOW); 
    } 
    if (sira == 3) 
    { 
        devIoPut(&ledP43, LOW); 
        devIoPut(&ledP44, HIGH); 
        devIoPut(&ledP45, HIGH); 
        sira = 0 ;
    } 
    

    return 500; 
}

TIMER_CALLBACK_CREATE(LedBlinkTimer, LedBlinkTimerFunc , 0 )

// RCoS main loop
void rcosMainLoop(void)
{
    platformInit();
    coreInit();
    //timerCallBackStart(&LedBlinkTimer , 1000);
    processStart(&processButton);
    processStart(&encoderApp);

    coreRun();
}