/*
 * example_rmc75e_cpp.cpp
 *
 * Example usage of the RMC75EClient for explicit messaging with
 * a Delta RMC75E motion controller via EtherNet/IP.
 *
 * Demonstrates reading and writing registers using the Register Map Object
 * (class 0xC0), equivalent to RMCLink's readFloat/writeFloat.
 */

#include "RMC75EClient.h"
#include <iostream>
#include <iomanip>
#include <csignal>
#include <thread>
#include <chrono>

using namespace rmc75e;

// Global variable for Ctrl+C
volatile bool keep_running = true;

void signal_handler(int signum) {
    std::cout << "\nInterrupt signal received. Stopping..." << std::endl;
    keep_running = false;
}

int main(int argc, char** argv) {
    std::cout << "========================================" << std::endl;
    std::cout << "  RMC75EClient - Explicit Messaging Test" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << std::endl;

    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

    // Configuration
    std::string plc_address = "192.168.17.200";
    if (argc > 1) {
        plc_address = argv[1];
    }

    std::cout << "RMC75E address: " << plc_address << std::endl;
    std::cout << std::endl;

    try {
        // Create client and connect
        RMC75EClient client(plc_address);
        client.connect();
        std::cout << "Connected to RMC75E" << std::endl;
        std::cout << std::endl;

        // -----------------------------------------------------------------
        // Example 1: Read a single float register
        //   F57:30 = Variable 286 current value
        //   Equivalent to: value = rmc.readFloat(57, 30, 1)[0]
        // -----------------------------------------------------------------
        std::cout << "--- Example 1: Read single float register ---" << std::endl;
        {
            auto values = client.readFloat(57, 30, 1);
            std::cout << "  F57:30 (Variable 286) = " << values[0] << std::endl;
        }
        std::cout << std::endl;

        // -----------------------------------------------------------------
        // Example 2: Read multiple float registers
        //   F57:30 to F57:31 = Variables 286-287 current values
        //   Equivalent to: data = rmc.readFloat(57, 30, 2)
        // -----------------------------------------------------------------
        std::cout << "--- Example 2: Read multiple float registers ---" << std::endl;
        {
            auto values = client.readFloat(57, 30, 2);
            for (size_t i = 0; i < values.size(); i++) {
                std::cout << "  F57:" << (30 + i) << " = " << std::fixed
                          << std::setprecision(4) << values[i] << std::endl;
            }
        }
        std::cout << std::endl;

        // -----------------------------------------------------------------
        // Example 3: Read integer register
        //   L57:32 = Variable 288 as 32-bit integer
        //   Equivalent to: val = rmc.readInt32(57, 32, 1)[0]
        // -----------------------------------------------------------------
        std::cout << "--- Example 3: Read integer register ---" << std::endl;
        {
            auto values = client.readInt32(57, 32, 1);
            std::cout << "  L57:32 = " << values[0]
                      << " (0x" << std::hex << values[0] << std::dec << ")"
                      << std::endl;
        }
        std::cout << std::endl;

        // -----------------------------------------------------------------
        // Example 4: Write a float register
        //   F57:33 = Variable 289
        //   Equivalent to: rmc.writeFloat(57, 33, [3.14])
        //   WARNING: This writes to the controller!
        // -----------------------------------------------------------------
        std::cout << "--- Example 4: Write float register ---" << std::endl;
        {
            std::vector<float> values = {3.14f};
            client.writeFloat(57, 33, values);
            std::cout << "  Wrote F57:33 = 3.14" << std::endl;

            // Read back to verify
            auto readback = client.readFloat(57, 33, 1);
            std::cout << "  Readback F57:33 = " << readback[0] << std::endl;

            values = {1.0f};
            client.writeFloat(57, 33, values);
            std::cout << "  Wrote F57:33 = 1.0" << std::endl;

            // Read back to verify
            readback = client.readFloat(57, 33, 1);
            std::cout << "  Readback F57:33 = " << readback[0] << std::endl;
        }

        std::cout << std::endl;

        // -----------------------------------------------------------------
        // Example 5: Continuous monitoring
        //   Poll L57:32 every second until Ctrl+C
        // -----------------------------------------------------------------
        std::cout << "--- Example 5: Continuous monitoring (Ctrl+C to stop) ---" << std::endl;
        std::cout << "  Polling L57:32 every second..." << std::endl;
        std::cout << std::endl;

        while (keep_running) {
            auto values = client.readInt32(57, 32, 1);
            std::cout << "  L57:32 = " << std::fixed << std::setprecision(6)
                      << values[0] << std::endl;

            // Sleep 1 second in small increments for responsive Ctrl+C
            for (int i = 0; i < 10 && keep_running; i++) {
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
        }

        // Disconnect
        std::cout << std::endl;
        client.disconnect();
        std::cout << "Disconnected" << std::endl;

        std::cout << std::endl;
        std::cout << "========================================" << std::endl;
        std::cout << "  Test completed successfully" << std::endl;
        std::cout << "========================================" << std::endl;
        return 0;

    } catch (const std::exception& e) {
        std::cerr << std::endl;
        std::cerr << "Exception: " << e.what() << std::endl;
        return 1;
    }
}
