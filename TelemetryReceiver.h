#pragma once

// UDPServer-Klasse
#include "UDPClient.h"

// TCPServer-Klasse
#include "TCPServer.h"

// StatusInformation-Klasse
#include "StatusInformation.h"

// OpenCV implementieren
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

// Enhält das Vector-Element
#include <vector>

// C++-thread-Klasse
#include <thread>

// Enhält Funktionen für Sockets
#include <sys/socket.h>

// Enhält das Map-Element
#include <map>

#define BYTE char

#define STATUS_CONNECT 0
#define STATUS_DISCONNECT 1

#define FIELD_CPU 0
#define FIELD_MEMORY 1
#define FIELD_DISK 2
#define FIELD_SSID 3
#define FIELD_SIGNAL 4
#define FIELD_BANDWIDTH 5

#define CAMERA_WIDTH 1280
#define CAMERA_HEIGHT 720

namespace THOMAS {

	class TelemetryReceiver
	{
	private:
		// Socket Struktur
		struct sockaddr_in;

		// Der TCP-Server.
		TCPServer *_server;

		// StatusInformations Klasse
		StatusInformation *_statusInformation;

		// Aufnahme Thread
		std::thread *_captureFrameThread;

		// VideoCapture Device
		cv::VideoCapture _videoCapture;

		// Liste mit ClientIDs
		std::map<int, UDPClient> UDPClientList;

		// Array mit den Parametern für OpenCV
		std::vector<int> param = std::vector<int>(2);

		// TCP Daten verarbeiten
		void ComputeTCPServerData(BYTE *data, int dataLength, int clientID);

		// Neuer Client verbunden
		void OnClientStatusChange(int clientID, int status, const char* ip);

		// Nimmt Frames im seperaten Thread auf
		void CaptureFrameThread();

		// Erstellt ein Byte Array
		std::vector<BYTE> GenerateByteArray(int cmdByte, int fieldID, std::string text);

		// Wrapper, um die ComputeClientCommand-Memberfunktion sauber an den TCP-Server zu übergeben.
		// Parameter:
		// -> data: Die vom Server empfangenen Daten.
		// -> dataLength: Die Länge der vom Server empfangenen Daten.
		// -> obj: Das zur ComputeClientCommand-Funktion gehörende MotorControl-Objekt.
		// -> clientID: ClientID jedes Clients zur Referenzierung
		static void ComputeTCPServerDataWrapper(BYTE *data, int dataLength, void *obj, int clientID)
		{
			(reinterpret_cast<TelemetryReceiver *>(obj))->ComputeTCPServerData(data, dataLength, clientID);
		}

		// Wraper, um die OnClientStatusChange Funktion sauber an den TCP-Server zu übergeben
		static void OnClientStatusChangeWrapper(int clientID, int status, void *obj, const char *ip)
		{
			(reinterpret_cast<TelemetryReceiver *>(obj))->OnClientStatusChange(clientID, status, ip);
		}

	public:
		// Konstruktor
		TelemetryReceiver();

		// Wird beim Start ausgeführt
		void Run(int videoDeviceID = 0);

	};
}
