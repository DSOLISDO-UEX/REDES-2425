#include "MasterStation.h"

MasterStation::MasterStation(Adapter *adapter, char groupNumber) : GenericStation(adapter, groupNumber)
{
}

bool MasterStation::checkInput()
{
    char protocolType[2] = {char(0x30 + m_groupNumber), static_cast<uint8_t>(FrameType::Common)};
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
                if (ch[0] == 'O' && !m_sendCharacters)
                {
                    // User hit FN key, check which FX was hit.
                    char functionKey = getch();
                    switch (functionKey)
                    {
                    case 'P': // F1 ------------------------------------------------------------
                    {
                        m_sendCharacters = true;
                        std::cout << "Master is ready to send data to slave" << std::endl;
                        break;
                    }
                    case 'Q': // F2 ------------------------------------------------------------
                    {
                        std::cout << "Master is ready to send a file to slave" << std::endl;

                        // Sends a file splitted in frames (Trama-XX....)
                        std::string path = "envio.txt";
                        std::vector<FrameBuilder> frames =
                            FrameBuilder(m_communication->getSourceMac(), m_communication->getDestinationMac(),
                                         protocolType)
                                .fromFile(path);

                        for (FrameBuilder &frame : frames)
                        {
                            if (!m_communication->write(frame))
                            {
                                std::cout << "Error while sending file!" << std::endl;
                                return false;
                            }
                        }

                        showMenu();
                        break;
                    }
                    case 'R': // F3
                    {
                        // Wait and stop protocol.
                        WaitAndStopService serv(m_communication.get());
                        
                        if (!serv.startMasterStablishment())
                        {
                            std::cout << "Error on Stablishment phase!" << std::endl;
                            return false;
                        }

                        if (!serv.startMasterTransfer())
                        {
                            std::cout << "Error on Transfer phase!" << std::endl;
                            return false;
                        }

                        if (!serv.closeMasterConnection())
                        {
                            std::cout << "Error on Close phase!" << std::endl;
                            return false;
                        }

                        showMenu();
                        break;
                    }
                    }
                }
            }
            else if (m_sendCharacters)
            {
                // Exit pressed inside a mode, restore and show menu again.
                m_sendCharacters = false;
                showMenu();
            }
            else
            {
                // ESC was hit on the menu, exit the program.
                std::cout << "Exitting..." << std::endl;
                m_working = false;
                
                return true; // Exit loop.
            }
        }
        else if (m_sendCharacters && ch[0] != 0)
        {
            // After selecting F1, user pressed a key that is not exit. Character must be sent.
            if (!m_communication->write(protocolType, ch, 2))
            {
                std::cout << "Error while sending character, aborting..." << std::endl;
                return false;
            }
        }
    }

    return true;
}

bool MasterStation::loop()
{
    m_working = true;
    showMenu();

    m_communication->setExpectedFrameType(FrameType::Common);
    while (m_working)
    {
        // Check input for menu
        if (!checkInput())
        {
            std::cout << "Error!" << std::endl;
            return false;
        }

        std::string received = m_communication->read();
        if (!received.empty())
        {
            if (m_sendCharacters) // Received data with F1 enabled.
            {
                std::cout << "Received: " << received << std::endl;
            }
        }
    }

    return true;
}

bool MasterStation::tryDiscovery()
{
    // MAC used to send the packet to all the devices of the network.
    char destinationMac[6] = {char(0xFF), char(0xFF), char(0xFF), char(0xFF), char(0xFF), char(0xFF)};
    char protocolType[2] = {char(0x30 + m_groupNumber), static_cast<char>(FrameType::AskForSlave)};

    m_communication->setDestinationMac(destinationMac);
    if (!m_communication->write(protocolType, nullptr, 0))
        return false;

    m_communication->setExpectedFrameType(FrameType::RepplyToMaster);
    while (true)
    {
        std::string received = m_communication->read();

        if (received.empty())
            continue;

        std::cout << "Received packet from slave" << std::endl;
        break;
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
    std::cout << "[F1] Interactive character sending" << std::endl;
    std::cout << "[F2] Sending of a file" << std::endl;
    std::cout << "[F3] Wait and stop protocol " << std::endl;
    std::cout << "[ESC] Exit" << std::endl;
}