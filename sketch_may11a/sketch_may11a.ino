#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Automaton.h>

// WiFi config
#define SSID "2.4GHzWiFiSSID"
#define PASSWORD "WiFiPassword"
WiFiClient espClient;

// MQTT config
#define MQTT_PORT 8883
#define MQTT_SERVER "HOSTNAME.com"
#define MQTT_USER "USERNAME"
#define MQTT_PASS "PASSWORD"
#define TOPIC "temperatures/temp6"
#define ID "sad_iot"
#define POST_INTERVAL 60000
PubSubClient client(espClient);

// SAD light config
// pin 2 is the built in LED?
#define POWER_PIN D1
#define POWER_ON_INTERVAL 1000 // how many milliseconds to press power for when turning on.
#define POWER_OFF_INTERVAL 3000 // how many milliseconds to press power for when turning off.
Atm_timer power_on_timer, power_off_timer;

Atm_command commands;
char cmd_buffer[64];
enum {ON,OFF};
const char cmd_list[] = "on off";


void power_pin_high(int idx=NULL, int v=NULL, int up=NULL) {
  pinMode(POWER_PIN, HIGH);
  Serial.println("Set power pin to high");
}

void power_pin_low(int idx=NULL, int v=NULL, int up=NULL) {
  pinMode(POWER_PIN, LOW);
  Serial.println("Set power pin to low");
}

void power_on(int idx=NULL, int v=NULL, int up=NULL) {
  Serial.println("Powering on");
  power_pin_high();
  power_on_timer.trigger(power_on_timer.EVT_START);
}

void power_off(int idx=NULL, int v=NULL, int up=NULL) {
  Serial.println("Powering off");
  power_pin_high();
  power_off_timer.trigger(power_off_timer.EVT_START);
}

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
  }
}




void setup()
{
  Serial.begin(115200);
  Serial.println("Start");
//  setup_wifi();
//  client.setServer(MQTT_SERVER, MQTT_PORT);

  pinMode(POWER_PIN, OUTPUT);

  power_on_timer.begin()
    .interval(POWER_ON_INTERVAL)
    .onTimer(power_pin_low)
    .start();

  power_off_timer.begin()
    .interval(POWER_OFF_INTERVAL)
    .onTimer(power_pin_low)
    .start();

  commands.begin(Serial, cmd_buffer, sizeof(cmd_buffer))
    .list(cmd_list)
    .onCommand(cmd_callback);

  Serial.print("Device :");
  Serial.println(ID);
}


void loop() {
  automaton.run();
}






void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(SSID);
  WiFi.begin(SSID, PASSWORD);

  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect(ID, MQTT_USER, MQTT_PASS)) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(10000);
    }
  }
}
