#include "../tcpSocket/tcpSocket.h"


int main()
{
	init_Socket();
	SOCKET serfd = createServerSocket();
	
	cout << "�ȴ�����..." << endl;
	HANDLE controlTrd = CreateThread(NULL, 0, ControlClt, (LPVOID)sck, 0, NULL);
	while (true) {
		SOCKET clifd = accept(serfd, NULL, NULL);
		if (INVALID_SOCKET != clifd) {                     //�����߳� ���Ҵ�����clientͨѶ���׽���
			HANDLE hThread = CreateThread(NULL, 0, ThreadFun, (LPVOID)clifd, 0, NULL);
			CloseHandle(hThread);                          //�رն��̵߳�����
		}
	}
	CloseHandle(controlTrd);
	closesocket(serfd);
	close_Socket();
	cout << "===========server closed==========" << endl;
	return 0;
}