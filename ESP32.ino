#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>   // Universal Telegram Bot Library written by Brian Lough: https://github.com/witnessmenow/Universal-Arduino-Telegram-Bot
#include <ArduinoJson.h>
int resolution=1/3300;
// Replace with your network credentials
char* ssid = "";
char* password = "";

// Initialize Telegram BOT
#define BOTtoken ""  // your Bot Token (Get from Botfather)

// Use @myidbot to find out the chat ID of an individual or a group
// Also note that you need to click "start" on a bot before it can
// message you
#define CHAT_ID ""

WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);

// Checks for new messages every 1 second.
int botRequestDelay = 1000;
unsigned long lastTimeBotRan;

//Variable to store temperature,heart rate and oxygen level
String tmp;
String hrt;
String oxy;

// Handle what happens when you receive new messages
void handleNewMessages(int numNewMessages) {
  Serial.println("handleNewMessages");
  Serial.println(String(numNewMessages));

  for (int i=0; i<numNewMessages; i++) {
    // Chat id of the requester
    String chat_id = String(bot.messages[i].chat_id);
    if (chat_id != CHAT_ID){
      bot.sendMessage(chat_id, "Unauthorized user", "");
      continue;
    }
    
    // Print the received message
    String text = bot.messages[i].text;
    Serial.println(text);

    String from_name = bot.messages[i].from_name;

    if (text == "/start") {
      String welcome = "Welcome, " + from_name + ".\n";
      welcome += "Use the following commands to control your outputs.\n\n";
      welcome += "/tmp to get the temperature \n";
      welcome += "/hrt to get the heart rate \n";
      welcome += "/oxy to get oxygen level \n";
      bot.sendMessage(chat_id, welcome, "");
    }
    
    if (text == "/tmp"){
      String tmp="Temperature:";
      float temp = ((analogRead(A0) * resolution) * 100)-283.15;
      tmp+=String(temp);
      tmp+="C";
      bot.sendMessage(chat_id,tmp, "");
    }

    if (text == "/hrt") {
      hrt="Sensor not connected";
      bot.sendMessage(chat_id,hrt, "");
    }

    if (text == "/oxy") {
      oxy="Sensor not connected";
      bot.sendMessage(chat_id,oxy, "");
    }
  }
}

void setup() {
  Serial.begin(9600);
  
  // Connect to Wi-Fi
  WiFi.begin(ssid, password);

  // Add root certificate for api.telegram.org
  client.setCACert(TELEGRAM_CERTIFICATE_ROOT); 

  //Checking if WiFi connected
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }
  
  // Print ESP32 Local IP Address
  Serial.println(WiFi.localIP());
}

void loop() {
  if (millis() > lastTimeBotRan + botRequestDelay){
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

    while(numNewMessages) {
      Serial.println("got response");
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }
    lastTimeBotRan = millis();
  }
}
