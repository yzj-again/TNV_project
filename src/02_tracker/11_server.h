// 跟踪服务器
// 声明服务器类
//
#pragma once
#include <lib_acl.hpp>
#include "09_status.h"

// 服务器类
class server_c : public acl::master_threads
{
protected:
  // 虚函数 签名唯一
  // 进程切换用户后被调用 进程启动时调用
  void proc_on_init() override;

  // 子进程意图退出时被调用 想退还没退！
  // 返回true，子进程立即退出，否则； 说不好
  // 若配置项ioctl_quick_abort非0，子进程立即退出，否则；
  // 待所有客户机连接都关闭后，子进程再退出  --理想的方式
  bool proc_exit_timer(size_t nclients, size_t nthreads) override;

  // 进程退出前被调用
  void proc_on_exit() override;

  // 线程有关 socket accept
  // 1若大量客户端连而不发，服务器总是在创建线程，阻塞在读上
  // 2.没法控制线程数
  // 设计思路：线程池，IO多路复用（读之前先看下，是不是有数据过来）epoll
  // 线程获得连接时被调用
  // 返回true，连接将被用于后续通信，否则
  // 函数返回后即关闭连接
  bool thread_on_accept(acl::socket_stream *conn) override;

  // 与线程绑定的连接socket可读时被调用
  // 返回true，保持长连接，否则
  // 函数返回后即关闭连接
  bool thread_on_read(acl::socket_stream *conn) override;

  // 线程读写连接超时时被调用
  // 返回true，继续等待下一次读写，否则
  // 函数返回后即关闭连接
  bool thread_on_timeout(acl::socket_stream *conn) override;

  // 与线程绑定的连接关闭时被调用
  void thread_on_close(acl::socket_stream *conn) override;

private:
  status_c *m_status; // 存储服务器状态检查线程
};
