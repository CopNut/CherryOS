# CherryOS
戒骄戒躁 精益求精
## 环境需求
**操作系统：**
windows
**必备环境与软件：**
1. MinGW（配置好环境变量，可以于cmd中运行gcc、as等命令）
2. VMware等仿真软件
## 编译步骤
1. 运行`cmd`
2. 切换至项目根目录
3. `make`
4. 于VMware中添加虚拟机，并设置为软盘启动，软盘镜像为项目根目录下的`link\CherryOS.img`
5. 启动虚拟机