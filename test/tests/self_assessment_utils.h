/* =========================================================================
    Unity - A Test Framework for C
    ThrowTheSwitch.org
    Copyright (c) 2007-25 Mike Karlesky, Mark VanderVoord, & Greg Williams
    SPDX-License-Identifier: MIT
========================================================================= */

#ifdef TEST_INSTANCES

#include <string.h>
#include <stdint.h>

/* Dividing by these constants produces +/- infinity.
 * The rationale is given in UnityAssertFloatIsInf's body.
 */
#ifndef UNITY_EXCLUDE_FLOAT
static const UNITY_FLOAT f_zero = 0.0f;
#endif

#ifndef UNITY_EXCLUDE_DOUBLE
static const UNITY_DOUBLE d_zero = 0.0;
#endif

/* Macros for Catching An Expected Failure or Ignore */
#define EXPECT_ABORT_BEGIN \
    startPutcharSpy();     \
    if (TEST_PROTECT())    \
    {

#define VERIFY_FAILS_END                                                       \
    }                                                                          \
    endPutcharSpy(); /* start/end Spy to suppress output of failure message */ \
    Unity.CurrentTestFailed = (Unity.CurrentTestFailed == 1) ? 0 : 1;          \
    if (Unity.CurrentTestFailed == 1) {                                        \
      SetToOneMeanWeAlreadyCheckedThisGuy = 1;                                 \
      UnityPrintNumberUnsigned(Unity.CurrentTestLineNumber);                   \
      UNITY_OUTPUT_CHAR(':');                                                  \
      UnityPrint(Unity.CurrentTestName);                                       \
      UnityPrint(":FAIL: [[[[ Test Should Have Failed But Did Not ]]]]");      \
      UNITY_OUTPUT_CHAR('\n');                                                 \
    }

#define VERIFY_IGNORES_END                                                     \
    }                                                                          \
    endPutcharSpy(); /* start/end Spy to suppress output of ignore message */  \
    Unity.CurrentTestFailed = (Unity.CurrentTestIgnored == 1) ? 0 : 1;         \
    Unity.CurrentTestIgnored = 0;                                              \
    if (Unity.CurrentTestFailed == 1) {                                        \
      SetToOneMeanWeAlreadyCheckedThisGuy = 1;                                 \
      UnityPrintNumberUnsigned(Unity.CurrentTestLineNumber);                   \
      UNITY_OUTPUT_CHAR(':');                                                  \
      UnityPrint(Unity.CurrentTestName);                                       \
      UnityPrint(":FAIL: [[[[ Test Should Have Ignored But Did Not ]]]]");     \
      UNITY_OUTPUT_CHAR('\n');                                                 \
    }

/* Tricky series of macros to set USING_OUTPUT_SPY */
#define USING_SPY_AS(a)           EXPAND_AND_USE_2ND(ASSIGN_VALUE(a), 0)
#define ASSIGN_VALUE(a)           VAL_##a
#define VAL_putcharSpy            0, 1
#define EXPAND_AND_USE_2ND(a, b)  SECOND_PARAM(a, b, throwaway)
#define SECOND_PARAM(a, b, ...)   b
#if USING_SPY_AS(UNITY_OUTPUT_CHAR)
  #define USING_OUTPUT_SPY /* true only if UNITY_OUTPUT_CHAR = putcharSpy */
#endif

#ifdef USING_OUTPUT_SPY
#include <stdio.h>
#define SPY_BUFFER_MAX 40
static char putcharSpyBuffer[SPY_BUFFER_MAX];
#endif
static UNITY_COUNTER_TYPE indexSpyBuffer;
static UNITY_COUNTER_TYPE putcharSpyEnabled;

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-prototypes"
#endif

void startPutcharSpy(void)
{
    indexSpyBuffer = 0;
    putcharSpyEnabled = 1;
}

void endPutcharSpy(void)
{
    putcharSpyEnabled = 0;
}

char* getBufferPutcharSpy(void)
{
#ifdef USING_OUTPUT_SPY
    putcharSpyBuffer[indexSpyBuffer] = '\0';
    return putcharSpyBuffer;
#else
    return NULL;
#endif
}

void putcharSpy(int c)
{
#ifdef USING_OUTPUT_SPY
    if (putcharSpyEnabled)
    {
        if (indexSpyBuffer < SPY_BUFFER_MAX - 1)
            putcharSpyBuffer[indexSpyBuffer++] = (char)c;
    } else
        putchar((char)c);
#else
    (void)c;
#endif
}

/* This is for counting the calls to the flushSpy */
static UNITY_COUNTER_TYPE flushSpyEnabled;
static UNITY_COUNTER_TYPE flushSpyCalls = 0;

void startFlushSpy(void)
{
    flushSpyCalls = 0;
    flushSpyEnabled = 1;
}

void endFlushSpy(void)
{
    flushSpyCalls = 0;
    flushSpyEnabled = 0;
}

UNITY_COUNTER_TYPE getFlushSpyCalls(void)
{
    return flushSpyCalls;
}

void flushSpy(void)
{
    if (flushSpyEnabled){ flushSpyCalls++; }
}

#ifdef __clang__
#pragma clang diagnostic pop
#endif

#define TEST_ASSERT_EQUAL_PRINT_NUMBERS(expected, actual) do {          \
        startPutcharSpy(); UnityPrintNumber((actual)); endPutcharSpy(); \
        TEST_ASSERT_EQUAL_STRING((expected), getBufferPutcharSpy());    \
        } while (0)

#define TEST_ASSERT_EQUAL_PRINT_UNSIGNED_NUMBERS(expected, actual) do {         \
        startPutcharSpy(); UnityPrintNumberUnsigned((actual)); endPutcharSpy(); \
        TEST_ASSERT_EQUAL_STRING((expected), getBufferPutcharSpy());            \
        } while (0)

#define TEST_ASSERT_EQUAL_PRINT_FLOATING(expected, actual) do {         \
        startPutcharSpy(); UnityPrintFloat((actual)); endPutcharSpy();  \
        TEST_ASSERT_EQUAL_STRING((expected), getBufferPutcharSpy());    \
        } while (0)

#endif

// The reason this isn't folded into the above diagnostic is to semi-isolate
// the header contents from the user content it is included into.
#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-prototypes"
#endif
