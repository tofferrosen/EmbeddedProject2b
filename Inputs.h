/*
 * Inputs.h
 *
 *  Created on: Nov 15, 2013
 *      Author: cbr4830
 */

#ifndef INPUTS_H_
#define INPUTS_H_

#include <queue>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <iostream>
#include <string>

class Inputs {
public:
	Inputs();
	virtual ~Inputs();
	void addCmdsToMotorQueues(unsigned char, unsigned char);
	void listen();
	void stop();
	void run();

private:
	static void * InputsRunFunction(void * This) {((Inputs *)This)->listen(); return NULL;}
	pthread_t _thread;
};

#endif /* INPUTS_H_ */
