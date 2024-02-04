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

  // BLINK OF SUCCESS OF SETUP
  blink(LED1, 200);
  blink(LED1, 200);
  blink(LED2, 200);
  blink(LED2, 200);

  // STATE 2 TRANSITION (Board is alive verification)
  state2(0);
}

/** 
 * STATE TO MAKE THE "BOARD IS ALIVE VERIFICATION"
 * This function makes the board send us 30 data for 30 seconds to 
 * verify that it is alive, once the 30 data are sent, it goes to 
 * listening mode in state3 (Waiting the ignition signal).
 * @param counter number of data 
*/
void state2(int counter) {
  // If the sensors were read and the data was sent for 30 seconds, change to state 3
  if (counter == 30) {
    // Pass to the STATE 3 (Waiting the ignition signal)
    state3();
  }

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

  counter += 1;
  delay(1000);
}

/**
  * STATE TO MAKE THE "WAIT THE IGNITION SIGNAL"
  * if recives the ignition signal, pass to the static
  * fire test. While not, turn on the leds to indicate
  * the board is waiting for the signal.
*/
void state3() {
  // try to parse packet
  int packetSize = LoRa.parsePacket();
  
  if (packetSize) {
    while (LoRa.available()) {
      char receivedChar = (char)LoRa.read();
      //Serial.print(receivedChar);

      // If the char recived is 'I' or 'i', goes to ignition state
      if (receivedChar == 'I' || receivedChar == 'i') {
        buzz(BUZZER, 500);
        buzz(BUZZER, 500);
        // Pass to the state4 (STATIC TEST STATE)
        state4(); 
      }
    }

    //Serial.print("' with RSSI ");
    //Serial.println(LoRa.packetRssi());
  }

  blink(LED1, 10);
  blink(LED2, 10);
  state3();
}

/**
  * STATE TO MAKE THE "STATIC FIRE TEST"
  * Saves and send 15 seconds of ambiental info for 
  * not lose any info, then activate the pyro chanel
  * and then save the rest of the data 
  * todo: improve this function for better readability
*/
void state4() {
  int c = 0;
  // Save 15 seconds of ambiental data so as not to lose information
  while (c < 75) {
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
    c += 1;
    delay(200);
  }

  // ACTIVATE THE PIROTECNIC CHANEL
  buzz(BUZZER,500);
  digitalWrite(CH1, HIGH);
  delay(500);
  digitalWrite(CH1, LOW);

  // REST OF THE STATIC FIRE TEST   
  while (true) {
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
}

void loop() {}
