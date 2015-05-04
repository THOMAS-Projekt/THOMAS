/*
-- TelemetryReceiver-KLASSE :: IMPLEMENTIERUNG --
*/

/* INCLUDES */

// Klassenheader
#include "TelemetryReceiver.h"
using namespace THOMAS;

// THOMAS-Exception Klasse
#include "THOMASException.h"

// UDP Client Klasse
#include "UDPClient.h"

// Enthält die Kommunikation für den Arduino
#include "ArduinoProtocol.h"

// OPENCV-Klassen
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

// IOStream-Klasse ethält u.a. den Vector
#include <iostream>

// Socket-Klasse
#include <sys/socket.h>

// Enthält Strukturen für Sockets
#include <netinet/in.h>

// C-String-Klasse enthält u.a. die strcpy-Funktion
#include <cstring>

// UNIX-Funktionen [Non-Standard]
// In diesem Header ist u.a. die usleep()-Funktion definiert.
#include <unistd.h>

/* FUNKTIONEN */

TelemetryReceiver::TelemetryReceiver()
{

}

void TelemetryReceiver::Run(ArduinoProtocol *arduinoProtocol, int videoDeviceID)
{
	// Kamera initialisieren
	_videoCapture = cv::VideoCapture(videoDeviceID);

	// Erfolgreich geöffnet
	if(!_videoCapture.isOpened())
		throw THOMASException("Das Videodevice konnte nicht geöffnet werden");

	// Kamera Größenparameter setzten
	_videoCapture.set(CV_CAP_PROP_FRAME_WIDTH, CAMERA_WIDTH);
	_videoCapture.set(CV_CAP_PROP_FRAME_HEIGHT, CAMERA_HEIGHT);
	_videoCapture.set(CV_CAP_PROP_FPS, CAMERA_MAX_FPS);

	// Komprimierungseinstellung setzten
	param[0] = CV_IMWRITE_JPEG_QUALITY;

	// Default Qualität => Wird vom Client überschrieben
	param[1] = 30;

	// ArduinoProtocol speichern
	_arduino = arduinoProtocol;

	// TCP Server initialisieren
	_server = new TCPServer(4223, ComputeTCPServerDataWrapper, OnClientStatusChangeWrapper, static_cast<void *>(this));
	_server->BeginListen();

	// Status Informations Klasse initialisieren
	_statusInformation = new StatusInformation();

	// Neuen Thread erstellen
	std::thread CPUThread(&StatusInformation::CPUCaptureThreadWrapper, _statusInformation);

	// Neuen Framesverarbeitungs-Thread erstellen
	std::thread captureFrameThread(&TelemetryReceiver::CaptureFrameThread, this);

	// Threads syncronisieren
	CPUThread.join();
	captureFrameThread.join();
}

void TelemetryReceiver::CaptureFrameThread()
{
	// Neue Frame Matrix
	cv::Mat frame;

	// Wiederhole bis Windows sicher wird
	while(true)
	{
		// Ist Client Liste leer?
		if(UDPClientList.empty())
		{
			// Warten -> CPU nicht unnötig belasten
			usleep(10000);
			continue;
		}

		// Neuen Frame erstellen
		_videoCapture >> frame;

		// Farben in 8Bit konvertieren
		cvtColor(frame, frame, CV_8U);

		// JPEG Format setzten
		param[0] = CV_IMWRITE_JPEG_QUALITY;

		// Durch alle Clients iterieren und Frame senden
		for(auto &client : UDPClientList)
		{

			// Läuft der Server bereits, oder wurde er nur instanziert
			if(!client.second.GetServerRunning())
				// Fortfahren
				continue;

			// Neuer Buffer
			std::vector<uchar> buff;

			// Neue Matrix, für den Client
			cv::Mat clientFrame;

			// Bilder auf die gewünschte Größe anpassen
			cv::resize(frame,clientFrame, cv::Size(CAMERA_WIDTH*((float) client.second.GetFrameSize()/100),CAMERA_HEIGHT*((float) client.second.GetFrameSize()/100)));

			// Kompressionsrate setzten
			param[1] = client.second.GetFrameQuality();

			// Bilder komprimieren und in den Buffer schreiben
			imencode(".jpeg", clientFrame, buff, param);

			// Durch Frames iterieren
			for(int i = 0; i < buff.size(); i++)
			{
				// Alle 64000 Bytes aufrufen
				if(i % 64000 == 0 && i != 0)
				{
					// Neues Array mit 64000 Bytes erstellen
					std::vector<uchar> newBuff(buff.begin() + i - 64000, buff.begin() + i);

					// Bytes senden
					client.second.Send(newBuff);
				}

				// Entspricht "i" die Buffersize?
				if(i == buff.size() - 1)
				{
					// Array mit restlichen Bytes erstellen
					std::vector<uchar> newBuff(buff.begin() + 64000*((int) buff.size() / 64000), buff.end());

					// Bytes senden
					client.second.Send(newBuff);
				}
			}
		}
	}
}

void TelemetryReceiver::OnClientStatusChange(int clientID, int status, const char *ip)
{
	// Status analysieren
	switch(status)
	{
		// Client connected?
		case STATUS_CONNECT:
		{
			// Neuen UDPClient erstellen und in Liste speichern
			UDPClientList[clientID] = UDPClient();

			// IP setzten
			UDPClientList[clientID].SetIP(ip);


			// CPU-Last Thread aktivieren
			_statusInformation->SetClientConnectStatus(true);

			break;
		}

		// Client disconnected?
		case STATUS_DISCONNECT:
		{
			// UDPClient löschen und aus Liste entfernen
			UDPClientList.erase(clientID);

			// CPU-Last Thread stoppen
			_statusInformation->SetClientConnectStatus(false);

			break;
		}

	}
}

