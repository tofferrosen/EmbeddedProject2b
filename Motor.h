/*
 * Header file for Servos.cpp
 *
 * Represents the Servos Motor. 
 * It is able to both execute command recipies and user input comamnds.
 *
 * It encapsulates two threads: 1) One is responsible for executing recipes and user input commands. 
 * It appropriately controls how long the uptime is required for the next time signal is sent.  
 * 2) The second thread generates the signal, or PWM, to the motor. It sends the pulse width uptime 
 * required to move the servos motor to required position every 20ms which is manipulated by thread 1. 
 * This is done by setting 1 on the digital IO board for the required uptime  to set position of the motor 
 * and then setting it back to zero every 20 milliseconds through the use of a timer.
 */

#ifndef MOTOR_H_
#define MOTOR_H_

#include <queue>
#include <stdint.h>       /* Resource(s): uintptr_t */
#include <hw/inout.h>     /* Resource(s): in*(), out*() */
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <iostream>
#include <string>
#include <deque>
#include <time.h>

#include <time.h>
#include <sys/netmgr.h>
#include <sys/neutrino.h>
#define MY_PULSE_CODE   _PULSE_CODE_MINAVAIL
typedef unsigned char UINT8;

/* USED FOR TIMMER */
typedef union {
        struct _pulse   pulse;
} my_message_t;

#define LOGIC_HIGH (0x0F)                /* For the wave generation */
#define LOGIC_LOW (0x00)                 /* For the wave generation */

// Instructions:
#define MOV (0b00100000)
#define WAIT (0b01000000)
#define LOOP_START (0b10000000)
#define END_LOOP (0b10100000)
#define RECIPE_END (0b00000000)
#define MIRROR (0b01100000)


class Motor {
public:

	/** See comments in .cpp file **/
	Motor(std::queue<unsigned char> *, UINT8*, UINT8, uintptr_t); 
	virtual ~Motor();
	void moveMotor(int); 
	void executeCmds();
	void startSignal();
	int mirror(int);
	void run();

private:
	static void * MotorRunFunction(void * This) {((Motor *)This)->executeCmds(); return NULL;}
	static void * PWMRunFunction(void * This) {((Motor *)This)->startSignal(); return NULL;}
	pthread_t _thread;	
	int _currentPos;				/* The current position of the motor */
	uintptr_t _port;				/* The digitial IO port */					
	std::queue<unsigned char> *_inputQueue;		/* Queue checked for input commands */
	UINT8* _recipe;					/* Pointer to array checked for recipe commands */
	UINT8 _size;					/* Size of the recipe array used for bounds check */
};
#endif /* MOTOR_H_ */
