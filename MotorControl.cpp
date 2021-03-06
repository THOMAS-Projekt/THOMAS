﻿/*
-- MOTOR-CONTROL-KLASSE :: IMPLEMENTIERUNG --
*/

/* INCLUDES */

// Klassenheader
#include "MotorControl.h"
using namespace THOMAS;

// THOMASException-Klasse
#include "THOMASException.h"

// CollisionDetection-Klasse
#include "CollisionDetection.h"

// RelayProtocol-Klasse
#include "RelayProtocol.h"

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

/* FUNKTIONEN */

MotorControl::MotorControl()
{

}

MotorControl::~MotorControl()
{
	// Steuerung ggf. stoppen
	if (_running)
		Stop();

	// Joystick-Daten-Arrays vernichten
	delete _joystickAxis;
	delete _joystickButtons;

	// Joystick-Daten-Mutex vernichten
	delete _joystickMutex;
}

void MotorControl::Run(ArduinoProtocol *arduinoProtocol, bool enableCollisionDetection)
{
	// Läuft die Steuerung schon?
	if (_running)
		throw THOMASException("Fehler: Die Motorsteuerung ist bereits aktiv!");
	_running = true;

	// Die Motoren sollten stehen
	_lastSpeed[0] = 0;
	_lastSpeed[1] = 1;

	// Joystick-Mutex erstellen
	_joystickMutex = new std::mutex();

	// Arduino-Verbindung herstellen
	_arduino = arduinoProtocol;

	// RS232-Verbindung herstellen
	_rs232 = new RS232();

	// Soll die Collisiondetection aktiviert werden?
	if (enableCollisionDetection) {
		// CollisionDetection initialisieren
		_collisionDetection = new CollisionDetection(_arduino);
	}

	// Relay-Steuerung initialisieren
	_relayProtocol = new RelayProtocol();

	// Motorgeschwindigkeitsanpassung starten
	_controlMotorSpeedThread = new std::thread(&MotorControl::ControlMotorSpeedWrapper, this);

	// Tastendruckverarbeitung starten
	_computeInputButtonsThread = new std::thread(&MotorControl::ComputeInputButtonsWrapper, this);

	// Soll die Collisiondetection aktiviert werden?
	if (enableCollisionDetection) {
		// UpdateUsensorDataThread starten
		_updateUSensorData = new std::thread(&CollisionDetection::UpdateUSensorDataWrapper, _collisionDetection);
	}

	// Server starten
	_server = new TCPServer(4242, ComputeClientCommandWrapper, OnClientStatusChangeWrapper, static_cast<void *>(this));
	_server->BeginListen();
}

void MotorControl::Stop()
{
	// Läuft die Steuerung?
	if (!_running)
		throw THOMASException("Fehler: Die Motorsteuerung ist nicht aktiv!");
	_running = false;

	// Arduino-Kommunikation stoppen
	delete _arduino;

	// Server herunterfahren
	_server->EndListen();
	delete _server;

	// Motorgeschwindigkeitsanpassung beenden
	_controlMotorSpeedThread->join();
	delete _controlMotorSpeedThread;

	// Tastendruckverarbeitung beenden
	_computeInputButtonsThread->join();
	delete _computeInputButtonsThread;

	// RS232-Verbindung beenden
	delete _rs232;
}

