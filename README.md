# ESP8266_Seniverse_MQTT  
## 项目介绍  
该项目使用ESP2866作为主控，结合DHT11采集环境温湿度，并通过心知天气API获取当地天气实况，最后在SSD1306上循环显示，达到温湿度计功能。
![图片](https://github.com/AnWangzi/ESP8266_Seniverse_MQTT/assets/36923553/41f78778-47a7-4018-9e4c-828fa57b38f5)
另外还可以连接到自建的MQTT平台，上传采集到的温湿度数据并进行统计，本人使用的是ThingsBoard。
![图片](https://github.com/AnWangzi/ESP8266_Seniverse_MQTT/assets/36923553/5a574e1a-8400-483d-9d61-7909f1e0fa69)
原理图及PCB请见[立创开源](https://oshwhub.com/anwangzi/usb-8266-wen-shi-du)  
本项目采用Platformio构建，可以直接将工程导入，上传使用。  
## 已知BUG  
### 1、开机时如果没有连上WiFi，屏幕不会显示  
### 2、开机后如果断网，数据显示会卡住  
## 反馈  
提交Issue或者发邮件到wz@wz2m.cn
