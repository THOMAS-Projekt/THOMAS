/*
-- LaserMeasurement-KLASSE :: IMPLEMENTIERUNG --
*/


/* INCLUDES */

// Klassenheader
#include "LaserMeasurement.h"
using namespace THOMAS;

// THOMAS-Exception Klasse
#include "THOMASException.h"

// IO-Stream
#include <iostream>

// OPENCV-Klassen
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

// Enthält die pow Funktion für Quadrate
#include <math.h>


/* FUNKTIONEN */

LaserMeasurement::LaserMeasurement()
{

}

int LaserMeasurement::GetDistanceFromImage(cv::Mat frame)
{
	// Laserposition abrufen
	float laserPosition = GetLaserPosition(frame, BAR_START, BAR_START + BAR_HEIGHT);

	// Wert auf Gültigkeit überprüfen
	if(laserPosition < 0)
	{
		// Spar dir die Berechnung, kommt sowieso Müll raus
		return 0;
	}

	// LaserPosition merken
	_lastLaserPosition = laserPosition;

	// Distanz berechnen
	float distance = (-LASER_DISTANCE / tan(ALPHA)) / (2 * (CAMERA_WIDTH / 2 - laserPosition) / CAMERA_WIDTH * tan(GAMMA / 2) / tan(ALPHA) - 1);

	// Distanz zurückgeben
	return distance;
}

int LaserMeasurement::GetLaserPosition(cv::Mat frame, int startY, int endY)
{
	// Startpunkt des Suchlaufes
	int sectionStart = -1;

	std::vector<int> bar;

	// X-Achte durchlaufen
	for(int x = 0; x < frame.cols; x++)
	{
		// Variable für hellsten Wert
		int topBrightness = 0;

		// Spalte durchlaufen
		for(int y = startY; y < endY; y++)
		{
			// BGR Werte abrufen
			int b = frame.at<cv::Vec3b>(y,x)[0];
			int g = frame.at<cv::Vec3b>(y,x)[1];
			int r = frame.at<cv::Vec3b>(y,x)[2];

			// Helligkeit berechnen
			int brightness = b + g + r;

			// Helligkeitswerte vergleichen
			if(brightness > topBrightness)
			{
				// Helligkeitswert übernehmen
				topBrightness = brightness;
			}
		}

		// Die größte Helligkeit speichern
		bar.push_back(topBrightness);

		// Ist die Helligkeit größer als der Mindestwert
		if(topBrightness > MIN_BRIGHTNESS)
		{
			// Prüfe:
			// 1. Es hat noch kein Abschnitt begonnen
			// 2. Durchschnittliche Helligkeit vor dem Abschnitt weist einen großen Kontrast auf
			if(sectionStart == -1 && GetBrightnessAvg(bar, x - (AVG_CHECK_WIDTH + AVG_CHECK_OFFSET), x - AVG_CHECK_OFFSET) < topBrightness - AVG_CHECK_BRIGHTNESS_OFFSET)
			{
				// Startpunkt setzen
				sectionStart = x;
			}
		}
		else
		{
			// Prüfe:
			// 1. Es hat bereits ein Abschnitt begonnen
			// 2. Die Laserbreite entspricht den Mindest- und Maximalwerten
			// 3. Die durchschnittliche Helligkeit nach dem Abschnitt weist einen großen Kontrast auf
			if(sectionStart != -1 && x - sectionStart > LASER_MIN_WIDTH && x - sectionStart < LASER_MAX_WIDTH && GetBrightnessAvg(bar, x + AVG_CHECK_OFFSET, x + (AVG_CHECK_WIDTH + AVG_CHECK_OFFSET)) < topBrightness - AVG_CHECK_BRIGHTNESS_OFFSET)
			{
				// Mittelpunkt des Laserpunktes zurückgeben
				return sectionStart + (x - sectionStart) / 2;
			}

			// Startpunkt zurücksetzen
			sectionStart = -1;

		}
	}

	// Keinen Laserpunkt gefunden
	return -1;
}

int LaserMeasurement::GetLastLaserPosition()
{
	// Letze Laserposition zurückgeben
	return _lastLaserPosition;
}

int LaserMeasurement::GetBrightnessAvg(std::vector<int> bar, int avgStart, int avgStop)
{
	// Division durch 0 verhindern
	if(avgStart < 0 || avgStop - avgStart <= 0 || avgStop >= bar.size())
	{
		// 0 zurückgeben
		return 0;
	}

	// Summe der Brightness
	int sum = 0;

	// Angegebenen Bereich durchlaufen
	for(int x = avgStart; x < avgStop; x++)
	{
		// Werte summieren
		sum += bar.at(x);
	}

	// Durchschnitt berechnen und zurückgeben
	return sum / (avgStop - avgStart);
}
