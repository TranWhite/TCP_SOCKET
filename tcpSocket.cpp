#include "tcpSocket.h"


int curClientNum = 0;                                                  // 记录客户端数量
int curMsgNum = 0;
SOCKET sck[MAX_CLIENT_NUM] = { 0 };                                    // 存储客户端socket
char clientMsg[MAX_MSG_NUM * 3][MAX_ID_NUM] = { 0 };

string to_String(int n)
{
	int m = n;
	char s[MAX_MSG_NUM];
	char ss[MAX_MSG_NUM];
	int i = 0, j = 0;
	if (n < 0)// 处理负数
	{
		m = 0 - m;
		j = 1;
		ss[0] = '-';
	}
	while (m > 0)
	{
		s[i++] = m % 10 + '0';
		m /= 10;
	}
	s[i] = '\0';
	i = i - 1;
	while (i >= 0)
	{
		ss[j++] = s[i--];
	}
	ss[j] = '\0';
	return ss;
}

// 打开、关闭网络库
bool init_Socket()
{
	//wsa Windows socket async 异步套接字
	//1：请求的socket版本  2：传出的参数
	//MAKEWORD 转换为一个word
	WSADATA wsadata;
	if (0 != WSAStartup(MAKEWORD(2, 2), &wsadata)) {
		error("WSAStartup");
		return false;
	}
	return true;
}

bool close_Socket()
{
	if (0 != WSACleanup()) {
		cout << "WSACleanup failed code " << WSAGetLastError() << endl;
		return false;
	}
	return true;
}

SOCKET createServerSocket()
{
	// 1.创建空socket
	// af 地址协议族 ipv4 ipv6
	// type 传输协议类型 流式套接字tcp 数据报udp
	// protocol 使用具体的某个传输协议
	// fd 描述符
	SOCKET fd=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP); 
	if (fd == INVALID_SOCKET) {  // 失败
		error("socket");
		return INVALID_SOCKET;
	}
	// 2.绑定本机ip和端口号
	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(PORT);  //转换为网络字节序 （大小端）
	addr.sin_addr.S_un.S_addr = ADDR_ANY; //ADDR_ANY 绑定到任任意端口  //inet_addr("127.0.0.1"); // inet_addr 将点分十进制转换为二进制
	if (SOCKET_ERROR == bind(fd, (SOCKADDR*)&addr, sizeof(addr))) {
		error("bind");
		return false;
	}
	// 3.监听
	listen(fd, 10);
	return fd;
}

SOCKET createClientSocket(const char *ip)
{
	// 1.创建空socket
	SOCKET fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (fd == INVALID_SOCKET) {  // 失败
		error("socket");
		return INVALID_SOCKET;
	}
	// 2.连接服务器
	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(PORT);  //转换为网络字节序 （大小端）
	inet_pton(AF_INET, ip, &addr.sin_addr);
	if (connect(fd, (SOCKADDR*)&addr, sizeof(addr)) == INVALID_SOCKET) {
		error("connect");
		return false;
	}
	return fd;
}

// 注册
bool registerMsg(SOCKET& clifd)
{
	string userid;
	string userkey;
	string wrongtag;
	char recvbuf[BUFSIZ] = { 0 };
	char sendbuf[BUFSIZ] = { 0 };
	int count = 0;

	::memset(recvbuf, 0, sizeof(recvbuf));
	::memset(sendbuf, 0, sizeof(sendbuf));
	strcpy_s(recvbuf, "请输入您的用户名：（不超过20个字符）");
	send(clifd, recvbuf, strlen(recvbuf), 0);
	if (recv(clifd, sendbuf, BUFSIZ, 0) > 0) {
		userid = sendbuf;
	}
	while (count < 3) {
		wrongtag = wrongtag + "请输入您的密码：（不超过20个字符）";
		::memset(recvbuf, 0, sizeof(recvbuf));
		::memset(sendbuf, 0, sizeof(sendbuf));
		strcpy_s(recvbuf, wrongtag.c_str());
		send(clifd, recvbuf, strlen(recvbuf), 0);
		if (recv(clifd, sendbuf, BUFSIZ, 0) > 0) {
			userkey = sendbuf;
		}
		::memset(recvbuf, 0, sizeof(recvbuf));
		::memset(sendbuf, 0, sizeof(sendbuf));
		strcpy_s(recvbuf, "请再次输入您的密码：");
		send(clifd, recvbuf, strlen(recvbuf), 0);
		if (recv(clifd, sendbuf, BUFSIZ, 0) > 0) {
			if (userkey == sendbuf) {                            // 如果两次输入相同，记录密码
				if (curMsgNum < MAX_MSG_NUM) {
					strcpy_s(clientMsg[curMsgNum * 3], userid.c_str());
					strcpy_s(clientMsg[curMsgNum * 3 + 1], userkey.c_str());
					curMsgNum++;
					return true;
				}
				else {
					::cout << "msg数组已满！" << endl;
					break;
				}
			}
			else {
				wrongtag = "两次密码输入不一致，请重新输入！\n";       // 如果两次输入不同，发送错误信息，重新输入
				count++;
			}
		}
	}	
	return false;
}

