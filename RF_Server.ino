/*
  Simple example for receiving
  
  https://github.com/sui77/rc-switch/
*/

#include <RCSwitch.h>
#include "RFButton.h"

#define RingButtonSt1Value 1940096467
#define RingButtonSt1Value 303879571
#define Lamp1ButtonValue 5971969

const char RingButtonSignalPin = 5;
const char Lamp1ButtonSignalPin = 5;

bool ringButtonState = false;
bool lamp1ButtonState = false;

RCSwitch mySwitch = RCSwitch();

void setup() {
    Serial.begin(115200);
    mySwitch.enableReceive(digitalPinToInterrupt(12));  // Receiver on interrupt 0 => that is pin #3 for arduino micro
    //  digitalPinToInterrupt(12)digitalPinToInterrupt(12) (D6) esp8266

    RFButton.add(0, Lamp1ButtonValue, true, true);
    RFButton.add(1, RingButtonSt1Value, false, false);
    RFButton.add(1, RingButtonSt1Value, false, false);

    RFButton.setCallback([&](uint8_t id, RFButtonEvent event) {
        // Only one button is added, no need to check the id.
        if (event == RFBUTTONEVENT_SINGLECLICK) {
            Serial.println("RFBUTTONEVENT_SINGLECLICK ");
        } else if (event == RFBUTTONEVENT_DOUBLECLICK) {
            Serial.println("RFBUTTONEVENT_DOUBLECLICK ");
        } else if (event == RFBUTTONEVENT_LONGCLICK) {
            Serial.println("RFBUTTONEVENT_LONGCLICK ");
        }
    });
}

unsigned long lastEventTime;
void loop() {
    RFButton.loop();

    if (mySwitch.available()) {
        unsigned long delay = millis() - lastEventTime;
        lastEventTime = millis();

        unsigned long value = mySwitch.getReceivedValue();

        Serial.print("Received ");
        Serial.print(value);
        Serial.print(" / ");
        Serial.print(mySwitch.getReceivedBitlength());
        Serial.print("bit ");
        Serial.print("Protocol: ");
        Serial.print(mySwitch.getReceivedProtocol());
        Serial.print("Delay: ");
        Serial.println(delay);

        RFButton.handleRFSignal(value);

        mySwitch.resetAvailable();
    }
}
