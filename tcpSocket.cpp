#include "tcpSocket.h"


int curClientNum = 0;                                                  // ��¼�ͻ�������
int curMsgNum = 0;
SOCKET sck[MAX_CLIENT_NUM] = { 0 };                                    // �洢�ͻ���socket
char clientMsg[MAX_MSG_NUM * 3][MAX_ID_NUM] = { 0 };

string to_String(int n)
{
	int m = n;
	char s[MAX_MSG_NUM];
	char ss[MAX_MSG_NUM];
	int i = 0, j = 0;
	if (n < 0)// ������
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

// �򿪡��ر������
bool init_Socket()
{
	//wsa Windows socket async �첽�׽���
	//1�������socket�汾  2�������Ĳ���
	//MAKEWORD ת��Ϊһ��word
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
	// 1.������socket
	// af ��ַЭ���� ipv4 ipv6
	// type ����Э������ ��ʽ�׽���tcp ���ݱ�udp
	// protocol ʹ�þ����ĳ������Э��
	// fd ������
	SOCKET fd=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP); 
	if (fd == INVALID_SOCKET) {  // ʧ��
		error("socket");
		return INVALID_SOCKET;
	}
	// 2.�󶨱���ip�Ͷ˿ں�
	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(PORT);  //ת��Ϊ�����ֽ��� ����С�ˣ�
	addr.sin_addr.S_un.S_addr = ADDR_ANY; //ADDR_ANY �󶨵�������˿�  //inet_addr("127.0.0.1"); // inet_addr �����ʮ����ת��Ϊ������
	if (SOCKET_ERROR == bind(fd, (SOCKADDR*)&addr, sizeof(addr))) {
		error("bind");
		return false;
	}
	// 3.����
	listen(fd, 10);
	return fd;
}

SOCKET createClientSocket(const char *ip)
{
	// 1.������socket
	SOCKET fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (fd == INVALID_SOCKET) {  // ʧ��
		error("socket");
		return INVALID_SOCKET;
	}
	// 2.���ӷ�����
	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(PORT);  //ת��Ϊ�����ֽ��� ����С�ˣ�
	inet_pton(AF_INET, ip, &addr.sin_addr);
	if (connect(fd, (SOCKADDR*)&addr, sizeof(addr)) == INVALID_SOCKET) {
		error("connect");
		return false;
	}
	return fd;
}

// ע��
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
	strcpy_s(recvbuf, "�����������û�������������20���ַ���");
	send(clifd, recvbuf, strlen(recvbuf), 0);
	if (recv(clifd, sendbuf, BUFSIZ, 0) > 0) {
		userid = sendbuf;
	}
	while (count < 3) {
		wrongtag = wrongtag + "�������������룺��������20���ַ���";
		::memset(recvbuf, 0, sizeof(recvbuf));
		::memset(sendbuf, 0, sizeof(sendbuf));
		strcpy_s(recvbuf, wrongtag.c_str());
		send(clifd, recvbuf, strlen(recvbuf), 0);
		if (recv(clifd, sendbuf, BUFSIZ, 0) > 0) {
			userkey = sendbuf;
		}
		::memset(recvbuf, 0, sizeof(recvbuf));
		::memset(sendbuf, 0, sizeof(sendbuf));
		strcpy_s(recvbuf, "���ٴ������������룺");
		send(clifd, recvbuf, strlen(recvbuf), 0);
		if (recv(clifd, sendbuf, BUFSIZ, 0) > 0) {
			if (userkey == sendbuf) {                            // �������������ͬ����¼����
				if (curMsgNum < MAX_MSG_NUM) {
					strcpy_s(clientMsg[curMsgNum * 3], userid.c_str());
					strcpy_s(clientMsg[curMsgNum * 3 + 1], userkey.c_str());
					curMsgNum++;
					return true;
				}
				else {
					::cout << "msg����������" << endl;
					break;
				}
			}
			else {
				wrongtag = "�����������벻һ�£����������룡\n";       // ����������벻ͬ�����ʹ�����Ϣ����������
				count++;
			}
		}
	}	
	return false;
}

