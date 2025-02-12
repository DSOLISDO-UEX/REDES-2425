#ifndef ADAPTER_H
#define ADAPTER_H

#include <cstdint>
#include <iomanip>
#include <iostream>
#include <sstream>
#include "linkLayer.h"

// TODO: Move frame stuff to a self-contained class.

/**
 * This class exists merely to stablish a bridge between C (LinkLayer) and C++ so the rest of the 
 * project can be coded in C++ without ever knowing the implementation behind Adapter.
 */
class Adapter
{
    public:
    /**
     * Tries to read a packet received by the adapter.
     * @return An assembled packet.
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
     * @return True if adapter could be selected.
     */
    bool open(const std::string &adapterName);

    /** 
     * Builds a frame with the given information and payload.
     * @param srcMac MAC of the source device.
     * @param dstMac MAC of the destination device.
     * @param protocol Protocol for the frame.
     * @param payload Data that will be in the frame.
     * @return Data of the generated frame.
     */
    unsigned char * buildFrame(unsigned char *srcMAC, unsigned char *dstMAC, unsigned char* protocol, unsigned char *payload);

    /** 
     * Builds the header of the frame.
     * @param srcMac MAC of the source device.
     * @param dstMac MAC of the destination device.
     * @param protocol Protocol for the frame.
     * @return Data of the generated header.
     */
    unsigned char * buildFrameHeader(unsigned char *srcMAC, unsigned char *dstMAC, unsigned char* protocol);

    /** 
     * @param p Payload data.
     * @param payloadSize Payload data size.
     * @return Returns something != -1 if no error.
     */
    int sendFrame(unsigned char *p, int payloadSize);

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
     * Opens the chosen adapter. It will print all available adapters and will ask the user to chose one.
     */
    void openFromAvailable();

    /**
     * Returns the mac for the selected interfaces. If it was not selected it will return an empty string.
     * @return A representation of the MAC in hex for ex: 10:45:8A:76:F0:00
     */
    std::string getMac();

    private:
        interface_t m_selectedInterface{}; // Selected interface
};

#endif