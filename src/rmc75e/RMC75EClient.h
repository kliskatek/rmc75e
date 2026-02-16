#ifndef RMC75E_CLIENT_H
#define RMC75E_CLIENT_H

#include <memory>
#include <string>
#include <vector>
#include <cstdint>
#include "SessionInfo.h"
#include "MessageRouter.h"

namespace rmc75e {

/**
 * @brief EtherNet/IP explicit messaging client for Delta RMC75E controllers.
 *
 * Provides register read/write via the RMC's Register Map Object (class 0xC0).
 * Service codes 0x4B/0x4C use LSB-first byte order.
 */
class RMC75EClient {
public:
    /// Register Map Object class ID
    static constexpr uint16_t REGISTER_MAP_CLASS    = 0xC0;
    /// Register Map Object instance
    static constexpr uint16_t REGISTER_MAP_INSTANCE = 0x01;

    /// Vendor-specific service codes (LSB-first variants)
    static constexpr uint8_t SVC_READ_LSB  = 0x4B;
    static constexpr uint8_t SVC_WRITE_LSB = 0x4C;
    /// MSB-first variants
    static constexpr uint8_t SVC_READ_MSB  = 0x4D;
    static constexpr uint8_t SVC_WRITE_MSB = 0x4E;

    /**
     * @brief Constructor
     * @param plcAddress RMC75E IP address (e.g. "192.168.1.100")
     * @param port EtherNet/IP port (default 0xAF12 = 44818)
     */
    explicit RMC75EClient(const std::string& plcAddress, uint16_t port = 0xAF12);
    ~RMC75EClient();

    /**
     * @brief Open an EtherNet/IP session to the RMC
     * @throws std::runtime_error on connection failure
     */
    void connect();

    /**
     * @brief Close the EtherNet/IP session
     */
    void disconnect();

    /**
     * @brief Check if connected
     */
    bool isConnected() const;

    /**
     * @brief Read floating-point registers (reads float registers from the RMC)
     * @param file   File number (e.g. 56 for Variables 0-255)
     * @param element Element offset within the file
     * @param count  Number of 32-bit values to read
     * @return Vector of float values
     * @throws std::runtime_error on failure
     */
    std::vector<float> readFloat(uint16_t file, uint16_t element, uint16_t count);

    /**
     * @brief Write floating-point registers (writes float registers to the RMC)
     * @param file    File number
     * @param element Element offset within the file
     * @param values  Float values to write
     * @throws std::runtime_error on failure
     */
    void writeFloat(uint16_t file, uint16_t element, const std::vector<float>& values);

    /**
     * @brief Read 32-bit integer registers (reads 32-bit integer registers from the RMC)
     * @param file   File number
     * @param element Element offset within the file
     * @param count  Number of 32-bit values to read
     * @return Vector of int32 values
     * @throws std::runtime_error on failure
     */
    std::vector<int32_t> readInt32(uint16_t file, uint16_t element, uint16_t count);

    /**
     * @brief Write 32-bit integer registers (writes 32-bit integer registers to the RMC)
     * @param file    File number
     * @param element Element offset within the file
     * @param values  Int32 values to write
     * @throws std::runtime_error on failure
     */
    void writeInt32(uint16_t file, uint16_t element, const std::vector<int32_t>& values);

    /**
     * @brief Send a raw CIP request via the Register Map Object
     * @param service Service code (e.g. 0x4B for read LSB-first)
     * @param data    Raw request payload
     * @return Raw response data
     * @throws std::runtime_error on failure
     */
    std::vector<uint8_t> sendRawRequest(uint8_t service, const std::vector<uint8_t>& data);

private:
    std::string plcAddress_;
    uint16_t port_;
    std::shared_ptr<eipScanner::SessionInfo> sessionInfo_;
    eipScanner::MessageRouter messageRouter_;

    /**
     * @brief Build the 6-byte request header: file(2) + element(2) + count(2)
     */
    static std::vector<uint8_t> buildReadPayload(uint16_t file, uint16_t element, uint16_t count);

    /**
     * @brief Build write payload: file(2) + element(2) + count(2) + data(4*count)
     */
    static std::vector<uint8_t> buildWritePayload(uint16_t file, uint16_t element, const void* values, uint16_t count);

    /**
     * @brief Execute a Register Map Object request and return the response data
     */
    std::vector<uint8_t> executeRequest(uint8_t service, const std::vector<uint8_t>& payload);
};

} // namespace rmc75e

#endif // RMC75E_CLIENT_H
