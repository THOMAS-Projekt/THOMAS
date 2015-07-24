// IOStream Klasse includieren
#include <iostream>

// RS232 Klasse includieren
// Enthält die Steuerung für die MotorSteuerung
#include "RS232.h"

// Enthält usleep
#include <unistd.h>

#define MRIGHT 1
#define MLEFT 2

#define FORWARDS 1
#define BACKWARDS 0

using namespace THOMAS;

RS232 *_rs232;

/* FUNKTIONEN */
void SetSpeed(short speed);

// Main Methode - Wird beim Start aufgerufen
int main(int argc, const char* argv[])
{
	// Alle Parameter angegeben?
	if(argc < 2)
	{
		// Fehlermeldung ausgeben
		std::cout << "Fehlende Parameter: <Zeitintervall zwischen Änderung>" << std::endl;

		// Programm beenden
		return 0;
	}

	// RS232 initialisieren
	_rs232 = new RS232();

	// Beschleunigung simulieren
	for(int i = 0; i < 128; i++)
	{
		// Geschwindigkeit setzten
		SetSpeed(i);

		// Per Parameter angegebenen Zeitdelay warten
		usleep(std::stoi(argv[1]) *1000);
	}
}


// Setzt die Geschwindigkeit
void SetSpeed(short speed)
{
	// Das Parameter-Array
	BYTE params[2] = {0};

	/* LINKS */
	// Richtung "FORWARD" senden
	params[0] = MLEFT;
	params[1] = FORWARDS;
	_rs232->Send(5, params, 2);

	// Geschwindigkeit für den rechten Motor senden
	params[0] = MLEFT;
	params[1] = static_cast<BYTE>(abs(speed));
	_rs232->Send(2, params, 2);

	/* RECHTS */
	// Richtung "FORWARD" senden
	params[0] = MRIGHT;
	params[1] = FORWARDS;
	_rs232->Send(5, params, 2);

	// Geschwindigkeit für den linken Motor senden
	params[0] = MRIGHT;
	params[1] = static_cast<BYTE>(abs(speed));
	_rs232->Send(2, params, 2);
}
