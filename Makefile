CPPARGS= -std=c++11 -pthread
OPENCVARGS= `pkg-config --cflags opencv`
LINKERARGS= -pthread -g -O0 -v -da -Q `pkg-config --libs opencv`

all: main.o RS232.o ArduinoCom.o ArduinoProtocol.o TCPServer.o MotorControl.o THOMASException.o TelemetryReceiver.o UDPClient.o StatusInformation.o CollisionDetection.o
	g++ $(OPENCVARGS) THOMASException.o RS232.o ArduinoCom.o ArduinoProtocol.o TCPServer.o MotorControl.o TelemetryReceiver.o UDPClient.o StatusInformation.o main.o CollisionDetection.o -o thomas $(LINKERARGS)

main.o: main.cpp
	g++ -c main.cpp $(CPPARGS)

test: test.o TCPServer.o
	g++ TCPServer.o test.o -o test $(LINKERARGS)

test.o: test.cpp
	g++ -c test.cpp $(CPPARGS)

RS232.o: RS232.cpp RS232.h
	g++ -c RS232.cpp $(CPPARGS)

ArduinoCom.o: ArduinoCom.cpp ArduinoCom.h
	g++ -c ArduinoCom.cpp $(CPPARGS)

ArduinoProtocol.o: ArduinoProtocol.cpp ArduinoProtocol.h
	g++ -c ArduinoProtocol.cpp $(CPPARGS)

TCPServer.o: TCPServer.cpp TCPServer.h
	g++ -c TCPServer.cpp $(CPPARGS)

MotorControl.o: MotorControl.cpp MotorControl.h
	g++ -c MotorControl.cpp $(CPPARGS)

THOMASException.o: THOMASException.cpp THOMASException.h
	g++ -c THOMASException.cpp $(CPPARGS)

TelemetryReceiver.o: TelemetryReceiver.cpp TelemetryReceiver.h
	g++ -c TelemetryReceiver.cpp $(CPPARGS)

UDPClient.o: UDPClient.cpp UDPClient.h
	g++ -c UDPClient.cpp $(CPPARGS)

StatusInformation.o: StatusInformation.cpp StatusInformation.h
	g++ -c StatusInformation.cpp $(CPPARGS)

CollisionDetection.o: CollisionDetection.cpp CollisionDetection.h
	g++ -c CollisionDetection.cpp $(CPPARGS)

clean:
	rm -rf ./*.o thomas test