// 登陆验证
bool loginCheck(SOCKET& clifd)
{
	// 登陆验证
	int count = 0;
	string userid;
	string userkey;
	string wrongtag;
	char recvbuf[BUFSIZ] = { 0 };
	char sendbuf[BUFSIZ] = { 0 };
	while (count < 3) {
		//发送消息
		wrongtag = wrongtag + "请登录：\n请输入用户名：";
		strcpy_s(recvbuf, wrongtag.c_str());
		if (send(clifd, recvbuf, strlen(recvbuf), 0) == SOCKET_ERROR) {
			error("send");
		}
		// 从指定客户端接收消息
		if (recv(clifd, sendbuf, BUFSIZ, 0) > 0) {
			userid = sendbuf;
			::memset(sendbuf, 0, sizeof(sendbuf));
		}
		//发送消息
		::memset(recvbuf, 0, sizeof(recvbuf));
		strcpy_s(recvbuf, "请输入密码：");
		if (send(clifd, recvbuf, strlen(recvbuf), 0) == SOCKET_ERROR) {
			error("send");
		}
		// 从指定客户端接收消息
		if (recv(clifd, sendbuf, BUFSIZ, 0) > 0) {
			userkey = sendbuf;
			::memset(sendbuf, 0, sizeof(sendbuf));
		}
		for (int i = 0; i < 3 * curMsgNum; i += 3) {
			if (strcmp(userid.c_str(), clientMsg[i]) == 0 && strcmp(userkey.c_str(), clientMsg[i + 1]) == 0) {
				cout << "111111" << endl;
				int c = (int)clifd;
				cout << c << endl;
				strcpy_s(clientMsg[i + 2], to_String(c).c_str());
				cout << "222222" << endl;
				::memset(recvbuf, 0, sizeof(recvbuf));
				strcpy_s(recvbuf, "登陆成功！");
				if (send(clifd, recvbuf, strlen(recvbuf), 0) == SOCKET_ERROR) {
					error("send");
				}
				return true;
			}
		}
		wrongtag = "用户名或密码错误！\n";
		count++;
	}
	::memset(recvbuf, 0, sizeof(recvbuf));
	strcpy_s(recvbuf, "已达三次输入上限，不可登录！\n");
	if (send(clifd, recvbuf, strlen(recvbuf), 0) == SOCKET_ERROR) {
		error("send");
	}
	return false;
}

bool welcomMenu(SOCKET& clifd)
{
	char recvbuf[BUFSIZ] = { 0 };
	char sendbuf[BUFSIZ] = { 0 };
	strcpy_s(recvbuf, "请选择：1.登录 2.注册\n");
	if (send(clifd, recvbuf, strlen(recvbuf), 0) == SOCKET_ERROR) {
		error("send");
	}
	if (recv(clifd, sendbuf, BUFSIZ, 0) > 0) {
		if (strcmp(sendbuf, "1") == 0) {          // 登录
			if (loginCheck(clifd))
				return true;
		}
		else if (strcmp(sendbuf, "2") == 0) {     // 注册
			if (registerMsg(clifd)) {
				if (loginCheck(clifd))
					return true;
			}
			else
				return false;
		}
		else {
			strcpy_s(recvbuf, "错误输入！");
			send(clifd, recvbuf, strlen(recvbuf), 0);
		}
	}
	return false;
}

// 客户端：等待接收服务端的消息
DWORD WINAPI WaitingforMsgFromServer(LPVOID cli)
{
	//cout << "hellohello?" << endl;
	SOCKET clifd = (SOCKET)cli;
	char sendbuf[BUFSIZ] = { 0 };
	int recvTag = recv(clifd, sendbuf, BUFSIZ, 0);
	while (recvTag != SOCKET_ERROR && recvTag > 0) {
		memset(sendbuf, 0, sizeof(sendbuf));
		recvTag = recv(clifd, sendbuf, BUFSIZ, 0);
		cout << sendbuf << endl;
	}
	return 0;
}

