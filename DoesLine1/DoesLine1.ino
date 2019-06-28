/*
  DoesLine1
  Line follow with 5 sensors
*/

//pin values for sensors
const int sensorL2 = 2;
const int sensorL1 = 3;
const int sensorC = 4;
const int sensorR1 = 5;
const int sensorR2 = 6;

// pin values for motor control
const int leftF = 10;  // set to true to go forward
const int leftB = 12;  // set to true to go backwards
const int leftP = 11;  // PWM power setting 0 - 255
const int rightF = 7;  // set to true to go forward
const int rightB = 8;  // set to true to go backwards
const int rightP = 9;  // PWM power setting 0 - 255

// constants for motor speed/direction combinations
const int M_STOP = 0;
const int M_FORWARD = 1;
const int M_BACK = 2;
const int M_ROTATE_RIGHT = 3;
const int M_ROTATE_LEFT = 4;
const int M_RIGHT = 5;
const int M_LEFT = 6;
const int M_PIVOT_RIGHT = 7;
const int M_PIVOT_LEFT = 8;
const int M_PIVOT_BACK_RIGHT = 9;
const int M_PIVOT_BACK_LEFT = 10;

// define the possible states
const int S_START = 0;
const int S_OFF_LINE = 1;
const int S_ON_LINE = 2;
const int S_MOVE_LEFT = 3;
const int S_MOVE_RIGHT = 4;
const int S_PIVOT_LEFT = 5;
const int S_PIVOT_RIGHT = 6;
const int S_ROTATE_LEFT = 7;
const int S_ROTATE_RIGHT = 8;

// define strings for use in debug messages
char *stateNames[] = { "START","OFF_LINE","ON_LINE","MOVE_LEFT","MOVE_RIGHT",
					  "PIVOT_LEFT","PIVOT_RIGHT","ROTATE_LEFT","ROTATE_RIGHT" };



int currentState = S_OFF_LINE;

// individual inputs
int left2S = 0;
int left1S = 0;
int centreS = 0;
int right1S = 0;
int right2S = 0;

// inouts in a single integer
int sensors = 0;
bool leftLine = false;
bool rightLine = false;

// timing
unsigned long currentMillis;
unsigned long stateMillis;
unsigned long targetMillis;

void setup() {
	Serial.begin(9600);

	// set motor pins to output
	pinMode(leftF, OUTPUT);
	pinMode(leftB, OUTPUT);
	pinMode(leftP, OUTPUT);
	pinMode(rightF, OUTPUT);
	pinMode(rightB, OUTPUT);
	pinMode(rightP, OUTPUT);

	// set sensor pins to input
	pinMode(sensorL2, INPUT);
	pinMode(sensorL1, INPUT);
	pinMode(sensorC, INPUT);
	pinMode(sensorR1, INPUT);
	pinMode(sensorR2, INPUT);

	leftLine = false;
	rightLine = false;

	// zero all motor outputs
	digitalWrite(leftF, LOW);
	digitalWrite(leftB, LOW);
	analogWrite(leftP, 0);
	digitalWrite(rightF, LOW);
	digitalWrite(rightB, LOW);
	analogWrite(rightP, 0);

	setState(S_START);
}

void loop() {


	updateState();
	//delay(10);


  /*
	// motor direction test
	setMotors(M_RIGHT_1);
	delay(2000);
	setMotors(M_LEFT_1);
	delay(2000);
	//setMotors(M_STOP);
	//delay(2000);
  */

 /*
 	// test the five sensors
	readSensors();
	Serial.print(left2S);
	Serial.print(left1S);
	Serial.print(centreS);
	Serial.print(right1S);
	Serial.print(right2S);
	Serial.println();
 */
 
}

void printSensors() {
	Serial.print(left2S);
	Serial.print(left1S);
	Serial.print(centreS);
	Serial.print(right1S);
	Serial.print(right2S);
	Serial.println();
}



void updateState() {

	readSensors();

	if ((sensors > 20) && (!leftLine)) {
		leftLine = true;
		rightLine = false;
		Serial.println("Left line detected");
	}
	if ((sensors == 7) && (!rightLine)) {
		rightLine = true;
		leftLine = false;
		Serial.println("Right line detected");
	}



	// this should only happen at the end of the line
	if ((sensors == 0) && (currentState != S_OFF_LINE)) {
		setState(S_OFF_LINE);
		return;
	}


	switch (currentState) {

	case S_START:
		if (sensors & 4) {
			setState(S_ON_LINE);
		}
		break;

	case S_OFF_LINE:
		if (sensors > 0) {
			setState(S_ON_LINE);
		}
		break;

	case S_ON_LINE:

		// pivot if L1&L2 or R1&R2 are triggered
		//if (sensors & 1) {
		//	setState(S_ROTATE_RIGHT);
		//}
		//else if (sensors & 16) {
		//	setState(S_ROTATE_LEFT);
		//}



		// don't correct until centre sensor is no longer active
		if ((sensors & 3) && !(sensors & 4)) {
			setState(S_MOVE_RIGHT);
		}
		if ((sensors & 24) && !(sensors & 4)) {
			setState(S_MOVE_LEFT);
		}
		break;

	case S_MOVE_RIGHT:
		if (sensors & 4) {
			setState(S_ON_LINE);
		}
		else if (sensors & 1) {
			setState(S_PIVOT_RIGHT);
		}
		break;

	case S_PIVOT_RIGHT:
		if (sensors & 2) {
			setState(S_MOVE_RIGHT);
		}
		break;

	case S_MOVE_LEFT:
		if (sensors & 4) {
			setState(S_ON_LINE);
		}
		else if (sensors & 16) {
			setState(S_PIVOT_LEFT);
		}
		break;

	case S_PIVOT_LEFT:
		if (sensors & 8) {
			setState(S_MOVE_LEFT);
		}
		break;

	case S_ROTATE_LEFT:
		if (sensors > 0) {
			setState(S_ON_LINE);
		}
		break;

	case S_ROTATE_RIGHT:
		if (sensors > 0) {
			setState(S_ON_LINE);
		}
		break;


	}


}


