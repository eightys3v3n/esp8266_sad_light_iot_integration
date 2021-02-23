// Written by Terrence Plunkett <eightys3v3n@gmail.com>
// 2021-02-22
//
// Allows the use of a Node ESP8266 to emulate a button for a BIOS S.A.D. light.
// This makes the non-smart light an IoT light with MQTT!


#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Automaton.h>

// WiFi config
#define SSID "2.4GhzWiFiName"
#define PASSWORD "WiFiPassword"
WiFiClient espClient;

// MQTT config
#define MQTT_PORT 1883
#define MQTT_SERVER "MQTT Host"
#define MQTT_USER "MQTT Username"
#define MQTT_PASS "MQTT Password"
#define TOPIC "sad_light/"
#define ID "sad_light"
PubSubClient client(espClient);

// Auto-reconnect WiFi and MQTT
#define NETWORK_INTERVAL 60000
Atm_timer network_timer;

// SAD light config
#define POWER_PIN D1
#define POWER_ON_INTERVAL 1000 // how many milliseconds to press power for when turning on.
#define POWER_OFF_INTERVAL 3000 // how many milliseconds to press power for when turning off.
Atm_timer power_on_timer, power_off_timer;

// Serial debugging interpreter
Atm_command commands;
char cmd_buffer[64];
enum {ON,OFF,RECONNECT_MQTT,RECONNECT};
const char cmd_list[] = "on off reconnect_mqtt reconnect";

// Functions used as a callback to change pin mode
void power_pin_high(int idx=NULL, int v=NULL, int up=NULL) {
  pinMode(POWER_PIN, HIGH);
  Serial.println("Set power pin to high");
}
void power_pin_low(int idx=NULL, int v=NULL, int up=NULL) {
  pinMode(POWER_PIN, LOW);
  Serial.println("Set power pin to low");
}

// Turn on the light
void power_on(int idx=NULL, int v=NULL, int up=NULL) {
  Serial.println("Powering on");
  power_pin_high();
  power_on_timer.trigger(power_on_timer.EVT_START);
}

// Turn off the light
void power_off(int idx=NULL, int v=NULL, int up=NULL) {
  Serial.println("Powering off");
  power_pin_high();
  power_off_timer.trigger(power_off_timer.EVT_START);
}

// Attempt to reconnect the WiFi and MQTT.
void network_reconnect(int idx=NULL, int v=NULL, int up=NULL) {
  if (WiFi.status() != WL_CONNECTED) {
    setup_wifi();
  }
  reconnect_mqtt();
  
}

// Connect to a WiFi network. Blocks until connected.
void setup_wifi() {
  Serial.print("Connecting to ");
  Serial.print(SSID);
  
  WiFi.begin(SSID, PASSWORD);
  
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");  
    delay(500);
  }
  Serial.println();
    
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

// Attempts to connect to the MQTT server once.
void reconnect_mqtt() {
  // Loop until we're reconnected
  if (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect(ID, MQTT_USER, MQTT_PASS)) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.println(client.state());
    }
  }
  client.loop();
}

// Receives messages from subscribed topics and dispatches the requested commands.
void mqtt_callback(String topic, byte* message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp;
  
  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();

  // Feel free to add more if statements to control more GPIOs with MQTT

  // If a message is received on the topic room/lamp, you check if the message is either on or off. Turns the lamp GPIO according to the message
  if(topic == TOPIC){
      Serial.print("Got MQTT message to power ");
      if(messageTemp == "on"){
        Serial.println("On");
        power_on();
      }
      else if(messageTemp == "off"){
        Serial.println("Off");
        power_off();
      } else {
        Serial.print("Got unknown MQTT message ");
        Serial.print(topic);
        Serial.print("/");
        Serial.println(messageTemp);
      }
  } else {
    Serial.print("Got unknown MQTT message ");
    Serial.print(topic);
    Serial.print("/");
    Serial.println(messageTemp);
  }
  Serial.println();
}

// Interprets serial commands for debugging.
void cmd_callback(int idx, int v, int up) {
  switch(v) {
  case ON:
    Serial.println("ON");
    power_on();
    return;
  case OFF:
    Serial.println("OFF");
    power_off();
    return;
  case RECONNECT:
    Serial.println("Reconnecting WiFi and MQTT");
    network_reconnect();
    return;
  case RECONNECT_MQTT:
    Serial.println("Reconnecting MQTT");
    reconnect_mqtt();
  }
}


void setup()
{
  Serial.begin(115200);
  delay(1000);
  Serial.print("Device: ");
  Serial.println(ID);

  // Setup MQTT server and start WiFi.
  client.setServer(MQTT_SERVER, MQTT_PORT);
  client.setCallback(mqtt_callback);
  network_reconnect();
  client.subscribe(TOPIC, 1);

  // Initialize utilized pins.
  pinMode(POWER_PIN, OUTPUT);
  digitalWrite(POWER_PIN, LOW);

  // Initiate and start various events based on time intervals.
  // Start this timer to turn the power pin back to low after turning the light on.
  power_on_timer.begin()
    .interval(POWER_ON_INTERVAL)
    .onTimer(power_pin_low);

  // Start this timer to turn the power pin back to low after turning the light off.
  // Has a longer delay than the turning on because its hold to turn off.
  power_off_timer.begin()
    .interval(POWER_OFF_INTERVAL)
    .onTimer(power_pin_low);

  // Start the command listener.
  commands.begin(Serial, cmd_buffer, sizeof(cmd_buffer))
    .list(cmd_list)
    .onCommand(cmd_callback);

  // Restart the network on a schedule.
  // A sloppy way to deal with disconnecting of MQTT or WiFi.
  network_timer.begin()
    .interval(NETWORK_INTERVAL)
    .repeat(-1)
    .onTimer(network_reconnect);
}

// Run the automaton timers and the MQTT client loop.
void loop() {
  automaton.run();
  client.loop();
}
