#pragma once
/*
-- TRACKING-KLASSE :: HEADER --
Definiert die Tracking-Klasse.
Diese Klasse ist f�r das Abrufen und analysieren des aktuellen Kamera-Bildes zust�ndig.
Es wird eine Webcam und OpenCV ben�tigt. Die Klasse funktioniert nur unter Verwendung von sudo-Rechten.
*/


/* INCLUDES */


/* KONSTANTEN */


/* KLASSE */
namespace THOMAS
{
	class Tracking
	{
	private:
		// Konstante, den Pfad zur Haarcascade
		const String hcc = "haarcascade.xml";
		
		// Konstante, die genauigkeit der Grafikanalyse (Je gr��er, desto genauer und langsamer)
		const int accuracy = 80;
		
		// Detektor
		CascadeClassifier *detector;
	
		// Mit der Kamera verbinden
		VideoCapture cap(0);

		// Variablen f�r die Bilder in Graustufen und in RGB
		Mat camFrames, grayFrames;

		// Array f�r die Rechtecke f�r die erkannten Objekte
		vector<Rect> objects;
		
		// Breite des Kamerabildes
		int camwidth = 0;
		
	public:
		// Konstruktor.
		// Erstellt eine neue Instanz der Tracking-Klasse.
		Tracking();
		
		// Destruktor.
		// Gibt den belegten Speicher frei
		~Tracking();
		
		// Startet die ruft die horizontale Objektposition im Bild ab (Objektmitte - Bildmitte)
		int get_objectposition();
		
	};
}