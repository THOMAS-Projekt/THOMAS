/*
-- ArduinoProtocol-KLASSE :: IMPLEMENTIERUNG --
*/

/* INCLUDES */

// Klassenheader
#include "ArduinoProtocol.h"
using namespace THOMAS;

// THOMASException-Klasse
#include "THOMASException.h"

// ArduinoCom-Klasse
#include "ArduinoCom.h"

// C++-IO-Stream-Klasse
#include <iostream>

// CString - Enthält underanderem die strcpy Funktion
#include <cstring>

// UNIX-Standard-Funktionen [Non-Standard]
// Die sleep()-Funktion wird benötigt.
#include <unistd.h>

/* FUNKTIONEN */

// Konstruktor
ArduinoProtocol::ArduinoProtocol()
{

}

// Erstellt neuen Thread
void ArduinoProtocol::Run()
{
	// Läuft die Kommunikation schon?
	if(running)
		throw THOMASException("Die Arduinokommunikation läuft bereits!");

	// Status des Threads auf True setzen.
	running = true;

	// Arduino Mutex erstellen
	arduinoMutex = new std::mutex();

	// Kommunikationsklasse instanzieren
	arduinoCom = new ArduinoCom();

	// Programm bis zum ersten Lebenszeichen des Arduinos pausieren
	// (Der Arduino startet beim Verbinden erst neu.)
	WaitForArduino();

	// FIXME: Es ist nicht möglich längere Texte zu senden, da der Arduino sich dabei aus irgendeinem Grund aufhängt bzw. neustartet.
	// FIXME: Möglicherweise liegt hier ein Problem wegen des horizontalen Scrollens vor, welches ich nicht finden konnte.
	// FIXME: Dieses Problem tritt allerdings nur bei mehrzeiligen Strings auf. Wenn nur eine lange Nachricht gesendet wird scollt dieser korrekt durch.
	//WriteMessage("keks1234567654345rztfthdggrzjgfhgs", PRIORITY_WARNING);
	//WriteMessage("keksd", PRIORITY_WARNING);
	SetCurrentSSID();
	//SetSignalStrength();
	//std::cout << "Sensor vorne Rechts: " << GetDistance(US_FRONT_LEFT) << "cm\n";

	// Neuen Signalstrength Thread
	signalStrengthThread = new std::thread(&ArduinoProtocol::SignalStrengthThreadWrapper,this);

	// Neuen Heartbeat-Thread
	heartbeatThread = new std::thread(&ArduinoProtocol::SendHeartbeatThreadWrapper,this);
}

void ArduinoProtocol::Stop()
{
	// Läuft die Kommunikation?
	if(!running)
		throw THOMASException("Die Arduinokommunikation ist nicht aktiv!");

	// Setzte Status auf False
	running = false;

	// Thread beenden
	signalStrengthThread->join();
	heartbeatThread->join();

	// Speicher freigeben
	delete signalStrengthThread;
	delete heartbeatThread;
}

// Thread zum Aktualisieren der Signalstärke
void ArduinoProtocol::UpdateSignalStrength()
{
	while(true)
	{
		// Aktuallisiert die Wlan Signalstärke
		SetSignalStrength();

		// In der Ruhe liegt die Kraft
		sleep(5);
	}
}

// Heartbeat senden
void ArduinoProtocol::SendHeartbeat()
{
	while(true)
	{
		// Aktuallisiert die Wlan Signalstärke
		Heartbeat();

		// In der Ruhe liegt die Kraft
		sleep(1);
	}

}

// Text auf das Display schreiben
void ArduinoProtocol::WriteMessage(std::string text, unsigned char priority)
{
	// Kommunikation sperren
	arduinoMutex->lock();
	{
		// Erstellt neuen Char mit der Länge des Strings
		BYTE textData[text.length()];

		// Kopire den String in das Char Array
		std::strcpy(textData, text.c_str());

		// Erstelle neues Package
		UBYTE package[3 + text.length()];

		// 0 = Meldung ausgeben
		package[0] = 1;

		// 1 = Priorität
		package[1] = priority;

		// 2 = Textlänge
		package[2] = text.length();

		// Zeichen durchlaufen
		for(int i = 0; i < text.length(); i++)
		{
			// Zeichen übernehmen
			package[i+3] = textData[i];
		}

		// Paket senden
		arduinoCom->Send(package, text.length() + 3);

		// Auf Antwort warten und diese prüfen
		if((int) arduinoCom->Receive()[0] != text.length())
		{
			// Fehlermeldung
			throw THOMASException("Fehler: Senden der Nachricht an den Arduino fehlgeschlagen!");
		}
	}
	arduinoMutex->unlock();
}

