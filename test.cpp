#include <iostream>

#include "socket.h"

void compute(BYTE *data, int len)
{
	for(int i = 0; i < len; i++)
		std::cout << data[i] << std::endl;
}

int main()
{
	THOMAS::Socket *s = new THOMAS::Socket(12345, compute);
	s->BeginListen();
	
	while(true);
	
	return 0;
}