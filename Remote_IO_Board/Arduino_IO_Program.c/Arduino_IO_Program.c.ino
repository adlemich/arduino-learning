#include <SPI.h>
#include <Ethernet.h>
#include <Wire.h>  
#include <LiquidCrystal_I2C.h> 
#include <Adafruit_NeoPixel.h> 
#include <limits.h>

#include "ArduninoIO_API_v2.h"

/* ==============================================================
 * DEFINITIONS FOR THE PIN LAYOUT - I/Os
 * ============================================================== */
#define DO_ANDON_LIGHT_RED    9
#define DO_ANDON_LIGHT_YELLOW 8
#define DO_ANDON_LIGHT_GREEN  7
#define DO_UPLINK_LIGHT_GREEN 6
#define DO_TEST_LIGHT_BLUE    1
#define DI_BUTTON_START       5
#define DI_BUTTON_ACK         4
#define DI_BUTTON_PATTERN     3
#define DO_LED_MATRIX         2

/* ==============================================================
 * DEFINITIONS FOR LCD PANEL
 * ============================================================== */
#define LCD_DISPLAY_CHARS 16	
#define LCD_DISPLAY_ROWS  2		

/* ==============================================================
 * DEFINITIONS FOR MATRIX DISPLAY
 * ============================================================== */
#define LED_MATRIX_SIZE       64
#define LED_MATRIX_OFF        ledMatrix.clear(); ledMatrix.show()
#define LED_BRIGHTNES         10 
#define LED_COLOR_RED         0 
#define LED_COLOR_GREEN       1
#define LED_COLOR_BLUE        2
#define LED_COLOR_ORANGE      3
#define LED_COLOR_WHITE       4

/* ==============================================================
 * DEFINITIONS FOR ETHERNET CONNECTION
 * ============================================================== */
#define ETHERNET_MAC_ADDR { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED }
#define ETHERNET_IP_ADDR  (192, 168, 1, 223)
#define ETHERNET_DNS_ADDR (192, 168, 1, 1)
#define ETHERNET_GW_ADDR  (192, 168, 1, 1)
#define ETHERNET_GW_SUBN  (255, 255, 255, 0)

/* ==============================================================
 * DEFINITIONS FOR THE TEST RUN
 * ============================================================== */
 #define TEST_RUN_SIZE 1000


/* ==============================================================
 * GLOBAL VARIABLES
 * ============================================================== */
EthernetServer thisServer = EthernetServer(API_SERVING_PORT);
EthernetClient clientApp;

LiquidCrystal_I2C lcdDisplay(0x27, LCD_DISPLAY_CHARS, LCD_DISPLAY_ROWS); 
Adafruit_NeoPixel ledMatrix(LED_MATRIX_SIZE, DO_LED_MATRIX, NEO_GRB + NEO_KHZ800);

bool lastButtonStateStart = false;
bool lastButtonStateAck   = false;
bool lastButtonStatePatt  = false;

char msgDataBuffer[API_MSG_MAX_SIZE + 1];
char* msgDataPointer = msgDataBuffer;
char msgDeviceName[API_DEVICE_NAME_LEN + 1];
char apiBase[API_BASE_ID_LEN + 1];
char msgType[API_MSG_TYPE_LEN + 1];

