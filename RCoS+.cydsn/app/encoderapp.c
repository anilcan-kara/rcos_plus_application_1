/** @file       encoderapp.c
 *  @brief      ENCODERAPP application program file
 *  @copyright  (c) 2021-Arcelik - All Rights Reserved
 *              Permission to use, reproduce, copy, prepare derivative works,
 *              modify, distribute, perform, display or sell this software and/or
 *              its documentation for any purpose is prohibited without the express
 *              written consent of Arcelik A.S.
 *  @author     Emre Karabek
 *  @date       2.06.2021
 */
#define FILE_ENCODERAPP_C

#include "encoderapp.h"
#include "dev/pattern.h"
#include <stdbool.h> // bool, true, false tanımları için

#define DEBUG_FILE_NAME ""

// #define DEBUG_PORT_
// DEBUG_START(0)

/**
 *  @addtogroup ENCODERAPP
 *  @{
 */
#define HIGH (1)
#define LOW (0)
#define SEGMENT_MAX 15
#define SEGMENT_MIN 0

uint8_t segmentValue = 0;
bool counting = false;
bool countUp = true;

const tsPatternNode melody[] =
    {
        PATTERN_NODE(NOTE_4_A, 2000),
        PATTERN_NODE(NOTE_5_SOL, 1000),
        PATTERN_NODE_END(0)};

const tsPatternNode melody2[] =
    {
        PATTERN_NODE(NOTE_6_A, 1000),
        PATTERN_NODE(NOTE_8_SOL, 1000),
        PATTERN_NODE_END(0)};

const tsPatternNode melody3[] =
    {
        PATTERN_NODE(NOTE_5_A, 3000),
        PATTERN_NODE(NOTE_7_SOL, 1000),
        PATTERN_NODE_END(0)};

const tsPatternNode melody4[] =
    {
        PATTERN_NODE(NOTE_7_Cf, 2000),
        PATTERN_NODE(NOTE_8_SOL, 2000),
        PATTERN_NODE_END(0)};

static void countSegments(tsEncoderAppParams *params, const tsEncoderAppConsts *consts);

static PT_THREAD(encoderAppThread)
{
    tsEncoderAppParams *params = process->parameters;
    const tsEncoderAppConsts *consts = process->constants;

    UNUSED(consts);
    UNUSED(params);

    PT_BEGIN();

    for (;;)
    {
        PT_YIELD();
    }

    PT_END();
}

static PROCESS_HANDLER_PROTO(encoderAppInitialStateHandler);
static PROCESS_HANDLER_PROTO(encoderAppLockingStateHandler);
static PROCESS_HANDLER_PROTO(encoderAppUnLockingStateHandler);
tsTarget target;

/// @brief  Initialization function of encoderApp
PROCESS_INIT_PROTO(encoderAppInit)
{
    tsEncoderAppParams *params = process->parameters;
    const tsEncoderAppConsts *consts = process->constants;

    PROCESS_STATE_CHANGE(process, encoderAppInitialStateHandler);
    // eventPost(process->enumeration, eEPEventUIUpdate, NULL, 0);
    timerEventStart(&(params->timerUITimer), 1500);

    devIoInit(consts->redLed, NULL);
    devIoInit(consts->blueLed, NULL);
    devIoInit(consts->greenLed, NULL);
    devIoInit(consts->sevenSegmentDisplay, NULL);
    devIoInit(consts->encoder, NULL);

    devIoPut(consts->redLed, HIGH);
    devIoPut(consts->blueLed, LOW);
    devIoPut(consts->greenLed, LOW);

    const tsDevCom *uartDevice = (const tsDevCom *)consts->uart;

    devComInit(uartDevice);
    devComOpen(uartDevice, &target);

    params->timerUIGeneral.post.destination = process->enumeration;
    params->timerUIGeneral.post.source = process->enumeration;

    threadStart(process, encoderAppThread);
    process->initialized = 1; // If process needs other checks, set this another time
}

