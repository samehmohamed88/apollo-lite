
#include "can_client/CANClient.h"
#include "can_client/CANDBC.h"
#include "can_client/SocketCANDevice.h"
#include "can_client/SocketCANInterfaceImpl.h"

#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>
#include <libusb-1.0/libusb.h>

#include <chrono>
#include <cstdint> // For uint8_t
#include <fstream>
#include <iostream>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

using namespace nav;
using namespace nav::can;

bool EXIT = false;

// dont send steering command right away
// but send everything else
bool DO_SEND = false;

int COUNTER = 0;

bool IN_CAR = true;

std::string getFileNameAsCurrentTimestamp(std::string path = std::string("/apollo/nav/can/data/")) {
    auto currentTime = std::chrono::system_clock::now();
    std::time_t time = std::chrono::system_clock::to_time_t(currentTime);
    std::tm timeinfo = *std::localtime(&time);
    char buffer[80];  // Adjust the buffer size as needed
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d-%H-%M-%S", &timeinfo);
    std::string filename = path + std::string("can-messages-received_") + std::string(buffer) + ".txt";  // You can change the file extension as needed
    return filename;
}

std::thread CreateWriteFileThread(CANClient<SocketCANDevice<SocketCANInterfaceImpl>>& canDevice) {
    auto thread = std::thread([&]{
        std::ofstream parsedData(getFileNameAsCurrentTimestamp());
        while (!EXIT) {
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
            auto messages = canDevice.getQueuedMessagesAndClearQueue();
            for (auto const& message : messages) {
                parsedData << message.getName();
//                parsedData << std::to_string(static_cast<int>(message.getCanBus()));
                for (auto const &signal : message.getSignals()) {
                    parsedData << "," << signal.getName() << "," << signal.getValue();
                }
                parsedData << "\n";
            }
        }
    });
    return thread;
}

std::thread CreateReceiveThread(CANClient<SocketCANDevice<SocketCANInterfaceImpl>>& canDevice) {
    auto thread = std::thread([&]{
        while (!EXIT) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            canDevice.getMessages<SocketCANMessage>();
            ++COUNTER;
            if (COUNTER % 10 == 0) {
                EXIT = true;
            }
        }
    });
    return thread;
}


int main() {

    auto canDBC = CANDBC::CreateInstance();

    auto socket = std::make_unique<SocketCANDevice<SocketCANInterfaceImpl>>(std::make_unique<SocketCANInterfaceImpl>("can0"));

    auto canDevice = CANClient<SocketCANDevice<SocketCANInterfaceImpl>>{std::move(socket), std::move(canDBC)};

    auto receiveThread = CreateReceiveThread(canDevice);
    auto saveThread = CreateWriteFileThread(canDevice);

    receiveThread.join();
    saveThread.join();

}
