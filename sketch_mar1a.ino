#include <WiFi.h>
#include "DHT.h"
#include <WiFiClientSecure.h>

// ----------------------------------------------------------------------------------------------
DHT dht;

// ----------------------------------------------------------------------------------------------
// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "Por";
char pass[] = "12345678";
String GAS_ID = "AKfycbyc-d3b_M0KKxZzmFbrw9l6lXRWH2R3NJcQj0HOBq6PJK3KrQiwdg5e3UAlO1kt-YAO"; //--> spreadsheet script ID

//Your Domain name with URL path or IP address with path
const char* host = "script.google.com"; // only google.com not https://google.com

// ----------------------------------------------------------------------------------------------
#define DHT_PIN 15
#define DHTTYPE DHT22
#define UPDATE_INTERVAL_HOUR  (0)
#define UPDATE_INTERVAL_MIN   (0)
#define UPDATE_INTERVAL_SEC   (30)

#define UPDATE_INTERVAL_MS    ( ((UPDATE_INTERVAL_HOUR*60*60) + (UPDATE_INTERVAL_MIN * 60) + UPDATE_INTERVAL_SEC ) * 1000 )

// ----------------------------------------------------------------------------------------------
int Temperature;
int Humidity;
// ----------------------------------------------------------------------------------------------
void update_google_sheet()
{
    Serial.print("connecting to ");
    Serial.println(host);
  
    // Use WiFiClient class to create TCP connections
    WiFiClientSecure client;
    const int httpPort = 443; // 80 is for HTTP / 443 is for HTTPS!
    
    client.setInsecure(); // this is the magical line that makes everything work
    
    if (!client.connect(host, httpPort)) { //works!
      Serial.println("connection failed");
      return;
    }
       
    //----------------------------------------Processing data and sending data
    String url = "/macros/s/" + GAS_ID + "/exec?temperature=";
   
    url += String(Temperature);
    
    url += "&humidity=";
    url += String(Humidity);

    Serial.print("Requesting URL: ");
    Serial.println(url);
  
    // This will send the request to the server
    client.print(String("GET ") + url + " HTTP/1.1\r\n" +
                 "Host: " + host + "\r\n" + 
                 "Connection: close\r\n\r\n");
  
    Serial.println();
    Serial.println("closing connection");  
}

// ----------------------------------------------------------------------------------------------
void setup()
{
  // Debug console
  Serial.begin(115200);



  // DHT Setup
  dht.setup(DHT_PIN); 

//----------------------------------------Wait for connection
  Serial.print("Connecting");
  WiFi.begin(ssid, pass); //--> Connect to your WiFi router
  while (WiFi.status() != WL_CONNECTED) 
  {
    Serial.print(".");
  }

}

// ----------------------------------------------------------------------------------------------
unsigned long time_ms;
unsigned long time_1000_ms_buf;
unsigned long time_sheet_update_buf;
unsigned long time_dif;

void loop()
{
  time_ms = millis();
  time_dif = time_ms - time_1000_ms_buf;

  // Read and print serial data every 1 sec
  if ( time_dif >= 1000 ) // 1sec 
  {
    time_1000_ms_buf = time_ms;
    Temperature = dht.getTemperature();
    Humidity = dht.getHumidity();
    // Print serial messages
    Serial.print("Humidity: " + String(Humidity) + " %");
    Serial.print("\t");
    Serial.println("Temperature: " + String(Temperature) + " C");


  }

  // Update data to google sheet in specific period
  time_ms = millis();
  time_dif = time_ms - time_sheet_update_buf;  
  if ( time_dif >= UPDATE_INTERVAL_MS ) // Specific period
  {
    time_sheet_update_buf = time_ms;
    update_google_sheet();
  }
 
}
