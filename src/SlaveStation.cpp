#include "SlaveStation.h"

SlaveStation::SlaveStation(Adapter *adapter, char groupNumber) : GenericStation(adapter, groupNumber)
{
}

bool SlaveStation::checkInput()
{
    if (kbhit())
    {
        // User hit a key.
        char ch = char(getch());
        std::cout << std::endl; // Make sure input and output don't overlap on console (only for better reading...).

        // FN input: ESC+'O'+FN_KEY_ID ('P', 'Q', 'R', 'S')

        if (ch == 27)
        {
            if (kbhit())
            {
                ch = getch();
                if (ch == 'O')
                {
                    // User hit FN key, check which FX was hit.
                    char functionKey = getch();
                    switch (functionKey)
                    {
                    case 'P': // F1
                    {
                        std::cout << "Slave is ready to send data to master" << std::endl;

                        if (!m_interactiveServ.startInteractiveSlave())
                        {
                            std::cout << "Error on Interactive sending" << std::endl;
                            return false;
                        }

                        showMenu();
                        break;
                    }
                    case 'R': // F3
                    {
                        // Wait and stop protocol.
                        WaitAndStopService serv(m_communication);
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
            else
            {
                // ESC was hit on the menu, exit the program.
                std::cout << "Exitting..." << std::endl;
                m_working = false;

                return true; // Exit loop.
            }
        }
    }

    return true;
}

bool SlaveStation::loop()
{
    showMenu();
    m_working = true;

    m_communication->setExpectedFrameType(FrameType::Common);
    while (m_working)
    {
        // Check input for menu
        if (!checkInput())
        {
            std::cout << "Error!" << std::endl;
            return false;
        }

        m_interactiveServ.serviceRead(); // Slave is ALWAYS listening for content.
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

    // Show menu.
    std::cout << "Mode selection: " << std::endl;
    std::cout << "[F1] Sent characters interactively. " << std::endl;
    std::cout << "[F3] Wait and stop protocol " << std::endl;
    std::cout << "[ESC] Exit" << std::endl;
}