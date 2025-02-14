/* ==============================================================
 * DEFINITIONS FOR THE SERVER API
 * The basic structure of a valid API message is this:
 *   ARIO_API_V02XX....<message specific data>....
 *   |        |  |-Message Type, starts with MT always, last 2 define exact type.
 *   |        |- Version of API, must be the same on both sides.
 *   |- API Base ID
 *
 * ============================================================== */
#define API_SERVING_PORT                  14141

#define API_BASE_ID                       "ARIO_API_V02"
#define API_BASE_ID_LEN                   12

 /// DEVICE NAMES 
#define API_DEVICE_NAME_LEN               10
#define API_DEVICE_BUTTON_START_NAME      "START_TEST" // Start a test run
#define API_DEVICE_BUTTON_ACK_NAME        "ACK_ERRALA" // Acknowledge Errors and Alarms
#define API_DEVICE_BUTTON_PATT_NAME       "PATTERNSWI" // Switch Patterns
#define API_DEVICE_ANDON_LED_NAME         "ANDONLIGHT" // Andon Light
#define API_DEVICE_LEDMATRIX_NAME         "LED_MATRIX" // LED Matrix
#define API_DEVICE_TEXTDISPLAY_NAME       "TEXTDISPLA" // Test Display

/// MESSAGE TYPES
#define API_MSG_TYPE_LEN                  2

// Sent by the board when a button was pressed. 
// Payload construct: BPNNNNNNNNNN
//                    | |- Device Name
//                    |- Message Type
#define API_MSG_TYPE_BUTTON_PRESS         "BP"   

// Sent to the board when an Andon Light shall be modified. 
// Payload construct: SANNNNNNNNNNXXX
//                    | |         |- 0 or 1 for off/on, in order Red, Yellow, Green
//                    | |- Device Name
//                    |- Message Type
#define API_MSG_TYPE_SET_ANDON            "SA" 
#define API_DEVICE_ANDON_VALUE_LEN        3
#define API_DEVICE_ANDON_VALUE_ON         '1'
#define API_DEVICE_ANDON_VALUE_OFF        '0'

// Sent to the board when the LED Matrix shall be modified. 
// Payload construct: SMNNNNNNNNNNXX.64.XX (64 times X)
//                    | |         |- R for red, G for Green, B for Blue, W for white, Y for Yellow, O for Orange, X for off. 
//                    | |         |- First element is the lowest left LED, moving right. 9th element is first LED, second row from the bottom.
//                    | |- Device Name
//                    |- Message Type
#define API_MSG_TYPE_SET_LEDMATRIX        "SM"
#define API_DEVICE_LEDMATRIX_VALUE_LEN    64
#define API_DEVICE_LEDMATRIX_VAL_WHITE    'W'
#define API_DEVICE_LEDMATRIX_VAL_RED      'R'
#define API_DEVICE_LEDMATRIX_VAL_YELLOW   'Y'
#define API_DEVICE_LEDMATRIX_VAL_BLUE     'B'
#define API_DEVICE_LEDMATRIX_VAL_ORANGE   'O'
#define API_DEVICE_LEDMATRIX_VAL_GREEN    'G'
#define API_DEVICE_LEDMATRIX_VAL_OFF      'X'

// Sent to the board when the Text Display shall be modified. 
// Payload construct: STNNNNNNNNNNUUUUUUUUUUUUUUUULLLLLLLLLLLLLLLL
//                    | |                         |- L - lower text, 16 chars
//                    | |         |- U - upper text, 16 chars
//                    | |- Device Name
//                    |- Message Type
#define API_MSG_TYPE_SET_TEXTDISPL        "ST"
#define API_DEVICE_TEXTDISPLAY_TEXT_LEN   LCD_DISPLAY_ROWS * LCD_DISPLAY_CHARS

// Sent to the board to let is start a roundtrip test run.
// This is the same as pushing the "Start" button on the board 
// Payload construct: TR
//                    |- Message Type
#define API_MSG_TYPE_START_TESTRUN       "TR"

// Sent from the board as part of a Test run - a virtual Input Signal. 
// This expects as virtual output signal change in response
// Payload construct: VI00000000
//                    | |- 8 digit sequence number in the test
//                    |- Message Type
#define API_MSG_TYPE_TEST_VIRT_IN        "VI"
#define API_MSG_TEST_SEQ_LEN             8
// Sent to the board as part of a Test run - a virtual Output Signal. 
// This shall be sent by the client in response to a VI signal. 
// The board will measure and log the duration from sending VI to receiving VO
// Payload construct: VO000000000
//                    | |- 8 digit sequence number in the test (mirroed from VI)
//                    |- Message Type
#define API_MSG_TYPE_TEST_VIRT_OUT        "VO"

/// MAX MESSAGE SIZE
#define API_MSG_MAX_SIZE                  API_BASE_ID_LEN + API_MSG_TYPE_LEN + API_DEVICE_NAME_LEN + API_DEVICE_LEDMATRIX_VALUE_LEN
