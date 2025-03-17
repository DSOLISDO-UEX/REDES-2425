#include "../include/Communication.h"

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

bool Communication::discoverSlave()
{
    // Build the packet.
    char destinationMac[6] = {char(0xFF), char(0xFF), char(0xFF), char(0xFF),
                              char(0xFF), char(0xFF)}; // MAC used to send the packet to all the devices of the network.
    char protocolType[2] = {char(0x30 + m_groupNumber), static_cast<char>(PacketType::AskForSlave)};

    if (!write(destinationMac, protocolType, nullptr))
        return false;

    std::string received;
    while (true)
    {
        received = read();

        if (received.empty())
            continue;

        std::cout << "Received packet when discovering, checking if it's from a slave" << std::endl;

        char *receivedTypeStart =
            (char *)(received.data() + 12); // Pointer to the start of the received packet protocol type.

        if (receivedTypeStart[0] == protocolType[0] &&
            receivedTypeStart[1] == static_cast<uint8_t>(PacketType::RepplyToMaster))
        {
            std::cout << "Received packet is from a slave! Saving source MAC" << std::endl;
            std::copy_n((char *)(received.data() + 6), sizeof(m_destinationMac), m_destinationMac);

            std::cout << "Slave MAC: " << m_adapter->getMacAsStr(m_destinationMac) << std::endl;
            break;
        }

        std::cout << "Received packet is not from a slave" << std::endl;
    }

    return true;
}

bool Communication::listenForMaster()
{
    char protocolType[2] = {char(0x30 + m_groupNumber),
                            static_cast<uint8_t>(PacketType::RepplyToMaster)}; // Initialized here for performace.
    std::string received;

    while (true)
    {
        received = read();

        if (received.empty())
            continue;

        std::cout << "Received packet when discovering, checking if it's from a master" << std::endl;

        char *receivedTypeStart =
            (char *)(received.data() + 12); // Pointer to the start of the received packet protocol type.

        if (receivedTypeStart[0] == protocolType[0] &&
            receivedTypeStart[1] == static_cast<uint8_t>(PacketType::AskForSlave))
        {
            std::cout << "Received packet is from a Master! Saving source MAC" << std::endl;
            std::copy_n((char *)(received.data() + 6), sizeof(m_destinationMac), m_destinationMac);

            std::cout << "Master MAC: " << m_adapter->getMacAsStr(m_destinationMac) << std::endl;
            std::cout << "Repplying to Master" << std::endl;
            // At this point we must notify the master that we are his slave.

            if (!write(m_destinationMac, protocolType, nullptr))
                return false;

            break;
        }
    }

    return true;
}

bool Communication::write(char destinationMac[6], char protocolType[2], char *data)
{
    FrameBuilder builder;

    if (data)
    {
        if (!builder.buildFrame(m_sourceMac, destinationMac, protocolType, data))
        {
            std::cout << "Could not build frame with data!" << std::endl;
            return false;
        }
    }
    else
    {
        if (!builder.buildEmptyFrame(m_sourceMac, destinationMac, protocolType))
        {
            std::cout << "Could not build frame without data!" << std::endl;
            return false;
        }
    }

    if (!m_adapter->sendFrame(builder.getBody(), builder.getBodySize()))
    {
        std::cout << "Could not send packet" << std::endl;
        return false;
    }

    std::cout << "Sent packet" << std::endl;
    return true;
}

std::string Communication::read()
{
    apacket_t receivedPacket{};
    receivedPacket = m_adapter->receiveFrame();

    if (receivedPacket.packet != nullptr)
    {
        std::cout << "Received packet" << std::endl;
        // With this constructor, string treats data as raw memory and not a regular string, ignoring middle \x00
        // characters.
        std::string result = std::string((char *)receivedPacket.packet, receivedPacket.header.len);
        m_adapter->freeFrameBuffer(); // Free received packet buffer because its content is already copied.

        return result;
    }

    return "";
}

void Communication::masterDoing()
{
    char stationMode;

    std::cout << "Mode selection: " << std::endl;
    std::cout << "[F1] - Interactive character sending" << std::endl;
    std::cout << "[F2] - Sending of a file" << std::endl;
    std::cout << "[ESC] - Exit" << std::endl;

    std::cin >> stationMode; //Hace falta?

    stationMode = getch();

    if (stationMode == 'O')
    {

        stationMode = getch();

        switch (stationMode)
        {
        case 'P': // F1

            char ch[2] = {'\x00', '\x00'};

            std::cin.ignore(); // Remove content from input buffer.

            bool working = true;
            while (working)
            {
                if (kbhit() == 1)
                {
                    ch[0] = getch();
                    std::cout << std::endl; // Ensure newline after writing a character.
                }

                if (ch[0] == 27)
                {
                    working = false;
                    std::cout << "Exiting..." << std::endl;

                    continue; // Next iteration will exit the WHILE.
                }

                // SEND
                if (ch[0] != '\x00')
                {
                    if (!write(m_destinationMac, 0, ch))
                    {
                        return;
                    }
                }

                // RECEIVE
                std::cout << read() << std::endl;

                std::this_thread::yield(); /*
                                            * Ensure this thread doesn't keep running on CPU without letting other
                                            * threads execute.
                                            */
            }

            break;

        case 'Q': // F2

            break;

        case 'R': // F3--Nada

            break;

        case 'S': // F4--Nada

            break;

        default:
            break;
        }
    }
}
