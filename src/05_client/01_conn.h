// 客户机
// 声明连接类
//
#pragma once

#include <string>
#include <lib_acl.hpp>
//
// 连接类
//
// 管理业务，底层通信由套接字流acl管理
class conn_c : public acl::connect_client
{
public:
	// 构造函数
	conn_c(char const *destaddr, int ctimeout = 30, int rtimeout = 60);
	// 析构函数
	~conn_c();

	// 跟踪服务器
	// 从跟踪服务器获取存储服务器地址列表
	int saddrs(char const *appid, char const *userid, char const *fileid, std::string &saddrs);
	// 从跟踪服务器获取组列表
	int groups(std::string &groups);

	// 存储服务器
	// 向存储服务器上传文件
	int upload(char const *appid, char const *userid, char const *fileid, char const *filepath);
	// 向存储服务器上传文件 不一定是文件，可以是内存里的数据
	int upload(char const *appid, char const *userid, char const *fileid, char const *filedata, long long filesize);
	// 向存储服务器询问文件大小
	int filesize(char const *appid, char const *userid, char const *fileid, long long *filesize);
	// 从存储服务器下载文件 接收一个指针的地址（二级指针） 往指针里面填那块内存的地址
	int download(char const *appid, char const *userid, char const *fileid, long long offset, long long size, char **filedata, long long *filesize);
	// 删除存储服务器上的文件
	int del(char const *appid, char const *userid, char const *fileid);

	// 只读接口 get方法
	// 获取错误号
	short errnumb() const;
	// 获取错误描述
	char const *errdesc() const;

protected:
	// 从基类中覆盖的受保护
	// 打开连接 数据由成员变量保存
	bool open();
	// 关闭连接
	void close();

private:
	// 报文中有相同的部分，构造公有请求（三ID）
	// 构造请求
	int makerequ(char command, char const *appid, char const *userid, char const *fileid, char *requ);
	// 接收包体 长度调用时预知不了，它来做内存的分配
	int recvbody(char **body, long long *bodylen);
	// 接收包头
	int recvhead(long long *bodylen);

	char *m_destaddr;						// 目的地址:端口 跟踪或存储
	int m_ctimeout;							// 连接超时
	int m_rtimeout;							// 读写超时
	acl::socket_stream *m_conn; // 连接对象
	short m_errnumb;						// 错误号
	acl::string m_errdesc;			// 错误描述
};
