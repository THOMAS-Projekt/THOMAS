// **********THOMAS-Objektverfolgung**********
// Von Marcus Wichelmann
// Version 1.0
// Vom 03.09.2013
// Dieses Programm ist vollständig für das Terminal ausgelegt und sollte auch ohne GUI funktionieren.
// WICHTIG: Zum Ausführen dieses Programmes wird eine Webcam und OpenCV2 benötigt.
// *******************************************

// Standardimports
#include <stdlib.h>

// Import von OpenCV
#include <opencv2/opencv.hpp>

// Oft verwendete Klassen
using namespace std;
using namespace cv;

// Haarcascade waehlen
String hcc = "haarcascade.xml";

// Detektor
CascadeClassifier detector;

// Hauptfunktion
int main()
{
    // Konsole leeren   
    system("clear");

    // Mit der Kamera verbinden
    VideoCapture cap(0);

    // Variablen für die Bilder in Graustufen und in RGB
    Mat camFrames, grayFrames;

    // Rechtecke für die erkannten Objekte
    vector<Rect> objects;

    // Haarcascade laden
    detector.load(hcc);
   
    // Endlosschleife starten, die ständig Bilder von der Kamera abruft und analysiert
    while (1)
    {
        // Bild aufnehmen
        cap >> camFrames;

        // Bild in Graustufen konvertieren
        cvtColor(camFrames, grayFrames, CV_BGR2GRAY);

        // Bildkontrast des grauen Bildes erhöhen
        equalizeHist(grayFrames, grayFrames);

        // Bild nach Objekten durchsuchen und in Array laden
        detector.detectMultiScale(grayFrames, objects, 1.1, 2, 0, Size(80, 80));
       
        // Genau ein Objekt gefunden?
        if (objects.size() == 1)
        {       
            // Mittelpunkt des erkannten Objektes:
            Point center(objects[0].x + objects[0].width * 0.5, objects[0].y + objects[0].height * 0.5);

            // Hier kann die Position des Objektes weiterverarbeitet und so der Roboter gesteuert werden.
            // ..........
			
			// Info-Meldung ausgeben
			cout << "Ein Objekt gefunden!" << endl;
        }

        // Frame speichern
        imwrite("image.jpg", camFrames);
    }
}
