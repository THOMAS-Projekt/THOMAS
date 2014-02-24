/*
-- TRACKING-KLASSE :: IMPLEMENTIERUNG --
*/


/* INCLUDES */

// Klassenheader
#include "Tracking.h"
using namespace THOMAS;

// THOMASException-Klasse
#include "THOMASException.h"

// C-Standardbibliothek
#include <cstdlib>


/* FUNKTIONEN */

Tracking::Tracking()
{
	// Detektor erzeugen
	_detector = new CascadeClassifier();
	
	// Haarcascade laden
    _detector.load(_hcc);
	
	// Kameraverbindung herstellen
	_cap = new VideoCapture(0);
	
	// Framebreite abrufen (CV_CAP_PROP_FRAME_WIDTH)
	_camWidth = _cap.get(3);
	
	// Framebreite gültig? (So wird das Teilen durch 0 verhindert)
	if(_camWidth <= 0)
	{
		// Fehler
		throw THOMASException("Fehler: Die Breite des Kameraframes ist ungültig!");
	}
}

Tracking::~Tracking()
{	
	// Kameraverbindung vernichten
	delete _cap;
	
	// Detektor vernichten
	delete _detector;
}

int Tracking::GetObjectPosition()
{
	// Bild aufnehmen
    _cap >> _camFrames;
	
    // Bild in Graustufen konvertieren
    cvtColor(_camFrames, _grayFrames, CV_BGR2GRAY);
	
    // Bildkontrast des grauen Bildes erhöhen
    equalizeHist(_grayFrames, _grayFrames);
	
    // Bild nach Objekten durchsuchen und in Array laden
    _detector.detectMultiScale(_grayFrames, _objects, 1.1, 2, 0, Size(_accuracy, _accuracy));
	
    // Genau ein Objekt gefunden?
    if(_objects.size() == 1)
	{       
        // Unterschied errechnen und zurückgeben (Objektmitte - Bildmitte): kleiner 0 => links, 0 => mitte, größer 0 => rechts (-100 bis 100)
		_horizontalPos = 100 - (((_camWidth / 2) - (_objects[0].x + _objects[0].width * 0.5)) * 400 / _camWidth);
		
		// Wertüberschreitungen vermeiden
		if(_horizontalPos > 100)
		{
			// Wert größer als 100 => 100
			_horizontalPos = 100;
		}
		else
		{
			if(_horizontalPos < -100)
			{
				// Wert kleiner als -100 => -100
				_horizontalPos = -100;
			}
		}
		
		return _horizontalPos;
    }
	else
	{
		// Kein Objekt gefunden => 1000 zurückgeben
		return 1000;
	}
}