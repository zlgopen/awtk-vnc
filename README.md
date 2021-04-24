# AWTK-VNC

AWTK-VNC 是 [AWTK](https://github.com/zlgopen/awtk)  对 VNC (Virtual Network Computing) 的支持。主要目的有两个：

* 方便在 Linux 下用 valgrind 做内存分析，而无需启动到 framebuffer 模式。

* 为支持远程桌面提供一个参考实现。

> 目前支持 Linux/Macos。

## 安装 libvncserver

* Ubuntu

```
sudo apt install libvncserver-dev
```

* Macos

```
brew install libvncserver
```

## 编译

```
scons
```

## 运行

```
./release.sh
```

```
cd release
./bin/demoui
```

## 启动客户端

```
vncviewer localhost
```
