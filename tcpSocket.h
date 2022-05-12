#pragma once
#include <stdio.h>
#include <iostream>
#include <WinSock2.h>
#include <cstring>
#include <string.h>
#include <WS2tcpip.h>
using namespace std;
#pragma comment(lib,"ws2_32.lib")

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define PORT 8000
#define MAX_CLIENT_NUM 10
#define MAX_MSG_NUM 20
#define MAX_ID_NUM 22

extern int curClientNum;                                      // ��¼�ͻ�������
extern int curMsgNum;
extern SOCKET sck[MAX_CLIENT_NUM];                            // �洢�ͻ���socket
extern char clientMsg[MAX_MSG_NUM * 3][MAX_ID_NUM];

string to_String(int n);
#define error(errMsg) ::cout << "[line:" << __LINE__ << "]" << errMsg << " failed code " << WSAGetLastError() << endl;
bool init_Socket();
bool close_Socket();
SOCKET createServerSocket();
SOCKET createClientSocket(const char* ip);

void clientChat(SOCKET &fd);                                  // ִ�пͻ������е�ͨ�Ų���
void Output();                                                // չʾ��ǰ�������߿ͻ���

char* getId(SOCKET& fd);
bool loginCheck(SOCKET& clifd);                               // ��½��֤
bool registerMsg(SOCKET& clifd);
bool welcomMenu(SOCKET& clifd);
DWORD WINAPI ThreadFun
(LPVOID IpThreadParameter);             // ����ˣ��½��߳���ͻ��˽���ͨ��
DWORD WINAPI ControlClt(LPVOID sckArr);                       // ����ˣ���������˵Ĳ���
DWORD WINAPI WaitingforMsgFromServer(LPVOID cli);             // �ͻ��ˣ��ȴ����շ���˵���Ϣ