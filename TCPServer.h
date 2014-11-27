#pragma once
/*
-- TCP-SERVER-KLASSE :: HEADER --
Definiert die TCPServer-Klasse.
Diese Klasse stellt einen TCP-Server an einem angegebenen Port dar, der Daten von einer beliebig großen Menge an Clients empfangen und an eine Verarbeitungsfunktion weiterreichen kann.
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
	class TCPServer
	{
		// Definiert den Typ der Funktion, die die empfangenen Client-Daten verarbeitet.
		// Parameter:
		// -> [1]: Ein Puffer-Array mit den empfangenen Daten.
		// -> [2]: Die Länge der empfangenen Daten im Puffer-Array beginnend bei Index 0. Der evtl. übrige Teil des Puffer-Arrays muss unter Umständen verworfen werden.
		// -> [3]: Weitere benutzerdefinierte Parameter, die an die Funktion übergeben werden sollen.
		typedef void (*ComputeReceivedDataFunction)(BYTE *, int, void *);

	private:
		// Das Socket-Handle.
		int _socket;

		// Gibt an, ob der Listen-Vorgang läuft.
		bool _listening = false;

		// Der interne Listen()-Thread.
		std::thread *_listenThread;

		// Die Funktion, die die empfangenen Client-Daten verarbeitet.
		ComputeReceivedDataFunction _computeReceivedDataFunction;

		// Die an die Verarbeitungsfunktion zu übergebenden benutzerdefinierten Parameter.
		void *_cRDFParams;

		// Wartet auf ankommende Clienten und empfängt dann deren Daten.
		void Listen();

		// Wrapper, um die Listen-Memberfunktion sauber an einen separaten Thread zu übergeben. Wird nur von BeginListen() benutzt.
		// Parameter:
		// -> obj: Das zur Listen-Funktion gehörende TCPServer-Objekt.
		static void ListenWrapper(TCPServer *obj)
		{
			obj->Listen();
		}

		// Wrapper, um die ReceiveClient-Memberfunktion sauber an einen separaten Thread zu übergeben. Wird nur von Listen() benutzt.
		// Parameter:
		// -> obj: Das zur ReceiveClient-Funktion gehörende TCPServer-Objekt.
		// -> clientSocket: Das Socket-Handle des verbundenen Client.
		static void ReceiveClientWrapper(TCPServer *obj, int clientSocket)
		{
			obj->ReceiveClient(clientSocket);
		}

		// Stellt den Daten-Empfangsthread eines spezifischen Client dar. Wird nur von Listen() benutzt.
		// Parameter:
		// -> clientSocket: Das Socket-Handle des verbundenen Client.
		void ReceiveClient(int clientSocket);

	public:
		// Konstruktor.
		// Erstellt eine neue TCPServer-Instanz mit dem angegebenen Port.
		// Parameter:
		// -> port: Der Port, an dem auf Clienten gewartet werden soll.
		// -> computeReceivedDataFunction: Die Funktion, die die empfangenen Client-Daten verarbeitet.
		// -> cRDFParams: Die Parameter, die zusätzlich an computeReceivedDataFunction übergeben werden sollen.
		TCPServer(unsigned short port, ComputeReceivedDataFunction computeReceivedDataFunction, void *cRDFParams);

		// Destruktor.
		// Trennt alle noch bestehenden Verbindungen und beendet den Server.
		~TCPServer();

		// Startet das asynchrone Warten auf Clienten.
		void BeginListen();

		// Beendet das asynchrone Warten auf Clienten.
		void EndListen();
	};
}
