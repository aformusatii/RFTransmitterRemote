/********************************************************************************
	Includes
********************************************************************************/

#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/sleep.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <util/delay.h>

#include "../nrf24l01/RF24.h"
#include "../atmega328/mtimer.h"
#include "../common/util.h"
#include "irMapper.h"

extern "C" {
#include "../atmega328/usart.h"
}

/********************************************************************************
	Macros and Defines
********************************************************************************/
#define DEBUG_ARRAY_SIZE 1
#define SKIP_REPEATED_IR 3

/********************************************************************************
	Function Prototypes
********************************************************************************/
void initGPIO();
void powerOnRF();
void powerDownRF();
void processIRCommand(uint64_t cmd);

/********************************************************************************
	Global Variables
********************************************************************************/
volatile uint16_t ir_index = 0;
volatile uint16_t ir_data_debug[DEBUG_ARRAY_SIZE];

// last negative and positive start pulse time cycles
volatile uint64_t lpp = 0;
volatile uint64_t lnp = 0;

// last negative and positive pulse time cycles duration
volatile uint16_t tlpp = 0;
volatile uint16_t tlnp = 0;

// when last IR command was received
volatile uint64_t lcr = 0;

// IR command decoded
volatile uint64_t ir_cmd = 0;
volatile uint16_t ir_cmd_index = 0;

// Repeated IR commands are too quick we need to skip some of them
volatile uint8_t skip_repeted_ir_count = 0;

volatile uint64_t sleep_watch = 0;

RF24 radio;
const uint64_t pipes[2] = { 0xF0F0F0F0E1LL, 0xF0F0F0F0D2LL };

bool ir_preamble_detected = false;
bool awake = false;
bool rfAwake = false;

/********************************************************************************
	Interrupt Service
********************************************************************************/
ISR(USART_RX_vect)
{
	handle_usart_interrupt();
}

ISR(TIMER1_OVF_vect)
{
	incrementOvf();
}

ISR(INT0_vect)
{
    // Keep the processor awake until we receive the entire IR command
    awake = true;

    uint64_t ctc = getCurrentTimeCycles();

    if (GET_REG1_FLAG(PIND, PD2)) {
        // PD2 is high, very high...
        tlnp = ctc - lnp;
        lpp = ctc;
    } else {
        // PD2 is low
        tlpp = ctc - lpp;
        lnp = ctc;

        /* if (ir_index < DEBUG_ARRAY_SIZE) {
            ir_data_debug[ir_index++] = tlnp;
            ir_data_debug[ir_index++] = tlpp;
        }*/

        if (ir_preamble_detected) {
            if (_between(tlnp, 5, 5)) {
                if (__between(tlpp, 34, 3, 8)) {
                    // One receiver - DVD mode only
                    ir_cmd |= ( ((uint64_t) 1) << ir_cmd_index++ );
                } else if (__between(tlpp, 12, 3, 8)) {
                    // One receiver - TV and DB modes
                    ir_cmd |= ( ((uint64_t) 1) << ir_cmd_index++ );
                } else if (_between(tlpp, 5, 5)) {
                    // Zero receiver - ALL modes
                    ir_cmd &= ~( ((uint64_t) 1) << ir_cmd_index++ );
                } else {
                    // this delay is unexpected, log it and stop receiving
                    ir_preamble_detected = false;
                    printf("\nUnrecognized positive pulse: %d", tlpp);
                }
            } else {
                // this delay is unexpected, log it and stop receiving
                ir_preamble_detected = false;
                printf("\nUnrecognized negative pulse: %d", tlnp);
            }
        } else if ((tlnp > 10) && (tlnp < 80) && _between(tlpp, 35, 4)) {
            // IR preamble was detected
            ir_preamble_detected = true;

            // reset IR command variables
            ir_cmd = 0;
            ir_cmd_index = 0;
        }
    }
}

ISR(TIMER2_OVF_vect)
{
	_NOP();
}

