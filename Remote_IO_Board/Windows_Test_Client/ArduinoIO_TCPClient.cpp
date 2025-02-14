#include <iostream>

#include "ArduinoIO_TcpClient.h"
#include "..\..\Arduino_IO_Program.c\ArduninoIO_API_v2.h"

//################################################################################
// Constructor
//################################################################################
ArduinoIOTcpClient::ArduinoIOTcpClient(const std::string& host) : server_host_(host), server_port_(API_SERVING_PORT) {
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    connect();
    receive_thread_ = std::thread(&ArduinoIOTcpClient::receiveData, this);
    bExit_ = false;
}

//################################################################################
// Destructor
//################################################################################
ArduinoIOTcpClient::~ArduinoIOTcpClient() {
    bExit_ = true;
    if (receive_thread_.joinable()) {
        receive_thread_.join();
    }
    closesocket(sock_);
    WSACleanup();
}

//################################################################################
// Send data to the board
//################################################################################
void ArduinoIOTcpClient::sendData(const std::string& data) {
    send(sock_, data.c_str(), data.size(), 0);
}


//################################################################################
// Connect to the board
//################################################################################
void ArduinoIOTcpClient::connect() {
    sock_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(server_port_);
    server_addr.sin_addr.s_addr = inet_addr(server_host_.c_str());

    if (::connect(sock_, (sockaddr*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        std::cerr << "Connection failed with error: " << WSAGetLastError() << std::endl;
        closesocket(sock_);
        WSACleanup();
        exit(1);
    }
    std::cout << "Connected to server." << std::endl;
}

//################################################################################
// General receiver on TCP Data
//################################################################################
void ArduinoIOTcpClient::receiveData() {
    std::string msgReceived;
    try {
        char buf[1024];
        while (!bExit_) {
            fd_set readfds;
            FD_ZERO(&readfds);
            FD_SET(sock_, &readfds);
            msgReceived.clear();

            timeval timeout;
            timeout.tv_sec = 0;
            timeout.tv_usec = 100000; // 100 milliseconds

            int selectResult = select(0, &readfds, nullptr, nullptr, &timeout);
            if (selectResult > 0 && FD_ISSET(sock_, &readfds)) {
                int bytes_received = recv(sock_, buf, sizeof(buf), 0);
                if (bytes_received > 0) {
                    msgReceived.assign(buf, bytes_received);
                    //std::cout << std::endl << "DATA RECEIVED: <" << msgReceived << ">" << std::endl;
                    //std::cout << "Choice: ";       

                    this->handleMsg(msgReceived);
                }
                else if (bytes_received == 0) {
                    std::cout << "Connection closed by server." << std::endl;
                    break;
                }
                else {
                    std::cerr << "Receive failed with error: " << WSAGetLastError() << std::endl;
                    break;
                }
            }
            else if (selectResult == 0) {
                // Timeout occurred, no data received
                continue;
            }
            else {
                std::cerr << "Select failed with error: " << WSAGetLastError() << std::endl;
                break;
            }
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Exception in receive thread: " << e.what() << std::endl;
    }
    return;
}


//################################################################################
// Handle incoming messages from the board
//################################################################################
void ArduinoIOTcpClient::handleMsg(const std::string& msgData) {

    int iDataPos = 0;
    
    // Now check basic parameters, if that is a valid API message 
    if (msgData.substr(iDataPos, API_BASE_ID_LEN) == API_BASE_ID) {
        iDataPos += API_BASE_ID_LEN;

        if (msgData.substr(iDataPos, API_MSG_TYPE_LEN) == API_MSG_TYPE_BUTTON_PRESS) {
            iDataPos += API_MSG_TYPE_LEN;            
            std::string deviceName = msgData.substr(iDataPos, API_DEVICE_NAME_LEN);
            handleButtonPressed(deviceName);
        }
        else if (msgData.substr(iDataPos, API_MSG_TYPE_LEN) == API_MSG_TYPE_TEST_VIRT_IN) {
            iDataPos += API_MSG_TYPE_LEN;
            std::string sequenceNum = msgData.substr(iDataPos, API_MSG_TEST_SEQ_LEN);
            handleTestVirtualIn(sequenceNum);
        }
        else {
            std::cout << "Bad Message Type received: " << msgData.substr(API_BASE_ID_LEN, API_MSG_TYPE_LEN);
        }
    }
    else {
        std::cout << "Bad API Base received: " << msgData.substr(0, API_BASE_ID_LEN);
    }

    return;
}

//################################################################################
// Handle Button actions
//################################################################################
void ArduinoIOTcpClient::handleButtonPressed(const std::string& deviceName) {
    std::cout << std::endl << "BUTTON WAS TRIGGERED. Name <" << deviceName << ">." << std::endl;
    std::cout << "Choice: ";       
}

//################################################################################
// Handle Incoming message for test sequence
//################################################################################
void ArduinoIOTcpClient::handleTestVirtualIn(const std::string& sequenceNum) {
    //std::cout << "Received Test virtual IN Signal. Seq <" << sequenceNum << ">." << " Will now auto-respond!" << std::endl;
    sendTestVirtualOut(sequenceNum);
}

//################################################################################
// Send test sequence response DO
//################################################################################
void ArduinoIOTcpClient::sendTestVirtualOut(const std::string& sequenceNum) {

    std::string msgToSend = API_BASE_ID;
    msgToSend += API_MSG_TYPE_TEST_VIRT_OUT;
    msgToSend += sequenceNum;

    std::cout << ".";
    this->sendData(msgToSend);

    return;
}

//################################################################################
// Instruct the board to set the Andon Lights
//################################################################################
void ArduinoIOTcpClient::sendSetAndonLightValue(const std::string& deviceValue) {
    
    std::string msgToSend = API_BASE_ID;
    msgToSend += API_MSG_TYPE_SET_ANDON;
    msgToSend += API_DEVICE_ANDON_LED_NAME;
    msgToSend += deviceValue;
    
    std::cout << "Sending message <" << msgToSend << ">." << std::endl;
    this->sendData(msgToSend);

    return;
}

//################################################################################
// Instruct the board to set the LED Matrix
//################################################################################
void ArduinoIOTcpClient::sendSetLEDMatrixValue(const std::string& deviceValue) {

    std::string msgToSend = API_BASE_ID;
    msgToSend += API_MSG_TYPE_SET_LEDMATRIX;
    msgToSend += API_DEVICE_LEDMATRIX_NAME;
    msgToSend += deviceValue;

    std::cout << "Sending message <" << msgToSend << ">." << std::endl;
    this->sendData(msgToSend);

    return;
}

//################################################################################
// Instruct the board to change the text of the display
//################################################################################
void ArduinoIOTcpClient::sendSetTextDisplayValue(const std::string& deviceValue) {

    std::string msgToSend = API_BASE_ID;
    msgToSend += API_MSG_TYPE_SET_TEXTDISPL;
    msgToSend += API_DEVICE_TEXTDISPLAY_NAME;
    msgToSend += deviceValue;

    std::cout << "Sending message <" << msgToSend << ">." << std::endl;
    this->sendData(msgToSend);

    return;
}

//################################################################################
// Remotely press the "Start Test Run" button
//################################################################################
void ArduinoIOTcpClient::sendStartTestRun() {

    std::string msgToSend = API_BASE_ID;
    msgToSend += API_MSG_TYPE_START_TESTRUN;

    std::cout << "Sending message <" << msgToSend << ">." << std::endl;
    this->sendData(msgToSend);

    return;
}