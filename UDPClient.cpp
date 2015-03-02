/*
-- UDPClient-KLASSE :: IMPLEMENTIERUNG --
*/

/* INCLUDES */

// Klassenheader
#include "UDPClient.h"
using namespace THOMAS;

// THOMAS-Exception Klasse
#include "THOMASException.h"

// IOStream-Klasse enhält u.a. Ein/Ausgabe Funktionen
#include <iostream>

// Socket-Klasse
#include <sys/socket.h>

// Enthält Strukturen für Sockets
#include <netinet/in.h>

// Enhält IP Strukturen
#include <arpa/inet.h>

// Netzwerk-Datenbank-Definitionen [Non-Standard]
// Hier wird u.a. die hostent-Struktur definiert, sowie die gethostbyname()-Funktion.
#include <netdb.h>

// String-Klasse
// Ehthält u.a. die memcpy Funktion
#include <string.h>

/* FUNKTIONEN */

UDPClient::UDPClient()
{

}

void UDPClient::Send(std::vector<unsigned char> buff)
{
	// Buffer Array erstellen
	BYTE data[buff.size()];

	// Daten in C-Array kopieren
	std::copy(buff.begin(), buff.end(), data);

	// Daten an UDP-Server senden
	sendto(_socket,data,buff.size(),0,(struct sockaddr *)&serverAddress,sizeof(serverAddress));
}

void UDPClient::CreateUDPClient(int port)
{
	// Status ausgeben
	std::cout << "\033[32m" << "[Status]" << " UDPClient erfolgreich erstellt! IP: " << _ip << " Port: " << port << "\033[0m" << std::endl;

	// Neuen Socket erstellen
	_socket = socket(AF_INET,SOCK_DGRAM,0);

	// Serverdaten abrufen
	hostent *serverData = gethostbyname(_ip);

	// Informationen übergeben
	// AdressenProtokoll definieren
	serverAddress.sin_family = AF_INET;

	// Port definieren
	serverAddress.sin_port = htons(port);

	// Die IP in die serverAddress Struktur kopieren
	memcpy(reinterpret_cast<char *>(&serverAddress.sin_addr.s_addr), reinterpret_cast<char *>(serverData->h_addr_list[0]), serverData->h_length);

	// Alles ok?
	if(_socket == -1)
	{
		// Nicht gut!
		throw THOMASException("Beim erstellen des Sockets ist ein Fehler aufgetreten");
	}
}

// Getter & Setter

void UDPClient::SetFrameSize(int value)
{
	_frameSize = value;
}

void UDPClient::SetFrameQuality(int value)
{
	_frameQuality = value;
}

void UDPClient::SetIP(const char* ip)
{
	_ip = ip;
}

int UDPClient::GetFrameSize()
{
	return _frameSize;
}

int UDPClient::GetFrameQuality()
{
	return _frameQuality;
}
