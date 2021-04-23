# AWTK-VNC

[AWTK](https://github.com/zlgopen/awtk) 是为嵌入式系统开发的 GUI 引擎库。


## 安装libvncserver

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