/********************************************************************************
	Main
********************************************************************************/
int main(void) {
    // initialize usart module
	usart_init();

    // Init Timer 1
    initTimer();

    // Init GPIO
    initGPIO();

    // enable interrupts
    sei();

    // Configure Sleep Mode - Power-down
    SMCR = (0<<SM2)|(1<<SM1)|(0<<SM0)|(0<<SE);

	// Console friendly output
    printf("Start...");
    printf(CONSOLE_PREFIX);

    // Init NRF24L01+
    radio.begin();
    radio.setRetries(15,15);
    radio.setPayloadSize(8);
    radio.setPALevel(RF24_PA_MAX);
    radio.setChannel(110);

    radio.openWritingPipe(pipes[0]);
    radio.openReadingPipe(1,pipes[1]);

    // Some RF module diagnostics logs
    radio.printDetails();

    // Init timer variables
    lpp = getCurrentTimeCycles();
    lnp = lpp;
    sleep_watch = lpp;
    lcr = lpp;

	// main loop
    while (1) {
    	// main usart loop for console
    	usart_check_loop();

    	// Process the IR command if it was received
    	if (ir_preamble_detected && (ir_cmd_index > 30)) {
    	    int16_t positivePulseDuration = getCurrentTimeCycles() - lpp;

    	    // Check if positive pulse is long enough then the last bit was received
    	    if (positivePulseDuration > 40) {
    	        // buffer the IR command in case a new one comes
    	        uint64_t buf_ir_cmd = ir_cmd;

                // end current IR command
                ir_preamble_detected = false;

    	        // Wake up the RF module
    	        powerOnRF();

    	        // Process the IR command
    	        processIRCommand(buf_ir_cmd);
    	    }

            // keep the device awake meanwhile but allow to go to sleep after a command is received
            sleep_watch = getCurrentTimeCycles();
            awake = false;
    	}

    	// Can we sleep? we are hungry when awake...
    	if (!awake && (getCurrentTimeCycles() - sleep_watch) > 1000) {
    	    // Power down the RF module
    	    powerDownRF();
    	    _delay_ms(10);

    	    // Turn the power down on the microcontroller
            sleep_mode();
    	}
    }
}

/********************************************************************************
	Functions
********************************************************************************/
void initGPIO() {
    _in(DDD2, DDRD); // INT0 input

    // GPIO Interrupt INT0
    EICRA = (0<<ISC11)|(0<<ISC10)|(0<<ISC01)|(1<<ISC00); // Any logical change on INT0 generates an interrupt request.
    EIMSK = (0<<INT1)|(1<<INT0); // Enable INT0
}

void powerOnRF() {
    if (!rfAwake) {
        radio.powerUp();
        rfAwake = true;
    }
}

void powerDownRF() {
    // need to flush tx buffer, fixed the issue with packet shift...
    radio.stopListening();
    radio.powerDown();
    rfAwake = false;
}

void processIRCommand(uint64_t cmd) {
    uint64_t buf_ir_cmd = ir_cmd;

    /* for (int8_t i = 63; i >= 0; i--) {
        if (i == 7 || i == 15 || i == 23 || i == 31 || i == 39 || i == 47 || i == 55) {
            printf(" ");
        }
        printf("%d", (buf_ir_cmd & (((uint64_t) 1) << i)) ? 1 : 0);
    } */

    uint8_t mapped_cmd = 0;

    if ( (buf_ir_cmd & 0xFFFF) == 0x0707 ) { // TV command signature
        uint8_t cmd_b = (uint8_t) (buf_ir_cmd >> 16);
        uint8_t cmd_nb = ~ ((uint8_t) (buf_ir_cmd >> 24));

        if (cmd_b == cmd_nb) {
            mapped_cmd = mapTVKey(cmd_b);
        }
    } else if ( (buf_ir_cmd & 0xFFFFFF) == 0xF32066 ) { // BD command signature
        uint8_t cmd_b = (uint8_t) (buf_ir_cmd >> 26);
        uint8_t cmd_nb = ~ ((uint8_t) (buf_ir_cmd >> 34));

        if (cmd_b == cmd_nb) {
            mapped_cmd = mapBDKey(cmd_b);
        }
    } else if ( (buf_ir_cmd & 0x000FFFFF) == 0x000F0020 ) { // DVD command signature
        uint8_t cmd_b = (uint8_t) (buf_ir_cmd >> 21);
        uint8_t cmd_nb = ~ ((uint8_t) (buf_ir_cmd >> 29));

        if (cmd_b == cmd_nb) {
            mapped_cmd = mapDVDKey(cmd_b);
        }
    }

    if (mapped_cmd != 0) {
        int32_t whenLastCommandReceived = getCurrentTimeCycles() - lcr;
        lcr = getCurrentTimeCycles();

        if (whenLastCommandReceived < 1000) { // if it is ~840 this is a repeated IR command check if we need to skip it
            if (skip_repeted_ir_count < (SKIP_REPEATED_IR - 1)) { // skip first n IR repeated commands
                skip_repeted_ir_count++;
                return;
            }
        } else {
            skip_repeted_ir_count = 0;
        }

        printf("\n[%d]", mapped_cmd);

        lcr = getCurrentTimeCycles();

        // Send data via RF module
        uint8_t data[] = {mapped_cmd};
        radio.write(data, 1);
    } else {
        printf("\nUnknown mapping");
    }
}

void handle_usart_cmd(char *cmd, char *args) {
	if (strcmp(cmd, "test") == 0) {
		printf("\n TEST [%s]", args);
	}

	if (strcmp(cmd, "reset") == 0) {
	    ir_index = 0;
	    lpp = getCurrentTimeCycles();
	    lnp = lpp;
	}

	if (strcmp(cmd, "show") == 0) {
	    printf("\n=========== %d\n", ir_index);
	    for (uint16_t i = 0; i < ir_index; i++) {
	        printf("%d|", ir_data_debug[i]);
	    }
	}
}
