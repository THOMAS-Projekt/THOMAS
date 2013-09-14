#include <iostream>

#include "TCPServer.h"

void compute(BYTE *data, int len)
{
	for(int i = 0; i < len; i++)
		std::cout << data[i] << std::endl;
}

int main()
{
	THOMAS::TCPServer *s = new THOMAS::TCPServer(12345, compute);
	s->BeginListen();
	
	while(true);
	
	return 0;
}