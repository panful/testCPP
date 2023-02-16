## Linux安装Boost

### 1.查看Boost版本

`dpkg -S /usr/include/boost/version.hpp`适用于在线安装的boost

### 2.删除当前安装的boost

`sudo apt-get autoremove libboost***.dev`***是boost版本号

### 3.下载需要的Boost库

[Boost](https://sourceforge.net/projects/boost/files/boost/)

### 4.预设置

解压之后在Boost根目录下执行

`./bootstrap.sh --with-toolset=gcc`默认编译所有库

`./bootstrap.sh --with-libraries=all --with-toolset=gcc`指定编译那些库，`all`表示所有

`./bootstrap.sh --with-libraries=system,thread,filesystem,data_time --with-tooset=gcc`编译指定的四个库

### 5.安装

`sudo ./b2 install`默认安装路径：库`/usr/local/lib` 头文件`/usr/local/include/boost`需要权限才能拷贝文件到该目录

`./b2 --prefix=/home/user/boost --with-system`指定安装路径为：`/home/user/boost` 安装指定库：`system`

### 6.设置环境变量

```shell
# 打开配置文件
$sudo vim /etc/profile
# 编辑文件设置环境变量
export CPLUS_INCLUDE_PATH=/usr/boost/include:$CPLUS_INCLUDE_PATH
export LIBRARY_PATH=/usr/boost/lib:$LIBRARY_PATH
export LD_LIBRARY_PATH=/usr/boost/lib:$LD_LIBRARY_PATH
# 使环境变量生效
$source /etc/profile
```

## Windows安装Boost