char valLedMatrix[API_DEVICE_LEDMATRIX_VALUE_LEN + 1] = { API_DEVICE_LEDMATRIX_VAL_OFF, API_DEVICE_LEDMATRIX_VAL_OFF, API_DEVICE_LEDMATRIX_VAL_OFF, API_DEVICE_LEDMATRIX_VAL_OFF, API_DEVICE_LEDMATRIX_VAL_OFF, API_DEVICE_LEDMATRIX_VAL_OFF, API_DEVICE_LEDMATRIX_VAL_OFF, API_DEVICE_LEDMATRIX_VAL_OFF, \
                                                          API_DEVICE_LEDMATRIX_VAL_OFF, API_DEVICE_LEDMATRIX_VAL_OFF, API_DEVICE_LEDMATRIX_VAL_OFF, API_DEVICE_LEDMATRIX_VAL_OFF, API_DEVICE_LEDMATRIX_VAL_OFF, API_DEVICE_LEDMATRIX_VAL_OFF, API_DEVICE_LEDMATRIX_VAL_OFF, API_DEVICE_LEDMATRIX_VAL_OFF, \
                                                          API_DEVICE_LEDMATRIX_VAL_OFF, API_DEVICE_LEDMATRIX_VAL_OFF, API_DEVICE_LEDMATRIX_VAL_OFF, API_DEVICE_LEDMATRIX_VAL_OFF, API_DEVICE_LEDMATRIX_VAL_OFF, API_DEVICE_LEDMATRIX_VAL_OFF, API_DEVICE_LEDMATRIX_VAL_OFF, API_DEVICE_LEDMATRIX_VAL_OFF, \
                                                          API_DEVICE_LEDMATRIX_VAL_OFF, API_DEVICE_LEDMATRIX_VAL_OFF, API_DEVICE_LEDMATRIX_VAL_OFF, API_DEVICE_LEDMATRIX_VAL_OFF, API_DEVICE_LEDMATRIX_VAL_OFF, API_DEVICE_LEDMATRIX_VAL_OFF, API_DEVICE_LEDMATRIX_VAL_OFF, API_DEVICE_LEDMATRIX_VAL_OFF, \
                                                          API_DEVICE_LEDMATRIX_VAL_OFF, API_DEVICE_LEDMATRIX_VAL_OFF, API_DEVICE_LEDMATRIX_VAL_OFF, API_DEVICE_LEDMATRIX_VAL_OFF, API_DEVICE_LEDMATRIX_VAL_OFF, API_DEVICE_LEDMATRIX_VAL_OFF, API_DEVICE_LEDMATRIX_VAL_OFF, API_DEVICE_LEDMATRIX_VAL_OFF, \
                                                          API_DEVICE_LEDMATRIX_VAL_OFF, API_DEVICE_LEDMATRIX_VAL_OFF, API_DEVICE_LEDMATRIX_VAL_OFF, API_DEVICE_LEDMATRIX_VAL_OFF, API_DEVICE_LEDMATRIX_VAL_OFF, API_DEVICE_LEDMATRIX_VAL_OFF, API_DEVICE_LEDMATRIX_VAL_OFF, API_DEVICE_LEDMATRIX_VAL_OFF, \
                                                          API_DEVICE_LEDMATRIX_VAL_OFF, API_DEVICE_LEDMATRIX_VAL_OFF, API_DEVICE_LEDMATRIX_VAL_OFF, API_DEVICE_LEDMATRIX_VAL_OFF, API_DEVICE_LEDMATRIX_VAL_OFF, API_DEVICE_LEDMATRIX_VAL_OFF, API_DEVICE_LEDMATRIX_VAL_OFF, API_DEVICE_LEDMATRIX_VAL_OFF, \
                                                          API_DEVICE_LEDMATRIX_VAL_OFF, API_DEVICE_LEDMATRIX_VAL_OFF, API_DEVICE_LEDMATRIX_VAL_OFF, API_DEVICE_LEDMATRIX_VAL_OFF, API_DEVICE_LEDMATRIX_VAL_OFF, API_DEVICE_LEDMATRIX_VAL_OFF, API_DEVICE_LEDMATRIX_VAL_OFF, API_DEVICE_LEDMATRIX_VAL_OFF, \
                                                          '\0'};

char valAndonLight[API_DEVICE_LEDMATRIX_VALUE_LEN + 1] = {API_DEVICE_ANDON_VALUE_OFF, API_DEVICE_ANDON_VALUE_OFF, API_DEVICE_ANDON_VALUE_OFF, '\0'};
char valTextUpper[LCD_DISPLAY_CHARS + 1] = {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '\0'};
char valTextLower[LCD_DISPLAY_CHARS + 1] = {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '\0'};

// Variables for test run
bool testRunActive = false;
int testRunSeqNum = 0;
unsigned long testRunMinDuration = 0;
unsigned long testRunMaxDuration = 0;
double testRunAvgDuration = 0.0;
unsigned long durationTotal = 0;
unsigned long measurementStart = 0;

/*############################################################################################
### ============================== ###########################################################
### BUTTON HANDLING                ###########################################################
### ============================== ###########################################################
##############################################################################################*/

/* ==============================================================
 *
 * ============================================================== */
