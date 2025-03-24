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
    // MAC used to send the packet to all the devices of the network.
    char destinationMac[6] = {char(0xFF), char(0xFF), char(0xFF), char(0xFF), char(0xFF), char(0xFF)};
    char protocolType[2] = {char(0x30 + m_groupNumber), static_cast<char>(PacketType::AskForSlave)};

    if (!write(destinationMac, protocolType, nullptr, 0))
        return false;

    std::string received;
    while (true)
    {
        received = read();

        if (received.empty())
            continue;

        std::cout << "Received packet when discovering, checking if it's from a slave" << std::endl;

        // Pointer to the start of the received packet protocol type.
        char *receivedTypeStart = (char *)(received.data() + 12);

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
    // Initialized here for performace.
    char protocolType[2] = {char(0x30 + m_groupNumber), static_cast<uint8_t>(PacketType::RepplyToMaster)};
    std::string received;

    while (true)
    {
        received = read();

        if (received.empty())
            continue;

        std::cout << "Received packet when discovering, checking if it's from a master" << std::endl;

        // Pointer to the start of the received packet protocol type.
        char *receivedTypeStart = (char *)(received.data() + 12);

        if (receivedTypeStart[0] == protocolType[0] &&
            receivedTypeStart[1] == static_cast<uint8_t>(PacketType::AskForSlave))
        {
            std::cout << "Received packet is from a Master! Saving source MAC" << std::endl;
            std::copy_n((char *)(received.data() + 6), sizeof(m_destinationMac), m_destinationMac);

            std::cout << "Master MAC: " << m_adapter->getMacAsStr(m_destinationMac) << std::endl;
            std::cout << "Repplying to Master" << std::endl;
            // At this point we must notify the master that we are his slave.

            if (!write(m_destinationMac, protocolType, nullptr, 0))
                return false;

            break;
        }
    }

    return true;
}

