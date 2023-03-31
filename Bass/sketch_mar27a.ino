#include <WiFi.h>
#include <HTTPClient.h>


const char * ssid = "siya";
const char * password = "siyasiya";
String GOOGLE_SCRIPT_ID = "AKfycbzvq8fJbpsOKB5FmScUq5dqLb0ImkLhOR56NbMoLnKgHaACxL4QU6-TCkW9wdBAXLvwCA";


#define relay1_pin 13

const int sendInterval = 2000;

WiFiClientSecure client;


void setup() {

  pinMode(relay1_pin, OUTPUT);
  Serial.begin(115200);
  delay(10);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("OK");

}

void loop() {
  read_google_sheet();
  
  delay(sendInterval);
}

void read_google_sheet(void) {

   HTTPClient http;
   String url="https://script.google.com/macros/s/"+GOOGLE_SCRIPT_ID+"/exec?read";

  Serial.println("Reading Data From Google Sheet.....");
  http.begin(url.c_str());
  http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
  int httpCode = http.GET();
  Serial.print("HTTP Status Code: ");
  Serial.println(httpCode);
  if(httpCode <= 0){Serial.println("Error on HTTP request"); http.end(); return;}
  String payload = http.getString();
  Serial.println("Payload: "+payload);
  if(httpCode == 200){
    String temp = payload.substring(0, 1);
    int relay_number = temp.toInt();
    payload.remove(0, 1);
    payload.toLowerCase();
    payload.trim();
    Serial.println("Payload - Command: "+payload);
    if(payload != "on" && payload != "off")
      {Serial.println("Invalid Command"); http.end(); return;}
    if(relay_number < 1 || relay_number > 4)
      {Serial.println("Invalid Relay Number"); http.end(); return;}
    int relay_state = control_relay(relay_number, payload);
    write_google_sheet( "relay_number="+String(relay_number)+"&relay_state="+String(relay_state) );
  }
  http.end();
}

void write_google_sheet(String params) {
   HTTPClient http;
   String url="https://script.google.com/macros/s/"+GOOGLE_SCRIPT_ID+"/exec?"+params;
    Serial.println("Updating Status of Relay");
    http.begin(url.c_str());
    http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
    int httpCode = http.GET();  
    Serial.print("HTTP Status Code: ");
    Serial.println(httpCode);
    
    String payload;
    if (httpCode > 0) {
        payload = http.getString();
        Serial.println("Payload: "+payload);     
    }
    http.end();
}

int control_relay(int relay, String command){
  switch (relay) {
    case 1:
      if(command == "on"){
        digitalWrite(relay1_pin, HIGH);
        Serial.println("Relay 1 is ON");
        return 1;
      } else {
        digitalWrite(relay1_pin, LOW);
        Serial.println("Relay 1 is OFF");
        return 0;
      }
      break; 
    default:
      return -1;
      break;
  }
}