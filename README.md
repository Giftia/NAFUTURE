# NAFUTURE
## 一款基于ESP8266的农业用温湿度测试器的二次开发固件
### 使用说明：
- 1.使用M-TTL烧录器或第三方TTL连接光和未来温湿度传感计(以下简称设备)；
- 2.按住M-TTL烧录器上的FLASH按钮 并打开设备开关，应能观察到设备蓝色指示灯微光常亮，进入烧录模式；
- 3.使用安装了对应库的 Arduino IDE 打开本工程，修改25、26行的 ssid 和 password 为自己的WiFi；
- 4.点击菜单-工具中的 ESP8266 Sketch Data Upload 刷入 SPIFFS，等待完毕后重复第二步；
- 5.点击上传，等待完毕后重启设备开关；
- 6.打开电脑或手机的WiFi，连接一个名字类似 ESP_1A2B3C 的WiFi；
- 7.使用浏览器访问 192.168.4.1，查看页面上显示的 本地ip地址，接着切换到自己的WiFi；
- 8.访问上面显示的 本地ip地址 即可在局域网查看数据页。进入睡眠状态后，设备将会在1小时后醒来10分钟。

  所需的M-TTL烧录器：https://item.taobao.com/item.htm?id=638998843446

  M-TTL烧录器硬件开源于：https://oshwhub.com/Giftina/guang-ge-wei-lai-shao-lu-zhuai-j
