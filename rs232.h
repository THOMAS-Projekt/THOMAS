#pragma once
/*
-- RS232-KLASSE :: HEADER --
Definiert die RS232-Klasse.
Diese Klasse übernimmt die direkte Low-Level-Kommunikation mit der Motorsteuerung am RS232-Port.
*/


/* INCLUDES */


/* KONSTANTEN */

// BYTE-Typ.
#define BYTE char


/* KLASSE */
namespace THOMAS
{
	class RS232
	{
	private:
		// Das interne RS232-Handle.
		int _handle;
		
	public:
		// Konstruktor.
		// Stellt die Verbindung mit der Motorsteuerung her.
		RS232();
		
		// Destruktor.
		// Trennt die Verbindung mit der Motorsteuerung.
		~RS232();
		
		// Sendet die übergebenen Bytes an den RS232-Port und gibt bei Erfolg true zurück, bei Fehlern false.
		// Parameter:
		// -> com: Das zu übergebende Befehlsbyte.
		// -> params: Die zu übergebenden Parameter.
		void Send(BYTE com, BYTE *params, int paramCount);
	};
}