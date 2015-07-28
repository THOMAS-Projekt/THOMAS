#pragma once
/*
-- LaserMeasurement-Klasse :: HEADER --
Definiert die LaserMeasurement
Diese Klasse misst die Entfernung zum Laserpunkt
*/


/* INCLUDES */

// OpenCV-Klassen
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

// Vector-Klasse
#include <vector>


/* KONSTANTEN */

// Kamera Einstellungen
#define CAMERA_WIDTH 1280

// Startpunkt (Y) des Erkennungsbalkens
#define BAR_START 330

// Höhe des Erkennungsbalkens
#define BAR_HEIGHT 60

// Mindest- und Maximalgröße des Laserpunktes
#define LASER_MIN_WIDTH 1
#define LASER_MAX_WIDTH 80

// Mindesthelligkeit des Punktes
#define MIN_BRIGHTNESS 740

// Beginn des Kontrastüberprüfungsmittelwertstreifens
#define AVG_CHECK_OFFSET 2

// Breite von dem s.o.
#define AVG_CHECK_WIDTH 20

// Mindestkontrast
#define AVG_CHECK_BRIGHTNESS_OFFSET 50

// PI
#define PI 3.1415926535

// Abstand zwischen Laser und Kamera
#define LASER_DISTANCE 11.5

// Laserwinkel (siehe Zeichnung)
#define ALPHA 24 * PI / 180

// Kamerawinkel (siehe Zeichnung)
#define GAMMA 52.5 * PI / 180


/* KLASSE */

namespace THOMAS
{
	class LaserMeasurement
	{
	private:
		// Gibt die Laserpositon zurück
		int GetLaserPosition(cv::Mat frame, int startY, int endY);

		// Ermittelt die Helligkeit für einen bestimmten Abschnitt
		int GetBrightnessAvg(std::vector<int> bar, int avgStart, int avgStop);

		// Die letzte Laserposition
		int _lastLaserPosition = -1;

		// Der letzte Abstandswert
		int _lastDistance = -1;

	public:
		// Konstruktor
		LaserMeasurement();

		// Ermittelt die Entfernung in einem Image
		int GetDistanceFromImage(cv::Mat frame);

		// Gibt die letzte Laserposition zurück
		int GetLastLaserPosition();

		// Gibt den letzten Abstandswert zurück
		int GetLastDistance();

	};
}
