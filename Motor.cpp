/*
 * Motor.cpp
 *
 *  Created on: Nov 15, 2013
 *      Author: cbr4830
 */

#include "Motor.h"


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

#define DIOIN_PORTAB (0b0000000) //TODO only DIRA + DIRB
/* Port Direction Register */
#define PORT_DIR_OFFSET (11)
#define PORTAB_DIR_ADDR (DIGITAL_IO_BASE_ADDR + PORT_DIR_OFFSET)

// Bounds:
#define MAX_POS (6)
#define LOW_POS (1)

/* Motor Constants */
#define NSP (5)
#define SERVO_MIN (1000000)
#define SERVO_MAX (2000000)
#define SERVO_POSITION(x) (((SERVO_MIN) + (((SERVO_MAX-SERVO_MIN) * x)/NSP)))

#define MOV(x)           (0x20 | (x))


Motor::Motor(std::queue<unsigned char> *inputQ, std::deque<unsigned char> *recipe, uintptr_t port) {
	// TODO Auto-generated constructor stub
	_inputQueue = inputQ;
	_currentPos = 1;
	_port = port;
	_recipe = recipe;
}

Motor::~Motor() {
	// TODO Auto-generated destructor stub
}

void Motor::startSignal(){
	struct sigevent event;
	struct itimerspec itime;
	timer_t timer_id;
	int chid;
	int rcvid;
	my_message_t msg;

	chid = ChannelCreate(0);

	event.sigev_notify = SIGEV_PULSE;
	event.sigev_coid = ConnectAttach(ND_LOCAL_NODE, 0, chid, _NTO_SIDE_CHANNEL,
			0);
	event.sigev_priority = getprio(0);
	event.sigev_code = MY_PULSE_CODE;
	timer_create(CLOCK_REALTIME, &event, &timer_id);

	itime.it_value.tv_sec = 0;
	/* 20000 million nsecs = 20 usecs */
	itime.it_value.tv_nsec = 20000000;
	itime.it_interval.tv_sec = 0;
	/* 20000 million nsecs = 20 usecs */
	itime.it_interval.tv_nsec = 20000000;
	timer_settime(timer_id, 0, &itime, NULL);

	while(true){
	//	upTime = (POS0 + (INCR * _currentPos));
	//	out8(_port, LOGIC_HIGH);
	//	nanospin_ns(upTime);
	//	std::cout << "Nanoing for... " << SERVO_POSITION(_currentPos) << "\n";
	//	nanospin_ns(SERVO_POSITION(_currentPos));
	// 	out8(_port, LOGIC_LOW);
	//	usleep(1000);
		rcvid = MsgReceive(chid, &msg, sizeof(msg), NULL);
		if (rcvid == 0) { /* we got a pulse */
			if (msg.pulse.code == MY_PULSE_CODE) {
				//printf("we got a pulse from our timer\n");
				out8(_port, LOGIC_HIGH);
				nanospin_ns(SERVO_POSITION(_currentPos));
				out8(_port, LOGIC_LOW);
				sched_yield();
			} /* else other pulses ... */
		} /* else other messages ... */

	}
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

	std::cout << "Motor executing commands \n";
	unsigned char cmd;

	while(true){
		if(!_inputQueue->empty()){
			cmd = _inputQueue->front();
			_inputQueue->pop();

			if(cmd == 'l'){
				if(_currentPos > 0){
					_currentPos--;
				}
			} else if (cmd == 'r'){
				if(_currentPos < 5){
					_currentPos++;
				}
			}
			std::cout << cmd << "!!! " << _currentPos << "\n";

		} else {
			//std::cout << "SIZE: " + _recipe.size();
			if(_recipe->size() > 0){
			//	std::cout << "CHECK RECIPE \n";
				unsigned char recCmd = _recipe->front();
				int delta = _currentPos;

				if(recCmd >= 31 && recCmd < 64){
					 _currentPos = recCmd & 0b00011111;
				}
				_recipe->pop_front();

				//nanospin_ns(SERVO_POSITION(_currentPos));

				delta -= _currentPos;
				if( delta < 0 ){
					delta *= -1;
				}
				//printf("Value: %d\n",delta);
				//nanospin_ns( delta*1000000000 );
				sleep(delta);
			//	std::cout << "SIZE: " << _recipe->size() << "\n";
			}

		}
		sched_yield();
	}

}

void Motor::run() {
	int rc = pthread_create(&_thread, NULL, MotorRunFunction, this);
	_active = true;
	if (rc) {
		fprintf(stderr, "ERROR(%d): PThread not created.\n", rc);
		return;
	}

	rc = pthread_create(&_thread, NULL, PWMRunFunction, this);

	if (rc) {
		fprintf(stderr, "ERROR(%d): PThread not created.\n", rc);
		return;
	}
}

