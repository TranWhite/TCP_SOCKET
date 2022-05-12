#include "../tcpSocket/tcpSocket.h"
#include <stdio.h>
#include <iostream>
using namespace std;

int main()
{
	init_Socket();
	SOCKET fd = createClientSocket("127.0.0.1");
	clientChat(fd);
	closesocket(fd);
	close_Socket();
	cout << "===========client closed==========" << endl;
	getchar();
	return 0;
}