bool Communication::write(char destinationMac[6], char protocolType[2], char *data, size_t dataSize)
{
    FrameBuilder builder;
    if (data && dataSize != 0)
    {
        if (!builder.buildFrame(m_sourceMac, destinationMac, protocolType, data, dataSize))
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

    return write(destinationMac, protocolType, builder);
}

bool Communication::write(char destinationMac[6], char protocolType[2], FrameBuilder &data)
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

void Communication::masterLoop()
{
    showMasterMenu();

    bool sendCharacters = false;
    char expectedProtocolType[2] = {char(0x30 + m_groupNumber), static_cast<uint8_t>(PacketType::Common)};
    std::string received;

    while (true)
    {
        received = read();

        // Only read if character sending was enabled.
        if (!received.empty() && sendCharacters)
        {
            // There's data to read. Check if it's for us.
            char *receivedTypeStart = (char *)(received.data() + 12);
            char *receivedDataStart = (char *)(received.data() + 14);
            // Pointer to the start of the received packet protocol type.

            if (receivedTypeStart[0] == expectedProtocolType[0] && receivedTypeStart[1] == expectedProtocolType[1])
            {
                std::cout << "Received packet is from slave" << std::endl;
                // Packet is from slave. Show it. Data gets a copy of the packet data.
                // .size() returns the size of the BUFFER not the string length. Use recommended
                // on the C++ standard to work with raw memory bytes.
                std::string data = std::string(receivedDataStart, received.size() - 14);
                std::cout << "Received: " << data << std::endl;
            }
        }

        // Check input for menu
        if (kbhit())
        {
            // User hit a key.
            char ch[2] = {0, 0};
            ch[0] = char(getch());  // Ensure this is a valid null-terminated string that can be passed straight to
                                    // our write function.
            std::cout << std::endl; // Make sure input and output don't overlap on console (only for better reading...)

            // FN input: ESC+'O'+FN_KEY_ID ('P' , 'Q' , 'R' , 'S') -> {27, 'O', FN_KEY_ID}

            if (ch[0] == 27)
            {
                if (kbhit())
                {
                    ch[0] = getch();
                    if (ch[0] == 'O' && !sendCharacters)
                    {
                        // User hit FN key, check which FX was hit.
                        char functionKey = getch();
                        switch (functionKey)
                        {
                        case 'P': // F1 ------------------------------------------------------------
                        {
                            sendCharacters = true;
                            std::cout << "Master is ready to send data to slave" << std::endl;
                            break;
                        }
                        case 'Q': // F2 ------------------------------------------------------------
                        {
                            std::cout << "Master is ready to send a file to slave" << std::endl;

                            // Sends a file splitted in frames (Trama-XX....)
                            std::string path = "envio.txt";
                            std::vector<FrameBuilder> frames =
                                FrameBuilder().fromFile(m_sourceMac, m_destinationMac, expectedProtocolType, path);

                            for (FrameBuilder &frame : frames)
                            {
                                if (!write(m_destinationMac, expectedProtocolType, frame))
                                {
                                    std::cout << "Error while sending file!" << std::endl;
                                    return;
                                }
                            }

                            showMasterMenu();
                        }
                        }
                    }
                }
                else if (sendCharacters)
                {
                    // Exit pressed inside a mode, restore and show menu again.
                    sendCharacters = false;
                    showMasterMenu();
                }
                else
                {
                    // ESC was hit on the menu, exit the program.
                    std::cout << "Exitting..." << std::endl;
                    return; // Exit loop.
                }
            }
            else if (sendCharacters && ch[0] != 0)
            {
                // After selecting F1, user pressed a key that is not exit. Character must be sent.
                if (!write(m_destinationMac, expectedProtocolType, ch, 2))
                {
                    std::cout << "Error while sending character, aborting..." << std::endl;
                    return;
                }
            }
        }
    }
}

void Communication::slaveLoop()
{
    showSlaveMenu();

    // Initialized here for performace.
    bool sendCharacters = false;
    char expectedProtocolType[2] = {char(0x30 + m_groupNumber), static_cast<uint8_t>(PacketType::Common)};
    std::string received;

    while (true)
    {
        received = read();
        if (!received.empty())
        {
            char *receivedTypeStart = (char *)(received.data() + 12);
            char *receivedDataStart = (char *)(received.data() + 14);
            // Pointer to the start of the received packet protocol type.

            if (receivedTypeStart[0] == expectedProtocolType[0] && receivedTypeStart[1] == expectedProtocolType[1])
            {
                std::cout << "Received packet is from master" << std::endl;
                // Packet is from master. Show it. Data gets a copy of the packet data.
                // .size() returns the size of the BUFFER not the string length. Use recommended on the C++ standard to
                // work with raw memory bytes.
                std::string data = std::string(receivedDataStart, received.size() - 14);
                std::cout << "Received: " << data << std::endl;
            }
        }

        // Check input for menu
        if (kbhit())
        {
            // User hit a key.
            char ch[2] = {0, 0};
            ch[0] = char(getch());  // Ensure this is a valid null-terminated string that can be passed straight to
                                    // out write function.
            std::cout << std::endl; // Make sure input and output don't overlap on console (only for better reading...).

            // FN input: ESC+'O'+FN_KEY_ID ('P', 'Q', 'R', 'S')

            if (ch[0] == 27)
            {
                if (kbhit())
                {
                    ch[0] = getch();
                    if (ch[0] == 'O' && !sendCharacters)
                    {
                        // User hit FN key, check which FX was hit.
                        char functionKey = getch();
                        switch (functionKey)
                        {
                        case 'P': // F1
                        {
                            std::cout << "Slave is ready to send data to master" << std::endl;
                            sendCharacters = true;

                            break;
                        }
                        }
                    }
                }
                else if (sendCharacters)
                {
                    // Exit pressed inside a mode, restore and show menu again.
                    sendCharacters = false;
                    showSlaveMenu();
                }
                else
                {
                    // ESC was hit on the menu, exit the program.
                    std::cout << "Exitting..." << std::endl;
                    return; // Exit loop.
                }
            }
            else if (sendCharacters && ch[0] != 0)
            {
                // After selecting F1, user pressed a key that is not exit. Character must be sent.
                if (!write(m_destinationMac, expectedProtocolType, ch, 2))
                {
                    std::cout << "Error while sending character, aborting..." << std::endl;
                    return;
                }
            }
        }
    }
}

std::string Communication::read()
{
    apacket_t receivedPacket{};
    receivedPacket = m_adapter->receiveFrame();

    if (receivedPacket.packet != nullptr)
    {
        // With this constructor, string treats data as raw memory and not a regular string, ignoring middle \x00
        // characters.
        std::string result = std::string((char *)receivedPacket.packet, receivedPacket.header.len);

        std::cout << "Received packet of length: " << result.size() << std::endl;
        return result;
    }

    return "";
}

void Communication::showMasterMenu()
{
    if (std::cin.rdbuf()->in_avail() > 0)
        std::cin.ignore(); // Ensure input buffer is clean without blocking calling thread.

    std::cout << "Initializating master loop" << std::endl;

    // Menu
    std::cout << "Mode selection: " << std::endl;
    std::cout << "[F1] - Interactive character sending" << std::endl;
    std::cout << "[F2] - Sending of a file" << std::endl;
    std::cout << "[ESC] - Exit" << std::endl;
}

void Communication::showSlaveMenu()
{
    if (std::cin.rdbuf()->in_avail() > 0)
        std::cin.ignore(); // Ensure input buffer is clean without blocking calling thread.

    std::cout << "Initializing slave loop" << std::endl;

    // Show menu.
    std::cout << "Chose working method: " << std::endl;
    std::cout << "[F1] Sent characters interactively. " << std::endl;
    std::cout << "[ESC] Exit." << std::endl;
}