bool checkButtonStartPressed() {

  // Read data from DI
  bool buttonState = digitalRead(DI_BUTTON_START);
  bool result = false;
    
  if (buttonState !=  lastButtonStateStart && buttonState == true) {
    // The button state when from non-signaled to signaled, action now
    result = true;
  }
  lastButtonStateStart = buttonState;

  return result; 
}
/* ==============================================================
 *
 * ============================================================== */
bool checkButtonAckPressed() {

  // Read data from DI
  bool buttonState = digitalRead(DI_BUTTON_ACK);
  bool result = false;
    
  if (buttonState !=  lastButtonStateAck && buttonState == true) {
    // The button state when from non-signaled to signaled, action now
    result = true;
  }
  lastButtonStateAck = buttonState;
  
  return result; 
}
/* ==============================================================
 *
 * ============================================================== */
bool checkButtonPattPressed() {

  // Read data from DI
  bool buttonState = digitalRead(DI_BUTTON_PATTERN);
  bool result = false;
    
  if (buttonState !=  lastButtonStatePatt && buttonState == true) {
    // The button state when from non-signaled to signaled, action now
    result = true;
  }
  lastButtonStatePatt = buttonState;
  
  return result; 
}



/* ==============================================================
 * Handle the actions for Button Start Test
 * ============================================================== */
void onButtonStart() {

  // Inform the remote side that we start a test run
  if(clientApp && clientApp.connected()) {
    String msgPayload = API_BASE_ID;
    msgPayload += API_MSG_TYPE_BUTTON_PRESS;
    msgPayload += API_DEVICE_BUTTON_START_NAME;

    clientApp.print(msgPayload);

    // Reset test values
    testRunActive = true;
    testRunSeqNum = 1;
    testRunMinDuration = UINT_MAX;
    testRunMaxDuration = 0;
    testRunAvgDuration = 0.0;
    durationTotal = 0;

    // Update Display
    strncpy(valTextUpper, "Starting 1k Msg ", LCD_DISPLAY_CHARS);
    strncpy(valTextLower, "Roundtrip Test..", LCD_DISPLAY_CHARS);
    setTextDisplay(); 

    // Update LED
    digitalWrite(DO_TEST_LIGHT_BLUE, HIGH);

    // Send first DI
    sendNextTestDI();  
  }
  else {
    strncpy(valTextUpper, "Connect a Client", LCD_DISPLAY_CHARS);
    strncpy(valTextLower, "to start a Test.", LCD_DISPLAY_CHARS);
    setTextDisplay(); 
  }
}

/* ==============================================================
 * Handle the actions for Button Acknowledge
 * ============================================================== */
void onButtonAck() {

  if(clientApp && clientApp.connected()) {
    String msgPayload = API_BASE_ID;
    msgPayload += API_MSG_TYPE_BUTTON_PRESS;
    msgPayload += API_DEVICE_BUTTON_ACK_NAME;

    clientApp.print(msgPayload);
  }

  strncpy(valTextUpper, "                ", LCD_DISPLAY_CHARS);
  strncpy(valTextLower, "                ", LCD_DISPLAY_CHARS);
  setTextDisplay(); 

}

/* ==============================================================
 * Handle the actions for Button Pattern Change
 * ============================================================== */
void onButtonPattern() {

  if(clientApp && clientApp.connected()) {
    String msgPayload = API_BASE_ID;
    msgPayload += API_MSG_TYPE_BUTTON_PRESS;
    msgPayload += API_DEVICE_BUTTON_PATT_NAME;

    clientApp.print(msgPayload);
  }
}

/*############################################################################################
### ============================== ###########################################################
### TCP/IP SERVER FUNCTIONS        ###########################################################
### ============================== ###########################################################
##############################################################################################*/

/* ==============================================================
 * initEthernet()
 * A function to start the Ethernet communication over TCP/IP
 * ============================================================== */
