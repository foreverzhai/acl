#include "stdafx.h"
#include "http_servlet.h"
#include "http_service.h"

//////////////////////////////////////////////////////////////////////////////
// ����������

char *var_cfg_str;
acl::master_str_tbl var_conf_str_tab[] = {
	{ "str", "test_msg", &var_cfg_str },

	{ 0, 0, 0 }
};

int  var_cfg_bool;
acl::master_bool_tbl var_conf_bool_tab[] = {
	{ "bool", 1, &var_cfg_bool },

	{ 0, 0, 0 }
};

int  var_cfg_rw_timeout;
acl::master_int_tbl var_conf_int_tab[] = {
	{ "rw_timeout", 30, &var_cfg_rw_timeout, 0, 0 },

	{ 0, 0 , 0 , 0, 0 }
};

long long int  var_cfg_int64;
acl::master_int64_tbl var_conf_int64_tab[] = {
	{ "int64", 120, &var_cfg_int64, 0, 0 },

	{ 0, 0 , 0 , 0, 0 }
};

//////////////////////////////////////////////////////////////////////////////

http_service::http_service(void)
{
}

http_service::~http_service(void)
{
}

void http_service::on_accept(acl::socket_stream* conn)
{
	logger("connect from %s, fd %d", conn->get_peer(),
		conn->sock_handle());

	conn->set_rw_timeout(var_cfg_rw_timeout);
	if (var_cfg_rw_timeout > 0) {
		conn->set_tcp_non_blocking(true);
	}

	acl::memcache_session session("127.0.0.1:11211");
	http_servlet servlet(handlers_, conn, &session);

	// charset: big5, gb2312, gb18030, gbk, utf-8
	servlet.setLocalCharset("utf-8");

	while (servlet.doRun()) {}

	logger("disconnect from %s", conn->get_peer());
}

void http_service::proc_on_listen(acl::server_socket& ss)
{
	logger(">>>listen %s ok<<<", ss.get_addr());
}

void http_service::proc_on_init(void)
{
}

void http_service::proc_on_exit(void)
{
}

bool http_service::proc_on_sighup(acl::string&)
{
	logger(">>>proc_on_sighup<<<");
	return true;
}

void http_service::do_cgi(void)
{
	acl::memcache_session session("127.0.0.1:11211");
        http_servlet servlet(handlers_, NULL, &session);
        servlet.setLocalCharset("utf-8");
        servlet.doRun();
}

//////////////////////////////////////////////////////////////////////////

http_service& http_service::Get(const char* path, http_handler_t fn)
{
	Service(http_handler_get, path, fn);
	return *this;
}

http_service& http_service::Post(const char* path, http_handler_t fn)
{
	Service(http_handler_post, path, fn);
	return *this;
}

http_service& http_service::Head(const char* path, http_handler_t fn)
{
	Service(http_handler_head, path, fn);
	return *this;
}

http_service& http_service::Put(const char* path, http_handler_t fn)
{
	Service(http_handler_put, path, fn);
	return *this;
}

http_service& http_service::Patch(const char* path, http_handler_t fn)
{
	Service(http_handler_patch, path, fn);
	return *this;
}

http_service& http_service::Connect(const char* path, http_handler_t fn)
{
	Service(http_handler_connect, path, fn);
	return *this;
}

http_service& http_service::Purge(const char* path, http_handler_t fn)
{
	Service(http_handler_purge, path, fn);
	return *this;
}

http_service& http_service::Delete(const char* path, http_handler_t fn)
{
	Service(http_handler_delete, path, fn);
	return *this;
}

http_service& http_service::Options(const char* path, http_handler_t fn)
{
	Service(http_handler_options, path, fn);
	return *this;
}

http_service& http_service::Propfind(const char* path, http_handler_t fn)
{
	Service(http_handler_profind, path, fn);
	return *this;
}

http_service& http_service::Websocket(const char* path, http_handler_t fn)
{
	Service(http_handler_websocket, path, fn);
	return *this;
}

http_service& http_service::Unknown(const char* path, http_handler_t fn)
{
	Service(http_handler_unknown, path, fn);
	return *this;
}

http_service& http_service::Error(const char* path, http_handler_t fn)
{
	Service(http_handler_error, path, fn);
	return *this;
}

void http_service::Service(int type, const char* path, http_handler_t fn)
{
	if (type >= http_handler_get && type < http_handler_max
		&& path && *path) {

		// The path should lookup like as "/xxx/" with
		// lower charactors.

		acl::string buf(path);
		if (buf[buf.size() - 1] != '/') {
			buf += '/';
		}
		buf.lower();
		handlers_[type][buf] = fn;
	}
}
