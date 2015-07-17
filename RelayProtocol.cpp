/*
-- RelayProtocol-KLASSE :: IMPLEMENTIERUNG --
*/


/* INCLUDES */

// Klassenheader
#include "RelayProtocol.h"
using namespace THOMAS;

// Enthält Methoden für I/O
#include <cstdio>

// Utilities Klasse
#include <cstdlib>

// Enhält Methoden für Strings
#include <cstring>

// POSIX-Definition
#include <unistd.h>

// File-Control-Definitionen
#include <fcntl.h>

// Terminal-IO-Definitionen
#include <termios.h>

// IOStream Klasse
#include <iostream>

RelayProtocol::RelayProtocol()
{
	// Anschlussoptionen Struktur
	struct termios options;

	// Anschlusshandle erstellen
	_handle = open("/dev/ttyUSB1", O_RDWR | O_NOCTTY | O_NDELAY);

	// Anschluss-Flags leeren
	fcntl(_handle, F_SETFL, 0);

	// Aktuelle Anschluss-Optionen abrufen
	tcgetattr(_handle, &options);

	// Baundrate setzen (19200)
	cfsetispeed(&options, B19200); // Input
	cfsetospeed(&options, B19200); // Output

	options.c_cflag &= ~PARENB; // kein Partybit
	options.c_cflag &= ~CSTOPB; // 1 Stopbit
	options.c_cflag &= ~CSIZE; // 8 Datenbits
	options.c_cflag |= CS8;
	options.c_cflag |= (CLOCAL | CREAD); // CD Signal ignorieren

	options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
	options.c_oflag &= ~OPOST; // "raw" Input
	options.c_cc[VMIN]  = 0; // Auf 0 Zeichen warten
	options.c_cc[VTIME] = 10; // Timeout 10 Sekunden

	// Einstellungen flushen
	tcflush(_handle,TCIOFLUSH);

	tcsetattr(_handle, TCSAFLUSH, &options);

	fcntl(_handle, F_SETFL, FNDELAY);
	fcntl(_handle, F_SETFL, 0);

	// Relay initialisieren
	SendCommand(1, 1, 0);

	// Status lesen
	if(!ReceiveStatus())
		std::cout << "Fehler beim Initialisieren des Relays" << std::endl;
}

void RelayProtocol::SendCommand(UBYTE command, UBYTE address, UBYTE data)
{
	// Neues Buffer Array erstellen
	UBYTE buff[4];

	// Buffer füllen
	buff[0] = command;
	buff[1] = address;
	buff[2] = data;

	// Prüfsumme erstellen
	buff[3] = buff[0]^buff[1]^buff[2];

	// 50ms warten
	usleep(50000);

	// Daten absenden und ggf. Fehler ausgeben
	if(!write(_handle, buff, 4))
		std::cout << "Fehler beim Senden eines Statusbytes an das Relay!" << std::endl;
}

bool RelayProtocol::ReceiveStatus()
{
	// Neues Buffer Array erstellen
	UBYTE buff[4];

	// Daten einlesen
	read(_handle, buff, 4);

	// Prüfsumme berechnen
	int pruefsumme = buff[0]^buff[1]^buff[2];

	// Stimmen die Prüfsummen überein?
	if(pruefsumme != buff[3])
		return false;

	return true;
}

void RelayProtocol::SetRelay(int port, int status)
{
	// Port Value
	UBYTE val = -1;

	// Status prüfen
	switch (status)
	{
		// Relay ausschalten
		case OFF:
		{
			// Port in Binary umwandeln
			val = ~(1 << (port - 1));

			break;
		}

		// Relay anschalten
		case ON:
		{
			// Port in Binary umwandeln
			val = 1 << (port - 1);

			break;
		}
	}

	// Status senden
	SendCommand(3, 1, val);

	// Read-Buffer leeren
	ReceiveStatus();
}

void RelayProtocol::SetAll(int status)
{
	// Port Value
	UBYTE val = -1;

	// Status prüfen
	switch (status)
	{
		// Relay ausschalten
		case OFF:
		{
			// Port in Binary umwandeln
			val = 0;

			break;
		}

		// Relay anschalten
		case ON:
		{
			// Port in Binary umwandeln
			val = 255;

			break;
		}
	}

	// Status senden
	SendCommand(3, 1, val);

	// Read-Buffer leeren
	ReceiveStatus();
}
