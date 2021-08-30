# ESP_MASTER

<img src="https://komarev.com/ghpvc/?username=ESPMASTER" alt="ESPMASTER" />

* 视频介绍：https://www.bilibili.com/video/BV1SM4y1V7Yb

[![NetFlix on UWP](https://whycan.com/files/members/4115/8.png)](https://player.bilibili.com/player.html?aid=932590303&bvid=BV1SM4y1V7Yb&cid=397114463&page=1 "NetFlix on UWP")

This is a comprehensive project that combines the powerful computing capabilities of ESP32, with functions such as speech recognition, image recognition, GUI learning, and FFT music spectrum. It is suitable for learning image recognition algorithms and making LVGL-based UI interfaces.

	功能介绍：
	1、语音识别。将数字麦克风通过I2S连接到ESP32，经过esp-skainet离线语音识别，自定义语音命令词最大个数为 100
	2、物体识别、文字识别（车牌识别）。基于百度智能云，将摄像头获取的RGB图像转为JPG，经过base64和urlcode编码上传到百度云后获取识别结果来显示。
	3、颜色识别。可由摄像头获取的图像将RGB转为HSL判断每一个像素点，进行颜色块的识别，获取颜色块坐标。
	4、家电控制。可由语音设置空调类型，也可以在设置界面设置空调种类，当前支持格力、美的、海尔空调，空调码库由IREXT提供，码库存放在flash的spiffs分区中。
	5、LVGL。基于LVGL 7.6.0，10余个界面切换。
	6、ADC按键。在ESP32引脚极其不够的情况下，使用一个IO口，经过电阻分压可读取三个按键的状态。
	7、FFT频谱，将数字麦克风通过I2S连接到ESP32，经过软件fft获得频谱通过lvgl img显示。
	8、小游戏。移植2048和贪吃蛇游戏。
	9、每日鸡汤。进入每日鸡汤界面后，HTTP随机获取一句话显示。

## Languages and Tools:

![C](https://img.shields.io/badge/-C-A8B9CC?style=flat-square&logo=C&logoColor=white)
![C++](https://img.shields.io/badge/-C++-00599C?style=flat-square&logo=C%2B%2B&logoColor=white)
![Python](https://img.shields.io/badge/-Python-3776AB?style=flat-square&logo=Python&logoColor=white)
![JSON](https://img.shields.io/badge/-JSON-000000?style=flat-square&logo=JSON&logoColor=white)
![Altium Designer](https://img.shields.io/badge/-Altium%20Designer-A5915F?style=flat-square&logo=Altium-Designer&logoColor=white)
![Visual Studio Code](https://img.shields.io/badge/Visual%20Studio%20Code-444444?style=flat-square&logo=Visual-Studio-Code&logoColor=007ACC)

## Software

- [esp-idf v4.4](https://github.com/espressif/esp-idf)
- component
  * [esp-who](https://github.com/espressif/esp-who)
  * [esp-sr](https://github.com/espressif/esp-sr)
  * [esp-skainet](https://github.com/espressif/esp-skainet)
  * [esp-camera](https://github.com/espressif/esp32-camera)

## Hardware

- ESP32-D0WD-V3/ESP-WROVER-B
- PSRAM : 4Mb
- Flash : 16Mb
- Mic on Board  MSM261S4030H0
- Three ADC Buttons in one gpio
- IMU : MPU6050
- SENSOR : OV2640/OV7725/OV5640/OV7670
- ST7789 IPS 240\*240 1.3`/1.54'
- Type-c USB

## GUI

- [LVGL](https://github.com/lvgl/lvgl)

## Font

- 阿里汉仪智能黑体
## Pic:

<img src="https://whycan.com/files/members/4115/%E6%AD%A3%E9%9D%A2.jpg" height="500px" width="800px" title="正面" style="display:inherit;"/> 

<img src="https://whycan.com/files/members/4115/1_20210826-1733.png" height="500px" width="800px" title="反面" style="display:inherit;"/> 

<img src="https://whycan.com/files/members/4115/9.png" height="500px" width="800px" title="反面" style="display:inherit;"/> 

<img src="https://whycan.com/files/members/4115/4_20210826-1734.png" height="500px" width="800px" title="反面" style="display:inherit;"/> 

<img src="https://whycan.com/files/members/4115/5.png" height="500px" width="800px" title="反面" style="display:inherit;"/> 

<img src="https://whycan.com/files/members/4115/7.png" height="500px" width="800px" title="反面" style="display:inherit;"/> 

<img src="https://whycan.com/files/members/4115/8.png" height="500px" width="800px" title="反面" style="display:inherit;"/> 



## Thanks
	 本项目参考了较多开源项目，非常感谢以下开源作者提供了很多思路：
- [WatchX](https://github.com/FASTSHIFT/WatchX)
- [offasr](https://gitee.com/killerp/off_asr)
- [哔哩哔哩小电视](https://gitee.com/wangpeng25/the-little-bili-tv)

## 禁止商用

本项目开源协议为  CC-BY-NC-SA 3.0（知识共享-署名-非商业性使用-相同方式共享 3.0）
仅供个人学习使用（防止有人放到闲鱼倒卖资料）
