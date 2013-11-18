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

Motor::Motor(std::queue<unsigned char> *inputQ, UINT8 *recipe, UINT8 size,
		uintptr_t port) {
	// TODO Auto-generated constructor stub
	_inputQueue = inputQ;
	_currentPos = 0;
	_port = port;
	_recipe = recipe;
	_size = size;
}

Motor::~Motor() {
	// TODO Auto-generated destructor stub
}

void Motor::startSignal() {
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

	while (true) {

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

void Motor::moveMotor(int position) {

	/* Check bounds */
	if (position >= MAX_POS || position <= LOW_POS) {
		return;
	}

	/* Change current position */
	_currentPos = position;
}

int Motor::mirror(int pos){
	if(pos == 0){
		return 5;
	} else if (pos == 1){
		return 4;
	} else if (pos == 2){
		return 3;
	} else if (pos == 1){
		return 2;
	} else if (pos == 0){
		return 5;
	} else { // ERROR
		return 0;
	}
}

/**
 * Pulls commands from the queue and executes them
 */
void Motor::executeCmds() {
	UINT8 cmd;
	UINT8 cur = 0;
	UINT8 loopPointer = 0;
	UINT8 loopCounter = 0;
	UINT8 status = 0; /* 0 = running, 1 = stopped */
	int wait;
	bool pause = false;

	while (true) {
		if (!_inputQueue->empty()) {
			cmd = _inputQueue->front();
			_inputQueue->pop();

			if (cmd == 'l' || cmd == 'L') {
				if (_currentPos > 0 && pause) {
					_currentPos--;
				}
			} else if (cmd == 'r' || cmd == 'R') {
				if (_currentPos < 5 && pause) {
					_currentPos++;
				}
			} else if (cmd == 'p' || cmd == 'P'){
				pause = true;
			} else if (cmd == 'c' || cmd == 'C'){
				pause = false;
			} else if (cmd == 'b' || cmd == 'B'){
				cur = 0;
			} else if (cmd == 'n' || cmd == 'N'){
				// do nothing!
			}

		} else {
			if (cur < _size && !pause) {
				// Read Command & Instruction:
				int delta = _currentPos;
				UINT8 instr = _recipe[cur++];
				UINT8 cmd = instr & 0b11100000;
				UINT8 arg = instr & 0b00011111;

				if (cmd == MOV) { // MOVE COMMAND
					_currentPos = arg;
					delta -= _currentPos;
					if (delta < 0) {
						delta *= -1;
					}
					sleep(delta);

				} else if (cmd == WAIT) { // WAIT COMMAND
					wait = arg;
					usleep(100000 * wait);

				} else if (cmd == LOOP_START) { // LOOP START
					if (loopCounter > 0) { // Check Nested Loop
						/* Update LEDs: Not Required*/
						status = STAT_NLE;
					} else {
						loopCounter = arg;
						loopPointer = cur;
					}
				} else if (cmd == END_LOOP) { // END LOOP
					if( loopCounter > 0){
						cur = loopPointer;
						loopCounter--;
					}else{/* Continue to next instruction. */}
				} else if (cmd == RECIPE_END) { // RECIPE END
					/* Update the status */
					status = STAT_ERE;
				} else if (cmd == MIRROR){
					_currentPos = mirror(_currentPos);
					delta -= _currentPos;
					if (delta < 0) {
						delta *= -1;
					}
					sleep(delta);
				}

			}

		}
		sched_yield();
	}

}

void Motor::run() {
	int rc = pthread_create(&_thread, NULL, MotorRunFunction, this);

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

