#pragma once
/*
-- STATUSINFORMATION :: HEADER --
Definiert die StatusInformations-Klasse
Kann Serverdaten, wie CPU-Last etc. abfragen und zurück geben
*/

// Enhält das Vector-Element
#include <vector>

/* Klasse */
namespace THOMAS
{
	// Enthält Funktionen zum Abrufen von Computerdaten
	class StatusInformation
	{
	private:
		// Summe eine Arrays errechnen
		int CalculateSum(std::vector<int> data, int start, int end);

		// CPU Daten abrufen
		std::vector<int> GetCPUData();

		// Ruft die CPU Last im seperaten Thread auf.
		void CPUCaptureThread();

		// Ist Client verbunden?
		bool _status = false;

		float _CPUUsage = 0;

	public:
		// Konstruktor
		StatusInformation();

		// Errechnet die CPU-Last
		float GetCPUUsage();

		// Ermittelt die CPU Temperatur
		int GetCPUTemperature();

		// Festplatten Informationen
		int GetDiskSpace();

		// Memory Daten abrufen
		// 0 = Gesamt
		// 1 = Verfügbar
		// 2 = Benutzt
		std::vector<int> GetMemoryInfo();

		// Status, ob Client verbunden ist
		void SetClientConnectStatus(bool status);

		// Gibt die gespeicherte CPU Auslastung zurück
		float GetCPUUsageSaved();

		// Wraper, um die OnClientStatusChange Funktion sauber an den TCP-Server zu übergeben
		static void CPUCaptureThreadWrapper(void *obj)
		{
			(reinterpret_cast<StatusInformation *>(obj))->CPUCaptureThread();
		}
	};
}
