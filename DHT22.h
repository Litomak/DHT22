// DHT22.h
#ifndef DHT22_H
#define DHT22_H

#include <avr/io.h>
#include <util/delay.h>

#define DHT22_DDR DDRB // Set Data Direction Register
#define DHT22_PORT PORTB // Set Port
#define DHT22_INPUT PINB // Read Pin
#define DHT22_PIN PORTB5 // DHT22 Input Pin

const uint8_t MAX_TEMPERATURE = 80;
const int8_t MIN_TEMPERATURE = -40;
const uint8_t MAX_HUMIDITY = 100;
const uint8_t MIN_HUMIDITY = 0;

enum DHT22Errors {
	NO_ERROR = 0, // No errors
	NO_RESPONSE = -1, // Sensor not detected
	TIMEOUT_DATA_TRANSMISSION = -2, // Timeout during data transmission
	CHECKSUM_ERROR = -3, // Data integrity check failed
	TEMPERATURE_ERROR = -4, // Temperature out of range
	HUMIDITY_ERROR = -5 // Humidity out of range
};

// Function declarations
int8_t start_signal(); // Function to initiate the start signal
int8_t read_data(); // Function to read data from DHT22

/* Process data from DHT22 sensor and retrieve temperature in degrees Celsius
 * and humidity in percentage. */
int read_DHT22(float *temperature,
	       float *humidity); 

#endif