void initEthernet() {

  byte mac[] = ETHERNET_MAC_ADDR;
  IPAddress ip ETHERNET_IP_ADDR;
  IPAddress dns ETHERNET_DNS_ADDR;
  IPAddress gateway ETHERNET_GW_ADDR;
  IPAddress subnet ETHERNET_GW_SUBN;
  Ethernet.begin(mac, ip, dns, gateway, subnet);

  delay(1000);  // Give the Ethernet shield a second to initialize
  /*
  if (Ethernet.hardwareStatus() == EthernetNoHardware) {
    DEBUG_PRINTLN("INFO_: Ethernet shield was not found.");
  } else if (Ethernet.hardwareStatus() == EthernetW5100) {
    DEBUG_PRINTLN("INFO_: W5100 Ethernet controller detected.");
  } else if (Ethernet.hardwareStatus() == EthernetW5200) {
    DEBUG_PRINTLN("INFO_: W5200 Ethernet controller detected.");
  } else if (Ethernet.hardwareStatus() == EthernetW5500) {
    DEBUG_PRINTLN("INFO_: W5500 Ethernet controller detected.");
  }

  DEBUG_PRINTLN("My IP address: ");
  DEBUG_PRINTLN(Ethernet.localIP());
  */  
  // start listening for clients
  thisServer.begin();

  digitalWrite(DO_UPLINK_LIGHT_GREEN, LOW);
}

/* ==============================================================
 *
 * ============================================================== */
void checkNewClient() {

  // check for any new client connecting, and say hello (before any incoming data)
  EthernetClient newClient = thisServer.accept();

  if (newClient) {
    if (!clientApp) {
      clientApp = newClient;

      digitalWrite(DO_UPLINK_LIGHT_GREEN, HIGH);
      strncpy(valTextLower, "Uplink connected", LCD_DISPLAY_CHARS);
      setTextDisplay();      
    }
  }
}

/* ==============================================================
 *
 * ============================================================== */
