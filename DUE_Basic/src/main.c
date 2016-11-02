/* Includes -------------------------------------------------------------------*/
#include "main.h"

/* Variables ------------------------------------------------------------------*/
static volatile uint32_t tickCnt = 0;

/* Prototypes -----------------------------------------------------------------*/
static void delay_ms(uint32_t time_ms);
static void toggle_pin(Pio* pio, uint32_t pin);
static uint32_t get_ticks(void);

/**
 * Print a greeting message on the trace device and enter a loop
 * to count seconds.
 *
 * Trace support is enabled by adding the TRACE macro definition.
 * By default the trace messages are forwarded to the STDOUT output,
 * but can be rerouted to any device or completely suppressed, by
 * changing the definitions required in system/src/diag/trace_impl.c
 * (currently OS_USE_TRACE_ITM, OS_USE_TRACE_SEMIHOSTING_DEBUG/_STDOUT).
 */

/**
 * Sample pragmas to cope with warnings. Please note the related line at
 * the end of this function, used to pop the compiler diagnostics status.
 */
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wmissing-declarations"
#pragma GCC diagnostic ignored "-Wreturn-type"

/**
 * Normally at this stage most of the microcontroller subsystems, including
 * the clock, were initialised by the CMSIS SystemInit() function invoked
 * from the startup file, before calling main().
 * (see system/src/cortexm/_initialize_hardware.c)
 * If further initialisations are required, customise __initialize_hardware()
 * or add the additional initialisation here, for example:
 *
 * HAL_Init();
 *
 * In this sample the SystemInit() function is just a placeholder,
 * if you do not add the real one, the clock will remain configured with
 * the reset value, usually a relatively low speed RC clock (8-12MHz).
 *
 * By customising __initialize_args() it is possible to pass arguments,
 * for example when running tests with semihosting you can pass various
 * options to the test.
 * trace_dump_args(argc, argv);
 */
int main(int argc, char* argv[]) {

    // Disable watchdog timer
    WDT->WDT_MR = WDT_MR_WDDIS;

    // Send a greeting to the trace device (skipped on Release).
    trace_puts("Now it's configured to github. NICE!")
    trace_puts("Hello from Albin's DUE_Basic setup!");

    // The standard output and the standard error should be forwarded to
    // the trace device. For this to work, a redirection in _write.c is
    // required.
    puts("Standard output message.");
    fprintf(stderr, "Standard error message.\n");

    // At this stage the system clock should have already been configured
    // at high speed.
    trace_printf("System clock: %uHz\n", SystemCoreClock);


    /* Configure LED on arduino pin 13 */
    PIOB->PIO_WPMR = 0x50494F;
    PIOB->PIO_OER = PIO_PB27;
    PIOB->PIO_CODR = PIO_PB27;

    /*  Enable SysTick interrupts and configure 1ms interrupts */
    NVIC_EnableIRQ(SysTick_IRQn);
    SysTick_Config(SystemCoreClock/1000);

    uint32_t t0;
    uint32_t t1;
    // Infinite loop
    while (1) {
    	t0 = get_ticks();
    	toggle_pin(PIOB, PIO_PB27);
    	delay_ms(1000);
    	t1 = get_ticks();
    	trace_printf("Actual delay = %ums\n", t1 - t0);
    }
}

/**
 * Toggles the specified pin (if it is correctly instanciated)
 */
static void toggle_pin(Pio* pio, uint32_t pin) {
	if(pio->PIO_ODSR & pin) {
		pio->PIO_CODR = pin;
	} else {
		pio->PIO_SODR = pin;
	}
}

/**
 * Delays execution for the specified number of milliseconds
 */
static void delay_ms(uint32_t time_ms) {
	volatile uint32_t t0 = tickCnt;
	while(time_ms + t0 > get_ticks()) {
		asm("NOP");
	}

	return;
}

static uint32_t get_ticks(void) {
	return tickCnt;
}

/**
 * Increment counter every one millisecond.
 */
void SysTick_Handler() {
	tickCnt++;
}

#pragma GCC diagnostic pop

// ----------------------------------------------------------------------------
