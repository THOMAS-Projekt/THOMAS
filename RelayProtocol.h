#pragma once
/*
-- RelayProtocol-Klasse :: HEADER --
Definiert die RelayProtocol-Klasse
Diese Klasse übernimmt die direkte Steuerung des Relays
*/


/* INCLUDES */


/* KONSTANTEN */

// BYTE-Typ.
#define BYTE char
#define UBYTE unsigned char

#define OFF 0
#define ON 1

/* KLASSE */
namespace THOMAS
{

	class RelayProtocol
	{
	private:
		// USB-Handle
		int _handle;

		void SendCommand(UBYTE command, UBYTE address, UBYTE data);

		bool ReceiveStatus();

	public:
		// Konstruktor
		// Stellt die Verbindung zum Relay her und initialisiert diese
		RelayProtocol();

		// Setzt den Status eines Ports
		void SetRelay(int port, int status);

		// Setzt alle Ports auf den jeweiligen Status
		void SetAll(int status);
	};
}
