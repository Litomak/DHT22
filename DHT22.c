// DHT22.c
#include "DHT22.h"
#include <avr/io.h>
#include <util/delay.h>

// =========================================================
// Function: start_signal
// Initiates the start signal for DHT22.
int8_t start_signal()
{
	uint8_t timeout = 0;

	// 1ms LOW signal to initialise the sensor response
	DHT22_DDR |= (1 << DDB5); // Set Pin DHT22_PIN as OUTPUT
	DHT22_PORT &= ~(1 << DHT22_PIN); // LOW pulse
	_delay_ms(1);
	DHT22_PORT |= (1 << DHT22_PIN); // HIGH pulse
	DHT22_DDR &= ~(1 << DDB5); // Set Pin DHT22_PIN as INPUT

	// 40us response ###
	while (DHT22_INPUT & (1 << DHT22_PIN)) {
		_delay_us(1);
		timeout += 1;
		if (timeout > 50) {
			return NO_RESPONSE;
		}
	}
	timeout = 0;

	// 80us LOW response
	if (DHT22_INPUT & (1 << DHT22_PIN)) {
		return NO_RESPONSE;
	}
	_delay_us(81); // 80us are a little too short

	// 80us HIGH response
	if (!(DHT22_INPUT & (1 << DHT22_PIN))) {
		return NO_RESPONSE;
	}
	_delay_us(81);

	return NO_ERROR; // No errors during start signal
}

// =========================================================
// Function: read_data
// Reads data from DHT22 sensor.
int8_t read_data()
{
	uint8_t byte = 0x00;
	uint8_t timeout = 0;

	for (uint8_t i = 0; i < 8; ++i) {
		// Wait for HIGH pulse
		while (!(DHT22_INPUT & (1 << DHT22_PIN))) {
			_delay_us(1);
			timeout += 1;
			if (timeout > 60) { // timeout after 60us
				return TIMEOUT_DATA_TRANSMISSION;
			}
		}
		timeout = 0;

		_delay_us(50);

		// Write to byte
		if (DHT22_INPUT & (1 << DHT22_PIN)) {
			byte = (byte << 1) | (0x01); // Write '1' bit to byte

		} else {
			byte = (byte << 1); // Write "0" to byte
		}

		// Wait for LOW pulse
		while ((DHT22_INPUT & (1 << DHT22_PIN))) {
			_delay_us(1);
			timeout += 1;
			if (timeout > 30) { // timeout after 30us
				return TIMEOUT_DATA_TRANSMISSION;
			}
		}
		timeout = 0;
	}

	return byte; // Return the byte read from DHT22
}

// =========================================================
// Function: DHT22_init
// Initializes DHT22 sensor and processes data.
int read_DHT22(float *temperature, float *humidity)
{
	int16_t temp = 0;
	uint16_t hum = 0;
	uint8_t data[5] = { 0 };
	int8_t checksum = 0;
	int8_t error = 0;

	// Start signal initiation
	error = start_signal();

	if (error == NO_RESPONSE) {
		return NO_RESPONSE; // Return error if no response from DHT22
	}

// Data Format:
// MSB			LSB
// [][][][][][][][] [][][][][][][][] [][][][][][][][] [][][][][][][][] [][][][][][][][] 
// humidity high    humidity low     temperature high temperature low  parity

	for (uint8_t i = 0; i < 5; ++i) {
		// write to data[]
		// data[0] = temperature high
		// data[1] = temperature low
		// data[2] = humidity high
		// data[3] = humidity low
		// data[4] = parity

		data[i] = read_data();
		if ((data[i] == TIMEOUT_DATA_TRANSMISSION) && i < 5) {
			return TIMEOUT_DATA_TRANSMISSION;
		}
	}

	// Return error if the checksum doesn't match the parity byte
	checksum = data[0] + data[1] + data[2] + data[3];
	if ((checksum & 0xFF) != data[4]) {
		return CHECKSUM_ERROR;
	}

	// Write humidity high bits to hum
	hum = data[0];
	hum = (hum << 8);
	// Write humidity low bits to hum
	hum |= data[1];

	// Write temperature high bits to temp
	temp = data[2];
	temp = (temp << 8);
	// Write temperature low bits to temp
	temp |= data[3];

	// MSB = '1' means negative temperature
	if (temp >> 15) {
		temp &= ~(1 << 15); // Set sign bit to 0
		temp *= -1; // Negate
	}

	// Check if temperature and humidity values are inside the defined range
	if (temp > MAX_TEMPERATURE * 10) {
		return TEMPERATURE_ERROR; // Return error if temperature out of range
	} else if (temp < MIN_TEMPERATURE * 10) {
		return TEMPERATURE_ERROR; // Return error if temperature out of range
	}
	if (hum > MAX_HUMIDITY * 10) {
		return HUMIDITY_ERROR; // Return error if humidity out of range
	} else if (hum < MIN_HUMIDITY * 10) {
		return HUMIDITY_ERROR; // Return error if humidity out of range
	}

	*temperature = temp / 10.0; // Store temperature in Celsius
	*humidity = hum / 10.0; // Store humidity in percentage
	return NO_ERROR;
}
