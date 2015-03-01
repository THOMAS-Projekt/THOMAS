// Die Header Datei
#include "StatusInformation.h"

// Eingabe-Ausgabe-Stream Header
#include <iostream>

// File-Stream header
#include <fstream>

// Enhält u.a die atoi-Funktion
#include <cstdlib>

// Standard Symbolische Konstanten und Typen
#include <unistd.h>

// Enthält das Vector-Element
#include <vector>

// Enthält die statvfs-Struktur => Festplatten informationen
#include <sys/statvfs.h>

using namespace THOMAS;

StatusInformation::StatusInformation()
{

}

std::vector<int> StatusInformation::GetMemoryInfo()
{
	// Array erstellen
	std::vector<int> memoryData(3);

	// Werte ermitteln 
	long numPages = sysconf(_SC_PHYS_PAGES)/1024;
	long pageSize = sysconf(_SC_PAGESIZE)/1024;
	long freePages = sysconf( _SC_AVPHYS_PAGES)/1024;

	// Werte setzten
	memoryData[0] = (int) (numPages * pageSize);
	memoryData[1] = (int) (pageSize * freePages);
	memoryData[2] = (int) ((numPages * pageSize) - (pageSize * freePages));
   	
	return memoryData;
}

int StatusInformation::GetDiskSpace()
{
	// Struktur mit Infos erstellen
	struct statvfs buff;

	// Informationen in Buffer laden
	statvfs("/", &buff);

	// Größe berechnen und in MB umwandeln
	return (((double)buff.f_bavail * buff.f_bsize) / 1048576 );
}


int StatusInformation::GetCPUTemperature()
{

	// Datei-Handler
	std::ifstream CPUTemperatureFile;
	
	// Input String
	std::string input;

	// Datei mit CPU Temperatur öffnen TODO: NR. an System anpassen. (0 = default)
	CPUTemperatureFile.open("/sys/class/thermal/thermal_zone2/temp");

	// Ist geöffnet?
	if(!CPUTemperatureFile.is_open())
	{
		// Windoof?
		std::cout << "Konnte Datei nicht öffnen!" << std::endl;
	}
	
	// Wert lesen
	std::getline(CPUTemperatureFile, input);

	// Wert zurück geben und durch 1000 Teilen
	return atoi(input.c_str())/1000;
}

int StatusInformation::CalculateSum(std::vector<int> data, int start, int end)
{
	// Summe initialisieren
	int sum = 0;
	
	// Array auslesen und addieren
	for(int i = start; i < end; i++)
	{
		sum += data[i];
	}

	return sum;
}

float StatusInformation::GetCPUUsage()
{
	// Array mit Berechneten Werten
	std::vector<int> calculationData(4);

	// Werte einfügen = first
	calculationData[0] = CalculateSum(GetCPUData(), 2, 9);
	calculationData[1] = CalculateSum(GetCPUData(), 2, 5);

	// CPU Last von 1 Sekunde
	usleep(100000);

	// Werte einfügen => second
	calculationData[2] = CalculateSum(GetCPUData(), 2, 9);
	calculationData[3] = CalculateSum(GetCPUData(), 2, 5);

	// CPU Last berrechnen
	float cpuLoad = ((float) (calculationData[3] - calculationData[1]) / (float) (calculationData[2] - calculationData[0])) * 100;
	
	return cpuLoad;
}

std::vector<int> StatusInformation::GetCPUData()
{
	// Neues Array mit CPU Informationen
	std::vector<int> cpuData(9);

	// Datei-Handler
	std::ifstream cpuStats;

	// Input String
	std::string input;

	// Datei mit CPU Infos öffnen
	cpuStats.open("/proc/stat");

	// Ist geöffnet?
	if(!cpuStats.is_open())
	{
		// Windoof?
		std::cout << "Konnte Datei nicht öffnen!" << std::endl;
	}

	// Daten auslesen
	for(int i = 0; i < 9; i++)
	{
		// Daten abrufen und speichern
		std::getline(cpuStats, input, ' ');
		cpuData[i] = atoi(input.c_str());
	}

	// Datei schließen
	cpuStats.close();

	return cpuData;
}