void MotorControl::ControlMotorSpeed()
{
	// Auf Initialisierung der Joystick-Daten-Arrays warten
	while (!_joystickDataOK)
		usleep(100000);

	// Motorgeschwindigkeit kontinuierlich regeln, bis die Motorsteuerung beendet wird
	float corrSum; // Joystick-Korrektursumme
	short wantedSpeed[2] = {0, 0}; // Die angestrebte Geschwindigkeit {links, rechts}
	short newWantedSpeed[2] = {0, 0}; // Anhand der Sensordaten korrigierte Geschwindigkeit
	bool overwriteSpeed = false; // Gibt an ob die Beschleuningung umgangen werden soll

	std::vector<short> speedVector (2); // Vector mit der Geschwindigkeit

	while (_running)
	{
		// Joystick-Daten sperren
		_joystickMutex->lock();
		{
			// Steuerung nach X- und Z-Achse bei Joystick-Auslenkung >= 10%, Steuerung nach R-Achse bei Joystick-Auslenkung < 10%
			if (abs(_joystickAxis[0]) >= _joystickNoPowerZone || abs(_joystickAxis[1]) >= _joystickNoPowerZone)
			{
				// Joystick-Korrektursumme berechnen
				corrSum = static_cast<float>(abs(_joystickAxis[0]) + abs(_joystickAxis[1]));

				// Nach Korrektursumme unterscheiden
				if (corrSum <= _joystickMaxAxis)
				{
					// Es muss nicht korrigiert werden, Geschwindigkeiten direkt berechnen
					wantedSpeed[MLEFT_ARR] = -static_cast<short>(_joystickAxisInvConv * (-_joystickAxis[0] + _joystickAxis[1]));
					wantedSpeed[MRIGHT_ARR] = -static_cast<short>(_joystickAxisInvConv * (_joystickAxis[1] + _joystickAxis[0]));
				}
				else
				{
					// Geschwindigkeiten berechnen, dabei korrigieren
					wantedSpeed[MLEFT_ARR] = -static_cast<short>(_joystickScale * (-_joystickAxis[0] + _joystickAxis[1]) / corrSum);
					wantedSpeed[MRIGHT_ARR] = -static_cast<short>(_joystickScale * (_joystickAxis[1] + _joystickAxis[0]) / corrSum);
				}
			}
			else // Steuerung nach R-Achse
			{
				// Geschwindigkeiten beider Räder berechnen (genau gleich schnell, aber entgegengesetzt => Drehung um eigene Achse)
				// Negation, da Motoren sonst falschherum drehen
				wantedSpeed[MLEFT_ARR] = static_cast<short>(_joystickAxisInvConv * _joystickAxis[2]);
				wantedSpeed[MRIGHT_ARR] = static_cast<short>(_joystickAxisInvConv * -_joystickAxis[2]);
			}
		}
		_joystickMutex->unlock();

		// Auf Hindernisse prüfen und ggf. Werte ändern
		speedVector = _collisionDetection->CorrectWantedSpeed(wantedSpeed);
		std::copy(speedVector.begin(), speedVector.end(), newWantedSpeed);

		// Prüfen ob die Beschleunigung umgangen werden soll
		overwriteSpeed = !(wantedSpeed[0] == newWantedSpeed[0] && wantedSpeed[1] == newWantedSpeed[1]);

		// Korrigierte Geschwindigkeit übernehmen
		wantedSpeed[0] = newWantedSpeed[0];
		wantedSpeed[1] = newWantedSpeed[1];

		// Geschwindigkeitswerte angleichen: Links
		{
			// Würde die direkte Annahme der Wunschgeschwindigkeit in diesem Takt die Maximalbeschleunigung überschreiten?
			if (!overwriteSpeed && abs(wantedSpeed[MLEFT_ARR] - _lastSpeed[MLEFT_ARR]) > _speedMaxAcc)
			{
				// Motor angemessen beschleunigen, auch wenn Wunschgeschwindigkeit nicht erreicht wird
				// Der ternäre Operator ist hier die schönste und kürzeste Variante; er passt nur je nach Vorzeichen der Geschwindigkeitsdifferenz das Vorzeichen der Beschleunigung an.
				SendMotorSpeed(MLEFT, _lastSpeed[MLEFT_ARR] + (wantedSpeed[MLEFT_ARR] > _lastSpeed[MLEFT_ARR] ? _speedMaxAcc : -_speedMaxAcc));
			}
			else
			{
				// Beschleunigung OK, direkt Wunschgeschwindigkeit annehmen
				SendMotorSpeed(MLEFT, wantedSpeed[MLEFT_ARR]);
			}
		}

		// Geschwindigkeitswerte angleichen: Rechts
		{
			// Überschreitung der Maximalbeschleunigung?
			if (!overwriteSpeed && abs(wantedSpeed[MRIGHT_ARR] - _lastSpeed[MRIGHT_ARR]) > _speedMaxAcc)
			{
				// Motor angemessen beschleunigen
				SendMotorSpeed(MRIGHT, _lastSpeed[MRIGHT_ARR] + (wantedSpeed[MRIGHT_ARR] > _lastSpeed[MRIGHT_ARR] ? _speedMaxAcc : -_speedMaxAcc));
			}
			else
			{
				// Wunschgeschwindigkeit annehmen
				SendMotorSpeed(MRIGHT, wantedSpeed[MRIGHT_ARR]);
			}
		}

		// Kurz warten, um den RS232-Port nicht zu überlasten und die Motoren nicht zu stark zu beschleunigen
		usleep(_motorAccDelay);
	}

	// Beide Motoren direkt anhalten
	SendMotorSpeed(MBOTH, 0);
}

