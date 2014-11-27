CPPARGS= -std=c++11 -pthread
LINKERARGS= -pthread

all: main.o RS232.o ArduinoCom.o TCPServer.o MotorControl.o THOMASException.o
	g++ THOMASException.o RS232.o ArduinoCom.o TCPServer.o MotorControl.o main.o -o thomas $(LINKERARGS)

test: test.o TCPServer.o
	g++ TCPServer.o test.o -o test $(LINKERARGS)

test.o: test.cpp
	g++ -c test.cpp $(CPPARGS)

main.o: main.cpp
	g++ -c main.cpp $(CPPARGS)

RS232.o: RS232.cpp RS232.h
	g++ -c RS232.cpp $(CPPARGS)

ArduinoCom.o: ArduinoCom.cpp ArduinoCom.h
	g++ -c ArduinoCom.cpp $(CPPARGS)

TCPServer.o: TCPServer.cpp TCPServer.h
	g++ -c TCPServer.cpp $(CPPARGS)

MotorControl.o: MotorControl.cpp MotorControl.h
	g++ -c MotorControl.cpp $(CPPARGS)

THOMASException.o: THOMASException.cpp THOMASException.h
	g++ -c THOMASException.cpp $(CPPARGS)

clean:
	rm -rf ./*.o thomas test