/// @brief  Deinitialization function of encoderApp
PROCESS_DEINIT_PROTO(encoderAppDeinit)
{
    tsEncoderAppParams *params = process->parameters;
    const tsEncoderAppConsts *consts = process->constants;

    UNUSED(params); // REMOVE IF USED
    UNUSED(consts); // REMOVE IF USED

    PROCESS_STATE_CHANGE(process, NULL);
    threadStop(process, process->threadFunction);
    process->initialized = 0; // If process needs other checks, clear this another time
}
uint8_t txb[2] = {'o', 'k'};
uint8_t rxb[2];
uint8_t Prerxb[2];

/// @brief  Event handler function of encoderApp Initial state
static PROCESS_HANDLER_PROTO(encoderAppInitialStateHandler)
{
    tsEncoderAppParams *params = process->parameters;
    const tsEncoderAppConsts *consts = process->constants;

    UNUSED(params); // REMOVE IF USED
    UNUSED(consts); // REMOVE IF USED

    switch (eventCurrent.event)
    {
    case eEPEventUIUpdate:
    {
        //      devIoPut(consts->redLed , LOW);
        //      devIoPut(consts->buzzer , (uint32_t)melody);
        //      timerEventStart(&(params->timerUITimer) , 1);
        //      devIoPut(consts->sevenSegmentDisplay , 1 );
    }
    break;

    case eEPEventLedsBlink:
    {
        //      devIoPut(consts->blueLed , HIGH);
        //      devIoPut(consts->redLed , LOW);
        //      devIoPut(consts->buzzer , (uint32_t)melody);
        //      timerEventStart(&(params->timergreenLedBlink) , 2000 );
    }
    break;

    case eEPEventgreenLedsBlink:
    {
    }
    break;

    case eEPEventLed2:
    {
    }
    break;

    case p71Released:
    {
        //      devComSend(consts->uart , txb , sizeof(txb));
    }
    break;

    case plusButtonPressed: // + tuşu event'ı
    {
        if (segmentValue < SEGMENT_MAX)
        {
            segmentValue++;
            devIoPut(consts->sevenSegmentDisplay, segmentValue);
            countUp = true;
        }
        else
        {
            devIoPut(consts->buzzer, (uint32_t)melody);
        }
    }
    break;

    case minusButtonPressed: // - tuşu event'ı
    {
        if (segmentValue > SEGMENT_MIN)
        {
            segmentValue--;
            devIoPut(consts->sevenSegmentDisplay, segmentValue);
            countUp = false;
        }
        else
        {
            devIoPut(consts->buzzer, (uint32_t)melody);
        }
    }
    break;

    case okButtonPressed: // OK tuşu event'ı
    {
        counting = !counting; // Saymayı başlat veya durdur
        if (counting)
        {
            timerEventStart(&(params->timerUITimer), 1000); // Hocam burada 1 saniye sonra sayma işlemi başlasın dedik
        }
    }
    break;

    case eEPEventUITimer:
    {
        countSegments(params, consts);
    }
    break;
    }
}

static void countSegments(tsEncoderAppParams *params, const tsEncoderAppConsts *consts)
{
    if (counting)
    {
        if (countUp)
        {
            if (segmentValue < SEGMENT_MAX)
            {
                segmentValue++;
                devIoPut(consts->sevenSegmentDisplay, segmentValue);
            }
            else
            {
                devIoPut(consts->buzzer, (uint32_t)melody);
                counting = false;
            }
        }
        else
        {
            if (segmentValue > SEGMENT_MIN)
            {
                segmentValue--;
                devIoPut(consts->sevenSegmentDisplay, segmentValue);
            }
            else
            {
                devIoPut(consts->buzzer, (uint32_t)melody);
                counting = false;
            }
        }

        if (counting)
        {
            timerEventStart(&(params->timerUITimer), 1000); // 1 saniye sonra tekrar sayma işlemi yap
        }
    }
}
