/*
-- TCP-SERVER-KLASSE :: IMPLEMENTIERUNG --
*/


/* INCLUDES */

// Klassenheader
#include "TCPServer.h"
using namespace THOMAS;

// THOMASException-Klasse
#include "THOMASException.h"

// C++-IO-Stream-Klasse
#include <iostream>

// C++-Forward-List-Klasse
#include <forward_list>

// C-String-Funktionen
// In diesem Header ist u.a. die memset()-Funktion definiert.
#include <cstring>

// UNIX-Funktionen [Non-Standard]
// In diesem Header ist u.a. die close()-Funktion definiert.
#include <unistd.h>

// POSIX-Socket-Funktionen [Non-Standard]
#include <sys/socket.h>

// POSIX-Internet-Protokoll-Funktionen [Non-Standard]
#include <netinet/in.h>


/* FUNKTIONEN */

TCPServer::TCPServer(unsigned short port, ComputeReceivedDataFunction computeReceivedDataFunction, void *cRDFParams)
{
	// Datenverarbeitungsfunktion merken
	_computeReceivedDataFunction = computeReceivedDataFunction;
	_cRDFParams = cRDFParams;

	// Socket erstellen
	_socket = socket(AF_INET, SOCK_STREAM, 0);

	// Ist bei der Socketerstellung ein Fehler aufgetreten?
	if(_socket == -1)
	{
		// Exception auslösen
		throw THOMASException("Beim Erstellen des Server-Sockets ist ein Fehler aufgetreten!");
	}

	// Server-Adressstruktur erstellen
	sockaddr_in serverAddress;
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(port);
	serverAddress.sin_addr.s_addr = INADDR_ANY; // Adresse des Computers, auf dem der Server läuft
	memset(&serverAddress.sin_zero, 0, 8); // Das letzte Element der Struktur muss auf 0 gesetzt sein

	// Socket binden
	if(bind(_socket, reinterpret_cast<sockaddr *>(&serverAddress), sizeof(serverAddress)) == -1)
	{
		// Ein Fehler ist aufgetreten, Exception auslösen
		throw THOMASException("Beim Binden des Server-Sockets ist ein Fehler aufgetreten!");
	}
}

TCPServer::~TCPServer()
{
	// Listen-Thread ggf. beenden
	if(_listening)
		EndListen();
}

void TCPServer::BeginListen()
{
	// Läuft der Listen-Vorgang schon?
	if(_listening)
		throw THOMASException("Fehler: Der Listen-Vorgang ist bereits aktiv!");

	// Listen-Thread erstellen
	_listening = true;
	_listenThread = new std::thread(&TCPServer::ListenWrapper, this);
}

void TCPServer::EndListen()
{
	// Läuft überhaupt ein Listen-Vorgang?
	if(!_listening)
		throw THOMASException("Fehler: Es ist kein Listen-Vorgang aktiv!");

	// Listen-Vorgang abbrechen, der Thread terminiert von selbst, der aktuelle Thread wird so lange angehalten
	_listening = false;
	_listenThread->join();

	// Speicher freigeben
	delete _listenThread;
}

void TCPServer::Listen()
{
	// Client-Warteschlange erstellen (Länge: 5)
	if(listen(_socket, 5) == -1)
	{
		// Fehler, Exception auslösen
		throw THOMASException("Konnte Client-Warteschlange nicht erstellen!");
	}

	// Die einzelnen Client-Receive-Threads
	std::forward_list<std::thread *> clientReceiveThreads;

	// Auf Client warten und dessen Daten annehmen
	sockaddr_in clientAddress;
	socklen_t clientAddressLength = sizeof(clientAddress);
	int clientSocket;
	while(_listening)
	{
		// Auf Client warten
		clientSocket = accept(_socket, reinterpret_cast<sockaddr *>(&clientAddress), &clientAddressLength);

		// Wurde der Client nicht erfolgreich angenommen?
		if(clientSocket == -1)
		{
			// Fehler
			throw THOMASException("Der Client konnte nicht angenommen werden!");
		}

		// Daten in separatem Thread empfangen
		clientReceiveThreads.push_front(new std::thread(&TCPServer::ReceiveClientWrapper, this, clientSocket));
	}

	// Abwarten, bis alle Client-Receive-Threads beendet sind
	for(auto i = clientReceiveThreads.begin(); i != clientReceiveThreads.end(); ++i)
	{
		// Auf das Threadende warten
		(*i)->join();

		// Thread-Objekt vernichten
		delete *i;
	}

	// Server-Socket schließen
	close(_socket);
}

void TCPServer::ReceiveClient(int clientSocket)
{
	// Solange kein Abbruchbefehl kommt, Daten empfangen
	int dataLength;
	const int bufferLength = 256;
	BYTE *buffer = new BYTE[bufferLength];
	while(_listening)
	{
		// Daten empfangen, währenddessen blockieren
		dataLength = recv(clientSocket, buffer, bufferLength, 0);

		// Ist ein Fehler aufgetreten?
		if(dataLength == -1)
		{
			// Fehlermeldung ausgeben
			perror ("Fehler");

			// Nicht gut
			throw THOMASException("Fehler beim Empfangen von Client-Daten!");
		}

		// Ist die Verbindung abgebrochen?
		if(dataLength == 0)
		{
			// Empfangsfunktion abbrechen
			break;
		}

		// Daten verarbeiten
		_computeReceivedDataFunction(buffer, dataLength, _cRDFParams);
	}

	// Client-Socket schließen
	close(clientSocket);
}
