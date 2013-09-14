/*
-- MOTOR-CONTROL-KLASSE :: IMPLEMENTIERUNG --
*/


/* INCLUDES */

// Klassenheader
#include "MotorControl.h"
using namespace THOMAS;

// C++-String-Klasse
#include <string>

// C-Standardbibliothek
// Die abs()-Funktion wird hier benötigt.
#include <cstdlib>

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
		throw std::string("Fehler: Die Motorsteuerung ist bereits aktiv!");
	_running = true;
	
	// Die Motoren sollten stehen
	_lastSpeed[0] = 0;
	_lastSpeed[1] = 1;
	
	// RS232-Verbindung herstellen
	_rs232 = new RS232();
	
	// Server starten
	_server = new TCPServer(4242, ComputeClientCommandWrapper, static_cast<void *>(this));
	_server->BeginListen();
}

void MotorControl::Stop()
{
	// Läuft die Steuerung?
	if(!_running)
		throw std::string("Fehler: Die Motorsteuerung ist nicht aktiv!");
	_running = false;
	
	// Server herunterfahren
	_server->EndListen();
	delete _server;
	
	// RS232-Verbindung beenden
	delete _rs232;
}

void MotorControl::ComputeClientCommand(BYTE *data, int dataLength)
{
	for(int i = 0; i < dataLength; i++)
		std::cout << data[i] << std::endl;
}

void MotorControl::SendMotorSpeed(int motor, int speed)
{
	// Speed auf 255er-Werte umrechnen
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