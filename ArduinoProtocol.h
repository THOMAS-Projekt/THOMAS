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

// C++-mutex-Klasse
#include <mutex>

// Vector Klasse
#include <vector>

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

		// Signalstrength Thread
		std::thread *signalStrengthThread;

		// Heartbeat Thread
		std::thread *heartbeatThread;

		// ArduinoMutex
		std::mutex *arduinoMutex;

		// SSID -> Standard Wert setzen
		std::string _SSID = "undefined";

		int _signalStrength = -1;

		// Status des Threads
		bool running = false;

		// Updatet die Signalstärke im speraten Thread
		void UpdateSignalStrength();

		// Heartbeat Thread
		void SendHeartbeat();

		// Wrapper, um die UpdateSignalStrength-Memberfunktion sauber an einen separaten Thread zu übergeben
		static void SignalStrengthThreadWrapper(ArduinoProtocol *obj)
		{
			obj->UpdateSignalStrength();
		}

		// Wrapper, um die SendHeartbeat-Memberfunktion sauber an einen separaten Thread zu übergeben
		static void SendHeartbeatThreadWrapper(ArduinoProtocol *obj)
		{
			obj->SendHeartbeat();
		}

	public:
		// Konstruktor
		ArduinoProtocol();

		// Setze Text auf LCD
		void WriteMessage(std::string text, unsigned char priority);

		// Messwerte der Ultraschallsensoren abrufen
		std::vector<int> GetDistance();

		// Position der Kamera setzen
		int SetCamPosition(unsigned char camera, unsigned char degree);

		// Kamera um einen bestimmten Wert drehen
		int ChangeCamPosition(unsigned char camera, int degree);

		// Funktion zum Anpingen des Arduinos (Bei einem Fehlschlag stoppt das Programm.)
		void Heartbeat();

		// Auf Neustart des Arduinos warten
		void WaitForArduino();

		// Sendet den Betriebsmodus
		void SendMode(bool minimalMode);

		// Die SSID des verbundenen Netzwerkes an den Arduino senden
		void SetCurrentSSID();

		// Die Signalstärke des verbundenen Netzwerkes an den Arduino senden
		void SetSignalStrength();

		// Gibt die SSID zurück
		std::string GetSSID();

		// Gibt die Signalstärke zurück
		int GetSignalStrength();

		// Gibt die aktuelle Bandbreite zurücl
		std::string GetBandwidth();

		// Startet einen neuen Thread
		void Run(bool minimalMode = false);

		// Stoppt den Thread
		void Stop();
	};
}
