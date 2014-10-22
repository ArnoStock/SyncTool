//
// This file is part of the GNU ARM Eclipse distribution.
// Copyright (c) 2014 Liviu Ionescu.
//

// ----------------------------------------------------------------------------

#include <stdio.h>
#include "diag/Trace.h"

#include "Timer.h"
#include "SyncToolA.h"
#include "measurement.h"
#include "evaluate.h"

// ----------------------------------------------------------------------------
//
// STM32F1 led blink sample (trace via NONE).
//
// In debug configurations, demonstrate how to print a greeting message
// on the trace device. In release configurations the message is
// simply discarded.
//
// To demonstrate POSIX retargetting, reroute the STDOUT and STDERR to the
// trace device and display messages on both of them.
//
// Then demonstrates how to blink a led with 1Hz, using a
// continuous loop and SysTick delays.
//
// On DEBUG, the uptime in seconds is also displayed on the trace device.
//
// Trace support is enabled by adding the TRACE macro definition.
// By default the trace messages are forwarded to the NONE output,
// but can be rerouted to any device or completely suppressed, by
// changing the definitions required in system/src/diag/trace_impl.c
// (currently OS_USE_TRACE_SEMIHOSTING_DEBUG/_STDOUT).
//
// The external clock frequency is specified as a preprocessor definition
// passed to the compiler via a command line option (see the 'C/C++ General' ->
// 'Paths and Symbols' -> the 'Symbols' tab, if you want to change it).
// The value selected during project creation was HSE_VALUE=8000000.
//
// Note: The default clock settings take the user defined HSE_VALUE and try
// to reach the maximum possible system clock. For the default 8MHz input
// the result is guaranteed, but for other values it might not be possible,
// so please adjust the PLL settings in system/src/cmsis/system_stm32f10x.c
//

// ----- Timing definitions -------------------------------------------------

// Keep the LED on for 2/3 of a second.
#define BLINK_ON_TICKS  (TIMER_FREQUENCY_HZ * 1 / 20)
#define BLINK_OFF_TICKS (TIMER_FREQUENCY_HZ - BLINK_ON_TICKS)
#define MAIN_LOOP_IDLE_TICKS (TIMER_FREQUENCY_HZ * 1 / 50)

// ----- main() ---------------------------------------------------------------

// Sample pragmas to cope with warnings. Please note the related line at
// the end of this function, used to pop the compiler diagnostics status.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wmissing-declarations"
#pragma GCC diagnostic ignored "-Wreturn-type"

char s[40];

int
main(int argc, char* argv[])
{
	// configure system clock

  // By customizing __initialize_args() it is possible to pass arguments,
  // for example when running tests with semihosting you can pass various
  // options to the test.
  // trace_dump_args(argc, argv);

  // Send a greeting to the trace device (skipped on Release).
  trace_puts("Hello ARM World!");

  // The standard output and the standard error should be forwarded to
  // the trace device. For this to work, a redirection in _write.c is
  // required.
  puts("Standard output message.");
  fprintf(stderr, "Standard error message.\n");

  // At this stage the system clock should have already been configured
  // at high speed.
  trace_printf("System clock: %uHz\n", SystemCoreClock);

  timer_start();

  blink_led_init();

  LCD_Initializtion();

  TP_Init();

  gfx_set_page (0);

  Coordinate screen;

  /* Infinite loop */
  while (0)
  {		  if (Read_TouchPosition(&screen)) {
  	  		  sprintf (s, "X=%5.5d / Y=%5.5d", screen.x, screen.y);
  	  		  GUI_Text(40, 120, s, Black, White);
  	  		  TP_DrawPoint(screen.x, screen.y);
  	  	  }
  }

  init_measurement_subsystem ();
  

//  uint32_t seconds = 0;

  // Infinite loop
  while (1)
    {

	  if (evaluate_buffer & MEASUREMENT_BUFFER_FULL_FLAG) {
		  blink_led_off();
		  EVAL_analyze_values (evaluate_buffer & 0x0f);
		  update_active_page ();
		  evaluate_buffer = 0;
		  blink_led_on();
	  }

//      blink_led_on();
//      timer_sleep(BLINK_ON_TICKS);

//      blink_led_off();
//      timer_sleep(BLINK_OFF_TICKS);

	  if (timer_peek()) {
	  	  gfx_poll_touch_controller ();
	  	  timer_set (MAIN_LOOP_IDLE_TICKS);
	  }
//      ++seconds;

      // Count seconds on the trace device.
//      trace_printf("Second %u\n", seconds);
    }
  // Infinite loop, never return.
}

#pragma GCC diagnostic pop

// ----------------------------------------------------------------------------
