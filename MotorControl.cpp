﻿/*
-- MOTOR-CONTROL-KLASSE :: IMPLEMENTIERUNG --
*/


/* INCLUDES */

// Klassenheader
#include "MotorControl.h"
using namespace THOMAS;

// THOMASException-Klasse
#include "THOMASException.h"

// C++-Stringstream-Klasse
// Diese Klasse erlaubt die Verkettung von Zeichenfolgen, sie wird hier für die Erzeugung von aussagekräftigen Exeptions benötigt.
#include <sstream>

// C-Standardbibliothek
// Die abs()-Funktion wird hier benötigt.
#include <cstdlib>

// C-Stringfunktionen
// Die memcpy() und die memset()-Funktion werden hier benötigt.
#include <cstring>

// UNIX-Standardfunktionen [Non-Standard]
// Hier wird die usleep()-Funktion benötigt.
#include <unistd.h>

#include <iostream>
/* FUNKTIONEN */

MotorControl::MotorControl()
{
	// Die Steuerung läuft noch nicht
	_running = false;
}

MotorControl::~MotorControl()
{
	// Steuerung ggf. stoppen
	if(_running)
		Stop();
}

void MotorControl::Run()
{
	// Läuft die Steuerung schon?
	if(_running)
		throw THOMASException("Fehler: Die Motorsteuerung ist bereits aktiv!");
	_running = true;
	
	// Die Motoren sollten stehen
	_lastSpeed[0] = 0;
	_lastSpeed[1] = 1;
	
	// Joystick-Mutex erstellen
	_joystickMutex = new std::mutex();
	
	// Auf die Joystick-Daten muss gewartet werden
	_joystickDataOK = false;
	
	// RS232-Verbindung herstellen
	_rs232 = new RS232();
	
	// Motorgeschwindigkeitsanpassung starten
	_controlMotorSpeedThread = new std::thread(&MotorControl::ControlMotorSpeedWrapper, this);
	
	// Server starten
	_server = new TCPServer(4242, ComputeClientCommandWrapper, static_cast<void *>(this));
	_server->BeginListen();
}

void MotorControl::Stop()
{
	// Läuft die Steuerung?
	if(!_running)
		throw THOMASException("Fehler: Die Motorsteuerung ist nicht aktiv!");
	_running = false;
	
	// Server herunterfahren
	_server->EndListen();
	delete _server;
	
	// Motorgeschwindigkeitsanpassung beenden
	_controlMotorSpeedThread->join();
	
	// RS232-Verbindung beenden
	delete _rs232;
}

void MotorControl::ControlMotorSpeed()
{
	// Auf Initialisierung der Joystick-Daten-Arrays warten
	while(!_joystickDataOK)
		usleep(100000);
	
	// Motorgeschwindigkeit kontinuierlich regeln, bis die Motorsteuerung beendet wird
	const float joystickMaxAxis = 32768.0f; // Konstante, definiert Joystick-Maximalauslenkung
	const float joystickScale = 255.0f; // Konstante, definiert den Wertebereich (= die Skalierung) der Motorsteuerung
	const float joystickAxisInvConv = joystickScale / joystickMaxAxis; // Konstante, definiert den Umrechnungsfaktor auf die Motorsteuerung bezogen auf die Joystick-Maximalauslenkung
	float corrSum; // Joystick-Korrektursumme
	short wantedSpeed[2] = {0, 0}; // Die angestrebte Geschwindigkeit {links, rechts}
	while(_running)
	{
		// Joystick-Daten sperren
		_joystickMutex->lock();
		{
			// Joystick-Korrektursumme berechnen
			corrSum = static_cast<float>(abs(_joystickAxis[0]) + abs(_joystickAxis[1]));
			
			// Nach Korrektursumme unterscheiden
			if(corrSum <= joystickMaxAxis)
			{
				// Es muss nicht korrigiert werden, Geschwindigkeiten direkt berechnen
				wantedSpeed[0] = static_cast<short>(joystickAxisInvConv * (_joystickAxis[0] + _joystickAxis[1]));
				wantedSpeed[1] = static_cast<short>(joystickAxisInvConv * (_joystickAxis[1] - _joystickAxis[0]));
			}
			else
			{
				// Geschwindigkeiten berechnen, dabei korrigieren
				wantedSpeed[0] = static_cast<short>(joystickScale * (_joystickAxis[0] + _joystickAxis[1]) / corrSum);
				wantedSpeed[1] = static_cast<short>(joystickScale * (_joystickAxis[1] - _joystickAxis[0]) / corrSum);
			}
			
			// Test-Ausgabe
			std::ostringstream tests;
			tests << "[" << _joystickAxis[0] << ", " << _joystickAxis[1] << "] -> [" << wantedSpeed[0] << ", " << wantedSpeed[1] << "]";
			std::cout << tests.str() << std::endl;
			
			// TODO: Rotation wegen R-Achse...
		}
		_joystickMutex->unlock();
		
		// Geschwindigkeit angleichen
		// TODO...
		
		// Kurz warten, um den RS232-Port nicht zu überlasten und die Motoren nicht zu stark zu beschleunigen
		usleep(MOTOR_ACC_DELAY);
	}
	
	// Beide Motoren anhalten
	SendMotorSpeed(MBOTH, 0);
}

