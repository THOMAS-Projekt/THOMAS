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


void CollisionDetection::UpdateUSensorData()
{
	// Wiederhole immer
	while(true)
	{
		// Durch die Sensoren iterieren
		for(int sensorID = 0; sensorID < SENSOR_COUNT; sensorID++)
		{
			// Messwert abrufen und speichern
			int readDistance = _arduinoProtocol->GetRealDistance(sensorID, _tolerance);

			// Zugriff von anderen Threads sperren
			mutex.lock();

			// Aktuellen Messwert in das Array an entsprechende Stelle kopieren
			USensorMessurements.at(sensorID) = readDistance;

			// Zugriff von anderen Threads erlauben
			mutex.unlock();
		}
	}
}

std::vector<short> CollisionDetection::CorrectWantedSpeed(short wantedSpeed[])
{
	// Neuen Vector mit dem Speed erstellen
	std::vector<short> newWantedSpeed (2);

	// Ist vorne frei?
	if(USensorMessurements.at(US_FRONT_MIDDLE) > _warnDistance)
	{
		// Geradeaus weiter fahren
		newWantedSpeed.at(0) = wantedSpeed[0];
		newWantedSpeed.at(1) = wantedSpeed[1];

		return newWantedSpeed;
	}

	// Ist Links frei
	if(USensorMessurements.at(US_FRONT_LEFT) > _warnDistanceRL)
	{
		// Nach links fahren
		newWantedSpeed.at(0) = 0;
		newWantedSpeed.at(1) = 32767;

		return newWantedSpeed;
	}

	// Ist Rechts frei
	if(USensorMessurements.at(US_FRONT_RIGHT) > _warnDistanceRL)
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


