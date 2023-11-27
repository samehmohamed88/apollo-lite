
#include "nav/can_client/CANDBC.h"
#include "nav/can_client/UsbDevice.h"
#include "nav/can_client/LibUsbDevice.h"
#include "nav/can_client/CommaAICANInterfaceWithBoostBuffer.h"

#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>
#include <libusb-1.0/libusb.h>

#include <fstream>
#include <chrono>
#include <thread>
#include <vector>
#include <iostream>
#include <cstdint>  // For uint8_t

using namespace nav;

int main() {
    using namespace nav;
    static constexpr uint16_t vendorID_ = 0xbbaa;
    static constexpr uint16_t productID_ = 0xddcc;

    auto device = std::make_unique<can::UsbDevice<can::LibUsbDevice>>(
            std::make_unique<can::LibUsbDevice>(),
                    vendorID_,
                    productID_,
                    0);

    auto canDevice = can::CommaAICANInterfaceWithBoostBuffer<can::UsbDevice<can::LibUsbDevice>>{std::move(device)};
    auto hw = canDevice.getHardwareType();
    assert(hw == 7);
    std::cout << "Hardware Type == " << std::to_string(hw) << std::endl;

    std::string filename = "/apollo/nav/can_client/data/can_output_2023-11-25_22-32-26.bin"; // Replace with your filename
    std::ifstream file(filename, std::ios::binary);
    const size_t chunkSize = 1024; // Size of each chunk in bytes
    std::vector<uint8_t> buffer(chunkSize);

    std::ofstream parsedData("/apollo/nav/can_client/data/can_parsed_2023-11-25_22-32-26.csv");


    auto thread1 = std::thread([&]{
        while (file) {
            auto messages = canDevice.getCANMessagesAndClearContainer();
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
//            std::cout << " size of messages " << messages.size() << std::endl;
            for (auto const& message : messages) {
//                std::cout << "Message name is "
//                          << message.name
//                          << " and address is "
//                          << message.address
//                          << " and signal size "
//                          << message.signals.size() << std::endl;
                for (auto const& signal : message.signals) {
                    parsedData << message.name << ",";
                    parsedData << signal.name << ",";
                    parsedData << signal.value;
                    parsedData << "\n";
                }
            }
        }
    });

    auto thread2 = std::thread([&]{
        while (file) {
//            std::this_thread::sleep_for(std::chrono::milliseconds(75));
            file.read(reinterpret_cast<char*>(buffer.data()), chunkSize);
            size_t bytesRead = file.gcount();
//            std::cout << " number of bytes read = " << bytesRead << std::endl;
            if (bytesRead > 0) {
                buffer.resize(bytesRead); // Resize buffer to actual bytes read
                canDevice.receiveMessages(buffer);
                buffer.resize(chunkSize); // Resize back for next read
            }
        }
    });
    thread1.join();
    thread2.join();
    std::cout << "All clear" << std::endl;
    return 0;
}