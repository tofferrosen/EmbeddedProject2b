/*
 * Motor.h
 *
 *  Created on: Nov 15, 2013
 *      Author: cbr4830
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

typedef union {
        struct _pulse   pulse;
        /* your other message structures would go
           here too */
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
	int _currentPos;
	uintptr_t _port;
	int _pulseWidthNS;
	std::queue<unsigned char> *_inputQueue;
	UINT8* _recipe;
	UINT8 _size;
};
#endif /* MOTOR_H_ */
