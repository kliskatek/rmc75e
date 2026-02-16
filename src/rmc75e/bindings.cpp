/*
 * Python bindings for RMC75EClient using pybind11.
 *
 * Exposes the RMC75EClient class for explicit messaging with
 * Delta RMC75E motion controllers via EtherNet/IP.
 */

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "RMC75EClient.h"

namespace py = pybind11;
using namespace rmc75e;

#ifndef RMC75E_VERSION
#define RMC75E_VERSION "0.0.0"
#endif

PYBIND11_MODULE(rmc75e_binding, m) {
    m.doc() = "EtherNet/IP explicit messaging client for Delta RMC75E motion controllers";

    py::class_<RMC75EClient>(m, "RMC75EClient",
        "EtherNet/IP explicit messaging client for Delta RMC75E controllers.\n\n"
        "Provides register read/write via the RMC's Register Map Object (class 0xC0).\n"
        "Service codes 0x4B/0x4C use LSB-first byte order.")
        .def(py::init<const std::string&, uint16_t>(),
            py::arg("plc_address"),
            py::arg("port") = 0xAF12,
            "Create a new RMC75E client.\n\n"
            "Args:\n"
            "    plc_address: RMC75E IP address (e.g. '192.168.1.100')\n"
            "    port: EtherNet/IP port (default 44818)")
        .def("connect", &RMC75EClient::connect,
            "Open an EtherNet/IP session to the RMC75E.\n\n"
            "Raises:\n"
            "    RuntimeError: If connection fails")
        .def("disconnect", &RMC75EClient::disconnect,
            "Close the EtherNet/IP session.")
        .def("is_connected", &RMC75EClient::isConnected,
            "Check if connected to the RMC75E.\n\n"
            "Returns:\n"
            "    True if connected, False otherwise")
        .def("read_float", &RMC75EClient::readFloat,
            py::arg("file"), py::arg("element"), py::arg("count"),
            "Read floating-point registers from the RMC.\n\n"
            "Args:\n"
            "    file: File number (e.g. 56 for Variables 0-255)\n"
            "    element: Element offset within the file\n"
            "    count: Number of 32-bit float values to read\n\n"
            "Returns:\n"
            "    List of float values\n\n"
            "Raises:\n"
            "    RuntimeError: If the read fails")
        .def("write_float", &RMC75EClient::writeFloat,
            py::arg("file"), py::arg("element"), py::arg("values"),
            "Write floating-point registers to the RMC.\n\n"
            "Args:\n"
            "    file: File number\n"
            "    element: Element offset within the file\n"
            "    values: List of float values to write\n\n"
            "Raises:\n"
            "    RuntimeError: If the write fails")
        .def("read_int32", &RMC75EClient::readInt32,
            py::arg("file"), py::arg("element"), py::arg("count"),
            "Read 32-bit integer registers from the RMC.\n\n"
            "Args:\n"
            "    file: File number\n"
            "    element: Element offset within the file\n"
            "    count: Number of 32-bit integer values to read\n\n"
            "Returns:\n"
            "    List of int32 values\n\n"
            "Raises:\n"
            "    RuntimeError: If the read fails")
        .def("write_int32", &RMC75EClient::writeInt32,
            py::arg("file"), py::arg("element"), py::arg("values"),
            "Write 32-bit integer registers to the RMC.\n\n"
            "Args:\n"
            "    file: File number\n"
            "    element: Element offset within the file\n"
            "    values: List of int32 values to write\n\n"
            "Raises:\n"
            "    RuntimeError: If the write fails")
        .def("send_raw_request", &RMC75EClient::sendRawRequest,
            py::arg("service"), py::arg("data"),
            "Send a raw CIP request via the Register Map Object.\n\n"
            "Args:\n"
            "    service: Service code (e.g. 0x4B for read LSB-first)\n"
            "    data: Raw request payload as bytes\n\n"
            "Returns:\n"
            "    Raw response data as bytes\n\n"
            "Raises:\n"
            "    RuntimeError: If the request fails");

    m.attr("__version__") = RMC75E_VERSION;
}
