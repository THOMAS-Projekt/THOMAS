/*
-- ArduinoProtocol-KLASSE :: IMPLEMENTIERUNG --
*/

/* INCLUDES */

// Klassenheader
#include "CollisionDetection.h"

//Arduino Protokoll Klasse
#include "ArduinoProtocol.h"

// Vector Klasse
#include <vector>

using namespace THOMAS;

/* FUNKTIONEN */

CollisionDetection::CollisionDetection(ArduinoProtocol *arduinoProtocol)
{
	// Arduino Protocol Klasse speichern.
	_arduinoProtocol = arduinoProtocol;
}

std::vector<short> CollisionDetection::CorrectWantedSpeed(short wantedSpeed[])
{
	// Neuen Vector mit dem Speed erstellen
	std::vector<short> newWantedSpeed (2);

	// Ist vorne frei?
	if(_arduinoProtocol->GetRealDistance(US_FRONT_MIDDLE, _tolerance) > _warnDistance)
	{
		// Geradeaus weiter fahren
		newWantedSpeed.at(0) = wantedSpeed[0];
		newWantedSpeed.at(1) = wantedSpeed[1];

		return newWantedSpeed;
	}

	// Ist Links frei
	if(_arduinoProtocol->GetRealDistance(US_FRONT_LEFT, _tolerance) > _warnDistanceRL)
	{
		// Nach links fahren
		newWantedSpeed.at(0) = 0;
		newWantedSpeed.at(1) = 32767;

		return newWantedSpeed;
	}

	// Ist Rechts frei
	if(_arduinoProtocol->GetRealDistance(US_FRONT_RIGHT, _tolerance) > _warnDistanceRL)
	{
		// Nach rechts fahren
		newWantedSpeed.at(0) = 32767;
		newWantedSpeed.at(1) = 0;

		return newWantedSpeed;

	}

	// Anhalten
	newWantedSpeed.at(0) = 0;
	newWantedSpeed.at(1) = 0;

	// OsterEi: THOMAS Dead Sound -> Wer hat da eine Tür hingestellt?
	system("beep -f 523 -l 38 -D 2 -n -f 554 -l 34 -D 2 -n -f 587 -l 28 -D 489 -n -f 195 -l 20 -D 0 -n -f 391 -l 20 -D 0 -n -f 493 -l 20 -n -f 195 -l 20 -D 0 -n -f 391 -l 20 -D 0 -n -f 493 -l 20 -D 0 -n -f 587 -l 20 -D 0 -n -f 698 -l 20 -n -f 587 -l 20 -D 0 -n -f 698 -l 20 -n -f 587 -l 20 -D 0 -n -f 698 -l 20 -D 145 -n -f 195 -l 20 -D 0 -n -f 587 -l 20 -D 0 -n -f 698 -l 20 -n -f 195 -l 20 -D 0 -n -f 587 -l 20 -D 0 -n -f 698 -l 20 -D 0 -n -f 195 -l 20 -D 0 -n -f 587 -l 20 -D 0 -n -f 698 -l 20 -D 99 -n -f 220 -l 20 -D 0 -n -f 523 -l 20 -D 0 -n -f 659 -l 20 -D 99 -n -f 246 -l 20 -D 0 -n -f 493 -l 20 -D 0 -n -f 587 -l 20 -D 99 -n -f 261 -l 20 -D 0 -n -f 391 -l 20 -D 0 -n -f 523 -l 20 -n -f 261 -l 20 -D 0 -n -f 391 -l 20 -D 0 -n -f 523 -l 20 -D 0 -n -f 329 -l 148 -D 0 -n -f 195 -l 147 -D 0 -n -f 329 -l 147 -D 0 -n -f 130 -l 20 -D 0 -n -f 261 -l 20 -n -f 130 -l 20 -D 0 -n -f 261 -l 20 -n -f 130 -l 20 -D 0 -n -f 261 -l 20 &");

	return newWantedSpeed;

}


