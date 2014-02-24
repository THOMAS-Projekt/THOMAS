CPPARGS= -std=c++11 -pthread
CFLAGS= `pkg-config --cflags opencv`
LIBS= `pkg-config --libs opencv`
LINKERARGS= -pthread

all: main.o RS232.o TCPServer.o MotorControl.o THOMASException.o Tracking.o
	g++ THOMASException.o RS232.o TCPServer.o MotorControl.o main.o -o thomas-server $(LINKERARGS)

test: test.o TCPServer.o
	g++ TCPServer.o test.o -o test-server $(LINKERARGS)

test.o: test.cpp
	g++ $(CFLAGS) $(LIBS) -c test.cpp $(CPPARGS)

main.o: main.cpp
	g++ $(CFLAGS) $(LIBS) -c main.cpp $(CPPARGS)

RS232.o: RS232.cpp RS232.h
	g++ $(CFLAGS) $(LIBS) -c RS232.cpp $(CPPARGS)

TCPServer.o: TCPServer.cpp TCPServer.h
	g++ $(CFLAGS) $(LIBS) -c TCPServer.cpp $(CPPARGS)

MotorControl.o: MotorControl.cpp MotorControl.h
	g++ $(CFLAGS) $(LIBS) -c MotorControl.cpp $(CPPARGS)

Tracking.o: Tracking.cpp Tracking.h
	g++ $(CFLAGS) $(LIBS) -c Tracking.cpp $(CPPARGS)

THOMASException.o: THOMASException.cpp THOMASException.h
	g++ $(CFLAGS) $(LIBS) -c THOMASException.cpp $(CPPARGS)

clean:
	rm -rf ./*.o thomas test