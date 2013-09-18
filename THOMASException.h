#pragma once
/*
-- THOMAS-EXCEPTION-KLASSE :: HEADER --
Definiert eine eigene Exception für das THOMAS-Projekt.
Diese Exception gibt standardmäßig immer die Fehlermeldung aus.
*/


/* INCLUDES */

// C++-exception-Klasse
#include <exception>

// C++-string-Klasse
// Hiermit wird die Fehlermeldung verarbeitet.
#include <string>


/* KLASSE */
namespace THOMAS
{
	class THOMASException : public std::exception
	{
	private:
		// Die Fehlermeldung.
		std::string _message;
		
	public:
		// Konstruktor.
		// Erstellt eine neue THOMASException mit der angegebenen Fehlermeldung.
		// Parameter:
		// -> message: Die Fehlermeldung.
		THOMASException(std::string message);
		
		// Gibt die Fehlernachricht zurück.
		const char* what();
	};
}