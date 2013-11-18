/*
 * Inputs.cpp
 *
 *  This will listen for user commands.
 *	Pushes cmds to approproate motor queues
 *  Created on: Nov 15, 2013
 *      Author: cbr4830
 */

#include "Inputs.h"

extern std::queue<unsigned char> *motorAInputQueue;
extern std::queue<unsigned char> *motorBInputQueue;

Inputs::Inputs() {
	// TODO Auto-generated constructor stub
}

Inputs::~Inputs() {
	// TODO Auto-generated destructor stub
}

/**
 * Adds to the each respective motor queue
 * the commands, such that the motors can then
 * execute it when it gets the chance.
 */
void Inputs::addCmdsToMotorQueues(unsigned char leftCmd, unsigned char rightCmd) {
	std::cout << "Adding to the motor queues \n";

	motorAInputQueue->push(leftCmd);
	motorBInputQueue->push(rightCmd);
}

/**
 * listen()
 * Purpose: Listens to user commands. User commands are limited to the first two
 * characters in a line before carriage return.
 */
void Inputs::listen() {
	std::string userInput;
	unsigned char leftCmd;
	unsigned char rightCmd;

	std::cout << "Enter User Commands: \n";

	while (true) {
		std::getline(std::cin, userInput);

		if(userInput.length() < 2){
			// ERROR
		} else {
			leftCmd = userInput[0];
			rightCmd = userInput[1];

			std::cout << userInput;
			std::cout << "Left command: " << leftCmd << "\n";
			std::cout << "Right command: " << rightCmd << "\n";
			addCmdsToMotorQueues(leftCmd, rightCmd);
		}
		usleep(100);
	}
}

/**
 * Start the inputs worker
 */
void Inputs::run() {

	int rc = pthread_create(&_thread, NULL, InputsRunFunction, this);
	if (rc) {
		fprintf(stderr, "ERROR(%d): PThread not created.\n", rc);
		return;
	}
}


void Inputs::stop() {
	pthread_join(_thread,NULL);
}


