#ifndef __NETBUS_H__
#define __NETBUS_H__

class netbus {
public:
	static netbus* instance(); //ȫ�ֶ���
public:
	void start_tcp_server(int port);  //����tcp_server�ӿ�
	void start_ws_server(int port);   //����ws server�ӿ�
	void run(); //��ʼ������������
};

#endif
