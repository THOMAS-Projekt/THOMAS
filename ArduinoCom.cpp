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

// Konstruktor
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

	// Konfigurieren, dass das Programm auf eine Antwort warten soll
	fOpt.c_cc[VMIN] = 1;
	fOpt.c_cc[VTIME] = 1;

	// TODO: Daten empfangen?
	/*
	// Flags setzen
	fOpt.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
	fOpt.c_cflag &= ~PARENB;
	fOpt.c_cflag &= ~CSTOPB;
	fOpt.c_cflag &= ~CSIZE;
	fOpt.c_cflag |= CS8;

	fOpt.c_cflag |= (CLOCAL | CREAD);
	*/

	// Neue Port-Optionen einsetzen
	tcsetattr(_handle, TCSANOW, &fOpt);

}

// Destruktor
ArduinoCom::~ArduinoCom()
{
	// Verbindung schließen
	close(_handle);
}

// Daten an den Arduino senden
void ArduinoCom::Send(BYTE *package, int packageLength)
{
	// Data Array mit packageLength
	BYTE *data = new BYTE[packageLength +1];

	// Einfügen der gesamten BYTE-Länge.
	data[0] = packageLength;

	// Inhalt des Paketes durchlaufen
	for(int i = 0; i < packageLength; i++)
	{
		// Einfügen des package-Arrays in das data-Array
		data[i+1] = package[i];
	}

	// Anzahl der gesendeten Daten merken
	int n = write(_handle, data, packageLength + 1);

	// Anzahl der gesendeten Daten pruefen
	if(n != packageLength +1)
	{
		// Mist
		throw THOMASException("Fehler beim Senden eines Befehls!");
	}

	// Speicher freigeben
	delete[] data;
}

// Auf ein eingehendes Paket warten und dieses zurückgeben
BYTE* ArduinoCom::Receive()
{
	// Erstelle Variable für die BYTE Länge
	ssize_t bytes;

	// Byte-Array für den Header
	BYTE header[1] = {0};

	// Temporärer Puffer
	BYTE tempBuffer[1] = {0};

	// Puffer für das Paket
	buffer = new BYTE[header[0]];

	// Header empfangen
	read (_handle, header, 1);

	// Daten empfangen
	for(int i = 0; i < header[0]; i++){
		// Nächstes Byte aus dem Stream lesen
		read(_handle, tempBuffer, 1);

		// Byte in den Puffer schreiben
		buffer[i] = tempBuffer[0];
	}

	// Puffer zurückgeben
	return buffer;
}

// Hilfsfunktion zum Ausführen eines Befehles in der Konsole
std::string ArduinoCom::Exec(std::string cmd)
{
	// String für die Antwort
	std::string data;

	// Stream
	FILE * stream;

	// Größe des Puffers
	const int max_buffer = 256;

	// Puffer
	char buffer[max_buffer];

	// String zum Weiterreichen der Ausgabe an den Befehl anhängen
	cmd.append(" 2>&1");

	// Stram zum Lesen öffnen
	stream = popen(cmd.c_str(), "r");

	// Öffnen erfolgreich...?
	if (stream)
	{
		// Schleife
		while (!feof(stream))
		{
			// Daten vorhanden?
			if (fgets(buffer, max_buffer, stream) != NULL)
			{
				// An die Antwort anhängen
				data.append(buffer);
			}

			// Stream schlißen
			pclose(stream);
		}
	}

	// Antwort zurückgeben
	return data;
}

