#include "RMC75EClient.h"
#include "cip/EPath.h"
#include "cip/GeneralStatusCodes.h"
#include "utils/Logger.h"
#include <sstream>
#include <iomanip>
#include <stdexcept>
#include <cstring>

#ifdef _WIN32
#include <winsock2.h>
#endif

using namespace rmc75e;
using namespace eipScanner;
using namespace eipScanner::cip;
using namespace eipScanner::utils;

RMC75EClient::RMC75EClient(const std::string& plcAddress, uint16_t port)
    : plcAddress_(plcAddress)
    , port_(port)
{
#ifdef _WIN32
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        throw std::runtime_error("WSAStartup failed");
    }
#endif
    Logger(LogLevel::INFO) << "RMC75EClient created for " << plcAddress << ":" << port;
}

RMC75EClient::~RMC75EClient() {
    disconnect();
#ifdef _WIN32
    WSACleanup();
#endif
}

void RMC75EClient::connect() {
    if (sessionInfo_) {
        Logger(LogLevel::WARNING) << "Already connected";
        return;
    }

    try {
        sessionInfo_ = std::make_shared<SessionInfo>(plcAddress_, port_);
        Logger(LogLevel::INFO) << "Connected to RMC75E at " << plcAddress_;
    } catch (const std::exception& e) {
        sessionInfo_.reset();
        throw std::runtime_error("Failed to connect to RMC75E at " + plcAddress_ + ": " + e.what());
    }
}

void RMC75EClient::disconnect() {
    if (sessionInfo_) {
        Logger(LogLevel::INFO) << "Disconnecting from RMC75E";
        sessionInfo_.reset();
    }
}

bool RMC75EClient::isConnected() const {
    return sessionInfo_ != nullptr;
}

// ---------------------------------------------------------------------------
// Register read/write
// ---------------------------------------------------------------------------

std::vector<float> RMC75EClient::readFloat(uint16_t file, uint16_t element, uint16_t count) {
    auto payload = buildReadPayload(file, element, count);
    auto raw = executeRequest(SVC_READ_LSB, payload);

    if (raw.size() < count * 4) {
        std::ostringstream ss;
        ss << "readFloat: expected " << count * 4 << " bytes, got " << raw.size();
        throw std::runtime_error(ss.str());
    }

    std::vector<float> result(count);
    std::memcpy(result.data(), raw.data(), count * sizeof(float));

    Logger(LogLevel::DEBUG) << "readFloat F" << file << ":" << element
                            << " count=" << count << " OK";
    return result;
}

void RMC75EClient::writeFloat(uint16_t file, uint16_t element, const std::vector<float>& values) {
    auto payload = buildWritePayload(file, element, values.data(), static_cast<uint16_t>(values.size()));
    executeRequest(SVC_WRITE_LSB, payload);

    Logger(LogLevel::DEBUG) << "writeFloat F" << file << ":" << element
                            << " count=" << values.size() << " OK";
}

std::vector<int32_t> RMC75EClient::readInt32(uint16_t file, uint16_t element, uint16_t count) {
    auto payload = buildReadPayload(file, element, count);
    auto raw = executeRequest(SVC_READ_LSB, payload);

    if (raw.size() < count * 4) {
        std::ostringstream ss;
        ss << "readInt32: expected " << count * 4 << " bytes, got " << raw.size();
        throw std::runtime_error(ss.str());
    }

    std::vector<int32_t> result(count);
    std::memcpy(result.data(), raw.data(), count * sizeof(int32_t));

    Logger(LogLevel::DEBUG) << "readInt32 L" << file << ":" << element
                            << " count=" << count << " OK";
    return result;
}

void RMC75EClient::writeInt32(uint16_t file, uint16_t element, const std::vector<int32_t>& values) {
    auto payload = buildWritePayload(file, element, values.data(), static_cast<uint16_t>(values.size()));
    executeRequest(SVC_WRITE_LSB, payload);

    Logger(LogLevel::DEBUG) << "writeInt32 L" << file << ":" << element
                            << " count=" << values.size() << " OK";
}

std::vector<uint8_t> RMC75EClient::sendRawRequest(uint8_t service, const std::vector<uint8_t>& data) {
    return executeRequest(service, data);
}

// ---------------------------------------------------------------------------
// Internal helpers
// ---------------------------------------------------------------------------

std::vector<uint8_t> RMC75EClient::buildReadPayload(uint16_t file, uint16_t element, uint16_t count) {
    std::vector<uint8_t> payload(6);
    // All fields are little-endian (LSB first)
    payload[0] = static_cast<uint8_t>(file & 0xFF);
    payload[1] = static_cast<uint8_t>(file >> 8);
    payload[2] = static_cast<uint8_t>(element & 0xFF);
    payload[3] = static_cast<uint8_t>(element >> 8);
    payload[4] = static_cast<uint8_t>(count & 0xFF);
    payload[5] = static_cast<uint8_t>(count >> 8);
    return payload;
}

std::vector<uint8_t> RMC75EClient::buildWritePayload(uint16_t file, uint16_t element, const void* values, uint16_t count) {
    std::vector<uint8_t> payload(6 + count * 4);
    // Header: file + element + count
    payload[0] = static_cast<uint8_t>(file & 0xFF);
    payload[1] = static_cast<uint8_t>(file >> 8);
    payload[2] = static_cast<uint8_t>(element & 0xFF);
    payload[3] = static_cast<uint8_t>(element >> 8);
    payload[4] = static_cast<uint8_t>(count & 0xFF);
    payload[5] = static_cast<uint8_t>(count >> 8);
    // Data: 4 bytes per value
    std::memcpy(payload.data() + 6, values, count * 4);
    return payload;
}

std::vector<uint8_t> RMC75EClient::executeRequest(uint8_t service, const std::vector<uint8_t>& payload) {
    if (!sessionInfo_) {
        throw std::runtime_error("Not connected - call connect() first");
    }

    auto response = messageRouter_.sendRequest(
        sessionInfo_,
        service,
        EPath(REGISTER_MAP_CLASS, REGISTER_MAP_INSTANCE),
        payload);

    if (response.getGeneralStatusCode() != GeneralStatusCodes::SUCCESS) {
        std::ostringstream ss;
        ss << "CIP request failed: service=0x" << std::hex << (int)service
           << " status=0x" << (int)response.getGeneralStatusCode();

        auto& additional = response.getAdditionalStatus();
        if (!additional.empty()) {
            ss << " additional=[";
            for (size_t i = 0; i < additional.size(); i++) {
                if (i > 0) ss << ", ";
                ss << "0x" << additional[i];
            }
            ss << "]";
        }
        throw std::runtime_error(ss.str());
    }

    return response.getData();
}
