#pragma once
/*
-- RS232-KLASSE :: HEADER --
Definiert die RS232-Klasse.
*/


/* INCLUDES */


/* KONSTANTEN */

// BYTE-Typ.
#define BYTE char

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
	class RS232
	{
	private:
		// Das interne RS232-Handle.
		int _handle;
		
		// Speichert die jeweilig letzten eingestellten Motorgeschwindigkeiten.
		// Inhalt:
		// [0]: MRIGHT.
		// [1]: MLEFT.
		int _lastSpeed[2];
		
		// Sendet die übergebenen Bytes an den RS232-Port und gibt bei Erfolg true zurück, bei Fehlern false.
		// Parameter:
		// -> com: Das zu übergebende Befehlsbyte.
		// -> params: Die zu übergebenden Parameter.
		bool Send(BYTE com, BYTE *params, int paramCount);
		
	public:
		// Konstruktor.
		// Stellt die Verbindung mit der Motorsteuerung her.
		RS232();
		
		// Destruktor.
		// Trennt die Verbindung mit der Motorsteuerung.
		~RS232();
		
		// Sendet den Geschwindigkeitsänderungsbefehl für den angegebenen Motor und gibt bei Erfolg true zurück, bei Fehlern false.
		// Der safeAcc-Parameter sorgt durch stufenweise Beschleunigung dafür, dass der Motor nicht plötzlich bei voller Fahrt
		// seine Drehrichtung ändert oder zu stark bremst / beschleunigt und dabei beschädigt wird.
		// -> motor: Der betroffene Motor (MRIGHT, MLEFT oder MBOTH).
		// -> speed: Die neue Motorgeschwindigkeit (-100 bis 100).
		bool SetMotorSpeed(int motor, int speed);
	};
}