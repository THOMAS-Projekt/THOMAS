/*
-- HAUPTPROGRAMM --
*/


/* INCLUDES */

// MotorControl-Klasse
#include "MotorControl.h"
using namespace THOMAS;


/* FUNKTIONEN */

// Programm-Einstiegsfunktion.
int main(int argc, char **argv)
{
	// Motorsteuerung starten
	MotorControl *motorControl = new MotorControl();
	motorControl->Run();
	
	while(true);
}