void workOnClientData() {

  if (clientApp.available() < API_BASE_ID_LEN + API_MSG_TYPE_LEN) {
    // do not read data until it is enough to compose a message
    return;
  }
 
  memset(msgDataBuffer, 0, sizeof(msgDataBuffer));
  memset(apiBase, 0, sizeof(apiBase));
  memset(msgType, 0, sizeof(msgType));
  memset(msgDeviceName, 0, sizeof(msgDeviceName));
 
  msgDataPointer = msgDataBuffer;
  char data = 0; 
  int iPos = 0;

  while (clientApp && clientApp.available()) {
    data = clientApp.read();
    if (data != -1) {
      if (iPos < API_MSG_MAX_SIZE) {
        msgDataBuffer[iPos] = data;
        iPos++;
      }
    } 
  }
  
  // Now check basic parameters, if that is a valid API message 
  strncpy(apiBase, msgDataPointer, API_BASE_ID_LEN);
  apiBase[API_BASE_ID_LEN] = '\0';
  msgDataPointer += API_BASE_ID_LEN;
   
  if (strncmp(apiBase, API_BASE_ID, API_BASE_ID_LEN) == 0) {      

      strncpy(msgType, msgDataPointer, API_MSG_TYPE_LEN);
      msgType[API_MSG_TYPE_LEN] = '\0';
      msgDataPointer += API_MSG_TYPE_LEN;

      if (strncmp(msgType, API_MSG_TYPE_SET_ANDON, API_MSG_TYPE_LEN) == 0) {
          // --------------------------------------------
          // SETTING THE ANDON LIGHT  
          // Payload construct: NNNNNNNNNNXXX
          //                    |         |- 0 or 1 for off/on, in order Red, Yellow, Green
          //                    |- Device Name
          // --------------------------------------------
          strncpy(msgDeviceName, msgDataPointer, API_DEVICE_NAME_LEN);
          msgDeviceName[API_DEVICE_NAME_LEN] = '\0';
          msgDataPointer += API_DEVICE_NAME_LEN;

          if (strcmp(msgDeviceName, API_DEVICE_ANDON_LED_NAME) == 0) {
            strncpy(valAndonLight, msgDataPointer, API_DEVICE_ANDON_VALUE_LEN);
            valAndonLight[API_DEVICE_ANDON_VALUE_LEN] = '\0';
            msgDataPointer += API_DEVICE_ANDON_VALUE_LEN;

            setAndonLight();

          }
      }  
      else if (strcmp(msgType, API_MSG_TYPE_SET_LEDMATRIX) == 0) {
        
          // --------------------------------------------
          // SETTING THE LED MATRIX  
          // Payload construct: SMNNNNNNNNNNXX.64.XX (64 times X)
          //                    | |         |- R for red, G for Green, W for white, B for Blue, 0 for off. 
          //                    | |         |- First element is the lowest left LED, moving right. 9th element is first LED, second row from below.
          //                    | |- Device Name
          // --------------------------------------------
          strncpy(msgDeviceName, msgDataPointer, API_DEVICE_NAME_LEN);
          msgDeviceName[API_DEVICE_NAME_LEN] = '\0';
          msgDataPointer += API_DEVICE_NAME_LEN;

          if (strcmp(msgDeviceName, API_DEVICE_LEDMATRIX_NAME) == 0) {
            strncpy(valLedMatrix, msgDataPointer, API_DEVICE_LEDMATRIX_VALUE_LEN);
            valLedMatrix[API_DEVICE_LEDMATRIX_VALUE_LEN] = '\0';
            msgDataPointer += API_DEVICE_LEDMATRIX_VALUE_LEN;
          
            setLEDMatrix();

          }
      }
      else if (strcmp(msgType, API_MSG_TYPE_SET_TEXTDISPL) == 0) {
        
          // --------------------------------------------
          // SETTING THE Display Text  
          // Payload construct: STNNNNNNNNNNUUUUUUUUUUUUUUUULLLLLLLLLLLLLLLL
          //                    | |                         |- L - lower text, 16 chars
          //                    | |         |- U - upper text, 16 chars
          //                    | |- Device Name
          //                    |- Message Type
          // --------------------------------------------
          strncpy(msgDeviceName, msgDataPointer, API_DEVICE_NAME_LEN);
          msgDeviceName[API_DEVICE_NAME_LEN] = '\0';
          msgDataPointer += API_DEVICE_NAME_LEN;

          if (strcmp(msgDeviceName, API_DEVICE_TEXTDISPLAY_NAME) == 0) {
            
            strncpy(valTextUpper, msgDataPointer, LCD_DISPLAY_CHARS);
            valTextUpper[LCD_DISPLAY_CHARS] = '\0';
            msgDataPointer += LCD_DISPLAY_CHARS;
            strncpy(valTextLower, msgDataPointer, LCD_DISPLAY_CHARS);
            valTextLower[LCD_DISPLAY_CHARS] = '\0';
            msgDataPointer += LCD_DISPLAY_CHARS;
          
            setTextDisplay();
          }
      } else if (strncmp(msgType, API_MSG_TYPE_START_TESTRUN, API_MSG_TYPE_LEN) == 0) {
        // Sent to the board to let is start a roundtrip test run.
        // This is the same as pushing the "Start" button on the board 
        // Payload construct: TR
        //                    |- Message Type
        onButtonStart();
      }
      else if (strncmp(msgType, API_MSG_TYPE_TEST_VIRT_OUT, API_MSG_TYPE_LEN) == 0) {
        // Sent to the board as part of a Test run - a virtual Output Signal. 
        // This shall be sent by the client in response to a VI signal. 
        // The board will measure and log the duration from sending VI to receiving VO
        // Payload construct: VO000000000
        //                    | |- 8 digit sequence number in the test (mirroed from VI)
        //                    |- Message Type

        if (testRunActive == true) {

          // Measure the time and do calcs
          unsigned long nowMicros = micros();
          unsigned long duration = nowMicros - measurementStart;
          durationTotal += duration;
          if (duration < testRunMinDuration) testRunMinDuration = duration;
          if (duration > testRunMaxDuration) testRunMaxDuration = duration;

          testRunSeqNum ++;
          if (testRunSeqNum > TEST_RUN_SIZE) {
            endTestRun();            
          } else {
            // Send next DI
            sendNextTestDI();
          }
        }
      }
  }  
  return;  
}

/* ==============================================================
 *
 * ============================================================== */
void sendNextTestDI() {
  char buffer[API_MSG_TEST_SEQ_LEN + 1];
  sprintf(buffer, "%08d", testRunSeqNum);
  String msgPayload = API_BASE_ID;
  msgPayload += API_MSG_TYPE_TEST_VIRT_IN;
  msgPayload += buffer;

  measurementStart = micros();
  clientApp.print(msgPayload);
}

/* ==============================================================
 *
 * ============================================================== */
