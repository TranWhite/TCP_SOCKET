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

extern int curClientNum;                                      // 记录客户端数量
extern int curMsgNum;
extern SOCKET sck[MAX_CLIENT_NUM];                            // 存储客户端socket
extern char clientMsg[MAX_MSG_NUM * 3][MAX_ID_NUM];

string to_String(int n);
#define error(errMsg) ::cout << "[line:" << __LINE__ << "]" << errMsg << " failed code " << WSAGetLastError() << endl;
bool init_Socket();
bool close_Socket();
SOCKET createServerSocket();
SOCKET createClientSocket(const char* ip);

void clientChat(SOCKET &fd);                                  // 执行客户端所有的通信操作
void Output();                                                // 展示当前所有在线客户端

char* getId(SOCKET& fd);
bool loginCheck(SOCKET& clifd);                               // 登陆验证
bool registerMsg(SOCKET& clifd);
bool welcomMenu(SOCKET& clifd);
DWORD WINAPI ThreadFun
(LPVOID IpThreadParameter);             // 服务端：新建线程与客户端进行通信
DWORD WINAPI ControlClt(LPVOID sckArr);                       // 服务端：监听服务端的操作
DWORD WINAPI WaitingforMsgFromServer(LPVOID cli);             // 客户端：等待接收服务端的消息