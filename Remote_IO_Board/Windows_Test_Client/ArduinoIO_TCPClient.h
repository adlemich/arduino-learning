
#ifndef CLASS_TCPCLIENT_DEFINED
#define CLASS_TCPCLIENT_DEFINED

#include <string>
#include <thread>
#include <winsock2.h>
#pragma comment(lib, "Ws2_32.lib")

class ArduinoIOTcpClient {
public:
    ArduinoIOTcpClient(const std::string& host);
    ~ArduinoIOTcpClient();

    void sendSetAndonLightValue(const std::string& deviceValue);
    void sendSetLEDMatrixValue(const std::string& deviceValue);
    void sendSetTextDisplayValue(const std::string& deviceValue);
    void sendTestVirtualOut(const std::string& sequenceNumber);
    void sendStartTestRun();

private:
    void connect();
    void receiveData();
    void sendData(const std::string& data);
    void handleMsg(const std::string& msgData);
    void handleButtonPressed(const std::string& deviceName);
    void handleTestVirtualIn(const std::string& sequenceNumber);

    std::string server_host_;
    int server_port_;
    SOCKET sock_;
    std::thread receive_thread_;
    bool bExit_;
};

#endif // !CLASS_TCPCLIENT_DEFINED