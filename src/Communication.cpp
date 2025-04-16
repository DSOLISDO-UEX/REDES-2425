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
    std::string wrapped = std::string(data, dataSize);

    if (data && dataSize != 0)
    {
        if (!builder.buildFrame(wrapped))
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
    if (!m_adapter->sendFrame(data.getBody(), data.getDataSize()))
    {
        std::cout << "Could not send packet" << std::endl;
        return false;
    }

    std::cout << "Sent packet of length:" << data.getBodySize() << std::endl;
    data.free(); // Free sent data.

    return true;
}

char *Communication::getCommonProtocol()
{
    // This is made static because 1 running program will only have 1 Communication.
    static char protocol[2] = {char(0x30 + m_groupNumber), static_cast<char>(FrameType::Common)};
    return protocol;
}

char *Communication::getDestinationMac()
{
    return m_destinationMac;
}

char *Communication::getSourceMac()
{
    return m_sourceMac;
}

void Communication::setDestinationMac(char dstMac[6])
{
    std::copy_n(dstMac, sizeof(m_destinationMac), m_destinationMac);
    std::cout << "Set destination MAC:" << m_adapter->getMacAsStr(dstMac) << std::endl;
}

void Communication::setExpectedFrameType(FrameType type)
{
    m_expectedFrameType = type;
}

std::string Communication::read()
{
    std::string received = readInternal();

    if (received.empty())
        return "";

    // There's data to read. Check if it's for us.
    char *receivedTypeStart = (char *)(received.data() + 12);
    char *receivedDataStart = (char *)(received.data() + 14);
    // Pointer to the start of the received packet protocol type.

    if (receivedTypeStart[0] == char(0x30 + m_groupNumber) && receivedTypeStart[1] == char(m_expectedFrameType))
    {
        if (m_expectedFrameType == FrameType::AskForSlave || m_expectedFrameType == FrameType::RepplyToMaster)
        {
            // If expected type is one of the discovering phase, we want to keep source mac and set it as destination.
            setDestinationMac((char *)(received.data() + 6));
        }

        // Packet is from slave. Show it. Data gets a copy of the packet data.
        if (received.size() > 14)
            return std::string(receivedDataStart, received.size() - 14); // Returns frame's data.
        else
            return "0"; // An empty frame was received. Return 1 character so Master / Slave know there was something read.
    }

    return "";
}

std::string Communication::readInternal()
{
    apacket_t receivedPacket{};
    receivedPacket = m_adapter->receiveFrame();

    if (receivedPacket.packet != nullptr)
    {
        // Calculate real length of the packet. It supposes packet content ends with \x00
        size_t realLength = 14;
        while (realLength < receivedPacket.header.len)
        {
            // Omit srcMac, dstMac, protocolType.
            if (receivedPacket.packet[realLength] == '\x00')
                break;

            realLength++;
        }

        // With this constructor, string treats data as raw memory and not a regular string, ignoring middle \x00
        // characters.
        std::string result = std::string((char *)receivedPacket.packet, realLength);
        std::cout << "Received packet of length: " << realLength << std::endl;

        return result;
    }

    return "";
}