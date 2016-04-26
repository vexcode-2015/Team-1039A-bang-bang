#ifndef flywheel_c
#define flywheel_c

void Beep() {							//plays a beeping sound to help with ball loading times for full court shots
	if(time1[T3] > 697) {
		playTone(1500, 4);
		time1[T3] = 0;
	}
}

#define FULLCOURT 1
#define HALFCOURT 2
#define FRONTCOURT 3
#define OFF 0

int maxError,
		tv,
		divisor = 1,
		error,
		Full = 1;

float flywheelMP;

volatile float batMul;

void UpdatePIDValues(int pwr) {
	if(pwr == FULLCOURT) {						// update all the flywheel variables for full court shots
		tv = 155;
		maxError = 750;
		divisor = 16;
		batMul = 0.18;
		Full = 8;
	}
	if(pwr == HALFCOURT) {						// update all the flywheel variables for half court shots
		tv = 128;
		maxError = 280;
		divisor = 45;
		batMul = 0.3;
		Full = 1;
	}
	if(pwr == FRONTCOURT) {						// update all the flywheel variables for front court shots
		tv = 108;
		maxError = 90;
		divisor = 500;
		batMul = 0.36;
		Full = 1;
	}
	if(pwr == OFF) {									// just turn off the flywheel
		tv = 0;
		flywheelMP = 0;
		error = 0;
		SensorValue[En1] = 0;
		Full = 1;
	}
}

void UpdateFlywheel() {
	if(time1[T2] <= 50) return;
	if(tv != 0) {
		int batteryLevel = nAvgBatteryLevel / divisor;
		int capError = (maxError - batteryLevel) * batMul;	//caperror = power level motors have to be based on battery voltage to shoot at constant didtance regardless of voltage (9v & 7v shoot same distance)
		error = (tv - SensorValue[En1])/ Full;							//calculates error fo flywheel code. Full determines the magnitude of which the error will affect the code
		if(error > 75) flywheelMP = (flywheelMP +1) * 1.2 ; //gradually speeds up flywheel if it is > 75 error
		else if(error >= 3) flywheelMP = capError + 80;			//puts flywheel to full power when error is > 3
		else if(error == 1 || error == 2) flywheelMP = capError + 40;	//puts flywheel at medium speed faster than coast speed
		else if(error < 1 && error > -1) flywheelMP = capError + 15;	//puts flwheel at coast speed
		else if(error <= -1) flywheelMP = capError;										//puts flywheel below coast speed
	}
	motor[fly1] = flywheelMP;
	motor[fly2] = flywheelMP;
	motor[fly3] = flywheelMP;
	motor[fly4] = flywheelMP;
	SensorValue[En1] = 0;
	time1[T2] = 0;
}

void FlywheelController() {
	if(vexRT[Btn8U] == 1) UpdatePIDValues(FULLCOURT);
	if(vexRT[Btn8L] == 1) UpdatePIDValues(HALFCOURT);
	if(vexRT[Btn8R] == 1) UpdatePIDValues(FRONTCOURT);
	if(vexRT[Btn8D] == 1) UpdatePIDValues(OFF);
	UpdateFlywheel();
	if(tv == 155) Beep();
}

#endif