// 执行客户端所有的通信操作
void clientChat(SOCKET& fd)
{
	char recvbuf[BUFSIZ] = "连接成功！";
	char sendbuf[BUFSIZ] = { 0 };
	
	// 登陆验证
	while (strcmp(sendbuf, "登陆成功！") != 0) {
		if (strcmp(recvbuf, "exit") == 0)
			return;
		// 从服务器端接收消息
		memset(sendbuf, 0, sizeof(sendbuf));
		int recvTag = recv(fd, sendbuf, BUFSIZ, 0);
		if (recvTag > 0) {
			//cout << "recv: " << sendbuf << endl;
			cout << sendbuf << endl;
			if (strcmp(sendbuf, "已达三次输入上限，不可登录！\n") == 0) {
				return;
			}
			if (strcmp(sendbuf, "登陆成功！") == 0) {
				break;
			}
			// 向服务器发送消息
			cout << " > ";
			::memset(recvbuf, 0, sizeof(recvbuf));
			gets_s(recvbuf, BUFSIZ);
			int tag = send(fd, recvbuf, strlen(recvbuf), 0);
			if (tag == SOCKET_ERROR) { error("send"); }
		}
		else {
			cout << "登陆失败！已自动退出..." << endl;
			return;
		}
	}
	// 接收服务端消息
	HANDLE WaitServerTrd = CreateThread(NULL, 0, WaitingforMsgFromServer, (LPVOID)fd, 0, NULL);
	// 发送消息
	while (strcmp(recvbuf, "exit") != 0) {
		cout << " > ";
		::memset(recvbuf, 0, sizeof(recvbuf));
		gets_s(recvbuf, BUFSIZ);
		int tag = send(fd, recvbuf, strlen(recvbuf), 0);
		if (tag == SOCKET_ERROR) {
			error("send");
			break;
		}
	}
	CloseHandle(WaitServerTrd);
}

// 展示当前所有在线客户端
void Output()
{
	cout << endl;
	cout << "============================当前连接数:" << curClientNum << "============================" << endl;
	cout << "当前连接的客户端:  ";
	for (int i = 0; i <MAX_CLIENT_NUM; i++)
	{
		if (sck[i] != NULL) {
			cout << sck[i] << "  ";
		}
	}
	cout << endl;
	cout << "====================================================================" << endl;
	cout << endl;
}

void Test(SOCKET &fd, int &tag)
{
	if (tag == SOCKET_ERROR) {                  // 如果发送失败，删除这个客户端
		cout << "send message to " << fd << " fail! deleting it..." << endl;
		closesocket(fd);
		for (int i = 0; i < MAX_CLIENT_NUM; i++) {
			if (sck[i] == fd) {
				sck[i] = NULL;
				curClientNum--;
			}
		}
		cout << "删除 " << fd << " 成功！" << endl;
	}
}

char* getId(SOCKET& fd)
{
	int fds = (int)fd;
	//char suf[] = " 说： ";
	for (int i = 0; i < 3 * curMsgNum; i += 3) {
		if (strcmp(clientMsg[i + 2], to_String(fds).c_str())==0) {
			cout << "fds:" << fds << endl;
			cout << "clientMsg[i + 2]" << clientMsg[i + 2] << endl;
			//if (id != nullptr) {
			//	sprintf_s(id, sizeof(clientMsg[i]), clientMsg[i], sizeof(suf), suf);
			//}
			return clientMsg[i];
		}
	}
	return nullptr;
}

