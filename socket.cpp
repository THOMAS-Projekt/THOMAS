/*
-- SOCKET-KLASSE :: IMPLEMENTIERUNG --
*/


/* INCLUDES */

// Klassenheader
#include "socket.h"
using namespace THOMAS;

// C++-IO-Stream-Klasse
#include <iostream>

// C++-String-Klasse
#include <string>

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

Socket::Socket(unsigned short port, ComputeReceivedDataFunction computeReceivedDataFunction)
{
	// Datenverarbeitungsfunktion merken
	_computeReceivedDataFunction = computeReceivedDataFunction;
	
	// Socket erstellen
	_socket = socket(AF_INET, SOCK_STREAM, 0);
	
	// Ist bei der Socketerstellung ein Fehler aufgetreten?
	if(_socket == -1)
	{
		// Exception ausl�sen
		throw std::string("Beim Erstellen des Server-Sockets ist ein Fehler aufgetreten!");
	}
	
	// Server-Adressstruktur erstellen
	sockaddr_in serverAddress;
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(port);
	serverAddress.sin_addr.s_addr = INADDR_ANY; // Adresse des Computers, auf dem der Server l�uft
	memset(&serverAddress.sin_zero, 0, 8); // Das letzte Element der Struktur muss auf 0 gesetzt sein
	
	// Socket binden
	if(bind(_socket, reinterpret_cast<sockaddr *>(&serverAddress), sizeof(serverAddress)) == -1)
	{
		// Ein Fehler ist aufgetreten, Exception ausl�sen
		throw std::string("Beim Binden des Server-Sockets ist ein Fehler aufgetreten!");
	}
	
	// Noch l�uft kein Listen-Vorgang
	_listening = false;
}

Socket::~Socket()
{
	
}

void Socket::BeginListen()
{
	// L�uft der Listen-Vorgang schon?
	if(_listening)
		throw std::string("Fehler: Der Listen-Vorgang ist bereits aktiv!");
	
	// Listen-Thread erstellen
	_listening = true;
	_listenThread = new std::thread(&Socket::ListenWrapper, this);
}

void Socket::EndListen()
{
	// L�uft �berhaupt ein Listen-Vorgang?
	if(!_listening)
		throw std::string("Fehler: Es ist kein Listen-Vorgang aktiv!");
	
	// Listen-Vorgang abbrechen, der Thread terminiert von selbst, der aktuelle Thread wird so lange angehalten
	_listening = false;
	_listenThread->join();
}

void Socket::Listen()
{
	// Client-Warteschlange erstellen (L�nge: 5)
	if(listen(_socket, 5) == -1)
	{
		// Fehler, Exception ausl�sen
		throw std::string("Konnte Client-Warteschlange nicht erstellen!");
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
			throw std::string("Der Client konnte nicht angenommen werden!");
		}
		
		// Daten in separatem Thread empfangen
		clientReceiveThreads.push_front(new std::thread(&Socket::ReceiveClientWrapper, this, clientSocket));
	}
	
	// Abwarten, bis alle Client-Receive-Threads beendet sind
	for(auto i = clientReceiveThreads.begin(); i != clientReceiveThreads.end(); ++i)
	{
		// Auf das Threadende warten
		(*i)->join();
		
		// Thread-Objekt vernichten
		delete *i;
	}
	
	// Server-Socket schlie�en
	close(_socket);
}

void Socket::ReceiveClient(int clientSocket)
{
	// Solange kein Abbruchbefehl kommt, Daten empfangen
	int dataLength;
	const int bufferLength = 256;
	BYTE *buffer = new BYTE[bufferLength];
	while(_listening)
	{
		// Daten empfangen, w�hrenddessen blockieren
		dataLength = recv(clientSocket, buffer, bufferLength, 0);
		
		// Ist ein Fehler aufgetreten?
		if(dataLength == -1)
		{
			// Nicht gut
			throw std::string("Fehler beim Empfangen von Client-Daten!");
		}
		
		// Ist die Verbindung abgebrochen?
		if(dataLength == 0)
		{
			// Empfangsfunktion abbrechen
			break;
		}
		
		// Daten verarbeiten
		_computeReceivedDataFunction(buffer, dataLength);
	}
	
	// Client-Socket schlie�en
	close(clientSocket);
}