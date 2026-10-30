#include <Wire.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include "mpu6050.h"

/* ---- Réglages ---- */
const char *MQTT_BROKER = "broker.hivemq.com";
const int   MQTT_PORT   = 1883;
/* CHANGE la partie "monpseudo" : le topic doit être unique */
const char *MQTT_TOPIC  = "r44/r4ffik/mpu6050";

WiFiClient   wifiClient;
PubSubClient mqtt(wifiClient);

/* ---- I2C (inchangé) ---- */
static int i2c_read(uint8_t dev_addr, uint8_t reg, uint8_t *buf, uint8_t len)
{
  Wire.beginTransmission(dev_addr);
  Wire.write(reg);
  if (Wire.endTransmission(false) != 0) {
    return -1;
  }
  if (Wire.requestFrom((uint16_t)dev_addr, (size_t)len, true) != len) {
    return -1;
  }
  for (uint8_t i = 0; i < len; i++) {
    buf[i] = Wire.read();
  }
  return 0;
}

static int i2c_write(uint8_t dev_addr, uint8_t reg, const uint8_t *buf, uint8_t len)
{
  Wire.beginTransmission(dev_addr);
  Wire.write(reg);
  Wire.write(buf, len);
  return (Wire.endTransmission() == 0) ? 0 : -1;
}

/* ---- Réseau ---- */
static void connect_wifi()
{
  Serial.print("Connexion Wi-Fi");
  WiFi.begin("Wokwi-GUEST", "", 6);          // réseau virtuel de Wokwi, canal 6
  while (WiFi.status() != WL_CONNECTED) {
    delay(250);
    Serial.print(".");
  }
  Serial.println(" OK");
}

static void connect_mqtt()
{
  while (!mqtt.connected()) {
    Serial.print("Connexion MQTT...");
    /* identifiant aléatoire : deux clients ne doivent pas avoir le même */
    String id = "esp32-" + String(random(0xFFFFFF), HEX);
    if (mqtt.connect(id.c_str())) {
      Serial.println(" OK");
    } else {
      Serial.print(" echec, code = ");
      Serial.println(mqtt.state());
      delay(2000);
    }
  }
}

/* ---- Programme principal ---- */
static mpu6050_t imu;

void setup() {
  Serial.begin(115200);
  Wire.begin();                              // SDA = GPIO 21, SCL = GPIO 22

  int err = mpu6050_init(&imu, MPU6050_ADDR_LOW, i2c_read, i2c_write);
  if (err != MPU6050_OK) {
    Serial.print("Erreur init MPU6050, code = ");
    Serial.println(err);
    while (true) {
      delay(1000);
    }
  }
  Serial.println("MPU6050 initialise");

  connect_wifi();
  mqtt.setServer(MQTT_BROKER, MQTT_PORT);
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    connect_wifi();
  }
  if (!mqtt.connected()) {
    connect_mqtt();
  }
  mqtt.loop();

  mpu6050_vec3_t a;
  float t;
  if (mpu6050_read_accel(&imu, &a) == MPU6050_OK &&
      mpu6050_read_temp(&imu, &t) == MPU6050_OK) {
    char msg[80];
    snprintf(msg, sizeof msg, "ax=%.2f ay=%.2f az=%.2f T=%.1f",
             a.x, a.y, a.z, t);
    mqtt.publish(MQTT_TOPIC, msg);
    Serial.println(msg);
  } else {
    Serial.println("Erreur de lecture");
  }
  delay(1000);
}