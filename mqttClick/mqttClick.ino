

#include <adafruit_aio.h>

#define WLAN_SSID         "CloudyBrain"
#define WLAN_PASS         "333stamp"

#define AIO_USERNAME      "cdeister"
#define AIO_KEY           "d388540dc18242eabf04f54fd9993b5f"


#define BUTTONFEED     "ci01_solenoid"
#define ONOFF_FEED         "onoff"
#define USE_TLS             0


AdafruitAIO       aio(AIO_USERNAME, AIO_KEY);
AdafruitAIOFeed   photocell (&aio, BUTTONFEED);

// button state
bool current = false;
bool last = false;

// set up the 'digital' feed
AdafruitIO_Feed *digital = io.feed("ci01_solenoid");

void setup() {

  // set button pin as an input
  pinMode(LED_BUILTIN, INPUT);

  Serial.begin(115200);
  delay(1);

  while ( !connectAP() ) {
    delay(500); // delay between each attempt
  }

  // Tell the MQTT client to auto print error codes and halt on errors
  aio.err_actions(true, true);

  // Set ClientID if defined
#ifdef CLIENTID
  aio.clientID(CLIENTID);
#endif

  if ( USE_TLS )
  {
    aio.connectSSL(); // Will halted if an error occurs
  } else
  {
    aio.connect(); // Will halted if an error occurs
  }
  Serial.println("OK");

  // 'Follow' the onoff feed to capture any state changes
  onoff.follow(feed_callback);
}

void loop() {

  // io.run(); is required for all sketches.
  // it should always be present at the top of your loop
  // function. it keeps the client connected to
  // io.adafruit.com, and processes any incoming data.
  io.run();

  // grab the current state of the button.
  // we have to flip the logic because we are
  // using a pullup resistor.
  if (digitalRead(LED_BUILTIN) == LOW)
    current = true;
  else
    current = false;

  // return if the value hasn't changed
  if (current == last)
    return;

  // save the current state to the 'digital' feed on adafruit io
  Serial.print("sending button -> ");
  Serial.println(current);
  digital->save(current);

  // store last button state
  last = current;

}
