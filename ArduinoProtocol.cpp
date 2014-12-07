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

/* FUNKTIONEN */

// Konstruktor
ArduinoProtocol::ArduinoProtocol()
{
	// Instanziere die ArduinoCom Klasse
	arduinoCom = new ArduinoCom();
}

int ArduinoProtocol::SetText(std::string text)
{

	// Erstellt neuen Char mit der Länge des Strings
	BYTE textData[text.length()];

	// Kopire den String in das Char Array
	std::strcpy(textData, text.c_str());

	// Erstelle neues Package
	BYTE package[3 + text.length()];

	// 0 = Meldung ausgeben
	package[0] = 1;

	// 1 = Priorität
	package[1] = 0;

	// 2 = Text Länge
	package[2] = text.length();

	// Lese das textData Array und speichere es in das package-Array
	for(int i = 0; i < text.length(); i++)
	{
		package[i+3] = textData[i];
	}

	// Sende das Package an den Arduino
	arduinoCom->Send(package,text.length() + 3);
	
	return (int) arduinoCom->Receive()[0];
}

int ArduinoProtocol::GetDistance(USensor us)
{
	// Erstelle Package zum Senden.
	// Parameter:
	// 2 = Sensoren ansprechen
	// 0 = Ultraschallsensoren
	// x = SensorID (us)
	// 2 = Messwet abrufen
	BYTE package[4] = {2,0,us,2};

	// Sende das Package an den Arduino
	arduinoCom->Send(package,4);

	// Gelesenen Wert zurück geben
	return (int) arduinoCom->Receive()[0];
}

int ArduinoProtocol::GetStatus(USensor us, bool newRequest)
{
	// Wähle zwischen aktuallisieren und abrufen
	BYTE request = newRequest ? 0 : 1;

	// Erstelle Package
	// Parameter:
	// 2 = Sensoren ansprechen
	// 0 = Ultraschallsensoren
	// x = SensorID (us)
	// 1/2 Status aktuallisieren/abrufen
	BYTE package[4] = {2,0,us,request};

	// Sende das Package an den Arduino
	arduinoCom->Send(package,4);

	return (int) arduinoCom->Receive()[0];
}

int ArduinoProtocol::SetCamPosition(CamID camID, int grad)
{

	// Erstelle Package
	// Parameter
	// 3 = Aktoren ansprechen
	// 0 = Gerät
	// x = ServoID
	// 0 = Drehe auf Wert
	// y = Grad
	BYTE package[5] = {3,0,camID,0,(char) grad};

	// Sende das Package an den Arduino
	arduinoCom->Send(package,5);

	return (int) arduinoCom->Receive()[0];
}

int ArduinoProtocol::ChangeCamPosition(CamID camID, int grad)
{

	// Erstelle Package
	// Parameter
	// 3 = Aktoren ansprechen
	// 0 = Gerät
	// x = ServoID
	// 1 = Drehe um Wert
	// y = Grad
	BYTE package[5] = {3,0,camID,1,(char) grad};

	// Sende das Package an den Arduino
	arduinoCom->Send(package,5);

	return (int) arduinoCom->Receive()[0];
}




