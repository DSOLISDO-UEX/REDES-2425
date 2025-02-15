#include "../include/Adapter.h"

apacket_t Adapter::receiveFrame()
{
    return ReceiveFrame(&m_selectedInterface);
}

bool Adapter::close()
{
    return CloseAdapter(&m_selectedInterface) == 0;
}

bool Adapter::open(const char* adapterName)
{
    bool ret = setDeviceName(&m_selectedInterface, (char*)adapterName) == 0;
    ret &= GetMACAdapter(&m_selectedInterface) == 0;
    ret &= OpenAdapter(&m_selectedInterface) == 0;

    return ret;
}

int Adapter::sendFrame(unsigned char *p, int payloadSize)
{
    return SendFrame(&m_selectedInterface, p, payloadSize);
}

pcap_if_t *Adapter::GetAdapters()
{
    return GetAvailAdapters();
}

void Adapter::freeFrameBuffer()
{
    FreeBuffer();
}

void Adapter::openFromAvailable()
{
    pcap_if_t * current = GetAdapters();
    size_t i = 0;
    while(current != nullptr)
    {
        std::cout << "[" << i << "]" << " " << current->name << std::endl;

        i++;
        current = current->next;
        
    }

    if (i == 0)
    {
        // If i is 0 then it means the loop before didn't execute.
        std::cout << "There isn't any adapter available!" << std::endl;
        return;
    }

    pcap_if_t *chosenAdapter = GetAdapters();
    size_t chosenId = SIZE_MAX;

    std::cout << "Choose one adapter: " << std::flush;
    std::cin >> chosenId;

    i = 0;
    while(i != chosenId && chosenAdapter != nullptr)
    {
        chosenAdapter = chosenAdapter->next;
        i++;
    }

    if (i != chosenId)
    {
        // If loop ended and i != chosenId, some adapters stopped working (or were disconnected) after printin them.
        std::cout << "Could not find selected adapter, was it disconnected?" << std::endl;
        return;
    }

    open(chosenAdapter->name);
    std::cout << "Chosed " << chosenAdapter->name << std::endl;
}

std::string Adapter::getMac()
{
    std::stringstream stream;
    for (size_t i = 0; i < 6; i++)
    {
        /*
         * This formats HEX numbers adding a 0 on the left when they are just 1 digit ->
         * 9 (hex) -> 09 (hex); A (hex) -> 0A (hex)
         */
        stream << std::setfill('0') << std::setw(2) << std::hex << int(m_selectedInterface.MACaddr[i]);

        if (i < 5)
        {
            stream << ":";
        }
    }

    return stream.str();
}