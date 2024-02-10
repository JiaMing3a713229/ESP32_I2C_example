# ESP32  入門教學
---
## CPS LAB1
---
#### ESP32 官方支援文檔
#### [https://docs.espressif.com/projects/arduino-esp32/en/latest/getting_started.html](https://docs.espressif.com/projects/arduino-esp32/en/latest/getting_started.html)
![image](https://hackmd.io/_uploads/HkxJarMsT.png)

---
## 1. 下載Arduino
#### 版本2.3.0 [https://www.arduino.cc/en/software](https://www.arduino.cc/en/software)
![image](https://hackmd.io/_uploads/HJv-pSMsT.png)
---

## 2.下載並安裝Arduino-ESP32
#### Arduino-ESP32 [https://docs.espressif.com/projects/arduino-esp32/en/latest/installing.html](https://docs.espressif.com/projects/arduino-esp32/en/latest/installing.html)


* ### 開發版管理器新增ESP32
* ### File -> Preference -> Additional Board Manager URL
![image](https://hackmd.io/_uploads/SyIARSzsp.png)
===
![image](https://hackmd.io/_uploads/rJ29CBGja.png)
* ### Stable release link
:::info
https://espressif.github.io/arduino-esp32/package_esp32_index.json
:::

---

## 4. 從開發版管理器找到Arduino ESP32 Boaed並且安裝至Arduino
![image](https://hackmd.io/_uploads/H1MygUzs6.png)
===
## 5. 進入ESP32開發環境
#### 安裝完成後可以從Tool->Board選擇ESP32開發版進行ESP32程式撰寫
![image](https://hackmd.io/_uploads/ByZlfLMoa.png)
===

# 測試範例檔案
```C
/* The true ESP32 chip ID is essentially its MAC address.
This sketch provides an alternate chip ID that matches 
the output of the ESP.getChipId() function on ESP8266 
(i.e. a 32-bit integer matching the last 3 bytes of 
the MAC address. This is less unique than the 
MAC address chip ID, but is helpful when you need 
an identifier that can be no more than a 32-bit integer 
(like for switch...case).

created 2020-06-07 by cweinhofer
with help from Cicicok */
	
uint32_t chipId = 0;

void setup() {
	Serial.begin(115200);
}

void loop() {
	for(int i=0; i<17; i=i+8) {
	  chipId |= ((ESP.getEfuseMac() >> (40 - i)) & 0xff) << i;
	}

	Serial.printf("ESP32 Chip model = %s Rev %d\n", ESP.getChipModel(), ESP.getChipRevision());
	Serial.printf("This chip has %d cores\n", ESP.getChipCores());
  Serial.print("Chip ID: "); Serial.println(chipId);
  
	delay(3000);

}

```

* ## 選擇串列傳輸埠燒錄程式至ESP32
    * ### Tool -> Port
![image](https://hackmd.io/_uploads/rJgEVUMjp.png)

* ## 打開Serial port monitor 查看 ESP32 print("")打印文字
![image](https://hackmd.io/_uploads/H1blrIzj6.png)

### 因ESP32 與 PC 是採用非同步串列傳輸的放式進行資訊傳輸，所以鮑率Baud rate需一致，才可正確讀取資料。
![image](https://hackmd.io/_uploads/ByEIPUMo6.png)

---
# **測試OLED、AHT10溫溼度感測器及MQTT傳輸通訊**
### **測試範例程式**
#### [https://github.com/JiaMing3a713229/ESP32_I2C_example](https://github.com/JiaMing3a713229/ESP32_I2C_example)
### **OLED 說明文檔**
[https://hackmd.io/Y93OKJtMQfmQu3ARHtU9Ww](https://https://hackmd.io/Y93OKJtMQfmQu3ARHtU9Ww)
### **AHT10 說明文檔**
[https://hackmd.io/maJTnTudSQ6FCcbPDPmOow](https://hackmd.io/maJTnTudSQ6FCcbPDPmOow)








