#include <cstdlib>
#include <iostream>
#include <queue> 			// std::queue
#include <Inputs.h> 		// listens for user inputs
#include <Motor.h> 			// servos motors execute user input commands

std::queue<unsigned char> *motorAQueue; // contains cmds for left motor
std::queue<unsigned char> *motorBQueue; // contains cmds for right motor

int main(int argc, char *argv[]) {
	motorAQueue = new std::queue<unsigned char>();
	motorBQueue = new std::queue<unsigned char>();

	Inputs *inputs = new Inputs();
	Motor *leftMotor = new Motor(motorAQueue);
	Motor *rightMotor = new Motor(motorBQueue);

	inputs->run();
	leftMotor->run();
	//rightMotor->run();

	while(true){sleep(1);}
	return EXIT_SUCCESS;
}
