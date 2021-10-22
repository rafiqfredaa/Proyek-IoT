#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>
#include "DHT.h"
#define DHT11PIN 27

DHT dht(DHT11PIN, DHT11);

// Menghubungkan pada Wifi
const char* ssid = "KontsBaru";
const char* password = "kontsbaru123";

// Initialize Telegram BOT
#define BOTtoken "2066308050:AAFAf5ytEp1IPHWn081t4zkjYQU3m6-flIk" 
#define CHAT_ID "1326789755"

//Checks for new messages every 1 second.
int botRequestDelay = 1000;
unsigned long lastTimeBotRan;

WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);

void handleNewMessages(int numNewMessages) {
  Serial.println("handleNewMessages");
  Serial.println(String(numNewMessages));

  for (int i=0; i<numNewMessages; i++) {
    String chat_id = String(bot.messages[i].chat_id);
    String text = bot.messages[i].text;

    String from_name = bot.messages[i].from_name;
    if (from_name == "") from_name = "Guest";
 
    //Cek Pembacaan Sensor DHT11
    if (text == "/statussuhu") {
      int t = dht.readTemperature();
       String temp = "Suhu saat ini : ";
       temp += int(t);
       temp +=" *C\n";
      
      bot.sendMessage(chat_id,temp, "");
    }
    if (text == "/statuskelembapan") {
      int h = dht.readHumidity();
       String temp = "Kelembaban: ";
       temp += int(h);
       temp += " %";
     
      bot.sendMessage(chat_id,temp, "");
    }
    
    //Cek Command untuk setiap aksi
    if (text == "/start") {
      String welcome = "Welcome  " + from_name + ".\n";
      welcome += "/statussuhu : Status Suhu\n";
      welcome += "/statuskelembapan : Status Kelembapan\n";
      bot.sendMessage(chat_id, welcome, "Markdown");
    }
  }
}

void setup() {
  Serial.begin(115200);
  // Start the DHT11 Sensor
  dht.begin();

  // Attempt to connect to Wifi network:
  Serial.print("Connecting Wifi: ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  client.setCACert(TELEGRAM_CERTIFICATE_ROOT);
  
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  
  bot.sendMessage(CHAT_ID, "Monitor Suhu Kamar Mulai", "");
}

void loop() {
  int t = dht.readTemperature();
  int h = dht.readHumidity();
  if (millis() > lastTimeBotRan + botRequestDelay)  {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

    while(numNewMessages) {
      Serial.println("got response");
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }
    lastTimeBotRan = millis();
  }
}