// Gibt den Messwert des angegebenen Ultraschallsensors in cm zurück
int ArduinoProtocol::GetDistance(unsigned char sensorID)
{
	// Speicher für die Daten
	int data;

	// Kommunikation sperren
	arduinoMutex->lock();
	{
		// Paket erstellen:
		// 2 = Sensoren ansprechen
		// 0 = Ultraschallsensoren
		// x = SensorID
		// 2 = Messwet abrufen
		UBYTE package[4] = {2, 0, sensorID, 2};

		// Sende das Paket an den Arduino
		arduinoCom->Send(package, 4);

		// Gelesenen Wert in cm umrechnen und zurückgeben FIXME: Den Wert stattdessen als genaueren Integer übertragen.
		data = (int) arduinoCom->Receive()[0] * 2;
	}
	arduinoMutex->unlock();

	// Daten zurückgeben
	return data;
}

// Ruft den Status des angegebenen Sensors ab, bzw. aktualisiert diesen vorher
int ArduinoProtocol::GetStatus(unsigned char sensorID, bool newRequest)
{
	// Speicher für die Daten
	int data;

	// Kommunikation sperren
	arduinoMutex->lock();
	{
		// Wähle zwischen aktuallisieren und abrufen
		UBYTE requestType = newRequest ? 0 : 1;

		// Paket erstellen:
		// 2 = Sensoren ansprechen
		// 0 = Ultraschallsensoren
		// x = SensorID
		// 1/2 Status aktuallisieren/abrufen
		UBYTE package[4] = {2, 0, sensorID, requestType};

		// Sende das Paket an den Arduino
		arduinoCom->Send(package, 4);

		// Auf eine Antwort warten und den Status zurückgeben
		data = (int) arduinoCom->Receive()[0];
	}
	arduinoMutex->unlock();

	// Daten zurückgeben
	return data;
}

// Position der Kamera auf einen bestimmten Winkel setzen
int ArduinoProtocol::SetCamPosition(unsigned char camera, unsigned char degree)
{
	// Speicher für die Daten
	int data;

	// Kommunikation sperren
	arduinoMutex->lock();
	{
		// Paket erstellen:
		// 3 = Aktoren ansprechen
		// 0 = Kameraservo
		// x = KameraID
		// 0 = Drehe auf Wert
		// y = Grad
		UBYTE package[5] = {3, 0, camera, 0, degree};

		// Sende das Paket an den Arduino
		arduinoCom->Send(package, 5);

		// Auf eine Antwort warten und den neuen Winkel zurückgeben
		data = (int) arduinoCom->Receive()[0];
	}
	arduinoMutex->unlock();

	// Daten zurückgeben
	return data;
}

// Position der Kamera um einen bestimmten Winkel drehen
int ArduinoProtocol::ChangeCamPosition(unsigned char camera, int degree)
{
	// Richtung
	unsigned char direction = degree < 0 ? 0 : 1;

	// Vorzeichen entfernen
	if (direction == 0) {
		// Mit -1 multiplizieren
		degree = degree * (-1);
	}

	// Gradzahl korrigieren
	degree = degree < 0 ? 0 : degree > 180 ? 180 : degree;

	// Speicher für die Daten
	int data;

	// Kommunikation sperren
	arduinoMutex->lock();
	{
		// Paket erstellen:
		// 3 = Aktoren ansprechen
		// 0 = Kameraservo
		// x = KameraID
		// 1 = Drehe um Wert
		// y = Richtung
		// z = Grad
		UBYTE package[6] = {3, 0, camera, 1, direction, (unsigned char)degree};

		// Sende das Paket an den Arduino
		arduinoCom->Send(package, 6);

		// Auf eine Antwort warten und den neuen Winkel zurückgeben
		data = (int) arduinoCom->Receive()[0];
	}
	arduinoMutex->unlock();

	// Daten zurückgeben
	return data;
}

