CPPARGS= -std=c++11

all: main.o RS232.o TCPServer.o MotorControl.o
	g++ RS232.o TCPServer.o MotorControl.o main.o -o thomas

test: test.o TCPServer.o
	g++ TCPServer.o test.o -o test

test.o: test.cpp
	g++ -c test.cpp ${CPPARGS}

main.o: main.cpp
	g++ -c main.cpp ${CPPARGS}

RS232.o: RS232.cpp RS232.h
	g++ -c RS232.cpp ${CPPARGS}

TCPServer.o: TCPServer.cpp TCPServer.h
	g++ -c TCPServer.cpp ${CPPARGS}

MotorControl.o: MotorControl.cpp MotorControl.h
	g++ -c MotorControl.cpp ${CPPARGS}
	
clean:
	rm -rf ./*.o thomas test