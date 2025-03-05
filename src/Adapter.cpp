#include "../include/Adapter.h"

apacket_t Adapter::receiveFrame()
{
    return ReceiveFrame(&m_selectedInterface);
}

bool Adapter::close()
{
    return CloseAdapter(&m_selectedInterface) == 0;
}

bool Adapter::open(const char *adapterName)
{
    if (setDeviceName(&m_selectedInterface, (char *)adapterName) != 0)
    {
        std::cout << "Could not set device name." << std::endl;
        return false;
    }

    if (GetMACAdapter(&m_selectedInterface) != 0)
    {
        std::cout << "Could not get MAC of adapter" << std::endl;
        return false;
    }

    if (OpenAdapter(&m_selectedInterface) != 0)
    {
        std::cout << "Could not open adapter" << std::endl;
        return false;
    }

    return true;
}

bool Adapter::openFromAvailable()
{
    pcap_if_t *current = GetAdapters();
    size_t i = 0;
    while (current != nullptr)
    {
        std::cout << "[" << i << "]" << " " << current->name << std::endl;

        i++;
        current = current->next;
    }

    if (i == 0)
    {
        // If i is 0 then it means the loop before didn't execute.
        std::cout << "There isn't any adapter available!" << std::endl;
        return false;
    }

    pcap_if_t *chosenAdapter = GetAdapters();
    size_t chosenId = SIZE_MAX;

    std::cout << "Choose one adapter: " << std::flush;
    std::cin >> chosenId;

    i = 0;
    while (i != chosenId && chosenAdapter != nullptr)
    {
        chosenAdapter = chosenAdapter->next;
        i++;
    }

    if (i != chosenId)
    {
        // If loop ended and i != chosenId, some adapters stopped working (or were disconnected) after printin them.
        std::cout << "Could not find selected adapter, was it disconnected?" << std::endl;
        return false;
    }

    if (!open(chosenAdapter->name))
        return false;

    std::cout << "Chosen " << chosenAdapter->name << std::endl;
    return true;
}

bool Adapter::sendFrame(char *p, size_t payloadSize)
{
    return SendFrame(&m_selectedInterface, (unsigned char*)p, payloadSize) == 0;
}

pcap_if_t *Adapter::GetAdapters()
{
    return GetAvailAdapters();
}

char* Adapter::getAdapterMac()
{
    return (char*)m_selectedInterface.MACaddr;
}

void Adapter::freeFrameBuffer()
{
    FreeBuffer();
}

std::string Adapter::getMacAsStr(char* macAddress)
{
    std::stringstream stream;
    for (size_t i = 0; i < 6; i++)
    {
        /*
         * This formats HEX numbers adding a 0 on the left when they are just 1 digit ->
         * 9 (hex) -> 09 (hex); A (hex) -> 0A (hex)
         */
        stream << std::setfill('0') << std::setw(2) << std::hex << int32_t((unsigned char)macAddress[i]);

        if (i < 5)
        {
            stream << ":";
        }
    }

    return stream.str();
}