#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <iostream>
#include <string>
using namespace std;

#include "uv.h"
#include "netbus.h"


extern "C" {

	// 当我们的event loop检车到handle上有数据可以读的时候,
	// 就会调用这个函数, 让这个函数给event loop准备好读入数据的内存;
	// event loop知道有多少数据,suggested_size,
	// handle: 发生读时间的handle;
	// suggested_size: 建议我们分配多大的内存来保存这个数据;
	// uv_buf_t: 我们准备好的内存，通过uv_buf_t,告诉even loop;
	static void
		uv_alloc_buf(uv_handle_t* handle,
			size_t suggested_size,
			uv_buf_t* buf) {

		if (handle->data != NULL) {
			free(handle->data);
			handle->data = NULL;
		}

		buf->base = malloc(suggested_size + 1);
		buf->len = suggested_size;
		handle->data = buf->base;
	}

	static void
		on_close(uv_handle_t* handle) {
		printf("close client\n");
		if (handle->data) {
			free(handle->data);
			handle->data = NULL;
		}
	}

	static void
		on_shutdown(uv_shutdown_t* req, int status) {
		uv_close((uv_handle_t*)req->handle, on_close);
		free(req);
	}

	static void
		after_write(uv_write_t* req, int status) {
		if (status == 0) {
			printf("write success\n");
		}
		uv_buf_t* w_buf = req->data;
		if (w_buf) {
			free(w_buf);
		}

		free(req);
	}

	// 参数: 
	// uv_stream_t* handle --> uv_tcp_t;
	// nread: 读到了多少字节的数据;
	// uv_buf_t: 我们的数据都读到到了哪个buf里面, base;
	static void after_read(uv_stream_t* stream,
		ssize_t nread,
		const uv_buf_t* buf) {
		// 连接断开了;
		if (nread < 0) {
			uv_shutdown_t* reg = malloc(sizeof(uv_shutdown_t));
			memset(reg, 0, sizeof(uv_shutdown_t));
			uv_shutdown(reg, stream, on_shutdown);
			return;
		}
		// end

		buf->base[nread] = 0;
		printf("recv %d\n", nread);
		printf("%s\n", buf->base);

		// 测试发送给我们的 客户端;
		uv_write_t* w_req = malloc(sizeof(uv_write_t));
		uv_buf_t* w_buf = malloc(sizeof(uv_buf_t));
		w_buf->base = buf->base;
		w_buf->len = nread;
		w_req->data = w_buf;
		uv_write(w_req, stream, w_buf, 1, after_write);
	}

	static void
		uv_connection(uv_stream_t* server, int status) {
		printf("new client comming\n");
		// 接入客户端;
		uv_tcp_t* client = malloc(sizeof(uv_tcp_t));
		memset(client, 0, sizeof(uv_tcp_t));
		uv_tcp_init(loop, client);
		uv_accept(server, (uv_stream_t*)client);
		// end

		// 告诉event loop,让他帮你管理哪个事件;
		uv_read_start((uv_stream_t*)client, uv_alloc_buf, after_read);
	}
}

static netbus global_netbus;
netbus* netbus::instance()
{
	return &global_netbus;
}

void netbus::start_tcp_server(int port)
{
	//create
	uv_tcp_t* listen = (uv_tcp_t*)malloc(sizeof(uv_tcp_t));
	memset( listen, 0, sizeof(uv_tcp_t) );

	//init
	uv_tcp_init(uv_default_loop(), listen);
	struct sockaddr_in addr;
	uv_ip4_addr("0.0.0.0", 6080, &addr);

	//bind
	int ret = uv_tcp_bind(listen, (const struct sockaddr*)&addr, 0);
	if (ret != 0)
	{
		printf("Bind error...\n"); 
		free(listen);
		return;
	}

FAILED:
}

void netbus::start_ws_server(int port)
{
}

void netbus::run()
{
}
