#include "config.h"
#include "Adafruit_Si7021.h"

#define expanderPin 14
#define expanderButton 12

Adafruit_Si7021 sensor = Adafruit_Si7021();

AdafruitIO_Feed *expander = io.feed("2P_BeamExpander");
AdafruitIO_Feed *humidity = io.feed("2P_Humidity");
AdafruitIO_Feed *temp = io.feed("2P_Temp");

int secCounter;
float initT;
bool pollTH = 0;
bool expanderState;

void setup() {

  pinMode(expanderPin, OUTPUT);
  digitalWrite(expanderPin, LOW);
  pinMode(expanderButton, INPUT);
  digitalWrite(expanderButton, LOW);
  expanderState=0;

  Serial.begin(115200);

    while (!Serial) {
    delay(10);
  }

  Serial.println("Si7021 test!");
  
  if (!sensor.begin()) {
    Serial.println("Did not find Si7021 sensor!");
    while (true);
  }

  // connect to io.adafruit.com
  Serial.print("Connecting to Adafruit IO");
  io.connect();


  expander->onMessage(handleMessage);

  // wait for a connection
  while (io.status() < AIO_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  // we are connected
  Serial.println();
  Serial.println(io.statusText());
  initT = millis();
}

void loop() {
  secCounter = millis() - initT;
  if (secCounter >= 30000) {
    initT = millis();
    pollTH = 1;
  }
  io.run();
  handleTrigger(expanderButton, expanderPin);
  if (pollTH == 1) {
    Serial.print("Humidity:    "); Serial.print(sensor.readHumidity(), 2);
    Serial.print("\tTemperature: "); Serial.println(sensor.readTemperature(), 2);
    humidity->save(sensor.readHumidity());
    pollTH = 0;
  }
}

void handleMessage(AdafruitIO_Data *data) {
  Serial.print("received <- ");
  if (data->toPinLevel() == HIGH)
    Serial.println("HIGH");
  else
    Serial.println("LOW");
  digitalWrite(expanderPin, data->toPinLevel());
}

void handleTrigger(int trigPin, int writePin) {
  static bool newState;
  bool curTrigState = digitalRead(trigPin);
  bool curWriteState = digitalRead(writePin);

  if (curWriteState == 1) {
    newState = 0;
  }
  else if (curWriteState == 0) {
    newState = 1;
  }
  if (curTrigState == 1) {
    digitalWrite(writePin, newState);
  }
  expanderState=newState;
}



