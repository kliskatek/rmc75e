#!/usr/bin/env python3
"""
example_python.py

Example usage of the rmc75e Python package for explicit messaging with
a Delta RMC75E motion controller via EtherNet/IP.

Demonstrates reading and writing registers using the Register Map Object
(class 0xC0), equivalent to RMCLink's readFloat/writeFloat.
"""

import signal
import sys
import time

from rmc75e import RMC75EClient


def main():
    print("========================================")
    print("  RMC75EClient - Python Example")
    print("========================================")
    print()

    # Configuration
    plc_address = "192.168.17.200"
    if len(sys.argv) > 1:
        plc_address = sys.argv[1]

    print(f"RMC75E address: {plc_address}")
    print()

    # Create client and connect
    client = RMC75EClient(plc_address)
    client.connect()
    print("Connected to RMC75E")
    print()

    try:
        # -----------------------------------------------------------------
        # Example 1: Read a single float register
        #   F57:30 = Variable 286 current value
        # -----------------------------------------------------------------
        print("--- Example 1: Read single float register ---")
        values = client.read_float(57, 30, 1)
        print(f"  F57:30 (Variable 286) = {values[0]}")
        print()

        # -----------------------------------------------------------------
        # Example 2: Read multiple float registers
        #   F57:30 to F57:31 = Variables 286-287 current values
        # -----------------------------------------------------------------
        print("--- Example 2: Read multiple float registers ---")
        values = client.read_float(57, 30, 2)
        for i, v in enumerate(values):
            print(f"  F57:{30 + i} = {v:.4f}")
        print()

        # -----------------------------------------------------------------
        # Example 3: Read integer register
        #   L57:32 = Variable 288 as 32-bit integer
        # -----------------------------------------------------------------
        print("--- Example 3: Read integer register ---")
        values = client.read_int32(57, 32, 1)
        print(f"  L57:32 = {values[0]} (0x{values[0]:X})")
        print()

        # -----------------------------------------------------------------
        # Example 4: Write a float register
        #   F57:33 = Variable 289
        #   WARNING: This writes to the controller!
        # -----------------------------------------------------------------
        print("--- Example 4: Write float register ---")
        client.write_float(57, 33, [3.14])
        print("  Wrote F57:33 = 3.14")

        readback = client.read_float(57, 33, 1)
        print(f"  Readback F57:33 = {readback[0]}")

        client.write_float(57, 33, [1.0])
        print("  Wrote F57:33 = 1.0")

        readback = client.read_float(57, 33, 1)
        print(f"  Readback F57:33 = {readback[0]}")
        print()

        # -----------------------------------------------------------------
        # Example 5: Continuous monitoring
        #   Poll L57:32 every second until Ctrl+C
        # -----------------------------------------------------------------
        print("--- Example 5: Continuous monitoring (Ctrl+C to stop) ---")
        print("  Polling L57:32 every second...")
        print()

        keep_running = True

        def signal_handler(signum, frame):
            nonlocal keep_running
            print("\nInterrupt signal received. Stopping...")
            keep_running = False

        signal.signal(signal.SIGINT, signal_handler)
        signal.signal(signal.SIGTERM, signal_handler)

        while keep_running:
            values = client.read_int32(57, 32, 1)
            print(f"  L57:32 = {values[0]}")
            time.sleep(1)

    finally:
        # Disconnect
        print()
        client.disconnect()
        print("Disconnected")

    print()
    print("========================================")
    print("  Test completed successfully")
    print("========================================")


if __name__ == "__main__":
    main()
