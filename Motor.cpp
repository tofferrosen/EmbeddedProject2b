/*
 * Motor.cpp
 *
 *  Created on: Nov 15, 2013
 *      Author: cbr4830
 */

#include "Motor.h"


// Instructions:
#define MOV (0b00100000)
#define WAIT (0b01000000)
#define LOOP_START (0b10000000)
#define END_LOOP (0b10100000)
#define RECIPE_END (0b00000000)


// Commands:
#define OVR_PAUSE ('p')
#define OVR_CONTINUE ('c')
#define OVR_RIGHT ('r')
#define OVR_LEFT ('l')
#define OVR_NOOP ('n')
#define OVR_RESTART ('b')

// Status:
#define STAT_NORM (0b0000)
#define STAT_RCE (0b1000)
#define STAT_NLE (0b0100)
#define STAT_ERE (0b0010)
#define STAT_RP (0b0001)

#define ENABLE (1)
#define DISABLE (0)

// Bounds:
#define MAX_POS (6)
#define LOW_POS (1)

Motor::Motor(std::queue<unsigned char> *queue, uintptr_t port) {
	// TODO Auto-generated constructor stub
	_queue = queue;
	_currentPos = 0;
	_port = port;
}

Motor::~Motor() {
	// TODO Auto-generated destructor stub
}

void Motor::moveMotor(int position){
	// check bounds
	if(position >= MAX_POS || position <= LOW_POS){
		return;
	}
	//TODO move motor
}

/**
 * Pulls commands from the queue and executes them
 */
void Motor::executeCmds(){
	unsigned char cmd;
	std::cout << "Motor executing commands \n";

	while(_active){

		if(!_queue->empty()){
			cmd = _queue->front();
			std::cout << "Motor trying to execute: " << cmd << "\n";
			_queue->pop();
			std::cout << "Queue size: " << _queue->size();
			//TODO execute command

			switch( cmd ){
					case OVR_PAUSE:
					//	updateStatus( s, STAT_RP, ENABLE );
						break;
					case OVR_CONTINUE:
					//	updateStatus( s, STAT_RP, DISABLE );
						break;
					case OVR_RIGHT:
						moveMotor(_currentPos+1);
						break;
					case OVR_LEFT:
						moveMotor(_currentPos-1);
						break;
					case OVR_RESTART:
					//	servoRestart( s, s->id ); // Clears status flag
					//	updateStatus( s, STAT_RP, DISABLE );
						break;
					case OVR_NOOP:
					default:
						// Do Nothing
						break;
				}
		}
		usleep(100);
	}
}

void Motor::run() {
	int rc = pthread_create(&_thread, NULL, MotorRunFunction, this);
	_active = true;
	if (rc) {
		fprintf(stderr, "ERROR(%d): PThread not created.\n", rc);
		return;
	}
}

