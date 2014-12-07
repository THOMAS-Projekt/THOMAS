#pragma once
/*
-- ArduinoProtocol :: HEADER --
Definiert die ArduinoProtocol-Klasse
Diese Klasse enthält das Protocol zur Kommunikation mit dem Arduino
*/


/* INCLUDES */

// ArduinoCom-Klasse
#include "ArduinoCom.h"

// C++-IO-Stream-Klasse
#include <iostream>

/* KONSTANTEN */

// BYTE-Typ.
#define BYTE char

/* KLASSE */
namespace THOMAS
{
	class ArduinoProtocol
	{
	private:
		// Liste der Ultraschallsensoren
		enum USensor{FRONT_RIGHT,FRONT_MIDDLE,FRONT_LEFT,BACK_RIGHT,BACK_LEFT};

		// Liste der CamServos
		enum CamID{FRONT_CAM};

		//ArduinoCom Instanz
		ArduinoCom *arduinoCom;


	public:
		// Konstruktor
		ArduinoProtocol();

		// Setze Text auf LCD 
		int SetText(std::string text);

		// Entfernung des USensors auslesen
		int GetDistance(USensor us);

		// Status abrufen
		// Parameter:
		// newRequest = Soll der Sensorwert aktuallisiert werden? Dauert 0,25 Sekunden
		int GetStatus(USensor us,bool newRequest);

		// Setze die Servoposition
		int SetCamPosition(CamID camID, int grad);

		// Ändere Kameraposition um Grad
		int ChangeCamPosition(CamID camID, int grad);
		
	};
}
