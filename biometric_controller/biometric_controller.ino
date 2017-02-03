
/*   biometric_controller.ino
 *    Runs several biometrics, compiles the data, and prints to serial.
 *    Written for Hypoxia by Kevin Výlet
 *   
 *   Sources:
 *   Pulse Sensor Code by Joel Murphy and Yury Gitman
 *   DHT11 Code by ladyada
*/
#include <DHT.h>
#include <DHT_U.h>

#include "DHT.h"
#define DHTPIN 2     // what digital pin we're connected to
#define DHTTYPE DHT11   // DHT 11

// Initialize DHT sensor.
DHT dht(DHTPIN, DHTTYPE);

//  Variables
int pulsePin = 0; // Pulse Sensor purple wire connected to analog pin 0
int counter = 0;  // Counter used to stagger reads of the

// Volatile Variables, used in the interrupt service routine!
volatile int BPM;               // int that holds raw Analog in 0. updated every 2mS
volatile int Signal;            // holds the incoming raw data
volatile int IBI = 600;         // int that holds the time interval between beats! Must be seeded!
volatile boolean Pulse = false; // "True" when User's live heartbeat is detected. "False" when not a "live beat".

void setup(){
  Serial.begin(115200);
  interruptSetup();     // sets p to read Pulse Sensor signal every 2mSu
  dht.begin();
}

void loop(){
  float h = dht.readHumidity();        // Get humidity
  float f = dht.readTemperature(true); // Get temp (in F)
  
  if (isnan(h) || isnan(f)) {          // Check if any reads failed and exit early.
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  // Print to serial
  // Format: heart rate(BPM)|humidity(%)|temp(F)
  Serial.print(BPM);
  Serial.print("|");
  Serial.print(h);
  Serial.print("|");
  Serial.print(f);
  Serial.println();  
  delay(20);
}
