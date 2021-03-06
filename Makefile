CPPARGS= -std=c++11 -pthread
OPENCVARGS= `pkg-config --cflags opencv`
LINKERARGS= -pthread -g -O0 -v -da -Q `pkg-config --libs opencv`

all: main.o RS232.o ArduinoCom.o ArduinoProtocol.o TCPServer.o MotorControl.o THOMASException.o LaserMeasurement.o TelemetryReceiver.o UDPClient.o StatusInformation.o CollisionDetection.o RelayProtocol.o
	g++ $(OPENCVARGS) THOMASException.o RS232.o ArduinoCom.o ArduinoProtocol.o TCPServer.o MotorControl.o LaserMeasurement.o TelemetryReceiver.o UDPClient.o StatusInformation.o main.o CollisionDetection.o RelayProtocol.o -o thomas $(LINKERARGS)

scanner: scanner.o RS232.o ArduinoCom.o ArduinoProtocol.o TCPServer.o MotorControl.o THOMASException.o LaserMeasurement.o TelemetryReceiver.o UDPClient.o StatusInformation.o CollisionDetection.o RelayProtocol.o
	g++ $(OPENCVARGS) THOMASException.o RS232.o ArduinoCom.o ArduinoProtocol.o TCPServer.o MotorControl.o LaserMeasurement.o TelemetryReceiver.o UDPClient.o StatusInformation.o scanner.o CollisionDetection.o RelayProtocol.o -o scanner $(LINKERARGS)

main.o: main.cpp
	g++ -c main.cpp $(CPPARGS)

scanner.o: scanner.cpp
	g++ -c scanner.cpp $(CPPARGS)

test: test.o TCPServer.o
	g++ TCPServer.o test.o -o test $(LINKERARGS)

mct: MotorControlTest.o RS232.o THOMASException.o
	g++ MotorControlTest.o RS232.o THOMASException.o -o mct $(LINKERARGS)

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

LaserMeasurement.o: LaserMeasurement.cpp LaserMeasurement.h
	g++ -c LaserMeasurement.cpp $(CPPARGS)

TelemetryReceiver.o: TelemetryReceiver.cpp TelemetryReceiver.h
	g++ -c TelemetryReceiver.cpp $(CPPARGS)

UDPClient.o: UDPClient.cpp UDPClient.h
	g++ -c UDPClient.cpp $(CPPARGS)

StatusInformation.o: StatusInformation.cpp StatusInformation.h
	g++ -c StatusInformation.cpp $(CPPARGS)

CollisionDetection.o: CollisionDetection.cpp CollisionDetection.h
	g++ -c CollisionDetection.cpp $(CPPARGS)

RelayProtocol.o: RelayProtocol.cpp RelayProtocol.h 
	g++ -c RelayProtocol.cpp $(CPPARGS)

MotorControlTest.o: MotorControlTest.cpp
	g++ -c MotorControlTest.cpp $(CPPARGS)

clean:
	rm -rf ./*.o thomas mct test
