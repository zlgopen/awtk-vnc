# AWTK-VNC

[AWTK](https://github.com/zlgopen/awtk) 是为嵌入式系统开发的 GUI 引擎库。


* install libvncserver

```
sudo apt install libvncserver-dev
```

```
scons
```

```
./release.sh
```

```
cd release
valgrind --leak-check=full ./bin/demoui
```

```
vncviewer localhost
```

