/**
 * Banquinho Code
 * @version 1.0
 * @author Carlos Andrade Hernandez
 * @author Lidia Monroy 
 * @author Natalie Jimena Roldan
 * @date 02/04/2024
*/

#include <Q2HX711.h>
#include <SdFat.h>
#include <SPI.h>
#include <LoRa.h>

// *-----LOAD CELL PINS-----*
#define HX711_DOUT1   6
#define HX711_SCK1    7

#define HX711_DOUT2   10
#define HX711_SCK2    11

// *-----PRESSURE SENSOR PINS-----*
#define pressurePin1   27
#define pressurePin2   26

// *-----TEMPERATURE SENSOR PINS-----*
#define temperaturePin1   14
#define temperaturePin2   2

// *-----SPI PINS-----*
#define MISO         16 //21 físico
#define CS_LORA      17 //17 - 29 físico 22 maybe
#define CS_SD        22 //29 maybe
#define MOSI         19 //25 físico
#define DIO_0        20 //100000000 - 14 físico
#define RST          21 //21 
#define SCK          18 //24 maybe
                                                                    
// *-----STATE SIGNALS PINS-----*
#define LED_1     8   
#define LED_2     9   
#define BUZZER    15

// *-----PIROTECNIC CHANEL PIN-----*
#define CH1       13

// *-----LOAD CELLS CONSTANTS, OBJECTS AND VARS-----*
#define A1            7.72797e-05
#define B1            -646.825

#define A2            7.72797e-05
#define B2            -646.825

Q2HX711 loadcell1(HX711_DOUT1,HX711_SCK1);
Q2HX711 loadcell2(HX711_DOUT2,HX711_SCK2);

unsigned long cellADC1;
unsigned long cellADC2;

float kgf1;
float kgf2;

// *-----PRESSURE SENSORS VARS-----*
unsigned long pressureADC1;
unsigned long pressureADC2;

float mpa1;
float mpa2;

// *-----TEMPERATURE SENSORS VARS-----*
unsigned long temperatureADC1;
unsigned long temperatureADC2;

float celsius1;
float celsius2;

// *-----DATA OBJECTS AND VARS-----*
SdFat sd;
String fileName;
String dataCSV;

// *-----EXTRA VARS-----*
int c = 0;

void setup() {
  Serial.begin(115200);
  
  // START MODULES (SD, LORA, LEDS AND BUZZER)
  start_modules();

  // CREATE THE FILE NAME
  fileName = createFileName("data", ".csv");

  // CREATE HEADERS TO THE FILE
  String header = "t[s], cellADC1, cellADC2, pressureADC1, pressureADC2, tempADC1, tempADC2,";
  header += "kgf1, kgf2, mpa1, mpa2, celsius1, celsius2";
  writeFile(fileName, header, false);

  // BUZZ OF SUCCESS OF SETUP
  buzz(BUZZER, 500);
}


void loop() {
  // ACTIVATE THE PIROTECNIC CHANEL 60 segs
  //if (c <= 600) {
    //kboom(CH1, 500, true);
  //} else {
    //c++;  
  //}
  
  // Read the values of the load cell and update their vars values
  readCells();
  // Read the values of pressure sensor and update their vars values
  readPressure();
  // Read the values of temperature sensor and update their vars values
  readTemperature();
  // Build the CSV string
  buildCsvString();
  // Send the CSV string with LoRa
  loraSend(dataCSV);
  // Write data into de SD file
  writeFile(fileName, dataCSV, true);
  delay(100);
}
