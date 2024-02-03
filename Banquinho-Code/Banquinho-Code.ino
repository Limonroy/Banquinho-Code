
#include <Q2HX711.h>
// *-----LOADCELLS-----*
Q2HX711 loadcell1(6,7);
Q2HX711 loadcell2(10,11);
unsigned long cellADC1;
unsigned long cellADC2;


void setup() {
  Serial.begin(115200);
  delay(200);
}

void loop() {
  cellADC1 = loadcell1.read();
  cellADC2 = loadcell2.read();
  Serial.println(cellADC1);
  Serial.println(cellADC2);
  delay(500);
  
  
}
