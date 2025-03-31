#include "../include/MasterStation.h"

MasterStation::MasterStation(Adapter *adapter, char groupNumber) : GenericStation(adapter, groupNumber)
{
}

bool MasterStation::loop()
{
    showMenu();

    bool sendCharacters = false;
    char expectedProtocolType[2] = {char(0x30 + m_groupNumber), static_cast<uint8_t>(PacketType::Common)};
    std::string received;

    while (true)
    {
        received = m_communication->read();

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
                            std::vector<FrameBuilder> frames = FrameBuilder().fromFile(
                                m_communication->getSourceMac(), m_communication->getDestinationMac(),
                                expectedProtocolType, path);

                            for (FrameBuilder &frame : frames)
                            {
                                if (!m_communication->write(expectedProtocolType, frame))
                                {
                                    std::cout << "Error while sending file!" << std::endl;
                                    return false;
                                }
                            }

                            showMenu();
                        }
                        }
                    }
                }
                else if (sendCharacters)
                {
                    // Exit pressed inside a mode, restore and show menu again.
                    sendCharacters = false;
                    showMenu();
                }
                else
                {
                    // ESC was hit on the menu, exit the program.
                    std::cout << "Exitting..." << std::endl;
                    return false; // Exit loop.
                }
            }
            else if (sendCharacters && ch[0] != 0)
            {
                // After selecting F1, user pressed a key that is not exit. Character must be sent.
                if (!m_communication->write(expectedProtocolType, ch, 2))
                {
                    std::cout << "Error while sending character, aborting..." << std::endl;
                    return false;
                }
            }
        }
    }

    return true;
}

bool MasterStation::tryDiscovery()
{
    // MAC used to send the packet to all the devices of the network.
    char destinationMac[6] = {char(0xFF), char(0xFF), char(0xFF), char(0xFF), char(0xFF), char(0xFF)};
    char protocolType[2] = {char(0x30 + m_groupNumber), static_cast<char>(PacketType::AskForSlave)};

    m_communication->setDestinationMac(destinationMac);

    if (!m_communication->write(protocolType, nullptr, 0))
        return false;

    std::string received;
    while (true)
    {
        received = m_communication->read();

        if (received.empty())
            continue;

        std::cout << "Received packet when discovering, checking if it's from a slave" << std::endl;

        // Pointer to the start of the received packet protocol type.
        char *receivedTypeStart = (char *)(received.data() + 12);

        if (receivedTypeStart[0] == protocolType[0] &&
            receivedTypeStart[1] == static_cast<uint8_t>(PacketType::RepplyToMaster))
        {
            std::cout << "Received packet is from a slave! Saving source MAC" << std::endl;
            m_communication->setDestinationMac((char *)(received.data() + 6));

            break;
        }

        std::cout << "Received packet is not from a slave" << std::endl;
    }

    return true;
}

void MasterStation::showMenu()
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