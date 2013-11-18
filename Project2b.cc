#include <cstdlib>
#include <iostream>
#include <stdint.h>                  /* Resource(s): uintptr_t */
#include <hw/inout.h>                /* Resource(s): in*(), out*() */
#include <sys/mman.h>                /* Resource(s): mmap_device_io() */
#include <sys/neutrino.h>       	 /* Resource(s): ThreadCtl() */
#include <queue> 					 /* std::queue */
#include <Inputs.h> 				 /* listens for user inputs */
#include <Motor.h> 					 /* servos motors execute user input commands */
#include <list>          			 /* std::list */
#include <deque>          // std::deque

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
#define DIOIN_PORTAB (0b0000000) //TODO only DIRA + DIRB

/* Servos */
#define NSP (5)
#define POS0 (800000)
#define INCR (200000)
#define PERIOD (20000000)

// Instructions:
#define MOV (0b00100000)
#define WAIT (0b01000000)
#define LOOP_START (0b10000000)
#define END_LOOP (0b10100000)
#define RECIPE_END (0b00000000)
#define MIRROR (0b01100000)


/* Queues containing motor commands for input */
std::queue<unsigned char> *motorAInputQueue; // contains cmds for left motor
std::queue<unsigned char> *motorBInputQueue; // contains cmds for right motor

std::deque<unsigned char> recipe1 (MOV+5,MOV+1);
std::deque<unsigned char> recipe2 (MOV+5, MOV+1);

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

		/* Initalize PORT A & B for Input */
		out8(portab_dir, DIOIN_PORTAB);

		/** TESTING MOTORS WORKS :)
		int upTime;
		int downTime;
		int pos = 5;
		while(true){

			upTime = (POS0 + (INCR*pos));
			downTime = PERIOD - upTime;

			out8(porta,0x00);
			out8(portb,0x00);
			nanospin_ns(downTime);

			out8(porta,1);
			out8(portb,1);
			nanospin_ns(upTime);
		} **/

		/* Initiaize queues */
		motorAInputQueue = new std::queue<unsigned char>();
		motorBInputQueue = new std::queue<unsigned char>();


		/* Initialze motors and inputs  */
		Inputs *inputs = new Inputs();
		out8(porta,0);
		out8(portb,0);
		//Motor *leftMotor = new Motor(motorAInputQueue, recipe1, porta);
		//Motor *rightMotor = new Motor(motorBInputQueue, recipe2, portb);

		/* Start */
		//inputs->run();
		//leftMotor->run();
		//rightMotor->run();

		while(true){}
		return EXIT_SUCCESS;
	}
}
