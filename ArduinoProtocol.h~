#pragma once
/*
-- ArduinoProtocol :: HEADER --
Definiert die ArduinoProtocol-Klasse
Diese Klasse enthält das Protokoll zur Kommunikation mit dem Arduino
*/


/* INCLUDES */

// ArduinoCom-Klasse
#include "ArduinoCom.h"

// C++-IO-Stream-Klasse
#include <iostream>

// C++ Thread-Klasse
#include <thread>

/* KONSTANTEN */

// BYTE-Typ.
#define BYTE char
#define UBYTE unsigned char

// Ultraschallsensoren
#define US_FRONT_RIGHT 0
#define US_FRONT_MIDDLE 1
#define US_FRONT_LEFT 2
#define US_BACK_RIGHT 3
#define US_BACK_LEFT 4

// Hardware-Zustände
#define HARDWARE_DEFEKT 0
#define HARDWARE_OK 1

// Kamera-Servos
#define CAM_FRONT 0

// Prioritäten
#define PRIORITY_INFO 0
#define PRIORITY_WARNING 1
#define PRIORITY_ERROR 2

/* KLASSE */
namespace THOMAS
{
	class ArduinoProtocol
	{
	private:
		// Das Kommunikations-Objekt
		ArduinoCom *arduinoCom;

		// ArduinoProtocol Thread		
		std::thread *arduinoProtocolThread;

		// Signalstrength Thread
		std::thread *signalStrengthThread;

		// Status des Threads
		bool running = false;

		// Wird beim Start eines neuen Threads aufgerufen
		void Setup();
		
		// Updatet die Signalstärke im speraten Thread
		void UpdateSignalStrength();

		
		static void ArduinoProtocolThreadWrapper(ArduinoProtocol *obj)
		{	
			obj->Setup();
		}

		static void SignalStrengthThreadWrapper(ArduinoProtocol *obj)
		{
			obj->UpdateSignalStrength();
		}

	public:
		// Konstruktor
		ArduinoProtocol();

		// Setze Text auf LCD
		void WriteMessage(std::string text, unsigned char priority);

		// Entfernung des Ultraschallsensors auslesen
		int GetDistance(unsigned char sensorID);

		// Sensorstatus abrufen und evtl. vorher aktualisieren
		int GetStatus(unsigned char sensorID, bool newRequest);

		// Position der Kamera setzen
		int SetCamPosition(unsigned char camera, unsigned char degree);

		// Kamera um einen bestimmten Wert drehen
		int ChangeCamPosition(unsigned char camera, unsigned char degree);

		// Funktion zum Anpingen des Arduinos (Bei einem Fehlschlag stoppt das Programm.)
		void Heartbeat();

		// Auf Neustart des Arduinos warten
		void WaitForArduino();

		// Die SSID des verbundenen Netzwerkes an den Arduino senden
		void SetCurrentSSID();

		// Die Signalstärke des verbundenen Netzwerkes an den Arduino senden
		void SetSignalStrength();

		// Startet einen neuen Thread
		void Run();

		// Stoppt den Thread
		void Stop();
	};
}