void MotorControl::ComputeClientCommand(BYTE *data, int dataLength)
{
	for(int i = 0; i < dataLength; i++)
		std::cout << data[i] << " ";
	std::cout << std::endl;
	
	// Kommandobyte prüfen
	switch(data[0])
	{
		// JOYSTICK_HEADER
		case 1:
		{
			// Joystick-Daten sperren
			_joystickMutex->lock();
			{
				// Werte auslesen
				_joystickAxisCount = static_cast<int>(data[1]);
				_joystickButtonCount = static_cast<int>(data[2]);
				
				// Empfangsarrays initialisieren
				_joystickAxis = new short[_joystickAxisCount];
				_joystickButtons = new BYTE[_joystickButtonCount];
				
				// Empfangsarrays auf 0 setzen, damit keine zufälligen Befehle ausgeführt werden
				for(int i = 0; i < _joystickAxisCount; ++i)
					_joystickAxis[i] = 0;
				for(int i = 0; i < _joystickButtonCount; ++i)
					_joystickButtons[i] = 0;
			}
			_joystickMutex->unlock();
			
			// Joystick-Daten-Arrays sind initialisiert
			_joystickDataOK = true;
			
			// Fertig
			break;
		}
		
		// JOYSTICK_DATA
		case 2:
		{
			// Wurde der Joystick-Daten-Header empfangen?
			if(!_joystickDataOK)
				throw THOMASException("Es wurde noch kein JOYSTICK_HEADER-Kommando empfangen!");
			
			// Joystick-Daten sperren
			_joystickMutex->lock();
			{
				// Achswerte kopieren
				data++;
				memcpy(_joystickAxis, data, sizeof(short) * _joystickAxisCount);
				
				// Buttonwerte kopieren
				data += sizeof(short) * _joystickAxisCount;
				memcpy(_joystickButtons, data, sizeof(BYTE) * _joystickButtonCount);
			}
			_joystickMutex->unlock();
			
			// Fertig
			break;
		}
		
		// Ungültiges Kommando
		default:
		{
			// Fehler
			std::ostringstream err;
			err << "Fehler: Das erhaltene Kommandobyte (" << data[0] << ") ist ungueltig!";
			throw err.str();
		}
	}
}

void MotorControl::SendMotorSpeed(int motor, int speed)
{
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
			_rs232->Send(5, params, 2);
		}
		else if(_lastSpeed[MRIGHT_ARR] >= 0 && speed < 0)
		{
			params[0] = MRIGHT;
			params[1] = BACKWARDS;
			_rs232->Send(5, params, 2);
		}
		
		if(_lastSpeed[MRIGHT_ARR] != speed) 
		{
			// Geschwindigkeit senden (ohne Vorzeichen)
			params[0] = MRIGHT;
			params[1] = abs(speed);
			_rs232->Send(2, params, 2);
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
			_rs232->Send(5, params, 2);
		}
		else if(_lastSpeed[MLEFT_ARR] >= 0 && speed < 0)
		{
			params[0] = MLEFT;
			params[1] = BACKWARDS;
			_rs232->Send(5, params, 2);
		}
		
		if(_lastSpeed[MLEFT_ARR] != speed) 
		{
			// Geschwindigkeit senden (ohne Vorzeichen)
			params[0] = MLEFT;
			params[1] = abs(speed);
			_rs232->Send(2, params, 2);
		}
		_lastSpeed[MLEFT_ARR] = speed;
	}
}