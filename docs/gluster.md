========
文件同步
========

1. 克隆开发虚机
---------------

           192.168.0.102(TNV)
          __________|__________
         /                     \
192.168.0.103(TNV1)   192.168.0.104(TNV2)

2. 添加域名映射
---------------

TNV1和TNV2$ sudo vi /etc/hosts

192.168.0.103   TNV1
192.168.0.104   TNV2

3. 准备存储分区
---------------

在VMware中为虚拟机扩展磁盘容量
TNV1和TNV2$ sudo apt-get install gparted
TNV1和TNV2$ sudo gparted
新分区设备名为/dev/sda3 // 设备文件名
TNV1和TNV2$ mkdir -p /home/tarena/Projects/TNV/gluster
TNV1和TNV2$ sudo mount /dev/sda3 /home/tarena/Projects/TNV/gluster
// 访问目录就是访问磁盘

4. 安装GlusterFS
----------------

TNV1和TNV2$ sudo apt-get install glusterfs-server

5. 添加集群节点
---------------

TNV1$ sudo gluster peer probe TNV2
或
TNV2$ sudo gluster peer probe TNV1

6. 创建逻辑卷
-------------

TNV1或TNV2$ sudo gluster volume create group001 replica 2
            TNV1:/home/tarena/Projects/TNV/gluster
            TNV2:/home/tarena/Projects/TNV/gluster
            force

7. 启动逻辑卷 激活
-------------

TNV1或TNV2$ sudo gluster volume start group001

8. 挂载逻辑卷 glusterfs TNV1:group001 == 设备
-------------

TNV1$ sudo mount -t glusterfs TNV1:group001 /home/tarena/Projects/TNV/data
TNV2$ sudo mount -t glusterfs TNV2:group001 /home/tarena/Projects/TNV/data

9. 同步结构
-----------

 ____________          ____________
|TNV1        |        |        TNV2|
|     storage|        |storage     |
|            |        |            |
|        data-group001-data        |
|         _______|________         |
|        /   |        |   \        |
|sda3-gluster|        |gluster-sda3|
|____________|        |____________|
