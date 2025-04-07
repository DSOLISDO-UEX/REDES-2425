#include "Communication.h"

Communication::Communication(Adapter *adapter, char groupNumber)
{
    m_adapter = adapter;
    m_groupNumber = groupNumber;
    std::copy_n(adapter->getAdapterMac(), sizeof(m_sourceMac), m_sourceMac);
}

Communication::~Communication()
{
    m_adapter = nullptr;
}

bool Communication::write(char protocolType[2], char *data, size_t dataSize)
{
    FrameBuilder builder(m_sourceMac, m_destinationMac, protocolType);
    if (data && dataSize != 0)
    {
        if (!builder.buildFrame(data, dataSize))
        {
            std::cout << "Could not build frame with data!" << std::endl;
            return false;
        }
    }
    else
    {
        if (!builder.buildEmptyFrame())
        {
            std::cout << "Could not build frame without data!" << std::endl;
            return false;
        }
    }

    return write(builder);
}

bool Communication::write(FrameBuilder &data)
{
    if (!m_adapter->sendFrame(data.getBody(), data.getBodySize()))
    {
        std::cout << "Could not send packet" << std::endl;
        return false;
    }

    std::cout << "Sent packet of length:" << data.getBodySize() << std::endl;
    data.free(); // Free sent data.

    return true;
}

char* Communication::getDestinationMac()
{
    return m_destinationMac;
}

char* Communication::getSourceMac()
{
    return m_sourceMac;
}

void Communication::setDestinationMac(char dstMac[6])
{
    std::copy_n(dstMac, sizeof(m_destinationMac), m_destinationMac);
    std::cout << "Set destination MAC:" << m_adapter->getMacAsStr(dstMac) << std::endl;
}

std::string Communication::read()
{
    apacket_t receivedPacket{};
    receivedPacket = m_adapter->receiveFrame();

    if (receivedPacket.packet != nullptr)
    {
        // Calculate real length of the packet. It supposes packet content ends with \x00
        size_t realLength = 14;
        while(realLength < receivedPacket.header.len)
        {
            realLength++;

            // Omit srcMac, dstMac, protocolType.
            if (receivedPacket.packet[realLength] == '\x00')
                break;
        }


        // With this constructor, string treats data as raw memory and not a regular string, ignoring middle \x00
        // characters.
        std::string result = std::string((char *)receivedPacket.packet, realLength);

        std::cout << "Received packet of length: " << realLength << std::endl;
        return result;
    }

    return "";
}