void setState(int newState) {

	printSensors();

	switch (newState) {
	case S_START:
		setMotors(M_STOP);
		break;

	case S_ROTATE_LEFT:
		setMotors(M_ROTATE_LEFT);
		break;

	case S_ROTATE_RIGHT:
		setMotors(M_ROTATE_RIGHT);
		break;


	case S_OFF_LINE:
		if (rightLine) {
			setMotors(M_ROTATE_RIGHT);
		}
		else {
			setMotors(M_ROTATE_LEFT);
		}
		rightLine = false;
		leftLine = false;
		break;

	case S_ON_LINE:
		setMotors(M_FORWARD);
		leftLine = false;
		rightLine = false;
		break;

	case S_MOVE_RIGHT:
		setMotors(M_RIGHT);
		break;

	case S_MOVE_LEFT:
		setMotors(M_LEFT);
		break;

	case S_PIVOT_LEFT:
		setMotors(M_PIVOT_LEFT);
		break;

	case S_PIVOT_RIGHT:
		setMotors(M_PIVOT_RIGHT);
		break;

	}


	currentState = newState;
	Serial.println(stateNames[currentState]);

}

void readSensors() {
	// set individual values as well as the combined integer
	left1S = 0;
	right1S = 0;
	left2S = 0;
	right2S = 0;
	centreS = 0;
	sensors = 0;

	if (digitalRead(sensorL2) == LOW) {
		left2S = 1;
		sensors |= 16;
	}
	if (digitalRead(sensorL1) == LOW) {
		left1S = 1;
		sensors |= 8;
	}
	if (digitalRead(sensorC) == LOW) {
		centreS = 1;
		sensors |= 4;
	}
	if (digitalRead(sensorR1) == LOW) {
		right1S = 1;
		sensors |= 2;
	}
	if (digitalRead(sensorR2) == LOW) {
		right2S = 1;
		sensors |= 1;
	}

	//Serial.print(left2S);
	//Serial.print(left1S);
	//Serial.print(centreS);
	//Serial.print(right1S);
	//Serial.print(right2S);
	//Serial.println(sensors);

}

void setMotors(int newDirection) {

	switch (newDirection) {
	case M_STOP:
		digitalWrite(leftF, LOW);
		digitalWrite(leftB, LOW);
		digitalWrite(rightF, LOW);
		digitalWrite(rightB, LOW);
		analogWrite(leftP, 0);
		analogWrite(rightP, 0);
		break;

	case M_FORWARD:
		digitalWrite(leftF, HIGH);
		digitalWrite(leftB, LOW);
		digitalWrite(rightF, HIGH);
		digitalWrite(rightB, LOW);
		analogWrite(leftP, 250);
		analogWrite(rightP, 250);
		break;

	case M_RIGHT:
		digitalWrite(leftF, HIGH);
		digitalWrite(leftB, LOW);
		digitalWrite(rightF, HIGH);
		digitalWrite(rightB, LOW);
		analogWrite(leftP, 250);
		analogWrite(rightP, 150);
		break;

	case M_PIVOT_RIGHT:
		digitalWrite(leftF, HIGH);
		digitalWrite(leftB, LOW);
		digitalWrite(rightF, LOW);
		digitalWrite(rightB, LOW);
		analogWrite(leftP, 250);
		analogWrite(rightP, 0);
		break;

	case M_LEFT:
		digitalWrite(leftF, HIGH);
		digitalWrite(leftB, LOW);
		digitalWrite(rightF, HIGH);
		digitalWrite(rightB, LOW);
		analogWrite(leftP, 110);
		analogWrite(rightP, 250);
		break;

	case M_PIVOT_LEFT:
		digitalWrite(leftF, LOW);
		digitalWrite(leftB, LOW);
		digitalWrite(rightF, HIGH);
		digitalWrite(rightB, LOW);
		analogWrite(leftP, 0);
		analogWrite(rightP, 250);
		break;

	case M_ROTATE_RIGHT:
		digitalWrite(leftF, HIGH);
		digitalWrite(leftB, LOW);
		digitalWrite(rightF, LOW);
		digitalWrite(rightB, HIGH);
		analogWrite(leftP, 200);
		analogWrite(rightP, 200);
		break;

	case M_ROTATE_LEFT:
		digitalWrite(leftF, LOW);
		digitalWrite(leftB, HIGH);
		digitalWrite(rightF, HIGH);
		digitalWrite(rightB, LOW);
		analogWrite(leftP, 200);
		analogWrite(rightP, 200);
		break;


	default:
		digitalWrite(leftF, LOW);
		digitalWrite(leftB, LOW);
		digitalWrite(rightF, LOW);
		digitalWrite(rightB, LOW);
		analogWrite(leftP, 0);
		analogWrite(rightP, 0);
		break;

	}
}
