#pragma once
/*
-- MOTOR-CONTROL-KLASSE :: HEADER --
Definiert die MotorControl-Klasse.
Diese Klasse übernimmt die Netzwerkkommunikation mit dem Joy-Stick und steuert dadurch die Motoren.
Verzögerte Beschleunigungen werden eingesetzt, um abrupte Geschwindigkeitsänderungen abzufangen.
*/


/* INCLUDES */

// RS232-Klasse
#include "RS232.h"

// TCPServer-Klasse
#include "TCPServer.h"


/* KONSTANTEN */

// Definiert den in Fahrtrichtung rechten Motor.
// Die Array-Konstante dient der einfacheren Angabe eines Array-Elements des jeweiligen Motors.
#define MRIGHT 1
#define MRIGHT_ARR 0

// Definiert den in Fahrtrichtung linken Motor.
// Die Array-Konstante dient der einfacheren Angabe eines Array-Elements des jeweiligen Motors.
#define MLEFT 2
#define MLEFT_ARR 1

// Definiert beide Motoren.
#define MBOTH 3

// Drehrichtungen
#define FORWARDS 1
#define BACKWARDS 0


/* KLASSE */
namespace THOMAS
{
	class MotorControl
	{
	private:
		// Gibt an, ob die Steuerung aktiv ist.
		bool _running;
		
		// Die RS232-Verbindung zur Motorsteuerung.
		RS232 *_rs232;
		
		// Der TCP-Server.
		TCPServer *_server;
		
		// Speichert die jeweils letzten gesendeten Motorgeschwindigkeiten.
		// Hiermit werden die benötigten Drehrichtungswechsel-Befehle der Motoren realisiert.
		// Inhalt:
		// [0]: MRIGHT.
		// [1]: MLEFT.
		int _lastSpeed[2];
		
		// Sendet den Geschwindigkeitsänderungsbefehl für den angegebenen Motor.
		// Parameter:
		// -> motor: Der betroffene Motor (MRIGHT, MLEFT oder MBOTH).
		// -> speed: Die neue Motorgeschwindigkeit (-100 bis 100).
		void SendMotorSpeed(int motor, int speed);
		
		// Verarbeitet vom Server empfangene Steuerbefehle.
		// Parameter:
		// -> data: Die vom Server empfangenen Daten.
		// -> dataLength: Die Länge der vom Server empfangenen Daten.
		void ComputeClientCommand(BYTE *data, int dataLength);
		
		// Wrapper, um die ComputeClientCommand-Memberfunktion sauber an den TCP-Server zu übergeben
		// Parameter:
		// -> obj: Das zur ComputeClientCommand-Funktion gehörende MotorControl-Objekt.
		static void ComputeClientCommandWrapper(BYTE *data, int dataLength, void *obj)
		{
			(reinterpret_cast<MotorControl *>(obj))->ComputeClientCommand(data, dataLength);
		}
		
	public:
		// Konstruktor.
		// Erstellt eine neue Instanz der MotorControl-Klasse.
		MotorControl();
		
		// Destruktor.
		// Beendet alle noch offenen Verbindungen und gibt belegten Speicher frei.
		~MotorControl();
		
		// Startet die Motorsteuerung.
		void Run();
		
		// Beendet die Motorsteuerung.
		void Stop();
	};
}