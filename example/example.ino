
extern "C"{
#include <DHT22.h>
}
#include <avr/io.h>
#include <util/delay.h>

/*   Error codes:
 *   0 = No errors
 *  -1 = Sensor not detected
 *  -2 = Timeout during data transmission
 *  -3 = Data integrity check failed
 *  -4 = Temperature out of range
 *  -5 = Humidity out of range
 */

void setup() {
  Serial.begin(9600);
}

void loop() {
    float temperature = 0;
    float humidity = 0;
    int8_t error;
    
    error =  read_DHT22(&temperature, &humidity);
    Serial.print((String)"Temperature: " + temperature + (String)"C\n");  
    Serial.print((String)"Humidity: " + humidity + (String)"%\n");  
    if(error < 0){
      Serial.print((String)"Error: " + error + (String)"\n");
    }
    delay(2000);
}
