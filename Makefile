CPPARGS= -std=c++11

all: main.o rs232.o socket.o
	g++ rs232.o socket.o main.o -o server

test: test.o socket.o
	g++ socket.o test.o -o test

test.o: test.cpp
	g++ -c test.cpp ${CPPARGS}

main.o: main.cpp
	g++ -c main.cpp ${CPPARGS}

rs232.o: rs232.cpp rs232.h
	g++ -c rs232.cpp ${CPPARGS}

socket.o: socket.cpp socket.h
	g++ -c socket.cpp ${CPPARGS}

thread.o: thread.cpp thread.h
	g++ -c thread.cpp ${CPPARGS}