void endTestRun() {

  testRunActive = false;

  // Calculate average
  testRunAvgDuration = durationTotal / TEST_RUN_SIZE / 1000.0;
  // Update Display
  char avg[6];
  char min[6];
  char max[7];
  dtostrf(testRunAvgDuration, 1, 3, avg);
  dtostrf(testRunMinDuration/1000.0, 1, 3, min);
  dtostrf(testRunMaxDuration/1000.0, 2, 3, max);

  sprintf(valTextUpper,"OK! Avg:%.5s ms", avg);
  sprintf(valTextLower, "L:%.5s H:%.6s ", min, max);
  setTextDisplay(); 

  // Update LED
  digitalWrite(DO_TEST_LIGHT_BLUE, LOW);

}

/* ==============================================================
 *
 * ============================================================== */
void onClientDisconnect() {
    // stop any clients which disconnect
  if (clientApp && !clientApp.connected()) {

    clientApp.stop();
    digitalWrite(DO_UPLINK_LIGHT_GREEN, LOW);
    strncpy(valTextUpper, "Uplink down!    ", LCD_DISPLAY_CHARS);
    strncpy(valTextLower, "                ", LCD_DISPLAY_CHARS);
    setTextDisplay();     
  }
}

/*############################################################################################
### ============================== ###########################################################
### LEDS FUNCTIONS                 ###########################################################
### ============================== ###########################################################
##############################################################################################*/

// ------------------------------------------------------------------------------------------------------------
// 
// ------------------------------------------------------------------------------------------------------------
void setAndonLight() {
  
  if (valAndonLight[0] == '1') digitalWrite(DO_ANDON_LIGHT_RED, HIGH);
  else digitalWrite(DO_ANDON_LIGHT_RED, LOW);

  if (valAndonLight[1] == '1') digitalWrite(DO_ANDON_LIGHT_YELLOW, HIGH);
  else digitalWrite(DO_ANDON_LIGHT_YELLOW, LOW);

  if (valAndonLight[2] == '1') digitalWrite(DO_ANDON_LIGHT_GREEN, HIGH);
  else digitalWrite(DO_ANDON_LIGHT_GREEN, LOW);
  
}

/*############################################################################################
### ============================== ###########################################################
### LED MATRIX FUNCTIONS           ###########################################################
### ============================== ###########################################################
##############################################################################################*/

// ------------------------------------------------------------------------------------------------------------
// LED_MATRIX_SETUP - Inits the LED Matrix
// ------------------------------------------------------------------------------------------------------------
void ledMatrixSetup() {
  
  // Erstmal starten
  ledMatrix.begin();
  
  memset(valLedMatrix, API_DEVICE_LEDMATRIX_VAL_RED, API_DEVICE_LEDMATRIX_VALUE_LEN);
  setLEDMatrix();
  delay(200);
  memset(valLedMatrix, API_DEVICE_LEDMATRIX_VAL_GREEN, API_DEVICE_LEDMATRIX_VALUE_LEN);
  setLEDMatrix();
  delay(200);
  memset(valLedMatrix, API_DEVICE_LEDMATRIX_VAL_BLUE, API_DEVICE_LEDMATRIX_VALUE_LEN);  
  setLEDMatrix();
  delay(200);

  LED_MATRIX_OFF;  

}

// ------------------------------------------------------------------------------------------------------------
// 
// ------------------------------------------------------------------------------------------------------------
void setLEDMatrix() {

  ledMatrix.clear(); 
  
  char cVal = ' ';
  for (int  iPos = 0; iPos < API_DEVICE_LEDMATRIX_VALUE_LEN; iPos++) {

      cVal = valLedMatrix[iPos];

      //DEBUG_PRINT("Setting LED Matrix at position <"); DEBUG_PRINT(iPos); DEBUG_PRINT("> to value <"); DEBUG_PRINT(cVal); DEBUG_PRINT(">.");

      if (cVal == API_DEVICE_LEDMATRIX_VAL_WHITE) {
        ledMatrix.setPixelColor(iPos, ledMatrix.Color(LED_BRIGHTNES, LED_BRIGHTNES, LED_BRIGHTNES));
      } else if (cVal == API_DEVICE_LEDMATRIX_VAL_ORANGE) {
        ledMatrix.setPixelColor(iPos, ledMatrix.Color(LED_BRIGHTNES, LED_BRIGHTNES/2, 0));
      } else if (cVal == API_DEVICE_LEDMATRIX_VAL_BLUE) {
        ledMatrix.setPixelColor(iPos, ledMatrix.Color(0, 0, LED_BRIGHTNES));
      } else if (cVal == API_DEVICE_LEDMATRIX_VAL_GREEN) {
        ledMatrix.setPixelColor(iPos, ledMatrix.Color(0, LED_BRIGHTNES, 0));
      } else if (cVal == API_DEVICE_LEDMATRIX_VAL_RED) {
        ledMatrix.setPixelColor(iPos, ledMatrix.Color(LED_BRIGHTNES, 0, 0));
      } else if (cVal == API_DEVICE_LEDMATRIX_VAL_YELLOW) {
        ledMatrix.setPixelColor(iPos, ledMatrix.Color(LED_BRIGHTNES, LED_BRIGHTNES, 0));
      }
  }

  ledMatrix.show();
  return;
}