// Arduino aningen
void ArduinoProtocol::Heartbeat()
{
	// Kommunikation sperren
	arduinoMutex->lock();
	{
		// Das Ping-Paket
		UBYTE package[1] = {0};

		// Paket an den Arduino senden
		arduinoCom->Send(package,1);

		// Wert korrekt?
		if((int) arduinoCom->Receive()[0] != 1)
		{
			// Fehler
			throw THOMASException("Fehler: Der Arduino hält sich nicht an das Protokoll!");
		}
	}
	arduinoMutex->unlock();
}

// Auf den Neustart des Arduinos warten
void ArduinoProtocol::WaitForArduino()
{
	// Gibt die Antwort an
	int resp = -1;

	// In Endlosschleife aufs erste Lebenszeichen warten
	do
	{
		// Antwort empfangen und merken
		resp = (int) arduinoCom->Receive()[0];
	}
	while(resp != 0);
}

// Die SSID des verbundenen Netzwerkes an den Arduino senden
void ArduinoProtocol::SetCurrentSSID()
{
	// Kommunikation sperren
	arduinoMutex->lock();
	{
		// SSID abrufen und als String speichern
		std::string SSID = arduinoCom->Exec("iwconfig wlan0 | grep -i 'ESSID' | cut -d ':' -f 2 | sed -e 's/\"//g' | tr -d ' '");

		// Das fehlerhafte Zeichen am Ende löschen
		SSID = SSID.substr(0, SSID.length() - 1);

		// Erstellt neuen Char mit der Länge des Strings
		BYTE textData[SSID.length()];

		// Kopire den String in das Char Array
		std::strcpy(textData, SSID.c_str());

		// Paket erstellen
		UBYTE package[3 + SSID.length()];

		// 4 = Status ändern
		package[0] = 4;

		// 0 = SSID setzen
		package[1] = 0;

		// 2 = Textlänge
		package[2] = SSID.length();

		// Text-Array durchlaufen
		for(int i = 0; i < SSID.length(); i++)
		{
			// Zeichen dem Paket hinzufügen
			package[i+3] = textData[i];
		}

		// Paket an den Arduino senden
		arduinoCom->Send(package,SSID.length() + 3);

		// Auf eine Antwort warten und diese prüfen
		if((int) arduinoCom->Receive()[0] != 1)
		{
			// Fehler
			throw THOMASException("Fehler: Senden der SSID an den Arduino fehlgeschlagen!");
		}
	}
	arduinoMutex->unlock();
}

// Die Signalstärke des verbundenen Netzwerkes an den Arduino senden
void ArduinoProtocol::SetSignalStrength()
{
	// Kommunikation sperren
	arduinoMutex->lock();
	{
		// Signalstärke abrufen und als String speichern
		std::string strength = arduinoCom->Exec("iwconfig wlan0 | grep -i Signal | cut -d = -f 3");

		// Das Vorzeichen und die Einheitsangabe entfernen
		UBYTE strg_value = atoi(strength.substr(0, strength.length() - 4).c_str()) * (-1);

		// Ausgabe leer?
		if(strg_value != 0)
		{
			// Invertieren
			strg_value = 100 - strg_value;

			// 0 => Am Schlechtesten
			// 70 => Am Besten

			// Skalierung ändern
			strg_value = ((float)strg_value / 70) * 100;

			// Wert falls nötig korrigieren
			strg_value = strg_value > 100 ? 100 : strg_value;
			strg_value = strg_value < 0 ? 0 : strg_value;
		}

		// Paket erstellen:
		// 4 = Status ändern
		// 1 = Signalstärke
		// x = Wert
		UBYTE package[3] = {4, 1, strg_value};

		// Paket senden
		arduinoCom->Send(package,3);

		// Auf eine Antwort warten und diese prüfen
		if((int) arduinoCom->Receive()[0] != 1)
		{
			// Fehler
			throw THOMASException("Fehler: Senden der Signalstärke an den Arduino fehlgeschlagen!");
		}
	}
	arduinoMutex->unlock();
}
