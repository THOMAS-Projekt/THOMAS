CPPARGS= -std=c++11 -pthread
CFLAGS= `pkg-config --cflags opencv`
LIBS= `pkg-config --libs opencv`
LINKERARGS= -pthread

all: main.o RS232.o TCPServer.o MotorControl.o THOMASException.o tracking.o
	g++ THOMASException.o RS232.o TCPServer.o MotorControl.o main.o -o thomas-server $(LINKERARGS) $(LIBS)

test: test.o TCPServer.o
	g++ TCPServer.o test.o -o test-server $(LINKERARGS) $(LIBS)

test.o: test.cpp
	g++ -c test.cpp $(CPPARGS) $(CFLAGS)

main.o: main.cpp
	g++ -c main.cpp $(CPPARGS) $(CFLAGS)

RS232.o: RS232.cpp RS232.h
	g++ -c RS232.cpp $(CPPARGS) $(CFLAGS)

TCPServer.o: TCPServer.cpp TCPServer.h
	g++ -c TCPServer.cpp $(CPPARGS) $(CFLAGS)

MotorControl.o: MotorControl.cpp MotorControl.h
	g++ -c MotorControl.cpp $(CPPARGS) $(CFLAGS)

tracking.o: tracking.cpp tracking.h
	g++ -c tracking.cpp $(CPPARGS) $(CFLAGS)

THOMASException.o: THOMASException.cpp THOMASException.h
	g++ -c THOMASException.cpp $(CPPARGS) $(CFLAGS)

clean:
	rm -rf ./*.o thomas test
