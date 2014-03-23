CPPARGS= -std=c++11 -pthread
LIBS= `pkg-config --cflags --libs opencv`
LINKERARGS= -pthread

all: main.o RS232.o TCPServer.o MotorControl.o THOMASException.o tracking.o
	g++ THOMASException.o RS232.o TCPServer.o MotorControl.o main.o -o thomas-server $(LINKERARGS) $(LIBS)

test: test.o TCPServer.o
	g++ TCPServer.o test.o -o test-server $(LINKERARGS) $(LIBS)

test.o: test.cpp
	g++ -c test.cpp $(CPPARGS)

main.o: main.cpp
	g++ -c main.cpp $(CPPARGS)

RS232.o: RS232.cpp RS232.h
	g++ -c RS232.cpp $(CPPARGS)

TCPServer.o: TCPServer.cpp TCPServer.h
	g++ -c TCPServer.cpp $(CPPARGS)

MotorControl.o: MotorControl.cpp MotorControl.h
	g++ -c MotorControl.cpp $(CPPARGS)

tracking.o: Tracking.cpp Tracking.h
	g++ -c Tracking.cpp $(CPPARGS)

THOMASException.o: THOMASException.cpp THOMASException.h
	g++ -c THOMASException.cpp $(CPPARGS)

clean:
	rm -rf ./*.o thomas test
