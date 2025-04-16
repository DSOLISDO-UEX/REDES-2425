#include "Interactive/InteractiveService.h"

InteractiveService::InteractiveService(Communication *comm) : m_comm(comm)
{
}

bool InteractiveService::startInteractiveMaster()
{
    while(true)
    {
        serviceRead(); // Reads, if there's anything to read.

        if (kbhit())
        {
            char ch = char(getch());
            std::cout << std::endl; // Ensure the character is on its own line. Clearer reading of the behaviour.

            if (ch == 27)
            {
                if (!kbhit())
                {
                    break; // Exit pressed inside this menu.
                }
            }
            else 
            {
                // Send character.
                if (!m_comm->write(m_comm->getCommonProtocol(), &ch, 1))
                {
                    std::cout << "Error while sending interactive character" << std::endl;
                    return false;
                }
            }
        }
    }

    return true;
}

bool InteractiveService::startInteractiveSlave()
{
    while(true)
    {
        serviceRead(); // This needs to be called here because the loop inside SlaveStation is NOT being executed.

        if (kbhit())
        {
            char ch = char(getch());
            std::cout << std::endl; // Ensure the character is on its own line. Clearer reading of the behaviour.

            if (ch == 27)
            {
                if (!kbhit())
                {
                    break; // Exit pressed inside this menu.
                }
            }
            else 
            {
                // Send character.
                if (!m_comm->write(m_comm->getCommonProtocol(), &ch, 1))
                {
                    std::cout << "Error while sending interactive character" << std::endl;
                    return false;
                }
            }
        }
    }

    return true;
}

void InteractiveService::serviceRead()
{
    std::string received = m_comm->read();
    if (!received.empty())
    {
        std::cout << "Received: " << received << std::endl;
    }
}

void InteractiveService::readAndDiscard()
{
    // Read if there's anything to read but don't do anything with it.
    if (!m_comm->read().empty()) 
    {
        std::cout << "Not on interactive mode, discarding..." << std::endl;
    }
}