void MotorControl::ComputeInputButtons()
{
	// Auf Initialisierung der Joystick-Daten-Arrays warten
	while (!_joystickDataOK)
		usleep(100000);

	// Tastendrücke bis zum Beenden der Motorsteuerung verarbeiten
	bool kill_server = false; // Beenden-Anfrage
	bool horn = false; // Hupe
	bool radio = false; // Radio
	bool radioButtonPressed = false; // Radio-Knopf gedrückt
	bool radioIsPlaying = false; // Radio aktiv?

	int cam_servo_direction = 0; // Richtung in den der Servo gedreht werden soll
	while (_running)
	{
		// Joystick-Daten sperren
		_joystickMutex->lock();
		{
			// Auf Tastendrücke prüfen
			{
				// Not-Aus-Schalter abfragen
				kill_server = (_joystickButtons[6] == 1);

				// Feuerknopf abfragen
				horn = (_joystickButtons[0] == 1);

				// Radio Button
				radio = (_joystickButtons[5] == 1);

				// Servosteuerung nach Wert des kleinen Steuerknüppels
				cam_servo_direction = _joystickAxis[4] < 0 ? -1 : _joystickAxis[4] > 0 ? 1 : 0;
			}
		}
		_joystickMutex->unlock();

		// Soll der Server beendet werden?
		if (kill_server)
		{
			// Beide Motoren direkt anhalten
			SendMotorSpeed(MBOTH, 0);

			// Fehler werfen und somit das Programm beenden
			throw THOMASException("Info: Der Server wurde durch einen der Clienten beendet.");
		}

		// Wurde der Status verändert?
		if (horn != _hornActive)
		{
			// Relay schalten
			_relayProtocol->SetRelay(1, horn);

			// Status merken
			_hornActive = horn;
		}

		// Wurde der Button losgelassen?
		if (radioButtonPressed && !radio)
		{
			// Spielt das Radio ab?
			if (radioIsPlaying)
			{
				// Radio beenden
				system("./radio stop");
			}
			else
			{
				// Radio starten
				system("./radio start");
			}

			// Radiozustand merken
			radioIsPlaying = !radioIsPlaying;
		}

		// Tastenzustand merken
		radioButtonPressed = radio;

		// Kamera-Servo drehen?
		if (cam_servo_direction != 0)
		{
			// Kamera drehen
			_arduino->ChangeCamPosition(0, cam_servo_direction * 2);
		}
	}
}

void MotorControl::ComputeClientCommand(BYTE *data, int dataLength, int clientID)
{
	// Kommandobyte prüfen
	switch (data[0])
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
			for (int i = 0; i < _joystickAxisCount; ++i)
				_joystickAxis[i] = 0;
			for (int i = 0; i < _joystickButtonCount; ++i)
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
		if (!_joystickDataOK)
			throw THOMASException("Es wurde noch kein JOYSTICK_HEADER-Kommando empfangen!");

		// Joystick-Daten sperren
		_joystickMutex->lock();
		{
			// Achsendaten kopieren
			memcpy(_joystickAxis, &data[1], sizeof(short) * _joystickAxisCount);

			// Buttonwerte kopieren
			memcpy(_joystickButtons, &data[1 + sizeof(short) * _joystickAxisCount], sizeof(BYTE) * _joystickButtonCount);
		}
		_joystickMutex->unlock();

		// Fertig
		break;
	}

	// Ungültiges Kommando
	default:
	{
		// Fehler
		std::cout << "\033[1;31m" << "[WARNUNG]" << "\033[0;1m " << "Das erhaltene Kommandobyte (" << data[0] << ") ist ungueltig!" << "\033[0m " << std::endl;
	}
	}
}

void MotorControl::SendMotorSpeed(int motor, short speed)
{
	// Das Parameter-Array
	BYTE params[2] = {0};

	// Rechter Motor oder beide Motoren?
	if ((motor & MRIGHT) == MRIGHT)
	{
		// Neue Rotation? Rotation bestimmen
		if (_lastSpeed[MRIGHT_ARR] <= 0 && speed > 0)
		{
			params[0] = MRIGHT;
			params[1] = FORWARDS;
			_rs232->Send(5, params, 2);
		}
		else if (_lastSpeed[MRIGHT_ARR] >= 0 && speed < 0)
		{
			params[0] = MRIGHT;
			params[1] = BACKWARDS;
			_rs232->Send(5, params, 2);
		}

		if (_lastSpeed[MRIGHT_ARR] != speed)
		{
			// Geschwindigkeit senden (ohne Vorzeichen)
			params[0] = MRIGHT;
			params[1] = static_cast<BYTE>(abs(speed));
			_rs232->Send(2, params, 2);
		}
		_lastSpeed[MRIGHT_ARR] = speed;
	}

	// Linker Motor oder beide Motoren?
	if ((motor & MLEFT) == MLEFT)
	{
		// Neue Rotation? Rotation bestimmen
		if (_lastSpeed[MLEFT_ARR] <= 0 && speed > 0)
		{
			params[0] = MLEFT;
			params[1] = FORWARDS;
			_rs232->Send(5, params, 2);
		}
		else if (_lastSpeed[MLEFT_ARR] >= 0 && speed < 0)
		{
			params[0] = MLEFT;
			params[1] = BACKWARDS;
			_rs232->Send(5, params, 2);
		}

		if (_lastSpeed[MLEFT_ARR] != speed)
		{
			// Geschwindigkeit senden (ohne Vorzeichen)
			params[0] = MLEFT;
			params[1] = static_cast<BYTE>(abs(speed));
			_rs232->Send(2, params, 2);
		}
		_lastSpeed[MLEFT_ARR] = speed;
	}
}

void MotorControl::OnClientStatusChange(int clientID, int status, const char *ip)
{
	// TODO: Optionale implementierung => Wenn sich ein Client verbindet
}
