#include "../tcpSocket/tcpSocket.h"


int main()
{
	init_Socket();
	SOCKET serfd = createServerSocket();
	
	cout << "等待连接..." << endl;
	HANDLE controlTrd = CreateThread(NULL, 0, ControlClt, (LPVOID)sck, 0, NULL);
	while (true) {
		SOCKET clifd = accept(serfd, NULL, NULL);
		if (INVALID_SOCKET != clifd) {                     //创建线程 并且传入与client通讯的套接字
			HANDLE hThread = CreateThread(NULL, 0, ThreadFun, (LPVOID)clifd, 0, NULL);
			CloseHandle(hThread);                          //关闭对线程的引用
		}
	}
	CloseHandle(controlTrd);
	closesocket(serfd);
	close_Socket();
	cout << "===========server closed==========" << endl;
	return 0;
}