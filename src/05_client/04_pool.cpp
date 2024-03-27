// 客户机
// 实现连接池类
//
#include "01_conn.h"
#include "03_pool.h"

// 构造函数
// connect_pool 传给基类的构造函数
/**
 * 构造函数
 * @param addr {const char*} 服务器监听地址，格式：ip:port(domain:port)
 * @param max {size_t} 连接池最大连接个数限制，如果该值设为 0，则不设置
 *  连接池的连接上限
 * @param idx {size_t} 该连接池对象在集合中的下标位置(从 0 开始)
 */
pool_c::pool_c(char const *destaddr, int count, size_t index) : connect_pool(destaddr, count, index), m_ctimeout(30), m_rtimeout(60), m_itimeout(90)
{
}

// 设置超时
void pool_c::timeouts(int ctimeout, int rtimeout, int itimeout)
{
	m_ctimeout = ctimeout;
	m_rtimeout = rtimeout;
	m_itimeout = itimeout;
}

// 获取连接
acl::connect_client *pool_c::peek()
{
	// 调基类里另一个函数，人为检测
	connect_pool::check_idle(m_itimeout);
	// peek到一个已经超时的连接
	return connect_pool::peek();
}

// 创建连接
acl::connect_client *pool_c::create_connect()
{
	return new conn_c(addr_, m_ctimeout, m_rtimeout);
}
