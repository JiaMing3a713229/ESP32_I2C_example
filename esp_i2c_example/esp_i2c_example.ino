#include <Arduino.h>
#include <WiFi.h>
#include <driver/i2c.h>
#include <PubSubClient.h>

#ifdef __cplusplus
extern "C"{
  #include "JM_SSD1306.h"
  #include "JM_AHT20.h"
}
#endif

#define I2C_MASTER_NUM 0                                /*!< esp32內部有2個I2C控制器，我們使用第1個(0)控制器*/
#define I2C_MASTER_FREQ_HZ          400000              /*!< I2C master clock frequency，400KHz */
#define I2C_MASTER_TX_BUF_DISABLE   0                   /*!< I2C master doesn't need buffer */
#define I2C_MASTER_RX_BUF_DISABLE   0                   /*!< I2C master doesn't need buffer */

#define SDA_PIN 21
#define SCL_PIN 22

#define AHT20_ADDRESS 0x38  // AHT20 I2C 位址

#define SSID          "ICLab"
#define PASS          "ICLab41208"
#define MQTT_SERVER   "broker.emqx.io"
#define MQTT_PORT      1883
#define MQTT_ID       "N96114093"

WiFiClient espClient;
PubSubClient client(espClient);

OLED oled;
int is_oled_initialized = 0;

AHT20 aht20;
char aht_buffer[50];
const char* esp32_Aht10 = "cps/esp32/Aht10";

uint32_t keepalive_time = 0;


/*
** @brief This function is used to connect to an Access point via WiFi
** @brief 此函式用於WiFi連線
*/
void WiFiConnect(const char* ssid, const char* pass)
{

  WiFi.begin(ssid, pass);
  while(WiFi.status() != WL_CONNECTED){
    if(is_oled_initialized){
      oled.print(&oled, 2, 3, "Connecting WiFi", 0);
    }
    printf(".");
  }
  printf("\n WiFi Connected \n");
  printf("Device IP Address: ");
  Serial.println(WiFi.localIP());

  vTaskDelay(2000 / portTICK_PERIOD_MS);

}

void reconnect() 
{
  while(!client.connected()){
    if (client.connect(MQTT_ID)) {
      client.subscribe(esp32_Aht10); // Subscribe to channel.
    }
  }
}


/* 
** @brief Set Callback used to retrieve message which include topic 、 message and length of length
** @brief 設置回調函數，用來取得MQTT訊息中的Topic、訊息和訊息長度 
*/
void callback(char* topic, byte* message, unsigned int length)
{

  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp;
  
  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();

}
/*
** @brief 此函數用於初始化並啟用ESP32 I2C傳輸控制器
*/

static esp_err_t i2c_master_init(int sda_io_num, int scl_io_num)
{

    int i2c_master_port = I2C_MASTER_NUM;

    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = sda_io_num,
        .scl_io_num = scl_io_num,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master = {
          .clk_speed = I2C_MASTER_FREQ_HZ,
        }
    };

    i2c_param_config(i2c_master_port, &conf);

    return i2c_driver_install(i2c_master_port, conf.mode, I2C_MASTER_RX_BUF_DISABLE, I2C_MASTER_TX_BUF_DISABLE, 0);
}

static esp_err_t init_i2c(void)
{

  while(i2c_master_init(SDA_PIN, SCL_PIN) != ESP_OK){

        vTaskDelay(20 / portTICK_PERIOD_MS);

  }
  return ESP_OK;

}

void mqttTask(void *pvParameter)
{

  WiFiConnect(SSID, PASS);
  oled.clear(&oled);
  vTaskDelay(1000 / portTICK_PERIOD_MS);
  client.setServer(MQTT_SERVER, 1883);
  client.setCallback(callback);
  vTaskDelay(200 / portTICK_PERIOD_MS);
  while(1){
    
    keepalive_time ++;
    if(!client.connected()){
        reconnect();
    }
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    Serial.printf("keepalive_time : %lu \n", keepalive_time);
    client.publish(esp32_Aht10, aht_buffer);
    client.loop();
  }

}

void aht20Task(void *pvParameter)
{

  vTaskDelay(100 / portTICK_PERIOD_MS);
  init_aht20(&aht20, AHT20_ADDRESS);

  while(1){

    snprintf(aht_buffer, sizeof(aht_buffer), " T: %2d H: %2d ", int(aht20.get_temperature(&aht20)), int(aht20.get_humidity(&aht20)));
    printf("%s \n", aht_buffer);
    vTaskDelay(500 / portTICK_PERIOD_MS);

  }
}

void displayTask(void *pvParameter)
{


  //初始化OLED暫存器配置
  init_oled(&oled, SDA_PIN, SCL_PIN, 0x3c);
  if((&oled) == NULL){
    printf("\nInitialize OLED error !\r\n");
  }

  vTaskDelay(100 / portTICK_PERIOD_MS);
  Serial.printf("start oled task \r\n");
  is_oled_initialized = 1;

  xTaskCreate(mqttTask, "mqttTask", 1024 * 4, NULL, 1, NULL);
  xTaskCreate(aht20Task, "aht20Task", 1024 * 2, NULL, 1, NULL);
  // oled.display_info(&oled);
  // oled.clear(&oled);
  // vTaskDelay(200 / portTICK_PERIOD_MS);

  while(1){

    oled.print(&oled, 1, 1, aht_buffer, 0);
    vTaskDelay(100 / portTICK_PERIOD_MS);

  }
}




void setup() {

  vTaskDelay(2000 / portTICK_PERIOD_MS);
  Serial.begin(115200);

  //初始化並且啟用ESP32 I2C通訊介面
  if(init_i2c() == ESP_OK){
    Serial.println("i2c initialize sucessed");
  }

  //建立並初始化顯示工作的Threads
  xTaskCreate(displayTask, "TaskforDisplay", 1024 * 2, NULL, 1, NULL);
  
}


void loop() {
  
}

