// 存储服务器
// 实现文件操作类
//
#include <unistd.h>
#include <cstring>
#include <fcntl.h>
#include <lib_acl.hpp>
#include "01_types.h"
#include "07_file.h"

// 构造函数
file_c::file_c() : m_fd(-1)
{
}

// 析构函数
file_c::~file_c()
{
	close();
}

// 打开文件
int file_c::open(char const *path, char flag)
{
	// 检查路径
	if (!path || !strlen(path))
	{
		logger_error("path is null");
		return ERROR;
	}

	// 打开文件
	if (flag == O_READ)
		m_fd = ::open(path, O_RDONLY);
	else if (flag == O_WRITE)
		m_fd = ::open(path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	else
	{
		logger_error("unknown open flag: %c", flag);
		return ERROR;
	}

	// 打开失败
	if (m_fd == -1)
	{
		logger_error("call open fail: %s, path: %s, flag: %c",
								 strerror(errno), path, flag);
		return ERROR;
	}

	return OK;
}

// 关闭文件
int file_c::close()
{
	if (m_fd != -1)
	{
		// 关闭文件
		if (::close(m_fd) == -1)
		{
			logger_error("call close fail: %s", strerror(errno));
			return ERROR;
		}

		m_fd = -1;
	}

	return OK;
}

// 读取文件
int file_c::read(void *buf, size_t count) const
{
	// 检查文件描述符
	if (m_fd == -1)
	{
		logger_error("invalid file handle");
		return ERROR;
	}

	// 检查文件缓冲区
	if (!buf)
	{
		logger_error("invalid file buffer");
		return ERROR;
	}

	// 读取给定字节数
	// ::read -> 第三个参数，想读多少就读多少，最理想
	// 				-> 大于0但小于第三个参数，读出来的数据少于期望读取的数据
	//        -> 0，读写指针已经在文件尾了
	//        -> -1，出错
	// ssize_t 是一个数据类型，通常用于表示有符号的大小或计数值 long int
	ssize_t bytes = ::read(m_fd, buf, count);
	if (bytes == -1)
	{
		logger_error("call read fail: %s", strerror(errno));
		return ERROR;
	}
	if ((size_t)bytes != count)
	{
		logger_error("unable to read expected bytes: %ld != %lu", bytes, count);
		return ERROR;
	}

	return OK;
}

// 写入文件
int file_c::write(void const *buf, size_t count) const
{
	// 检查文件描述符
	if (m_fd == -1)
	{
		logger_error("invalid file handle");
		return ERROR;
	}

	// 检查文件缓冲区
	if (!buf)
	{
		logger_error("invalid file buffer");
		return ERROR;
	}

	// 写入给定字节数
	if (::write(m_fd, buf, count) == -1)
	{
		logger_error("call write fail: %s", strerror(errno));
		return ERROR;
	}

	return OK;
}

// 设置偏移
int file_c::seek(off_t offset) const
{
	// 检查文件描述符
	if (m_fd == -1)
	{
		logger_error("invalid file handle");
		return ERROR;
	}

	// 检查文件偏移量
	if (offset < 0)
	{
		logger_error("invalid file offset");
		return ERROR;
	}

	// 设置文件偏移量
	if (lseek(m_fd, offset, SEEK_SET) == -1)
	{
		logger_error("call lseek fail: %s, offset: %ld", strerror(errno), offset);
		return ERROR;
	}

	return OK;
}

// 删除文件
int file_c::del(char const *path)
{
	// 检查路径
	if (!path || !strlen(path))
	{
		logger_error("path is null");
		return ERROR;
	}

	// 删除文件 删文件 硬链接删除
	if (unlink(path) == -1)
	{
		logger_error("call unlink fail: %s, path: %s", strerror(errno), path);
		return ERROR;
	}

	return OK;
}