void TelemetryReceiver::ComputeTCPServerData(BYTE *data, int dataLength, int clientID)
{
	// ClientID in der UDPCLient Liste?
	if(UDPClientList.find(clientID) == UDPClientList.end())
		throw THOMASException("Die Client ID konnte nicht gefunden werden");

	// Daten analysieren
	switch(data[0])
	{
		// Headerdaten => Kommandobyte, Port
		case 1: {
			// Bytes zu Short (uint16)
			unsigned short port = (data[1] << 8) | data[2];

			// Verbindng zum Client aufnehmen
			UDPClientList[clientID].CreateUDPClient(port);

			break;
		}

		// Statusfeld => Kommandobyte, ID des Feldes
		case 2:
		{
			switch(data[1])
			{
				// Anforderung der CPU-Last
				case FIELD_CPU:
				{

					// CPU-Last Sring erstellen
					std::string CPULoad = std::to_string(static_cast<int>(_statusInformation->GetCPUUsageSaved()));
					CPULoad += "%\n";

					// Informationen in Vector Laden
					std::vector<BYTE> vecData (GenerateByteArray(1, FIELD_CPU, CPULoad));

					// Neuen Byte Buffer erstellen
					BYTE buff[vecData.size()];

					// Daten in Vector kopieren
					std::copy(vecData.begin(), vecData.end(), buff);

					// Daten zum Client senden
					_server->Send(clientID, buff, vecData.size());

					break;
				}

				// Anforderung des Rams
				case FIELD_MEMORY:
				{
					// RAM-Nutzungs Sring erstellen
					std::string memoryUsage = std::to_string(_statusInformation->GetMemoryInfo()[2]);
					memoryUsage += "MB\n";

					// Informationen in Vector Laden
					std::vector<BYTE> vecData (GenerateByteArray(1, FIELD_MEMORY, memoryUsage));

					// Neuen Byte Buffer erstellen
					BYTE buff[vecData.size()];

					// Daten in Vector kopieren
					std::copy(vecData.begin(), vecData.end(), buff);

					// Daten zum Client senden
					_server->Send(clientID, buff, vecData.size());

					break;
				}

				// Anforderung der Festplattennutzung
				case FIELD_DISK:
				{
					// Festplatten-Nutzungs Sring erstellen
					std::string diskUsage = std::to_string(_statusInformation->GetDiskSpace());
					diskUsage += "MB\n";

					// Informationen in Vector Laden
					std::vector<BYTE> vecData (GenerateByteArray(1, FIELD_DISK, diskUsage));

					// Neuen Byte Buffer erstellen
					BYTE buff[vecData.size()];

					// Daten in Vector kopieren
					std::copy(vecData.begin(), vecData.end(), buff);

					// Daten zum Client senden
					_server->Send(clientID, buff, vecData.size());

					break;
				}

				// Anforderung der SSID
				case FIELD_SSID:
				{

					// SSID String erstellen
					std::string SSID = _arduino->GetSSID() + "\n";

					// Informationen in Vector Laden
					std::vector<BYTE> vecData (GenerateByteArray(1, FIELD_SSID, SSID));

					// Neuen Byte Buffer erstellen
					BYTE buff[vecData.size()];

					// Daten in Vector kopieren
					std::copy(vecData.begin(), vecData.end(), buff);

					// Daten zum Client senden
					_server->Send(clientID, buff, vecData.size());

					break;
				}

				// Anforderung der Signal-Stärke
				case FIELD_SIGNAL:
				{
					// Signalstärken String erstellen
					std::string signalStrength = std::to_string(_arduino->GetSignalStrength()) + "\n";

					// Informationen in Vector Laden
					std::vector<BYTE> vecData (GenerateByteArray(1, FIELD_SIGNAL, signalStrength));

					// Neuen Byte Buffer erstellen
					BYTE buff[vecData.size()];

					// Daten in Vector kopieren
					std::copy(vecData.begin(), vecData.end(), buff);

					// Daten zum Client senden
					_server->Send(clientID, buff, vecData.size());
					break;
				}

				// Anforderung der Bandbreite
				case FIELD_BANDWIDTH:
				{
					// Bandbreiten String erstellen
					std::string bandwidth = _arduino->GetBandwidth() + "MBit/s \n";

					// Informationen in Vector Laden
					std::vector<BYTE> vecData (GenerateByteArray(1, FIELD_BANDWIDTH, bandwidth));

					// Neuen Byte Buffer erstellen
					BYTE buff[vecData.size()];

					// Daten in Vector kopieren
					std::copy(vecData.begin(), vecData.end(), buff);

					// Daten zum Client senden
					_server->Send(clientID, buff, vecData.size());
					break;
				}
			}
			break;
		}

		case 3:
		{
			// Daten anpassen und setzten
			UDPClientList[clientID].SetFrameQuality((data[1] < 2) ? 1 : (data[1] > 100) ? 100 : data[1]);
			UDPClientList[clientID].SetFrameSize((data[2] < 2) ? 1 : (data[2] > 100) ? 100 : data[2]);

			break;
		}
	}
}

std::vector<BYTE> TelemetryReceiver::GenerateByteArray(int cmdByte, int fieldID, std::string text)
{
	// Byte Buffer initialisieren
	std::vector<BYTE> buff = std::vector<BYTE>(2);

	// Bytes setzten
	buff[0] = cmdByte;
	buff[1] = fieldID;

	// Neuen Vector für Textdata erstellen
	std::vector<BYTE> textData (text.begin(), text.end());

	// Neue Größe setzen
	buff.reserve(buff.size() + textData.size());

	// TextData-Vector in Buffer einfügen
	buff.insert(buff.end(), textData.begin(), textData.end());

	return buff;
}
