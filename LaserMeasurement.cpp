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

// Enthält die Tangens-Funktion
#include <math.h>


/* FUNKTIONEN */

LaserMeasurement::LaserMeasurement()
{

}

int LaserMeasurement::GetDistanceFromImage(cv::Mat frame)
{
	// Laserposition abrufen
	float laserPosition = GetLaserPosition(frame, BAR_START, BAR_START + BAR_HEIGHT);

	// LaserPosition merken
	_lastLaserPosition = laserPosition;

	// Wert auf Gültigkeit überprüfen
	if(laserPosition < 0)
	{
		// Abstandswert merken
		_lastDistance = -1;

		// Spar dir die Berechnung, kommt sowieso Müll raus
		return -1;
	}

	// Distanz berechnen
	float distance = (-LASER_DISTANCE / tan(ALPHA)) / (2 * (CAMERA_WIDTH / 2 - laserPosition) / CAMERA_WIDTH * tan(GAMMA / 2) / tan(ALPHA) - 1);

	// Abstandswert merken
	_lastDistance = distance;

	// Distanz zurückgeben
	return distance;
}

int LaserMeasurement::GetLaserPosition(cv::Mat frame, int startY, int endY)
{
	// Startpunkt des untersuchten Bereiches
	int sectionStart = -1;

	// Endpunkt des untersuchten Bereiches
	int sectionEnd = -1;

	// Helligkeit des Endpunktes
	int sectionEndBrightness = 0;

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
			int b = frame.at<cv::Vec3b>(y, x)[0];
			int g = frame.at<cv::Vec3b>(y, x)[1];
			int r = frame.at<cv::Vec3b>(y, x)[2];

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

		if(sectionEnd == -1)
		{
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
				if(sectionStart != -1 && x - sectionStart > LASER_MIN_WIDTH && x - sectionStart < LASER_MAX_WIDTH)
				{
					// Endpunkt setzen
					sectionEnd = x;

					// Helligkeit des Endpunktes merken
					sectionEndBrightness = topBrightness;
				}
				else
				{
					// Startpunkt zurücksetzen
					sectionStart = -1;
				}
			}
		}
		else if(sectionEnd + AVG_CHECK_OFFSET + AVG_CHECK_WIDTH <= x)
		{
			// Prüfe:
			// 1. Die durchschnittliche Helligkeit nach dem Abschnitt weist einen großen Kontrast auf
			if(GetBrightnessAvg(bar, sectionEnd + AVG_CHECK_OFFSET, sectionEnd + (AVG_CHECK_WIDTH + AVG_CHECK_OFFSET)) < sectionEndBrightness - AVG_CHECK_BRIGHTNESS_OFFSET)
			{
				// Mittelpunkt des Laserpunktes zurückgeben
				return sectionStart + (sectionEnd - sectionStart) / 2;
			}
			else
			{
				// Weiter geht's
				sectionStart = -1;
				sectionEnd = -1;
			}
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

int LaserMeasurement::GetLastDistance()
{
	// Letzen Abstandswert zurückgeben
	return _lastDistance;
}

int LaserMeasurement::GetBrightnessAvg(std::vector<int> bar, int avgStart, int avgStop)
{
	// Division durch 0 verhindern
	if(avgStart < 0 || avgStop - avgStart <= 0 || avgStop >= bar.size())
	{
		// Größtmöglichen Wert zurückgeben
		return 255 * 3;
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
