/*
-- Abgespeckter Scanner Kram --
*/


/* INCLUDES */

// MotorControl-Klasse
#include "MotorControl.h"

// TelemetryReceiver-Klasse
#include "TelemetryReceiver.h"

using namespace THOMAS;

// THOMASException-Klasse
#include "THOMASException.h"

// C++-iostream-Header
// Enthält die wichtigen Konsolen-Ein-/Ausgabe-Streams.
#include <iostream>

// UNIX-Standard-Funktionen [Non-Standard]
// Die sleep()-Funktion wird benötigt.
#include <unistd.h>


/* FUNKTIONEN */

// Programm-Einstiegsfunktion.
int main(int argc, char **argv)
{
    // Arduino Protokoll-Klasse starten
    ArduinoProtocol *arduino = new ArduinoProtocol();
    arduino->Run(true);

    // Motorsteuerung starten
    MotorControl *motorControl = new MotorControl();
    motorControl->Run(arduino, false);

    // TelemetryReceiver starten
    TelemetryReceiver *teleRecv = new TelemetryReceiver();
    teleRecv->Run(arduino);

    // Programm laufen lassen, Prozessor nicht unnötig belasten (alles läuft in separaten Threads)
    // TODO: Programm-Befehle per Tastatur etc.
    while(true)
        sleep(2);

    // Programm erfolgreich beendet
    return 0;
}
