/*
 * Inputs.cpp
 *
 *  Represents user interactions with the system. Creates a thread that will
 *  listen for user inputs and appropriately place them into the correct motor
 *  input queue, which the motors will check to see if a user has placed a command
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
 * addCmdsToMotorQueues
 * Responsible for adding the the command to the appropriate queue and 
 * prompting the user for additional input
 * 
 * @param leftCmd	Command to be placed for left servos motor
 * @param rightCmd	Command to be placed for the right servos motor
 */
void Inputs::addCmdsToMotorQueues(unsigned char leftCmd, unsigned char rightCmd) {
	motorAInputQueue->push(leftCmd);
	motorBInputQueue->push(rightCmd);

	std::cout << ">>";
}

/**
 * listen
 * Responnsible for listening to user commands. User commands are limited to the first two
 * characters in a line before carriage return.
 */
void Inputs::listen() {
	std::string userInput;
	unsigned char leftCmd;
	unsigned char rightCmd;

	std::cout << "Enter User Commands: \n>>" ;

	while (true) {
		std::getline(std::cin, userInput);

		if(userInput.length() < 2){
			// ERROR
		} else {
			leftCmd = userInput[0];
			rightCmd = userInput[1];
			addCmdsToMotorQueues(leftCmd, rightCmd);
		}
		usleep(100);
	}
}

/**
 * Initalizes the thread to start listening for user commands.
 * The thread will be running the listen method.
 */
void Inputs::run() {

	int rc = pthread_create(&_thread, NULL, InputsRunFunction, this);
	if (rc) {
		fprintf(stderr, "ERROR(%d): PThread not created.\n", rc);
		return;
	}
}

/**
 * This method is not used for this program. For future extensibility, this can be
 * called to stop this thread
 **/
void Inputs::stop() {
	pthread_join(_thread,NULL);
}


