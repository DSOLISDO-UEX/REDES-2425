#include "../include/Adapter.h"

apacket_t Adapter::receiveFrame()
{
    return ReceiveFrame(&m_selectedInterface);
}

bool Adapter::close()
{
    return CloseAdapter(&m_selectedInterface) == 0;
}

bool Adapter::open(const std::string &adapterName)
{
    bool ret = setDeviceName(&m_selectedInterface, (char*)adapterName.data()) == 0;
    ret &= GetMACAdapter(&m_selectedInterface) == 0;
    ret &= OpenAdapter(&m_selectedInterface);

    return ret;
}

unsigned char * Adapter::buildFrame(unsigned char *srcMAC, unsigned char *dstMAC, unsigned char* protocol, unsigned char *payload)
{
    return BuildFrame(srcMAC, dstMAC, protocol, payload);
}

unsigned char * Adapter::buildFrameHeader(unsigned char *srcMAC, unsigned char *dstMAC, unsigned char* protocol)
{
    return BuildHeader(srcMAC, dstMAC, protocol);
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

    pcap_if_t *chosenAdapter = GetAdapters();
    size_t chosenId = SIZE_MAX;

    std::cout << "Choose one adapter: " << std::flush;
    std::cin >> chosenId;

    i = 0;
    while(i != chosenId)
    {
        chosenAdapter = chosenAdapter->next;
        i++;
    }

    open(chosenAdapter->name);
    std::cout << "Chosed " << chosenAdapter->name << std::endl;
}

std::string Adapter::getMac()
{
    std::stringstream stream;
    for (size_t i = 0; i < 6; i++)
    {
        stream << std::setfill('0') << std::setw(2) << std::hex << int(m_selectedInterface.MACaddr[i]);

        if (i < 5)
        {
            stream << ":";
        }
    }

    return stream.str();
}