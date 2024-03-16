// 跟踪服务器
// 声明存储服务器状态检查线程类

#pragma once
#include <lib_acl.hpp>

// 存储服务器状态检查类
class status_c : public acl::thread
{
  // 覆盖执行虚函数
public:
  // 构造函数显式的初始化m_stop的值
  status_c();
  // 线程终止不可控 - 必须可控
  // 终止线程
  void stop();

protected:
  // 线程执行过程
  void *run();

private:
  // 检查存储服务器的状态
  int check() const;
  bool m_stop;
};