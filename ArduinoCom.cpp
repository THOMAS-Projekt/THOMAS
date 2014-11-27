/*
-- ArduinoCom-KLASSE :: IMPLEMENTIERUNG --
*/


/* INCLUDES */

// Klassenheader
#include "ArduinoCom.h"
using namespace THOMAS;

// THOMASException-Klasse
#include "THOMASException.h"

// C++-IO-Stream-Klasse
#include <iostream>

// Terminal-IO-Definitionen [Non-Standard]
// In diesem Header wird u.a. die termios-Struktur definiert.
#include <termios.h>

// POSIX-Definitionen [Non-Standard]
// Dieser Header enthält u.a. Definitionen für viele POSIX-Systemaufrufe.
#include <unistd.h>

// File-Control-Definitionen [Non-Standard]
// Dieser Header erhält u.a. die Definition des open()-Systemaufrufs.
#include <fcntl.h>


/* FUNKTIONEN */

ArduinoCom::ArduinoCom()
{
	// Anschlusshandle erstellen, RS232-Anschluss (ttyACM0) laden
	_handle = open("/dev/ttyACM0", O_RDWR | O_NOCTTY | O_NDELAY);

	// Fehler abfangen
	if(_handle == -1) {
		// Fehler
		throw THOMASException("Fehler: Es konnte keine Verbindung zum Arduino hergestellt werden!");
	}

	// Anschluss-Flags leeren
	fcntl(_handle, F_SETFL, 0);

	// Anschluss-Optionen erstellen
	termios fOpt;

	// Aktuelle Anschluss-Optionen abrufen
	tcgetattr(_handle, &fOpt);

	// Baudrate setzen (9600);
	cfsetispeed(&fOpt, B9600); // Input
	cfsetospeed(&fOpt, B9600); // Output

	// TODO: Daten empfangen?
	/*
	// Flags setzen
	fOpt.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
	fOpt.c_cflag &= ~PARENB;
	fOpt.c_cflag &= ~CSTOPB;
	fOpt.c_cflag &= ~CSIZE;
	fOpt.c_cflag |= CS8;
	fOpt.c_cc[VMIN] = 1;
	fOpt.c_cc[VTIME] = 0;
	fOpt.c_cflag |= (CLOCAL | CREAD);
	*/

	// Neue Port-Optionen einsetzen
	tcsetattr(_handle, TCSANOW, &fOpt);
}

ArduinoCom::~ArduinoCom()
{
	// Verbindung schließen
	close(_handle);
}

void ArduinoCom::Send(BYTE com, BYTE *params, int paramCount)
{
	// Befehlsarray erstellen (1 Kommandobyte + Parameter)
	BYTE *data = new BYTE[1 + paramCount];

	// Das Anfrage Byte an erste stelle setzten
	data[0] = com;

	// Parameterbytes zuweisen
	int i;
	for(i = 0; i < paramCount; i++)
	{
		data[i + 1] = params[i];
	}

	// Anzahl der gesendeten Daten merken
	int n = write(_handle, data, paramCount + 1);

	// Anzahl der gesendeten Daten pruefen
	if(n < (paramCount + 1))
	{
		// Mist
		throw THOMASException("Fehler beim Senden eines Befehls!");
	}

	// Speicher freigeben
	delete[] data;
}
