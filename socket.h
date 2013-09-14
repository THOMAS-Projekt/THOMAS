#pragma once
/*
-- SOCKET-KLASSE :: HEADER --
Definiert die Socket-Klasse.
*/


/* INCLUDES */

// Thread-Klasse
#include <thread>


/* KONSTANTEN */

// BYTE-Typ.
#define BYTE char


/* KLASSE */
namespace THOMAS
{
	class Socket
	{
		// Definiert den Typ der Funktion, die die empfangenen Client-Daten verarbeitet.
		// Parameter:
		// -> [1]: Ein Puffer-Array mit den empfangenen Daten.
		// -> [2]: Die L�nge der empfangenen Daten im Puffer-Array beginnend bei Index 0. Der evtl. �brige Teil des Puffer-Arrays muss unter Umst�nden verworfen werden.
		typedef void (*ComputeReceivedDataFunction)(BYTE *, int);
	
	private:
		// Das Socket-Handle.
		int _socket;
		
		// Gibt an, ob der Listen-Vorgang l�uft.
		bool _listening;
		
		// Der interne Listen()-Thread.
		std::thread *_listenThread;
		
		// Die Funktion, die die empfangenen Client-Daten verarbeitet.
		Socket::ComputeReceivedDataFunction _computeReceivedDataFunction;
		
		// Wartet auf ankommende Clienten und empf�ngt dann deren Daten.
		// Parameter: Werden ignoriert.
		void Listen();
		
		// Wrapper, um die Listen-Memberfunktion sauber an einen separaten Thread zu �bergeben. Wird nur von BeginListen() benutzt.
		// Parameter:
		// -> obj: Das zur Listen-Funktion geh�rende Socket-Objekt.
		static void ListenWrapper(Socket *obj)
		{
			obj->Listen();
		}
		
		// Wrapper, um die ReceiveClient-Memberfunktion sauber an einen separaten Thread zu �bergeben. Wird nur von Listen() benutzt.
		// Parameter:
		// -> obj: Das zur ReceiveClient-Funktion geh�rende Socket-Objekt.
		// -> clientSocket: Das Socket-Handle des verbundenen Client.
		static void ReceiveClientWrapper(Socket *obj, int clientSocket)
		{
			obj->ReceiveClient(clientSocket);
		}
		
		// Stellt den Daten-Empfangsthread eines spezifischen Client dar. Wird nur von Listen() benutzt.
		// Parameter:
		// -> clientSocket: Das Socket-Handle des verbundenen Client.
		void ReceiveClient(int clientSocket);
		
	public:
		// Konstruktor.
		// Erstellt eine neue Socket-Instanz (= Server) mit dem angegebenen Port.
		// Parameter:
		// -> port: Der Port, an dem auf Clienten gewartet werden soll.
		// -> computeReceivedDataFunction: Die Funktion, die die empfangenen Client-Daten verarbeitet.
		Socket(unsigned short port, ComputeReceivedDataFunction computeReceivedDataFunction);
		
		// Destruktor.
		// Trennt alle noch bestehenden Verbindungen und beendet den Server.
		~Socket();
		
		// Startet das asynchrone Warten auf Clienten.
		void BeginListen();
		
		// Beendet das asynchrone Warten auf Clienten.
		void EndListen();
	};
}