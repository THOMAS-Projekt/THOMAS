#pragma once
/*
-- UDPClient:: HEADER --
UDP-Klasse
Enthält u.a. Funktionen zum erstellen von UDPClients
*/

// Enhält das Vector Element
#include <vector>

// Enhält IP Strukturen
#include <arpa/inet.h>

#define BYTE char

namespace THOMAS {

	class UDPClient
	{
	private:
		// Socket ID
		int _socket;

		// Port
		int _port;

		// IP des Clients
		const char* _ip;

		// Frame Größe in Prozent
		int _frameSize = 50;

		// Frame Qualität
		int _frameQuality = 30;

		// Klassen-Objekt
		void *_cRDFParams;

		// Serveradress-Struktur
		struct sockaddr_in serverAddress;

	public:
		// Konstruktor
		UDPClient();

		// Example Funktion
		void CreateUDPClient(int port);

		// An Server senden
		void Send(std::vector<unsigned char> buff);

		/* Getter und Setter */

		// Setzt die Frame Größe
		void SetFrameSize(int value);

		// Setzt die Frame Qualität
		void SetFrameQuality(int value);

		// Setzt die IP
		void SetIP(const char* ip);

		// Gibt die Frame Größe zurück
		int GetFrameSize();

		// Gibt die Frame Qualität zurücl
		int GetFrameQuality();
	};
}
