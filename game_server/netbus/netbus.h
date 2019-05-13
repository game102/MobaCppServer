#ifndef __NETBUS_H__
#define __NETBUS_H__

class netbus {
public:
	static netbus* instance(); //全局对象
public:
	void start_tcp_server(int port);  //启动tcp_server接口
	void start_ws_server(int port);   //启动ws server接口
	void run(); //开始管理整个事务
};

#endif
