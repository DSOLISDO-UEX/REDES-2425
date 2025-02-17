#ifndef ADAPTER_H
#define ADAPTER_H

#include <cstdint>
#include <iomanip>
#include <iostream>
#include <sstream>
#include "linkLayer.h"

/**
 * This class exists merely to stablish a bridge between C (LinkLayer) and C++ so the rest of the 
 * project can be coded in C++ without ever knowing the implementation behind Adapter.
 * 
 * interface_t does not have its own class because it only contains information. It doesn't have 
 * any module directly associated with it that aren't relationated with Adapter.
 */
class Adapter
{
    public:
    /**
     * Tries to read a packet received by the adapter.
     * @return The received assembled packet.
     */
    apacket_t receiveFrame();

    /**
     * Tries to close the handle to the current selected adapter and returns true if succeeded.
     * @return True if handled could be closed.
     */
    bool close();

    /**
     * Opens the adapter that matches the given name and returns true if succeeded.
     * @param adapterName The name of the adapter that should be opened.
     * @return True if adapter could be opened.
     */
    bool open(const char* adapterName);

    /** 
     * Important!! To build a frame use FrameBuilder! This function only needs getBody || getHeader and their corresponding size.
     * @param p Payload data.
     * @param payloadSize Payload data size.
     * @return Returns true if there wasn't any errors while sending the frame..
     */
    bool sendFrame(unsigned char *p, int payloadSize);

    /**
     * Returns a list to every adapter found for this device.
     * @return pcaf_if_t
     */
    pcap_if_t *GetAdapters();

    /** 
     * Frees the frame buffer for the current selected adapter. 
     */
    void freeFrameBuffer();

    /** 
     * Opens the chosen adapter. It will print all available adapters and will ask the user to choose one.
     * @return True if 1 iface was opened.
     */
    bool openFromAvailable();

    /**
     * Returns the mac for the selected interfaces. If it was not selected it will return an empty string.
     * @return A representation of the MAC in hex for ex: 10:45:8A:76:F0:00
     */
    std::string getMac();

    private:
        interface_t m_selectedInterface{}; // Selected interface
};

#endif