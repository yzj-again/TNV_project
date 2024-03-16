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
  time_t last = time(NULL);
  while (!m_stop)
  {
    sleep(1);
    // 现在时间
    time_t now = time(NULL);
    // 若现在距离最近一次检查存储服务器状态已足够久
    if (now - last >= cfg_interval)
    {
      // 检查存储服务器的状态
      check();
      // 更新最近一次检查时间
      last = now;
    }
  }

  return nullptr;
}
// 检查存储服务器的状态
int status_c::check() const
{
  // 现在
  time_t now = time(NULL);
  // 检查最后一次心跳时间
  // 互斥锁加锁
  if ((errno = pthread_mutex_lock(&g_mutex)))
  {
    logger_error("call pthread_mutex_lock fail: %s",
                 strerror(errno));
    return ERROR;
  }
  // 遍历组表中的每一个组
  for (std::map<std::string, std::list<storage_info_t>>::iterator group = g_groups.begin(); group != g_groups.end(); ++group)
  {
    // 遍历该组中每一台存储服务器
    for (std::list<storage_info_t>::iterator si = group->second.begin(); si != group->second.end(); ++si)
    {
      if (now - si->si_btime >= cfg_interval)
      {
        // 将其标记为离线
        si->si_status = STORAGE_STATUS_OFFLINE;
      }
    }
  }
  // 互斥锁解锁
  if ((errno = pthread_mutex_unlock(&g_mutex)))
  {
    logger_error("call pthread_mutex_unlock fail: %s",
                 strerror(errno));
    return ERROR;
  }
  return OK;
}