/*
-- TRACKING-KLASSE :: IMPLEMENTIERUNG --
*/


/* INCLUDES */

// Klassenheader
#include "tracking.h"
using namespace THOMAS;

// THOMASException-Klasse
#include "THOMASException.h"

// C++-Stringstream-Klasse
// Diese Klasse erlaubt die Verkettung von Zeichenfolgen, sie wird hier f�r die Erzeugung von aussagekr�ftigen Exeptions ben�tigt.
#include <sstream>

// C-Standardbibliothek
#include <cstdlib>

// OpenCV
#include <opencv2/opencv.hpp>

/* FUNKTIONEN */

Tracking::Tracking()
{
	// Haarcascade laden
    detector.load(hcc);
	
	// Framebreite abrufen (CV_CAP_PROP_FRAME_WIDTH)
	camwidth = cap.get(3);
	
	// Framebreite g�ltig? (So wird das Teilen durch 0 verhindert)
	if (camwidth <= 0)
	{
		// Fehler
		throw THOMASException("Fehler: Die Breite des Kameraframes ist ung�ltig!");
	}
}

Tracking::~Tracking()
{	
	// Daten-Arrays vernichten
	delete objects;
}

int Tracking::get_objectposition()
{
	// Bild aufnehmen
    cap >> camFrames;

    // Bild in Graustufen konvertieren
    cvtColor(camFrames, grayFrames, CV_BGR2GRAY);

    // Bildkontrast des grauen Bildes erh�hen
    equalizeHist(grayFrames, grayFrames);

    // Bild nach Objekten durchsuchen und in Array laden
    detector.detectMultiScale(grayFrames, objects, 1.1, 2, 0, Size(accuracy, accuracy));
       
    // Genau ein Objekt gefunden?
    if (objects.size() == 1)
	{       
        // Unterschied errechnen und zur�ckgeben (Objektmitte - Bildmitte): kleiner 0 => links, 0 => mitte, gr��er 0 => rechts (-100 bis 100)
		return ((camwidth / 2) - (objects[0].x + objects[0].width * 0.5)) * 200 / camwidth;
    }
	else
	{
		// Kein Objekt gefunden => 200 zur�ckgeben
		return 200;
	}
}