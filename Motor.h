/*
 * Motor.h
 *
 *  Created on: Nov 15, 2013
 *      Author: cbr4830
 */

#ifndef MOTOR_H_
#define MOTOR_H_

#include <queue>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <iostream>
#include <string>


class Motor {
public:

	Motor(std::queue<unsigned char> *);
	virtual ~Motor();

	void executeCmds();
	void run();

private:
	static void * MotorRunFunction(void * This) {((Motor *)This)->executeCmds(); return NULL;}
	pthread_t _thread;
	std::queue<unsigned char> *_queue;
	bool _active;
};
#endif /* MOTOR_H_ */