// ��½��֤
bool loginCheck(SOCKET& clifd)
{
	// ��½��֤
	int count = 0;
	string userid;
	string userkey;
	string wrongtag;
	char recvbuf[BUFSIZ] = { 0 };
	char sendbuf[BUFSIZ] = { 0 };
	while (count < 3) {
		//������Ϣ
		wrongtag = wrongtag + "���¼��\n�������û�����";
		strcpy_s(recvbuf, wrongtag.c_str());
		if (send(clifd, recvbuf, strlen(recvbuf), 0) == SOCKET_ERROR) {
			error("send");
		}
		// ��ָ���ͻ��˽�����Ϣ
		if (recv(clifd, sendbuf, BUFSIZ, 0) > 0) {
			userid = sendbuf;
			::memset(sendbuf, 0, sizeof(sendbuf));
		}
		//������Ϣ
		::memset(recvbuf, 0, sizeof(recvbuf));
		strcpy_s(recvbuf, "���������룺");
		if (send(clifd, recvbuf, strlen(recvbuf), 0) == SOCKET_ERROR) {
			error("send");
		}
		// ��ָ���ͻ��˽�����Ϣ
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
				strcpy_s(recvbuf, "��½�ɹ���");
				if (send(clifd, recvbuf, strlen(recvbuf), 0) == SOCKET_ERROR) {
					error("send");
				}
				return true;
			}
		}
		wrongtag = "�û������������\n";
		count++;
	}
	::memset(recvbuf, 0, sizeof(recvbuf));
	strcpy_s(recvbuf, "�Ѵ������������ޣ����ɵ�¼��\n");
	if (send(clifd, recvbuf, strlen(recvbuf), 0) == SOCKET_ERROR) {
		error("send");
	}
	return false;
}

bool welcomMenu(SOCKET& clifd)
{
	char recvbuf[BUFSIZ] = { 0 };
	char sendbuf[BUFSIZ] = { 0 };
	strcpy_s(recvbuf, "��ѡ��1.��¼ 2.ע��\n");
	if (send(clifd, recvbuf, strlen(recvbuf), 0) == SOCKET_ERROR) {
		error("send");
	}
	if (recv(clifd, sendbuf, BUFSIZ, 0) > 0) {
		if (strcmp(sendbuf, "1") == 0) {          // ��¼
			if (loginCheck(clifd))
				return true;
		}
		else if (strcmp(sendbuf, "2") == 0) {     // ע��
			if (registerMsg(clifd)) {
				if (loginCheck(clifd))
					return true;
			}
			else
				return false;
		}
		else {
			strcpy_s(recvbuf, "�������룡");
			send(clifd, recvbuf, strlen(recvbuf), 0);
		}
	}
	return false;
}

// �ͻ��ˣ��ȴ����շ���˵���Ϣ
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

