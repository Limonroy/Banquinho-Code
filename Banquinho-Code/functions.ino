/**
  * Function to blink a defined led
  * @param pin  LED pin
  * @param time time between the blinks
*/
void blink(int pin, int time) {
  digitalWrite(pin, HIGH);  
  delay(time);               
  digitalWrite(pin, LOW);   
  delay(time);               
}

/**
  * Function to buzz a defined buzzer
  * @param pin  Buzzer pin
  * @param time time between the buzz
*/
void buzz(int pin, int time) {
  digitalWrite(pin, HIGH);
  delay(time);
  digitalWrite(pin, LOW);
  delay(time);  
}

void kboom(int pin, int time, bool toBuzz) {
  if (toBuzz) {
    buzz(BUZZER, 500);  
  }
  digitalWrite(pin, HIGH);
  delay(time);
  digitalWrite(pin, LOW);
}

/*
 * Function to initialize the LoRa Module
*/
void lora_init() {
  LoRa.setPins(CS_LORA, RST, DIO_0);
  if (!LoRa.begin(915E6)) { 
    Serial.println("LoRa INITIALIZATION ERROR");
    // while (1);
  }
  // SF -> 7
  // CR -> 4
  LoRa.setSyncWord(0xF3);
  //LoRa.setSpreadingFactor(6);
  //LoRa.setSignalBandwidth(125E3);
  //LoRa.setTxPower(18);
}

/*
 * Function to initialize the SD Module
*/
void sd_init() {
  while (!sd.begin(CS_SD, SPI_HALF_SPEED)) {
    Serial.println("ERROR IN THE SD INITIALIZATION!");
    blink(LED1, 50); // Blink until the SD BEGIN CORRECTLY
  } 
  Serial.println("SD INITIALIZATION SUCCESFULL!");
}

/**
  * Function to start the modules.
*/
void start_modules() {
  // LEDS AND PIROTECNIC CHANEL INITIALIZATION
  pinMode(LED_1, OUTPUT);
  pinMode(LED_2, OUTPUT);
  pinMode(BUZZER, OUTPUT);
  pinMode(CH1, OUTPUT);

  // SD BEGIN PROCEDURE
  sd_init();
  
  // LORA MODULE BEGIN PROCEDURE
  lora_init();
  
  // Blink four time to indicate success
  blink(LED1, 200);
  blink(LED2, 200);
  blink(LED1, 200);
  blink(LED2, 200);
}

/**
  * Function to create the final name of the file.
  * If exists a file with the proposed name, then this 
  * function adds a number to the name
  * @param name proposed file name
  * @param extension extension of the file
  * @return String with the final file name
*/
String createFileName(String name, String extension) {
  // if exists, then add a number to the name
  if (sd.exists("/" + name + extension)) {
    return nameWithNumber(name, extension,1);
  }
  name = name + extension;
  return name;
}

/**
  * Function that adds a number to the name of a file
  * @param name proposed file name
  * @param extension extension of the file
  * @param counter counter of the name
  * @return String name with the final name with a number
*/
String nameWithNumber(String name, String extension, int counter) {
  // generates the name with the number
  String proposedName = name + String(counter) + extension;

  // if already exists a file name with the counter number, tries with the name with counter + 1
  if (sd.exists("/" + proposedName)) {
    return nameWithNumber(name, extension, counter + 1);
  }
  return proposedName;
}

/**
  * Function to write data into the sd file
  * @param name file name
  * @param data data to write into the file
*/
void writeFile(String name, String data, bool toBlink) {
  // create or open file in write mode
  File dataFile = sd.open(name, FILE_WRITE);
  if (dataFile) {
    // write data into the dile
    dataFile.println(data);
    
    // close the file
    dataFile.close();
    
    // Blinks to indicate tha the data was written correctly
    if (toBlink) {
      blink(LED1, 10);
    }

  } else {
    Serial.println("ERROR WRITTING INTO FILE " + name);
  }
}

/**
  * Function to read and save the data of the load cells into vars
*/
void readCells() {
  // Load Cell 1 
  cellADC1 = loadcell1.read();
  kgf1 = A1 * cellADC1 + B1;

  // Load Cell 2 
  cellADC2 = loadcell2.read();
  kgf2 = A2 * cellADC2 + B2;
}

/**
 * Mapping
 * Given an input voltage and a range, deduce the pressure
 * @param x Input voltage to map
 * @param in_min Minimum input voltage value [V]
 * @param in_max Maximum input voltage value [V]
 * @param out_min Minimum output pressure value [MPa]
 * @param out_max Maximum output pressure value [MPa]
 * @return Pressure value in MPa
*/
float mapping(float X, float in_min, float in_max, float out_min, float out_max) {
  return out_min + ((X - in_min)*(out_max-out_min)/(in_max-in_min));
}

/**
 * Function to read the pressure sensors 
*/
void readPressure() {
  // Pressure Sensor 1
  pressureADC1 = analogRead(pressurePin1);
  float voltage1 = (float) pressureADC1 * (5.0/1023.0);
  mpa1 =  mapping(voltage1,0.5,4.5,0,6.8948);

  // Pressure Sensor 2
  pressureADC2 = analogRead(pressurePin2);
  float voltage2 = (float) pressureADC2 * (5.0/1023.0);
  mpa2 =  mapping(voltage2,0.5,4.5,0,6.8948);
}

/**
 * Function to read the temperature sensors 
*/
void readTemperature() {
  // Temperature Sensor 1
  temperatureADC1 = 0;
  celsius1 = 0;

  // Temperature Sensor 2
  temperatureADC2 = 0;
  celsius2 = 0;
}

/**
 * Function to update the casv string with the values of vars of the data
 * of that moment
*/
void buildCsvString() {
  dataCSV = (String) millis() + "," + (String) cellADC1 + "," + (String) cellADC2 + ",";
  dataCSV += (String) pressureADC1 + "," + (String) pressureADC2 + ",";
  dataCSV += (String) temperatureADC1 + "," + (String) temperatureADC2 + ",";  
  dataCSV += (String) kgf1 + "," + (String) kgf2 + "," + (String) mpa1 + ","; 
  dataCSV += (String) mpa2 + "," + (String) celsius1 + "," + (String) celsius2;
}

/**
  * Function to send data by LoRa
  * @param data data to write send by LoRa
*/
void loraSend(String data) {
  LoRa.beginPacket();
  LoRa.print(data);
  LoRa.endPacket();
}
