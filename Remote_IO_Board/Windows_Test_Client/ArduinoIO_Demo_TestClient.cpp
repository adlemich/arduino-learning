// ArduinoIO_Demo_TestClient.cpp: Definiert den Einstiegspunkt für die Anwendung.
//
#include "ArduinoIO_Demo_TestClient.h"
#include "ArduinoIO_TCPClient.h"

using namespace std;

#define ARDUINO_BOARD_IP_ADDR "192.168.1.223"

ArduinoIOTcpClient* pTCPClient = NULL;

//################################################################################
// Returns true if Exit was chosen
//################################################################################
bool handleInput(const std::string& input) {
	if (input == "c" || input == "C") {
		if (pTCPClient == NULL) {
			pTCPClient = new ArduinoIOTcpClient(ARDUINO_BOARD_IP_ADDR);
		}
		return false;
	}
	else if (input == "d" || input == "D") {
		if (pTCPClient != NULL) {
			delete pTCPClient;
			pTCPClient = NULL;
		}
		return false;
	}
	else if (input == "x" || input == "X") {
		std::cout << "BYE BYE!" << std::endl;
		return true;
	}	
	else if (input == "a" || input == "A") {
		std::string deviceValue;
		if (pTCPClient != NULL) {
			
			std::cout << "New Value - For AndonLight three vales for RYG 0/1 (Example: 010 puts only yellow to on) : ";
			std::cin >> deviceValue;
			std::cout << std::endl;

			pTCPClient->sendSetAndonLightValue(deviceValue);
		}
		return false;
	}
	else if (input == "l" || input == "L") {
		std::string deviceValue;
		if (pTCPClient != NULL) {

			std::cout << "New Value - For the LEDMatrix you need to supply 64 values for color (R/G/B/W/O/Y/X): ";
			std::cin >> deviceValue;
			std::cout << std::endl;

			pTCPClient->sendSetLEDMatrixValue(deviceValue);
		}
		return false;
	}
	else if (input == "t" || input == "T") {
		std::string deviceValue;
		if (pTCPClient != NULL) {

			std::cout << "New Value - For the Text Display - must be 2x16 chars, 16 for the upper text 16 for the lower text): ";
			std::cin >> deviceValue;
			std::cout << std::endl;

			pTCPClient->sendSetTextDisplayValue(deviceValue);
		}
		return false;
	}
	else if (input == "r" || input == "R") {
		std::string seqNum;
		if (pTCPClient != NULL) {

			pTCPClient->sendStartTestRun();
		}
		return false;
	}
	else {
		std::cout << "Invalid input!" << std::endl;
		return false;
	}
}

//################################################################################
// 
//################################################################################
int main()
{
	std::string userInput;
	bool bExit = false;

	while (!bExit) {
		cout << "============================================================" << endl;
		cout << "Arduino Remote IO Demo - Test App for the Ardunio IO Board. " << endl;
		cout << "============================================================" << endl;
		cout << endl;
		cout << "Menu:" << endl;
		cout << endl;
		cout << "  c - Connect to the Arduio IO Board." << endl;
		cout << "  d - Disconnect from the Arduio IO Board." << endl;
		cout << endl;
		cout << "  a - Set the Andon Light (update with new values)." << endl;
		cout << "  l - Set the LED Matrix (update with new values)." << endl;
		cout << "  t - Set the Text Display (update with new values)." << endl;
		cout << "  r - Remotly press the 'Start Test Run' button." << endl;
		cout << endl;
		cout << "  X - Exit this Application." << endl;
		cout << endl;
		cout << "Choice: ";

		std::cin >> userInput;
		bExit = handleInput(userInput);
	}

	return 0;
}
