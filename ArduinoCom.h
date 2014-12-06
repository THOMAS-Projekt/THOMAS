#pragma once
/*
-- ArduinoCom-KLASSE :: HEADER --
Definiert die ArduinoCom-Klasse.
Diese Klasse übernimmt die direkte Low-Level-Kommunikation mit dem Arduino für das Sensorsystem.
*/


/* INCLUDES */


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

	public:
		// Konstruktor.
		// Stellt die Verbindung mit dem Arduino her.
		ArduinoCom();

		// Destruktor.
		// Trennt die Verbindung mit dem Arduino.
		~ArduinoCom();

		// Sendet die übergebenen Bytes an den USB-Port und gibt bei Erfolg true zurück, bei Fehlern false.
		// Parameter:
		// -> com: Das zu übergebende Befehlsbyte.
		// -> params: Die zu übergebenden Parameter.
		void Send(BYTE *package, int paramCount);

		// Empfängt die BYTES aus dem USB Port
		void Receive();
	};
}
