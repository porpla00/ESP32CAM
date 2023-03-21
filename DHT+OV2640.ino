#include "esp_camera.h"
#include <WiFi.h>
#include "DHT.h"
#include <WiFiClientSecure.h>

DHT dht;

const char* ssid = "Por";
const char* password = "12345678";

String GAS_ID = "AKfycbyc-d3b_M0KKxZzmFbrw9l6lXRWH2R3NJcQj0HOBq6PJK3KrQiwdg5e3UAlO1kt-YAO";
const char* host = "script.google.com";

#define DHT_PIN 15
#define DHTTYPE DHT22
#define UPDATE_INTERVAL_HOUR (0)
#define UPDATE_INTERVAL_MIN (0)
#define UPDATE_INTERVAL_SEC (30)
#define UPDATE_INTERVAL_MS (((UPDATE_INTERVAL_HOUR * 60 * 60) + (UPDATE_INTERVAL_MIN * 60) + UPDATE_INTERVAL_SEC) * 1000)
#define CAMERA_MODEL_AI_THINKER

#if defined(CAMERA_MODEL_WROVER_KIT)
#define PWDN_GPIO_NUM -1
#define RESET_GPIO_NUM -1
#define XCLK_GPIO_NUM 21
#define SIOD_GPIO_NUM 26
#define SIOC_GPIO_NUM 27

#define Y9_GPIO_NUM 35
#define Y8_GPIO_NUM 34
#define Y7_GPIO_NUM 39
#define Y6_GPIO_NUM 36
#define Y5_GPIO_NUM 19
#define Y4_GPIO_NUM 18
#define Y3_GPIO_NUM 5
#define Y2_GPIO_NUM 4
#define VSYNC_GPIO_NUM 25
#define HREF_GPIO_NUM 23
#define PCLK_GPIO_NUM 22


#elif defined(CAMERA_MODEL_AI_THINKER)
#define PWDN_GPIO_NUM 32
#define RESET_GPIO_NUM -1
#define XCLK_GPIO_NUM 0
#define SIOD_GPIO_NUM 26
#define SIOC_GPIO_NUM 27

#define Y9_GPIO_NUM 35
#define Y8_GPIO_NUM 34
#define Y7_GPIO_NUM 39
#define Y6_GPIO_NUM 36
#define Y5_GPIO_NUM 21
#define Y4_GPIO_NUM 19
#define Y3_GPIO_NUM 18
#define Y2_GPIO_NUM 5
#define VSYNC_GPIO_NUM 25
#define HREF_GPIO_NUM 23
#define PCLK_GPIO_NUM 22

#else
#error "Camera model not selected"
#endif

int Temperature;
extern String WiFiAddr = "";

void startCameraServer();

void update_google_sheet() {
  Serial.print("connecting to ");
  Serial.println(host);

  WiFiClientSecure client;
  const int httpPort = 443;

  client.setInsecure();

  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    return;
  }


  String url = "/macros/s/" + GAS_ID + "/exec?temperature=";

  url += String(Temperature);

  Serial.print("Requesting URL: ");
  Serial.println(url);


  client.print(String("GET ") + url + " HTTP/1.1\r\n" + "Host: " + host + "\r\n" + "Connection: close\r\n\r\n");

  Serial.println();
  Serial.println("closing connection");
}

void setup() {
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  Serial.println();

  dht.setup(DHT_PIN);

  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;

  if (psramFound()) {
    config.frame_size = FRAMESIZE_UXGA;
    config.jpeg_quality = 10;
    config.fb_count = 2;
  } else {
    config.frame_size = FRAMESIZE_SVGA;
    config.jpeg_quality = 12;
    config.fb_count = 1;
  }

  // camera init
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }


  sensor_t* s = esp_camera_sensor_get();
  s->set_framesize(s, FRAMESIZE_CIF);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");

  startCameraServer();

  Serial.print("Camera Ready! Use 'http://");
  Serial.print(WiFi.localIP());
  WiFiAddr = WiFi.localIP().toString();
  Serial.println("' to connect");
}

unsigned long time_ms;
unsigned long time_1000_ms_buf;
unsigned long time_sheet_update_buf;
unsigned long time_dif;

void loop() {
  time_ms = millis();
  time_dif = time_ms - time_1000_ms_buf;


  if (time_dif >= 1000) {
    time_1000_ms_buf = time_ms;
    Temperature = dht.getTemperature();


    Serial.print("\t");
    Serial.println("Temperature: " + String(Temperature) + " C");
  }


  time_ms = millis();
  time_dif = time_ms - time_sheet_update_buf;
  if (time_dif >= UPDATE_INTERVAL_MS) {
    time_sheet_update_buf = time_ms;
    update_google_sheet();
  }
}