#include "SlaveStation.h"

SlaveStation::SlaveStation(Adapter *adapter, char groupNumber) : GenericStation(adapter, groupNumber)
{
}

bool SlaveStation::checkInput()
{
    char protocolType[2] = {char(0x30 + m_groupNumber), static_cast<uint8_t>(FrameType::Common)};
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
                if (ch[0] == 'O' && !m_sendCharacters)
                {
                    // User hit FN key, check which FX was hit.
                    char functionKey = getch();
                    switch (functionKey)
                    {
                    case 'P': // F1
                    {
                        std::cout << "Slave is ready to send data to master" << std::endl;
                        m_sendCharacters = true;

                        break;
                    }
                    case 'R': // F3
                    {
                        // Wait and stop protocol.
                        WaitAndStopService serv(m_communication.get());
                        if (!serv.startSlaveStablishment())
                        {
                            std::cout << "Error on Stablishment phase!" << std::endl;
                            return false;
                        }

                        if (!serv.startSlaveTransfer())
                        {
                            std::cout << "Error on Transfer phase!" << std::endl;
                            return false;
                        }

                        if (!serv.closeSlaveConnection())
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

bool SlaveStation::loop()
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
            // Packet is from slave. Show it. Data gets a copy of the packet data.
            // .size() returns the size of the BUFFER not the string length. Use recommended
            // on the C++ standard to work with raw memory bytes.
            std::cout << "Received: " << received << std::endl;
        }
    }

    return true;
}

bool SlaveStation::tryDiscovery()
{
    // Initialized here for performace.
    char protocolType[2] = {char(0x30 + m_groupNumber), static_cast<uint8_t>(FrameType::RepplyToMaster)};

    m_communication->setExpectedFrameType(FrameType::AskForSlave);
    while (true)
    {
        std::string received = m_communication->read();

        if (received.empty())
            continue;

        std::cout << "Received frame from Master, repplying" << std::endl;
        // At this point we must notify the master that we are his slave.

        if (!m_communication->write(protocolType, nullptr, 0))
            return false;

        break;
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
    std::cout << "[F3] Wait and stop protocol " << std::endl;
    std::cout << "[ESC] Exit" << std::endl;
}