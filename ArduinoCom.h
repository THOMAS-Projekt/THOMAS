#pragma once
/*
-- ArduinoCom-KLASSE :: HEADER --
Definiert die ArduinoCom-Klasse.
Diese Klasse übernimmt die direkte Low-Level-Kommunikation mit dem Arduino für das Sensorsystem.
*/


/* INCLUDES */

#include <iostream>

/* KONSTANTEN */

// BYTE-Typ.
#define BYTE char

/* KLASSE */
namespace THOMAS
{
	class ArduinoCom
	{
	private:
		// Das interne USB-Handle.
		int _handle;

		// Das Buffer Array: Bearbeiten für größere Daten, die der Arduino sendet
		BYTE *buffer;

	public:
		// Konstruktor.
		// Stellt die Verbindung mit dem Arduino her.
		ArduinoCom();

		// Destruktor.
		// Trennt die Verbindung mit dem Arduino.
		~ArduinoCom();

		// Sendet die übergebenen Bytes an den USB-Port und gibt bei Erfolg true zurück, bei Fehlern false.
		// Parameter:
		// -> package: Die zu übergebenden Parameter.
		void Send(BYTE *package, char packageLength);

		// Empfängt die BYTES aus dem USB Port und gibt die Daten zurück
		BYTE* Receive();

		// Eine Hilfsfunktion zum Abrufen der Rückgabe eines Befehles (Wird z.B. zum Ermitteln der SSID benötigt.)
		// TODO: In eine eigene Utils-Klasse verschieben
		std::string Exec(std::string cmd);
	};
}
