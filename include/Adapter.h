#ifndef ADAPTER_H
#define ADAPTER_H

#include <cstdint>
#include <iostream>
#include "linkLayer.h"

class Adapter
{
    public:
    /**
     * Tries to close the handle to the current selected adapter and returns true if succeeded.
     * @return True if handled could be closed.
     */
    bool closeHandle();

    /**
     * Tries to open a handle to the selected adapter (if selected) and returns true if succeeded.
     * @return True if a handle to the adapter could be opened.
     */
    bool openHandle();

    /**
     * Select the adapter that matches the given name and returns true if succeeded.
     * @param adapterName The name of the adapter that should be selected.
     * @return True if adapter could be selected.
     */
    bool selectAdapter(const std::string &adapterName);

    /**
     * Returns a list to every the adapter found for this device.
     * @return pcaf_if_t
     */
    pcap_if_t *GetAdapters();

    /**
     * Returns the mac for the selected adapter. If it was not selected it will return an empty string.
     * @return A representation of the MAC in hex for ex: 10:45:8A:76:F0:00
     */
    std::string getMac();

    private:
        interface_t m_selectedInterface; // Selected interface
};

#endif