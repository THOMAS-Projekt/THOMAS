#pragma once
/*
-- TRACKING-KLASSE :: HEADER --
Definiert die Tracking-Klasse.
Diese Klasse ist für das Abrufen und Analysieren des aktuellen Kamera-Bildes zuständig.
Es wird eine Webcam und OpenCV benötigt. Die Klasse funktioniert nur unter Verwendung von sudo-Rechten.
*/


/* INCLUDES */

// C++-string-Klasse
#include <string>

// OpenCV
#include <cv.h>


/* KONSTANTEN */


/* KLASSE */
namespace THOMAS
{
	class Tracking
	{
	private:
		// Konstante, der Pfad zur Haarcascade.
		const std::string _hcc = "haarcascade.xml";
		
		// Konstante, die Genauigkeit der Grafikanalyse (je größer, desto genauer und langsamer).
		const int _accuracy = 80;
		
		// Der Detektor.
		CascadeClassifier *_detector;
		
		// Die Kameraverbindung.
		VideoCapture *_cap;
		
		// Variablen für die Bilder in Graustufen und in RGB.
		Mat _camFrames, _grayFrames;
		
		// Array für die Rechtecke für die erkannten Objekte.
		vector<Rect> _objects;
		
		// Die Breite des Kamerabildes.
		int _camWidth = 0;
		
		// Die horizontale Position des Objektes
		int _horizontalPos = 0;
		
	public:
		// Konstruktor.
		// Erstellt eine neue Instanz der Tracking-Klasse.
		Tracking();
		
		// Destruktor.
		// Gibt den belegten Speicher frei.
		~Tracking();
		
		// Startet die Kameraabfrage und ruft die horizontale Objektposition im Bild ab (Bereich: -100(links) bis 100(rechts))
		int GetObjectPosition();
	};
}
