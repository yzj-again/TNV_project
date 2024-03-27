// 客户机
// 声明连接池管理器类
//
#pragma once

#include <lib_acl.hpp>
//
// 连接池管理器类
//
// 从一个池里获得的一堆连接，都指向同一个目的主机 ip和连接池的一个映射
class mngr_c : public acl::connect_manager
{
protected:
	// 创建连接池
	acl::connect_pool *create_pool(char const *destaddr, size_t count, size_t index);
};
