/*
-- THOMAS-EXCEPTION-KLASSE :: IMPLEMENTIERUNG --
*/


/* INCLUDES */

// Klassenheader
#include "THOMASException.h"
using namespace THOMAS;

// C++-iostream-Header
// Stellt u.a. den Konsolen-Ausgabe-Stream bereit.
#include <iostream>


/* FUNKTIONEN */

THOMASException::THOMASException(std::string message)
{
	// Fehlernachricht speichern
	_message = message;

	// Fehlernachricht ausgeben
	std::cout << "THOMASException: " << message << std::endl;
}

const char* THOMASException::what()
{
	// Fehlermeldung zurückgeben
	return _message.c_str();
}
