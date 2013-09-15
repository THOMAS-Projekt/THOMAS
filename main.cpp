/*
-- HAUPTPROGRAMM --
*/


/* INCLUDES */

// MotorControl-Klasse
#include "MotorControl.h"
using namespace THOMAS;

// UNIX-Standard-Funktionen [Non-Standard]
// Die sleep()-Funktion wird benötigt.
#include <unistd.h>

/* FUNKTIONEN */

// Programm-Einstiegsfunktion.
int main(int argc, char **argv)
{
	// Motorsteuerung starten
	MotorControl *motorControl = new MotorControl();
	motorControl->Run();
	
	// Programm laufen lassen, Prozessor nicht unnötig belasten (alles läuft in separaten Threads)
	// TODO: Programm-Befehle per Tastatur etc.
	while(true)
		sleep(2);
}