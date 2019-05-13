#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <iostream>
#include <string>
using namespace std;

#include "uv.h"
#include "netbus.h"


extern "C" {

	// �����ǵ�event loop�쳵��handle�������ݿ��Զ���ʱ��,
	// �ͻ�����������, �����������event loop׼���ö������ݵ��ڴ�;
	// event loop֪���ж�������,suggested_size,
	// handle: ������ʱ���handle;
	// suggested_size: �������Ƿ�������ڴ��������������;
	// uv_buf_t: ����׼���õ��ڴ棬ͨ��uv_buf_t,����even loop;
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

	// ����: 
	// uv_stream_t* handle --> uv_tcp_t;
	// nread: �����˶����ֽڵ�����;
	// uv_buf_t: ���ǵ����ݶ����������ĸ�buf����, base;
	static void after_read(uv_stream_t* stream,
		ssize_t nread,
		const uv_buf_t* buf) {
		// ���ӶϿ���;
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

		// ���Է��͸����ǵ� �ͻ���;
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
		// ����ͻ���;
		uv_tcp_t* client = malloc(sizeof(uv_tcp_t));
		memset(client, 0, sizeof(uv_tcp_t));
		uv_tcp_init(loop, client);
		uv_accept(server, (uv_stream_t*)client);
		// end

		// ����event loop,������������ĸ��¼�;
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
