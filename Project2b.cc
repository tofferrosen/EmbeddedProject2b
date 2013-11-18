/**
 * Project 2b. 
 * 
 * Reimplementing project 2a with QNX
 * Christoffer Rosen
 * Lennard Streat
 * 
 * 
 * This file is the main entry point for the program.
 * Creates the pointers to the memory mapped io for the GPIO port and initializes the motors and input listener.
 **/

#include <cstdlib>
#include <iostream>
#include <stdint.h>                  /* Resource(s): uintptr_t */
#include <hw/inout.h>                /* Resource(s): in*(), out*() */
#include <sys/mman.h>                /* Resource(s): mmap_device_io() */
#include <sys/neutrino.h>            /* Resource(s): ThreadCtl() */
#include <queue> 		     /* std::queue */
#include <Inputs.h> 		     /* listens for user inputs */
#include <Motor.h> 		     /* servos motors execute user input commands */
#include <list>          	     /* std::list */

/* Constant used for the mman library */
#define BYTE (1)

/* PORT A and PORT B Registers */
#define DIGITAL_IO_BASE_ADDR (0x280)
#define PORTA_OFFSET (8)	/* PORTA */
#define PORTB_OFFSET (9)	/* PORTB */
#define PORTA_ADDR (DIGITAL_IO_BASE_ADDR + PORTA_OFFSET)
#define PORTB_ADDR (DIGITAL_IO_BASE_ADDR + PORTB_OFFSET)

/* Port Direction Register */
#define PORT_DIR_OFFSET (11)
#define PORTAB_DIR_ADDR (DIGITAL_IO_BASE_ADDR + PORT_DIR_OFFSET)

/* Set Port A and Port B for Output */
#define DIOIN_PORTAB (0b0000000) // All to output


/* Servos */
#define NSP (5)
#define POS0 (800000)
#define INCR (200000)
#define PERIOD (20000000)

/******************************************************************************
 * Recipes
 ******************************************************************************/
UINT8 recipe1[] = {
	// Group 1:
	MOV+0, MOV+5, MOV+0,
	// Test Default Loop Behavior:
	MOV+3, LOOP_START+0, MOV+1, MOV+4, END_LOOP,
	MOV+0,
	// Move to verify WAIT+0:
	MOV+2, WAIT+0, MOV+3,
	// Measure the timing precision of 9.3 second delay:
	MOV+2, MOV+0, WAIT+31, WAIT+31, WAIT+31, MOV+5,
	RECIPE_END
};// recipe1

/* Queues containing motor commands for input */
std::queue<unsigned char> *motorAInputQueue; // contains cmds for left motor
std::queue<unsigned char> *motorBInputQueue; // contains cmds for right motor

int main(int argc, char *argv[]) {
	 /* Error Handling */
	int privity_err;
	int return_code = EXIT_SUCCESS;

	/* Memory Mapped IO: */
	uintptr_t porta;
	uintptr_t portb;
	uintptr_t portab_dir;

	/* Enable GPIO access to the current thread: */
	privity_err = ThreadCtl(_NTO_TCTL_IO, NULL );
	if (privity_err == -1) {
		printf("Error: Unable to acquire root permission for GPIO.\n");
		return_code = EXIT_FAILURE;
	} else {

		/* Create a pointer to Data Acquisition Port (Memory Mapped IO): */
		porta = mmap_device_io(BYTE, PORTA_ADDR);
		portb = mmap_device_io(BYTE, PORTB_ADDR);
		portab_dir = mmap_device_io(BYTE, PORTAB_DIR_ADDR);

		/* Initalize PORT A & B for output */
		out8(portab_dir, DIOIN_PORTAB);

		/* Initiaize queues */
		motorAInputQueue = new std::queue<unsigned char>();
		motorBInputQueue = new std::queue<unsigned char>();

		/* Initialze motors and inputs  */
		Inputs *inputs = new Inputs();

		Motor *leftMotor = new Motor(motorAInputQueue, recipe1,19, porta);
		Motor *rightMotor = new Motor(motorBInputQueue, recipe1,19, portb);

		/* Start */
		leftMotor->run();
		rightMotor->run();

		inputs->run();

		while(true){
			usleep(100);
		}
		return EXIT_SUCCESS;
	}
}