// 服务端：新建线程与客户端进行通信
DWORD WINAPI ThreadFun(LPVOID IpThreadParameter)
{
	char recvbuf[BUFSIZ] = "SUCCESS";
	char sendbuf[BUFSIZ] = { 0 };
	SOCKET clifd = (SOCKET)IpThreadParameter;
	memset(recvbuf, 0, sizeof(recvbuf));

	//登陆验证
	if (welcomMenu(clifd) == true) {
		// sck记录新客户端
		int cur;
		for (cur = 0; cur < MAX_CLIENT_NUM; cur++) {
			if (sck[cur] == NULL) {
				sck[cur] = clifd;
				break;
			}
		}
		curClientNum++;
		char* id = getId(clifd);
		printf("欢迎 %s 进入聊天室！\s", id);
		Output();

		// 接受客户端消息
		int ret = 0;
		do
		{
			ret = recv(clifd, sendbuf, BUFSIZ, 0);
			if (strcmp(sendbuf, "exit") == 0) { // 客户端关闭连接
				cout << clifd << " 退出了聊天室" << endl;
				break;
			}
			if (ret == SOCKET_ERROR) { 
				closesocket(clifd);
				return 1;
			}
			else { // 成功接收消息
				for (int i = 0; i < MAX_CLIENT_NUM; i++) {          // 转发消息//////////////////////////////////////////////////////
					if (sck[i] != NULL && sck[i] != clifd) {
						send(sck[i], id, strlen(id), 0);
						int tag = send(sck[i], sendbuf, strlen(sendbuf), 0);
						Test(sck[i], tag);
						if (tag <= 0) {
							cout << "tag==0" << endl;
						}
					}
				}
				printf(" %s (%d)说：%s\n", id, clifd, sendbuf);    // 聊天室更新信息
				//cout << id << "  说： " << sendbuf << endl;       
				memset(sendbuf, 0, sizeof(sendbuf));
			}
		} while (ret > 0);
		sck[cur] = NULL;
		curClientNum--;
		Output();
	}

	return 0;
}

// 服务端：新建线程与客户端进行通信
DWORD WINAPI ControlClt(LPVOID sckArr)
{
	strcpy_s(clientMsg[curMsgNum], "John");
	strcpy_s(clientMsg[curMsgNum + 1], "John666");
	curMsgNum++;

	SOCKET* client = (SOCKET*)sckArr;
	char ch[10];
	//int ch = 0;
	while (true) {
		cin >> ch;
		if (strcmp(ch,"0")==0) {
			cout << "1.删除客户端" << endl;
			cout << "2.向指定客户端发送消息" << endl;
			cout << "3.向全体客户端发送消息" << endl;
			cout << "4.展示当前在线用户" << endl;
			cout << "5.展示用户信息表" << endl;
		}
		// 删除客户端
		else if (strcmp(ch, "1") == 0) {
			cout << "删除谁" << endl;
			SOCKET delNum;
			cin >> delNum;
			closesocket(delNum);
			for (int i = 0; i < MAX_CLIENT_NUM; i++) {
				if (sck[i] == delNum) {
					sck[i] = NULL;
				}
			}
			curClientNum--;
			//cout << "删除 " << delNum << " 成功！" << endl;

			Output();
		}
		// 向指定客户端发送消息
		else if (strcmp(ch, "2") == 0) {
			SOCKET cli;
			cout << "向哪位用户发送消息：" << endl;
			cin >> cli;
			char sbuf[BUFSIZ] = { 0 };
			char idbuf[BUFSIZ] = { 0 };
			int tag = 0;
			while (tag != SOCKET_ERROR && strcmp(sbuf, "exit") != 0) {
				cout << "向 " << cli << " 发送：";
				memset(sbuf, 0, sizeof(sbuf));
				gets_s(sbuf, BUFSIZ);
				if (strcmp(sbuf, "exit") == 0)
					break;
				strcpy_s(idbuf, "服务端: ");
				send(cli, idbuf, strlen(idbuf), 0);
				tag = send(cli, sbuf, strlen(sbuf), 0);               // 发送信息
				Test(cli, tag);
			}
		}
		// 向全体客户端发送消息
		else if (strcmp(ch, "3") == 0) {
			char sbuf[BUFSIZ] = { 0 };
			char idbuf[BUFSIZ] = { 0 };
			int tag = 0;
			while (tag != SOCKET_ERROR && strcmp(sbuf, "exit") != 0) {
				cout << "向所有客户端发送:";  // 第一次会输出两边提示语
				memset(sbuf, 0, sizeof(sbuf));
				gets_s(sbuf, BUFSIZ);
				if (strcmp(sbuf, "exit") == 0)
					break;
				for (int i = 0; i < MAX_CLIENT_NUM; i++) {
					if (sck[i] != NULL) {
						strcpy_s(idbuf, "服务端: ");
						send(sck[i], idbuf, strlen(idbuf), 0);
						int tag = send(sck[i], sbuf, strlen(sbuf), 0);
						Test(sck[i], tag);
					}
				}	
			}
		}
		// 展示当前在线用户
		else if (strcmp(ch, "4") == 0) {
			Output();
		}
		else if (strcmp(ch, "5") == 0) {
			cout << "curMsgNum: " << curMsgNum << endl;
			for (int i = 0; i < 3 * curMsgNum; i++) {
				cout << clientMsg[i] << endl;
			}
		}
		else {
			cout << "无效输入" << endl;
		}
	}

	client = NULL;
	return 0;

}
