#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

#define TILT_SENSOR_PIN 14  // D5 on NodeMCU (GPIO14)
#define BUZZER_PIN 5        // D1 on NodeMCU (GPIO5)

char auth[] ="wZJKXv-OeIYU8Ed6Ft8jNFoSOkLTG6xL";
char ssid[] = "123456789";
char pass[] = "123456789";

#define BLYNK_LED_VPIN V0
WidgetLED led1(BLYNK_LED_VPIN);

int tiltCount = 0;
bool buzzerOn = false;
unsigned long tiltStartTime = 0;
bool timingTilt = false;

void setup() {
    Serial.begin(115200);
    pinMode(TILT_SENSOR_PIN, INPUT_PULLUP);
    pinMode(BUZZER_PIN, OUTPUT);
    Blynk.begin(auth, ssid, pass, "blynk.cloud", 80);
}

void loop() {
    Blynk.run();

    int tiltState = digitalRead(TILT_SENSOR_PIN);
    unsigned long currentTime = millis();

    if (tiltState == HIGH) {
        if (!timingTilt) {
            tiltStartTime = currentTime;
            timingTilt = true;
        } else if (currentTime - tiltStartTime >= 200) {
            // Valid 1-second tilt detected
            tiltCount++;
            Serial.print("Valid tilt detected. Count: ");
            Serial.println(tiltCount);
            timingTilt = false;  // Reset for next detection

            if (tiltCount >=1&& !buzzerOn) {
                tone(BUZZER_PIN, 1000);   // Buzzer ON
                led1.on();                // Blynk LED ON
                Blynk.logEvent("door_open", "Tilt detected 2 times (each >1s)!");
                buzzerOn = true;

                Serial.println("5 valid tilts: Buzzer ON, Notification sent.");
            }
        }
    } else {
        // Reset everything if tilt is not held
        noTone(BUZZER_PIN);     // Buzzer OFF
        led1.off();             // Blynk LED OFF
        tiltCount = 0;
        timingTilt = false;
        buzzerOn = false;
        Serial.println("No tilt: All reset.");
    }

    delay(50); // Small delay for stability
}
