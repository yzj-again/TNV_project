// 跟踪服务器
// 实现存储服务器状态检查线程类
#include <unistd.h>
#include "01_globals.h"
#include "09_status.h"

// 构造函数
status_c::status_c() : m_stop(false)
{
}
// 终止线程
void status_c::stop()
{
  m_stop = true;
}

// 线程执行过程
void *status_c::run()
{
  while (!m_stop)
  {
    // 现在时间

    // 若现在距离最近一次检查存储服务器状态已足够久
    // 检查存储服务器的状态
    // 更新最近一次检查时间
  }

  return nullptr;
}
// 检查存储服务器的状态
int status_c::check() const
{

  return OK;
}