/*############################################################################################
### ============================== ###########################################################
### TEXT FIELD FUNCTIONS           ###########################################################
### ============================== ###########################################################
##############################################################################################*/

// ------------------------------------------------------------------------------------------------------------
// setTextDisplay
// ------------------------------------------------------------------------------------------------------------
void setTextDisplay() {
  lcdDisplay.setCursor(0, 0);  
  lcdDisplay.print(valTextUpper);
  lcdDisplay.setCursor(0, 1);  
  lcdDisplay.print(valTextLower);
}

/*############################################################################################
### ============================== ###########################################################
### MAIN FUNCTIONS                 ###########################################################
### ============================== ###########################################################
##############################################################################################*/

/* ==============================================================
 * SETUP 
 * The setup function runs once when you press reset or power the board
 * ============================================================== */
void setup() {

  // Init global memory
  memset(msgDataBuffer, 0, sizeof(msgDataBuffer));
  memset(msgDeviceName, 0, sizeof(msgDeviceName));

  // SET PINS
  pinMode(DO_ANDON_LIGHT_RED,    OUTPUT);
  pinMode(DO_ANDON_LIGHT_YELLOW, OUTPUT);
  pinMode(DO_ANDON_LIGHT_GREEN,  OUTPUT);
  pinMode(DO_UPLINK_LIGHT_GREEN, OUTPUT);
  pinMode(DO_TEST_LIGHT_BLUE,    OUTPUT);
  pinMode(DI_BUTTON_START,       INPUT);
  pinMode(DI_BUTTON_ACK,         INPUT);
  pinMode(DI_BUTTON_PATTERN,     INPUT);

  // Turn lights on
  memset(valAndonLight, API_DEVICE_ANDON_VALUE_ON, API_DEVICE_ANDON_VALUE_LEN);  
  setAndonLight();
  digitalWrite(DO_UPLINK_LIGHT_GREEN, HIGH);
  digitalWrite(DO_TEST_LIGHT_BLUE,    HIGH);  

  // Start the LED Matrix
  ledMatrixSetup();

  // SET LCD Display
  lcdDisplay.init();                	// initalisiere LCD 
  lcdDisplay.backlight();           	// Hintergrundbeleuchtung an
  lcdDisplay.clear();					        // Speicher leeren
  strcpy(valTextUpper, "Greetings!");
  setTextDisplay();

  // START ETHERNET and Server
  initEthernet();

  // Turn lights off
  memset(valAndonLight, API_DEVICE_ANDON_VALUE_OFF, API_DEVICE_ANDON_VALUE_LEN);
  setAndonLight();
  digitalWrite(DO_UPLINK_LIGHT_GREEN, LOW);
  digitalWrite(DO_TEST_LIGHT_BLUE,    LOW); 

  // Wait for everything to stableize
  delay(1000);
}

/* ==============================================================
 * MAIN LOOP 
 * The loop function runs over and over again forever
 * ============================================================== */
void loop() {
   // Now check the buttons
  if (checkButtonStartPressed() == true) onButtonStart();
  if (checkButtonAckPressed()   == true) onButtonAck();
  if (checkButtonPattPressed()  == true) onButtonPattern();
  
  // Continously check if new connections are there and, or new data is there.
  onClientDisconnect();
  checkNewClient();
   
  if (clientApp && clientApp.available() > 0) {
    workOnClientData();
  }
}
