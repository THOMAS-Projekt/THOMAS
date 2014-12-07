/*
-- HAUPTPROGRAMM --
*/


/* INCLUDES */

// MotorControl-Klasse
#include "MotorControl.h"
using namespace THOMAS;

// THOMASException-Klasse
#include "THOMASException.h"

// ArduinoCom-Klasse
#include "ArduinoCom.h"

// ArduinoProtocol-Klasse
#include "ArduinoProtocol.h"

// C++-iostream-Header
// Enthält die wichtigen Konsolen-Ein-/Ausgabe-Streams.
#include <iostream>

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

	//Arduino Kommunikation starten -> Nur zum developen
	//ArduinoCom *arduinoCom = new ArduinoCom();

	//ArduinoProtocol *arduinoProtocol = new ArduinoProtocol();

	// Programm laufen lassen, Prozessor nicht unnötig belasten (alles läuft in separaten Threads)
	// TODO: Programm-Befehle per Tastatur etc.
	while(true)
		sleep(2);

	// Programm erfolgreich beendet
	return 0;
}
