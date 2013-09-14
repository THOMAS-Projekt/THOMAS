/*
-- RS232-KLASSE :: IMPLEMENTIERUNG --
*/


/* INCLUDES */

// Klassenheader
#include "rs232.h"
using namespace THOMAS;

// C++-IO-Stream-Klasse
#include <iostream>

// C++-String-Klasse
#include <string>

// C-Standardbibliothek
// Die abs()-Funktion wird hier benötigt.
#include <cstdlib>

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

RS232::RS232()
{
	// Anschlusshandle erstellen, RS232-Anschluss (ttyS0) laden
	_handle = open("/dev/ttyS0", O_RDWR | O_NOCTTY | O_NDELAY);
	
	// Fehler abfangen
	if(_handle == -1)
	{
		// Fehler
		throw std::string("Fehler: Es konnte keine Verbindung zum RS232-Port hergestellt werden!");
	}
	
	// Anschluss-Flags leeren
	fcntl(_handle, F_SETFL, 0);
	
	// Anschluss-Optionen erstellen
	termios fOpt;
	
	// Aktuelle Anschluss-Optionen abrufen
	tcgetattr(_handle, &fOpt);
	
	// Baudrate setzen (9600); TODO: Zu Testzwecken auf 1200
	cfsetispeed(&fOpt, B9600); // Input
	cfsetospeed(&fOpt, B9600); // Output
	
	// TODO: Daten empfangen??
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

RS232::~RS232()
{
	// Verbindung schliessen
	close(_handle);
}

bool RS232::Send(BYTE com, BYTE *params, int paramCount)
{
	// Befehlsarray erstellen (3 Sonderbytes + Kommandobyte + Parameter)
	BYTE *data = new BYTE[4 + paramCount];
	
	// Die beiden Steuerbytes setzen
	data[0] = 35;
	data[1] = 35;

	// Laengenbyte setzen
	data[2] = (paramCount + 1);

	// Kommandobyte setzen
	data[3] = com;

	// Parameterbytes zuweisen
	int i;
	for(i = 0; i < paramCount; i++)
	{
		data[i + 4] = params[i];
	}

	// Anzahl der gesendeten Daten merken
	int n = write(_handle, data, paramCount + 4);

	// Anzahl der gesendeten Daten pruefen
	if(n < (paramCount + 4))
	{
		// Mist
		throw std::string("Fehler beim Senden eines Befehls!");
	}
	
	// Speicher freigeben
	delete[] data;

	// Alles in Ordnung
	return true;
}

bool RS232::SetMotorSpeed(int motor, int speed)
{
	// Speed auf 255-Werte umrechnen
	speed = static_cast<int>(speed * 2.55);
	
	// Das Parameter-Array
	BYTE params[2] = {0};
	
	// Rechter Motor oder beide Motoren?
	if(motor & MRIGHT == MRIGHT)
	{
		// Neue Rotation? Rotation bestimmen
		if(_lastSpeed[MRIGHT_ARR] <= 0 && speed > 0)
		{
			params[0] = MRIGHT;
			params[1] = FORWARDS;
			if(!Send(5, params, 2))
				return false;
		}
		
		if(_lastSpeed[MRIGHT_ARR] >= 0 && speed < 0)
		{
			params[0] = MRIGHT;
			params[1] = BACKWARDS;
			if(!Send(5, params, 2))
				return false;
		}
		
		if(_lastSpeed[MRIGHT_ARR] != speed) 
		{
			// Geschwindigkeit senden (ohne Vorzeichen)
			params[0] = MRIGHT;
			params[1] = abs(speed);
			if(!Send(2, params, 2))
				return false;
		}
		_lastSpeed[MRIGHT_ARR] = speed;
	}
	
	// Linker Motor oder beide Motoren?
	if(motor & MLEFT == MLEFT)
	{
		// Neue Rotation? Rotation bestimmen
		if(_lastSpeed[MLEFT_ARR] <= 0 && speed > 0)
		{
			params[0] = MLEFT;
			params[1] = FORWARDS;
			if(!Send(5, params, 2))
				return false;
		}
		
		if(_lastSpeed[MLEFT_ARR] >= 0 && speed < 0)
		{
			params[0] = MLEFT;
			params[1] = BACKWARDS;
			if(!Send(5, params, 2))
				return false;
		}
		
		if(_lastSpeed[MLEFT_ARR] != speed) 
		{
			// Geschwindigkeit senden (ohne Vorzeichen)
			params[0] = MLEFT;
			params[1] = abs(speed);
			if(!Send(2, params, 2))
				return false;
		}
		_lastSpeed[MLEFT_ARR] = speed;
	}
	
	// Alles toll
	return true;
}