/*
 * Inputs.h
 * 
 * Header file. 
 * 
 * Represents user interactions with the system. Creates a thread that will
 * listen for user inputs and appropriately place them into the correct motor
 * input queue, which the motors will check to see if a user has placed a command
 *
 * Encapsulates a thread.
 * @author Christoffer Rosen
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
	Inputs(); // constructor
	virtual ~Inputs(); // deconstructor
	void addCmdsToMotorQueues(unsigned char, unsigned char); // adds commands to appropriate motor input queue
	void listen(); // forever listens for user interaction inputs
	void stop(); // stop the thread - not used
	void run();  // initalize the thread and start listening

private:
	static void * InputsRunFunction(void * This) {((Inputs *)This)->listen(); return NULL;}
	pthread_t _thread; 
};

#endif /* INPUTS_H_ */
