/*   biometric_controller.ino
 *    Runs several biometrics, compiles the data, and prints to serial.
 *    Written for Hypoxia by Kevin Výlet
 *   
 *   Sources:
 *   Pulse Sensor Code by Joel Murphy and Yury Gitman
 *   DHT11 Code by ladyada
 *   Wind Sensor Code by Paul Badger
*/
#include <DHT.h>
#include <DHT_U.h>
#include "DHT.h"

// Temp/humidity sensor definitions
#define DHTPIN 2     // what digital pin we're connected to
#define DHTTYPE DHT11   // DHT 11

// Wind sensor definitions
#define analogPinForRV    2   // Analog pins the wind sensor is connected to
#define analogPinForTMP   1

// Initialize DHT sensor.
DHT dht(DHTPIN, DHTTYPE);

/* * * * * * * * * * * * * * *
 *  Pulse Sensor Variables   *
 * * * * * * * * * * * * * * */
 
//  Variables
int pulsePin = 0; // Pulse Sensor purple wire connected to analog pin 0
int counter = 0;  // Counter used to stagger reads of the

// Volatile Variables, used in the interrupt service routine!
volatile int BPM;               // int that holds raw Analog in 0. updated every 2mS
volatile int Signal;            // holds the incoming raw data
volatile int IBI = 600;         // int that holds the time interval between beats! Must be seeded!
volatile boolean Pulse = false; // "True" when User's live heartbeat is detected. "False" when not a "live beat".

/* * * * * * * * * * * * * * *
 *   Wind Sensor Variables   *
 * * * * * * * * * * * * * * */

const float zeroWindAdjustment =  .2; // negative numbers yield smaller wind speeds and vice versa.

int TMP_Therm_ADunits;  //temp termistor value from wind sensor
float RV_Wind_ADunits;  //RV output from wind sensor 
float RV_Wind_Volts;
unsigned long lastMillis;
int TempCtimes100;
float zeroWind_ADunits;
float zeroWind_volts;
float WindSpeed_MPH;

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

  TMP_Therm_ADunits = analogRead(analogPinForTMP);
  RV_Wind_ADunits = analogRead(analogPinForRV);
  RV_Wind_Volts = (RV_Wind_ADunits *  0.0048828125);

  TempCtimes100 = (0.005 *((float)TMP_Therm_ADunits * (float)TMP_Therm_ADunits)) - (16.862 * (float)TMP_Therm_ADunits) + 9075.4;  
  zeroWind_ADunits = -0.0006*((float)TMP_Therm_ADunits * (float)TMP_Therm_ADunits) + 1.0727 * (float)TMP_Therm_ADunits + 47.172;
  zeroWind_volts = (zeroWind_ADunits * 0.0048828125) - zeroWindAdjustment;
  WindSpeed_MPH =  pow(((RV_Wind_Volts - zeroWind_volts) /.2300) , 2.7265); 

  // Print to serial
  // Format: heart rate(BPM)|humidity(%)|temp(F)|wind speed(MPH)
  Serial.print(BPM);
  Serial.print("|");
  Serial.print(h);
  Serial.print("|");
  Serial.print(f);
  Serial.print("|");
  Serial.print(WindSpeed_MPH);
  Serial.println();  
  delay(20);
}
