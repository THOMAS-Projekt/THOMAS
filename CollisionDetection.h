#pragma once
/*
-- CollisionDetection :: HEADER --
Definiert die CollisionDetection-Klasse
Enhält die Hindernissumfahrung
*/


/* INCLUDES */

// Arduino Klasse
#include "ArduinoProtocol.h"

// Vector-Klasse
#include <vector>

/* KONSTANTEN */

// Ultraschallsensoren
#define US_FRONT_RIGHT 0
#define US_FRONT_MIDDLE 1
#define US_FRONT_LEFT 2
#define US_BACK_RIGHT 3
#define US_BACK_LEFT 4

/* KLASSE */
namespace THOMAS
{
	class CollisionDetection
	{
	private:

		// ArduinoProtocol Klasse
		ArduinoProtocol *_arduinoProtocol;

		// Ab wann soll die Hindernisserkennung eingreifen (in cm)
		int _warnDistance = 50;

		// Ab wann soll die Hindernisserkennung eingreifen - Rechte und Linke Sensoren (in cm)
		int _warnDistanceRL = 100;

		// Die Tolleranz der Messungen
		int _tolerance = 10;

	public:
		// Konstruktor
		CollisionDetection(ArduinoProtocol *arduinoProtocol);

		// Prüft die Distanz und korrigiert die Motorgeschwindigkeit
		std::vector<short> CorrectWantedSpeed(short wantedSpeed[]);
	};
}