// ִ�пͻ������е�ͨ�Ų���
void clientChat(SOCKET& fd)
{
	char recvbuf[BUFSIZ] = "���ӳɹ���";
	char sendbuf[BUFSIZ] = { 0 };
	
	// ��½��֤
	while (strcmp(sendbuf, "��½�ɹ���") != 0) {
		if (strcmp(recvbuf, "exit") == 0)
			return;
		// �ӷ������˽�����Ϣ
		memset(sendbuf, 0, sizeof(sendbuf));
		int recvTag = recv(fd, sendbuf, BUFSIZ, 0);
		if (recvTag > 0) {
			//cout << "recv: " << sendbuf << endl;
			cout << sendbuf << endl;
			if (strcmp(sendbuf, "�Ѵ������������ޣ����ɵ�¼��\n") == 0) {
				return;
			}
			if (strcmp(sendbuf, "��½�ɹ���") == 0) {
				break;
			}
			// �������������Ϣ
			cout << " > ";
			::memset(recvbuf, 0, sizeof(recvbuf));
			gets_s(recvbuf, BUFSIZ);
			int tag = send(fd, recvbuf, strlen(recvbuf), 0);
			if (tag == SOCKET_ERROR) { error("send"); }
		}
		else {
			cout << "��½ʧ�ܣ����Զ��˳�..." << endl;
			return;
		}
	}
	// ���շ������Ϣ
	HANDLE WaitServerTrd = CreateThread(NULL, 0, WaitingforMsgFromServer, (LPVOID)fd, 0, NULL);
	// ������Ϣ
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

// չʾ��ǰ�������߿ͻ���
void Output()
{
	cout << endl;
	cout << "============================��ǰ������:" << curClientNum << "============================" << endl;
	cout << "��ǰ���ӵĿͻ���:  ";
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
	if (tag == SOCKET_ERROR) {                  // �������ʧ�ܣ�ɾ������ͻ���
		cout << "send message to " << fd << " fail! deleting it..." << endl;
		closesocket(fd);
		for (int i = 0; i < MAX_CLIENT_NUM; i++) {
			if (sck[i] == fd) {
				sck[i] = NULL;
				curClientNum--;
			}
		}
		cout << "ɾ�� " << fd << " �ɹ���" << endl;
	}
}

char* getId(SOCKET& fd)
{
	int fds = (int)fd;
	//char suf[] = " ˵�� ";
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

// ����ˣ��½��߳���ͻ��˽���ͨ��
DWORD WINAPI ThreadFun(LPVOID IpThreadParameter)
{
	char recvbuf[BUFSIZ] = "SUCCESS";
	char sendbuf[BUFSIZ] = { 0 };
	SOCKET clifd = (SOCKET)IpThreadParameter;
	memset(recvbuf, 0, sizeof(recvbuf));

	//��½��֤
	if (welcomMenu(clifd) == true) {
		// sck��¼�¿ͻ���
		int cur;
		for (cur = 0; cur < MAX_CLIENT_NUM; cur++) {
			if (sck[cur] == NULL) {
				sck[cur] = clifd;
				break;
			}
		}
		curClientNum++;
		char* id = getId(clifd);
		printf("��ӭ %s ���������ң�\s", id);
		Output();

		// ���ܿͻ�����Ϣ
		int ret = 0;
		do
		{
			ret = recv(clifd, sendbuf, BUFSIZ, 0);
			if (strcmp(sendbuf, "exit") == 0) { // �ͻ��˹ر�����
				cout << clifd << " �˳���������" << endl;
				break;
			}
			if (ret == SOCKET_ERROR) { 
				closesocket(clifd);
				return 1;
			}
			else { // �ɹ�������Ϣ
				for (int i = 0; i < MAX_CLIENT_NUM; i++) {          // ת����Ϣ//////////////////////////////////////////////////////
					if (sck[i] != NULL && sck[i] != clifd) {
						send(sck[i], id, strlen(id), 0);
						int tag = send(sck[i], sendbuf, strlen(sendbuf), 0);
						Test(sck[i], tag);
						if (tag <= 0) {
							cout << "tag==0" << endl;
						}
					}
				}
				printf(" %s (%d)˵��%s\n", id, clifd, sendbuf);    // �����Ҹ�����Ϣ
				//cout << id << "  ˵�� " << sendbuf << endl;       
				memset(sendbuf, 0, sizeof(sendbuf));
			}
		} while (ret > 0);
		sck[cur] = NULL;
		curClientNum--;
		Output();
	}

	return 0;
}

// ����ˣ��½��߳���ͻ��˽���ͨ��
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
			cout << "1.ɾ���ͻ���" << endl;
			cout << "2.��ָ���ͻ��˷�����Ϣ" << endl;
			cout << "3.��ȫ��ͻ��˷�����Ϣ" << endl;
			cout << "4.չʾ��ǰ�����û�" << endl;
			cout << "5.չʾ�û���Ϣ��" << endl;
		}
		// ɾ���ͻ���
		else if (strcmp(ch, "1") == 0) {
			cout << "ɾ��˭" << endl;
			SOCKET delNum;
			cin >> delNum;
			closesocket(delNum);
			for (int i = 0; i < MAX_CLIENT_NUM; i++) {
				if (sck[i] == delNum) {
					sck[i] = NULL;
				}
			}
			curClientNum--;
			//cout << "ɾ�� " << delNum << " �ɹ���" << endl;

			Output();
		}
		// ��ָ���ͻ��˷�����Ϣ
		else if (strcmp(ch, "2") == 0) {
			SOCKET cli;
			cout << "����λ�û�������Ϣ��" << endl;
			cin >> cli;
			char sbuf[BUFSIZ] = { 0 };
			char idbuf[BUFSIZ] = { 0 };
			int tag = 0;
			while (tag != SOCKET_ERROR && strcmp(sbuf, "exit") != 0) {
				cout << "�� " << cli << " ���ͣ�";
				memset(sbuf, 0, sizeof(sbuf));
				gets_s(sbuf, BUFSIZ);
				if (strcmp(sbuf, "exit") == 0)
					break;
				strcpy_s(idbuf, "�����: ");
				send(cli, idbuf, strlen(idbuf), 0);
				tag = send(cli, sbuf, strlen(sbuf), 0);               // ������Ϣ
				Test(cli, tag);
			}
		}
		// ��ȫ��ͻ��˷�����Ϣ
		else if (strcmp(ch, "3") == 0) {
			char sbuf[BUFSIZ] = { 0 };
			char idbuf[BUFSIZ] = { 0 };
			int tag = 0;
			while (tag != SOCKET_ERROR && strcmp(sbuf, "exit") != 0) {
				cout << "�����пͻ��˷���:";  // ��һ�λ����������ʾ��
				memset(sbuf, 0, sizeof(sbuf));
				gets_s(sbuf, BUFSIZ);
				if (strcmp(sbuf, "exit") == 0)
					break;
				for (int i = 0; i < MAX_CLIENT_NUM; i++) {
					if (sck[i] != NULL) {
						strcpy_s(idbuf, "�����: ");
						send(sck[i], idbuf, strlen(idbuf), 0);
						int tag = send(sck[i], sbuf, strlen(sbuf), 0);
						Test(sck[i], tag);
					}
				}	
			}
		}
		// չʾ��ǰ�����û�
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
			cout << "��Ч����" << endl;
		}
	}

	client = NULL;
	return 0;

}
