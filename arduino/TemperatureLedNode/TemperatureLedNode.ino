// Example sketch showing how to send in OneWire temperature readings
#include <MySensor.h>
#include <SPI.h>

#ifdef SOFTSPI 
#error Please turn off SOFTSPI
#endif

#ifdef DEV
  #define SLEEP_TIME 500 // Sleep time between reads (in milliseconds)
#else
  #define SLEEP_TIME 30000 // Sleep time between reads (in milliseconds)
#endif

unsigned long lastUpdate = 0;
MySensor gw;
boolean receivedConfig = false;
boolean metric = true;
// Initialize temperature message
MyMessage msg(0, V_TEMP);

void setup()
{ 
  analogReference(INTERNAL);
  pinMode(7, OUTPUT);
  pinMode(11, OUTPUT);
  pinMode(13, OUTPUT);
  pinMode(5, OUTPUT);
  // Startup and initialize MySensors library. Set callback for incoming messages.
  gw.begin(incomingMessage);

  // Send the sketch version information to the gateway and Controller
  gw.sendSketchInfo("Temperature Sensor", "1.0");

  // Present all sensors to controller
  gw.present(0, S_TEMP);
  gw.request(2, V_DIMMER);
  gw.request(3, V_DIMMER);
  gw.request(4, V_DIMMER);
  
}

void processTemperature() {
  if (millis() - lastUpdate > SLEEP_TIME) {
    float rawTemp = float(analogRead(A0)) / 400;
    float temperature = (rawTemp * 100) - 50;
    gw.send(msg.setSensor(0).set(temperature, 1));
    lastUpdate = millis();
  }
}

void loop() {
  // Process incoming messages (like config from server)
  gw.process();
  processTemperature();
}

void incomingMessage(const MyMessage &message) {
  // We only expect one type of message from controller. But we better check anyway.
  if (message.type == V_LIGHT && message.sensor == 1) {
      digitalWrite(7, message.getBool());
  }
  else if (message.type == V_DIMMER && message.sensor == 2) {
    float value = message.getInt()*2.55;
    analogWrite(11, value);
  }
  else if (message.type == V_DIMMER && message.sensor == 3) {
    float value = message.getInt()*2.55;
    analogWrite(13, value);
  }
  else if (message.type == V_DIMMER && message.sensor == 4) {
    float value = message.getInt()*2.55;
    analogWrite(5, value);
  }
}
