// 存储服务器
// 声明文件操作类
//
#pragma once

#include <sys/types.h>
//
// 文件操作类
//
class file_c
{
public:
	// 构造函数
	file_c();
	// 析构函数
	~file_c();

	// 打开文件
	int open(char const *path, char flag);
	// 关闭文件
	int close();

	// 读取文件
	int read(void *buf, size_t count) const;
	// 写入文件
	int write(void const *buf, size_t count) const;

	// 设置偏移
	int seek(off_t offset) const;
	// 删除文件 和对象无关，不用文件描述符，拿类调 静态，不能带常属性const，没有this指针
	static int del(char const *path);

	// 打开标志
	static char const O_READ = 'r';
	static char const O_WRITE = 'w';

private:
	int m_fd; // 文件描述符
};
