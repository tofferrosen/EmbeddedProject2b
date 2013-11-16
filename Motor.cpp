/*
 * Motor.cpp
 *
 *  Created on: Nov 15, 2013
 *      Author: cbr4830
 */

#include "Motor.h"

Motor::Motor(std::queue<unsigned char> *queue) {
	// TODO Auto-generated constructor stub
	_queue = queue;
}

Motor::~Motor() {
	// TODO Auto-generated destructor stub
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

