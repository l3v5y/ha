#include <SPI.h>
#include <MySensor.h>
#include <MyMQTT.h>
#include <Ethernet.h>
#include <DigitalIO.h>


#ifndef SOFTSPI 
#error Please use SOFTSPI
#endif

#define RADIO_CE_PIN        5		// radio chip enable
#define RADIO_SPI_SS_PIN    6		// radio SPI serial select
#define RADIO_ERROR_LED_PIN 7		// Error led pin
#define RADIO_RX_LED_PIN    8		// Receive led pin
#define RADIO_TX_LED_PIN    9		// the PCB, on board LED*/

#define TCP_PORT 1883						// Set your MQTT Broker Listening port.
IPAddress TCP_IP ( 192, 168, 1, 10 );				// Configure your static ip-address here
uint8_t TCP_MAC[] = { 0x02, 0xDE, 0xAD, 0x00, 0x00, 0x42 };	// Mac-address - You should change this! see note *2 above!

EthernetServer server = EthernetServer(TCP_PORT);
MyMQTT gw(RADIO_CE_PIN, RADIO_SPI_SS_PIN);

void processEthernetMessages() {
	char inputString[MQTT_MAX_PACKET_SIZE] = "";
	uint8_t inputSize = 0;
	EthernetClient client = server.available();
	if (client) {
		while (client.available()) {
			char inChar = client.read();
			inputString[inputSize] = inChar;
			inputSize++;
		}
#ifdef TCPDUMP
		Serial.print("<<");
		char buf[4];
		for (uint8_t a=0; a<inputSize; a++) { sprintf(buf,"%02X ", (uint8_t)inputString[a]); Serial.print(buf); } Serial.println("");
#endif
		gw.processMQTTMessage(inputString, inputSize);
	}
}

void writeEthernet(const char *writeBuffer, uint8_t *writeSize) {
#ifdef TCPDUMP
	Serial.print(">>");
	char buf[4];
	for (uint8_t a=0; a<*writeSize; a++) { sprintf(buf,"%02X ",(uint8_t)writeBuffer[a]);  Serial.print(buf); } Serial.println("");
#endif
	server.write((const uint8_t *)writeBuffer, *writeSize);
}


int main(void) {
	init();
	Ethernet.begin(TCP_MAC, TCP_IP);
	delay(1000);   // Wait for Ethernet to get configured.
	gw.begin(RF24_PA_LEVEL_GW, RF24_CHANNEL, RF24_DATARATE, writeEthernet, RADIO_RX_LED_PIN, RADIO_TX_LED_PIN, RADIO_ERROR_LED_PIN);
	server.begin();
	while (1) {
		processEthernetMessages();
		gw.processRadioMessage();
	}
}
