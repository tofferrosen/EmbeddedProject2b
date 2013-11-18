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

/* Motor Constants */
#define NSP (5)
#define POS0 (800000)
#define INCR (200000)
#define PERIOD (20000000)


Motor::Motor(std::queue<unsigned char> *inputQ, std::deque<unsigned char> recipe, uintptr_t port) {
	// TODO Auto-generated constructor stub
	_inputQueue = inputQ;
	_currentPos = 0;
	_port = port;
	_recipe = recipe;
}

Motor::~Motor() {
	// TODO Auto-generated destructor stub
}

void Motor::moveMotor(int position){

	/* Check bounds */
	if(position >= MAX_POS || position <= LOW_POS){
		return;
	}

	/* Change current position */
	_currentPos = position;
}

/**
 * Pulls commands from the queue and executes them
 */
void Motor::executeCmds(){
	unsigned char cmd;
	int upTime = 0;
	int downTime = 0;

	std::cout << "Motor executing commands \n";

	while(true){
		upTime = (POS0 + (INCR*_currentPos));
		downTime = PERIOD - upTime;
		out8(_port,LOGIC_LOW);
		nanospin_ns(downTime);
		out8(_port,LOGIC_HIGH);
		nanospin_ns(upTime);

		/* Check queue for user command
		 * Otherwise, execute recipe */
		if(!_inputQueue->empty()){
			cmd = _inputQueue->front();
			_inputQueue->pop();

		} else {
			if(_recipe.size() > 0){

			}

		}

	}

	while(true){

		std::cout << "HI";

		// check queue for cmd
		if(!_inputQueue->empty()){
			cmd = _inputQueue->front();
			std::cout << "Motor trying to execute: " << cmd << "\n";
			_inputQueue->pop();
			std::cout << "Queue size: " << _inputQueue->size();
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
		} else {

		}
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

