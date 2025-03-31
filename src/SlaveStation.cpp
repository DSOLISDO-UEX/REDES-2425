#include "../include/SlaveStation.h"

SlaveStation::SlaveStation(Adapter *adapter, char groupNumber) : GenericStation(adapter, groupNumber)
{
}

bool SlaveStation::loop()
{
    showMenu();

    // Initialized here for performace.
    bool sendCharacters = false;
    char expectedProtocolType[2] = {char(0x30 + m_groupNumber), static_cast<uint8_t>(PacketType::Common)};
    std::string received;

    while (true)
    {
        received = m_communication->read();
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

bool SlaveStation::tryDiscovery()
{
    // Initialized here for performace.
    char protocolType[2] = {char(0x30 + m_groupNumber), static_cast<uint8_t>(PacketType::RepplyToMaster)};
    std::string received;

    while (true)
    {
        received = m_communication->read();

        if (received.empty())
            continue;

        std::cout << "Received packet when discovering, checking if it's from a master" << std::endl;

        // Pointer to the start of the received packet protocol type.
        char *receivedTypeStart = (char *)(received.data() + 12);

        if (receivedTypeStart[0] == protocolType[0] &&
            receivedTypeStart[1] == static_cast<uint8_t>(PacketType::AskForSlave))
        {
            std::cout << "Received packet is from a Master! Saving source MAC" << std::endl;
            m_communication->setDestinationMac((char *)(received.data() + 6));

            std::cout << "Repplying to Master" << std::endl;
            // At this point we must notify the master that we are his slave.

            if (!m_communication->write(protocolType, nullptr, 0))
                return false;

            break;
        }
    }

    return true;
}

void SlaveStation::showMenu()
{
    if (std::cin.rdbuf()->in_avail() > 0)
        std::cin.ignore(); // Ensure input buffer is clean without blocking calling thread.

    std::cout << "Initializing slave loop" << std::endl;

    // Show menu.
    std::cout << "Chose working method: " << std::endl;
    std::cout << "[F1] Sent characters interactively. " << std::endl;
    std::cout << "[ESC] Exit." << std::endl;
}