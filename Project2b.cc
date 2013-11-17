#include <cstdlib>
#include <iostream>
#include <stdint.h>                  /* Resource(s): uintptr_t */
#include <hw/inout.h>                /* Resource(s): in*(), out*() */
#include <sys/mman.h>                /* Resource(s): mmap_device_io() */
#include <sys/neutrino.h>       	 /* Resource(s): ThreadCtl() */
#include <queue> 					 /* std::queue */
#include <Inputs.h> 				 /* listens for user inputs */
#include <Motor.h> 					 /* servos motors execute user input commands */

/* Constant used for the mman library */
#define BYTE (1)

/* PORT A and PORT B Registers */
#define DIGITAL_IO_BASE_ADDR (0x280)
#define PORTA_OFFSET (1)
#define PORTB_OFFSET (9)
#define PORTA_ADDR (DIGITAL_IO_BASE_ADDR + PORTA_OFFSET)
#define PORTB_ADDR (DIGITAL_IO_BASE_ADDR + PORTB_OFFSET)

/* Port Direction Register */
#define PORT_DIR_OFFSET (11)
#define PORTAB_DIR_ADDR (DIGITAL_IO_BASE_ADDR + PORT_DIR_OFFSET)

/* Set Port A and Port B for Input */
#define DIOIN_PORTAB (0b1111111) //TODO only DIRA + DIRB

/* Queues containing motor commands */
std::queue<unsigned char> *motorAQueue; // contains cmds for left motor
std::queue<unsigned char> *motorBQueue; // contains cmds for right motor


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

		/* Initiaize queues */
		motorAQueue = new std::queue<unsigned char>();
		motorBQueue = new std::queue<unsigned char>();

		/* Initialze motors and inputs  */
		Inputs *inputs = new Inputs();
		Motor *leftMotor = new Motor(motorAQueue, porta);
		Motor *rightMotor = new Motor(motorBQueue, portb);

		/* Start */
		inputs->run();
		leftMotor->run();
		//rightMotor->run();

		while(true){sleep(1);}
		return EXIT_SUCCESS;
	}
}
