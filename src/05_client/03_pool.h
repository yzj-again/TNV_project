// 客户机
// 声明连接池类
//
#pragma once

#include <lib_acl.hpp>
//
// 连接池类
//
class pool_c : public acl::connect_pool
{
public:
    // 构造函数
    pool_c(char const *destaddr, int count, size_t index = 0);

    // 设置超时
    void timeouts(int ctimeout = 30, int rtimeout = 60, int itimeout = 90);
    // 获取连接
    /**
     * 从连接池中尝试性获取一个连接，当服务器不可用、距上次服务端连接异常
     * 时间间隔未过期或连接池连接个数达到连接上限则将返回 NULL；当创建一个
     * 新的与服务器的连接时失败，则该连接池会被置为不可用状态
     * @param on {bool} 该参数决定当连接池没有可用连接时是否创建新的连接，
     *  如果为 false，则不会创建新连接
     * @return {connect_client*} 如果为空则表示该服务器连接池对象不可用
     */
    // connect_client* peek(bool on = true);
    acl::connect_client *peek();

protected:
    // 虚函数，创建连接
    // 基类是connect_client类型，其实返回的是子类对象
    acl::connect_client *create_connect();

private:
    int m_ctimeout; // 连接超时
    int m_rtimeout; // 读写超时
    int m_itimeout; // 空闲超时
};
