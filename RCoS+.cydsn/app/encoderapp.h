/** @file       encoderapp.h
 *  @brief      ENCODERAPP application header file
 *  @copyright  (c) 2021-Arcelik - All Rights Reserved
 *              Permission to use, reproduce, copy, prepare derivative works,
 *              modify, distribute, perform, display or sell this software and/or
 *              its documentation for any purpose is prohibited without the express
 *              written consent of Arcelik A.S.
 *  @author     Emre Karabek
 *  @date       2.06.2021
 */
#ifndef FILE_ENCODERAPP_H
#define FILE_ENCODERAPP_H

#define plusButtonPressed 1001  // + tuşu event ID
#define minusButtonPressed 1002 // - tuşu event ID
#define okButtonPressed 1003    // OK tuşu event ID
#define eEPEventUITimer 1004    // Timer event ID

/** INCLUDES ******************************************************************/
#include "rcos.h"

/// Interface definition for this file,
/// there should not be any includes after this define.
#ifndef FILE_ENCODERAPP_C
#define INTERFACE extern
#else
#define INTERFACE
#endif

/**
 *  @defgroup   ENCODERAPP
 *  @ingroup    APP
 *  @brief      Encoder application for RCOS SDK DB Board
 *  @{
 */

/// @brief  Events of a EncoderApp process
typedef enum
{
    eEncoderAppEventNone,
    p46Pressed,
    p46Released,
    p70Pressed,
    p70Released,
    p71Pressed,
    p71Released,
    eEPEventUIUpdate,
    eEPEventLedsBlink,
    eEPEventgreenLedsBlink,
    eEPEventUIUpdate2,
    eEPEventLed2,
} teEncoderAppEvents;

/// @brief  Parameters of a EncoderApp process
typedef struct
{
    tsTimerEvent timerUIGeneral; ///<      Timer event for general UI operations
    tsTimerEvent timerUITimer;   ///<      Timer event for general UI operations
    tsTimerEvent timerLedBlink;
    tsTimerEvent timergreenLedBlink;
    tsTimerEvent timerLed2Blink;

    uint8_t lockingState;  ///<    Storing locking states
    uint8_t passwordIndex; ///<    Storing password item index
    uint16_t password;     ///<    Password value
    uint16_t unlockValue;  ///<    Value to unlock password
} tsEncoderAppParams;

/// @brief  Constants of a EncoderApp process
typedef struct
{
    const tsDevIo *redLed;              ///<    IO device for red led
    const tsDevIo *blueLed;             ///<    IO device for blue led
    const tsDevIo *greenLed;            ///<    IO device for green led
    const tsDevIo *sevenSegmentDisplay; ///<    IO device for seven segment display
    const tsDevIo *encoder;             ///<    IO device for encoder
    const tsDevIo *buzzer;              ///<    IO device for buzzer
    const tsDevIo *uart;                ///<    IO device for buzzer
} tsEncoderAppConsts;

/** @brief  EncoderApp process object creation macro
 *  @param  _name   Name of process object
 *  @param  _enum   Process enumeration for this object
 *  @param  _redLed IO device for red led
 *  @param  _blueLed IO device for blue led
 *  @param  _greenLed IO device for green led
 *  @param  _sevenSegmentDisplay IO device for seven segment display
 *  @param  _encoder IO device for encoder
 *  @param  _buzzer IO device for buzzer
 */
#define PROCESS_ENCODERAPP_CREATE(_name, _enum, _redLed, _blueLed, _greenLed, _sevenSegmentDisplay, _encoder, _buzzer, _devUart) \
    tsEncoderAppParams _name##Params =                                                                                           \
        {                                                                                                                        \
            .timerLedBlink = TIMER_EVENT_INIT(_enum, _enum, eEPEventLedsBlink),                                                  \
            .timergreenLedBlink = TIMER_EVENT_INIT(_enum, _enum, eEPEventgreenLedsBlink),                                        \
            .timerLed2Blink = TIMER_EVENT_INIT(_enum, _enum, eEPEventLed2),                                                      \
            .timerUITimer = TIMER_EVENT_INIT(_enum, _enum, eEPEventUIUpdate),                                                    \
    };                                                                                                                           \
    const tsEncoderAppConsts _name##Consts =                                                                                     \
        {                                                                                                                        \
            .redLed = &_redLed,                                                                                                  \
            .blueLed = &_blueLed,                                                                                                \
            .greenLed = &_greenLed,                                                                                              \
            .sevenSegmentDisplay = &_sevenSegmentDisplay,                                                                        \
            .encoder = &_encoder,                                                                                                \
            .buzzer = &_buzzer,                                                                                                  \
            .uart = &_devUart,                                                                                                   \
    };                                                                                                                           \
    PROCESS_CREATE(_name, encoderAppInit, encoderAppDeinit, _enum, &_name##Params, &_name##Consts)

/** @} */

#undef INTERFACE // Should not let this roam free

#endif // FILE_ENCODERAPP_H
