#include "WaitAndStop/WaitAndStopService.h"

WaitAndStopService::WaitAndStopService(Communication *comm) : m_currentFrameNumber('0'), m_comm(comm), m_errorsLeft(0)
{
}

bool WaitAndStopService::closeMasterConnection()
{
    m_currentFrameNumber = '0';
    WaitAndStopFrameBuilder builder(m_comm->getSourceMac(), m_comm->getDestinationMac(), m_comm->getCommonProtocol());

    if (!builder.buildControl('R', static_cast<char>(ControlType::EOT), m_currentFrameNumber))
    {
        std::cout << "Could not build EOT Control Frame" << std::endl;
        return false;
    }

    if (!serviceWrite(builder))
    {
        std::cout << "Could not send EOT Control Frame" << std::endl;
        return false;
    }

    std::cout << "Waiting for EOT's ACK repply" << std::endl;

    if (waitForReply())
    {
        std::cout << "Ready to finish connection" << std::endl;
        return true;
    }

    return false;
}

bool WaitAndStopService::closeSlaveConnection()
{
    m_currentFrameNumber = '0';
    sendAck(); // EOT was received in the transfer phase, slave only needs to reply.

    std::cout << "Finished connection with Master" << std::endl;
    return true;
}

bool WaitAndStopService::startMasterStablishment()
{
    m_currentFrameNumber = '0';
    // Send the ENQ frame and wait for a successfull reply.
    WaitAndStopFrameBuilder builder(m_comm->getSourceMac(), m_comm->getDestinationMac(), m_comm->getCommonProtocol());
    if (!builder.buildControl('R', static_cast<char>(ControlType::ENQ), m_currentFrameNumber))
    {
        std::cout << "Could not build ENQ frame" << std::endl;
        return false;
    }

    if (!serviceWrite(builder))
    {
        std::cout << "Could not send ENQ control frame" << std::endl;
        return false;
    }

    std::cout << "Waiting for ENQ's ACK frame..." << std::endl;

    if (waitForReply())
    {
        std::cout << "Ready to transfer content!" << std::endl;
        return true;
    }

    return false;
}

bool WaitAndStopService::startMasterTransfer()
{
    m_currentFrameNumber = '0';

    WaitAndStopFrameBuilder builder(m_comm->getSourceMac(), m_comm->getDestinationMac(), m_comm->getCommonProtocol());
    std::vector<WaitAndStopFrameBuilder> builtFrames = builder.fromFile(
        'R', static_cast<char>(ControlType::STX), m_currentFrameNumber,
        [this](char *data, size_t dataSize) -> char { // C++11 lambda functions.
            return calculateBCE(data, dataSize);
        },
        "EProtoc.txt");

    for (auto &frame : builtFrames)
    {
        if (kbhit())
        {
            char ch = char(getch());
            if (ch == 27 && kbhit())
            {
                // User pressed FN key. Check if it's F4.
                ch = char(getch());
                if (ch == 'O' && kbhit())
                {
                    ch = char(getch());
                    if (ch == 'S')
                        m_errorsLeft++; // Introduce an error.
                }
            }
        }

        // This loop will wait for a reply. If reply is NACK, frame will be resent. If it's ACK, next
        // piece of data will be sent. Other replies will be ignored.
        bool introducedError = false;
        do
        {
            // Ensure that if a frame is replied with NACK, it can be re-sent without it have being freed.
            WaitAndStopFrameBuilder copy = WaitAndStopFrameBuilder(frame);

            if (m_errorsLeft > 0 && !introducedError)
            {
                std::cout << "Introducing error" << std::endl;
                // Introduce only 1 error in a frame 1 time. Next time frame is sent, it will be sent properly.
                // If 'ç' is used, a weird conversion error happens. Decided to use its numerical value.
                copy.hotChange(char(135));

                introducedError = true;
                m_errorsLeft--;
            }

            if (!serviceWrite(copy)) // Remember write also frees the frame, this avoids potential memory leaks.
            {
                std::cout << "Could not send data frame" << std::endl;
                return false;
            }

        } while (!waitForReply());

        // Frame has been replied with an ACK, it can now be freed.
        frame.free();
    }

    std::cout << "Finished transfering!" << std::endl;
    return true;
}

bool WaitAndStopService::startSlaveStablishment()
{
    m_currentFrameNumber = '0';
    std::cout << "Waiting for Master's ENQ frame" << std::endl;

    while (true)
    {
        ControlType type = ControlType::INVALID;
        serviceRead(type);

        if (type == ControlType::ENQ)
        {
            std::cout << "Received Master's ENQ, replying with ACK" << std::endl;
            sendAck();

            return true;
        }
    }

    return false;
}

bool WaitAndStopService::startSlaveTransfer()
{
    m_currentFrameNumber = '0';

    std::ofstream out("RProtoc.txt",
                      std::ios::out | std::ios::trunc); // Open file, create if it doesn't exist and delete if exists.

    if (!out.is_open())
    {
        std::cout << "Could not create out file!" << std::endl;
        return false;
    }

    ControlType type = ControlType::NOFRAME;
    while (type == ControlType::NOFRAME)
    {
        std::string data = serviceRead(type);

        if (type == ControlType::STX)
        {
            // Frame is correct, print its content.
            out << data;
            sendAck();

            // Update frame number.
            if (m_currentFrameNumber == '0')
                m_currentFrameNumber = '1';
            else
                m_currentFrameNumber = '0';
        }
        else if (type == ControlType::INVALID)
        {
            // There was an error with the frame, ask master to resend it.
            std::cout << "Asking master to resend frame" << std::endl;
            sendNAck();
        }
        else if (type == ControlType::EOT)
        {
            // Master asked for the finalization of the connection.
            std::cout << "Master asked to finish transfering" << std::endl;

            out.close(); // Ensure file is closed.
            return true;
        }

        type = ControlType::NOFRAME; // Reset so this while doesn't end.
    }

    return false;
}

char WaitAndStopService::calculateBCE(const std::string &data)
{
    return calculateBCE((char *)data.data(), data.size());
}

char WaitAndStopService::calculateBCE(char *data, size_t dataSize)
{
    char bce = data[0];

    for (size_t i = 1; i < dataSize; i++)
        bce ^= data[i];

    if (bce == 0 || bce == 255)
        bce = 1;

    return bce;
}

bool WaitAndStopService::sendAck()
{
    WaitAndStopFrameBuilder builder(m_comm->getSourceMac(), m_comm->getDestinationMac(), m_comm->getCommonProtocol());

    if (!builder.buildControl('R', static_cast<char>(ControlType::ACK), m_currentFrameNumber))
    {
        std::cout << "Could not build ACK Control Frame" << std::endl;
        return false;
    }

    return serviceWrite(builder);
}

bool WaitAndStopService::sendNAck()
{
    WaitAndStopFrameBuilder builder(m_comm->getSourceMac(), m_comm->getDestinationMac(), m_comm->getCommonProtocol());

    if (!builder.buildControl('R', static_cast<char>(ControlType::NACK), m_currentFrameNumber))
    {
        std::cout << "Could not build NACK Control Frame" << std::endl;
        return false;
    }

    return serviceWrite(builder);
}

bool WaitAndStopService::serviceWrite(FrameBuilder &frame)
{
    // If frame's real type is not WaitAndStopFrameBuilder undefined behaviour is assured!!!!
    WaitAndStopFrameBuilder &casted = *reinterpret_cast<WaitAndStopFrameBuilder *>(&frame);
    const std::string &frameData = casted.getData();

    ControlType control = static_cast<ControlType>(frameData[1]);

    std::cout << "E\t" << frameData[0] << "\t" << ControlType2Str[control] << "\t" << frameData[2] << "\t"
              << std::flush;

    switch (control)
    {
    case ControlType::STX: {
        unsigned char dataSize = frameData[3];
        char bce =
            *((unsigned char *)frameData.data() + 4 + dataSize); // If shown as signed weird stuff happens.

        std::cout << int(bce) << "\t" << std::flush; // Int conversion is needed so it doesn't print characters like [ | ...
    }
    }
    std::cout << std::endl;

    bool result = m_comm->write(casted);
    casted.free(); // Free sub class information too.

    return result;
}

bool WaitAndStopService::waitForReply()
{
    ControlType type = ControlType::NOFRAME;
    while (type == ControlType::NOFRAME)
    {
        serviceRead(type);

        if (type == ControlType::ACK)
        {
            std::cout << "Received ACK" << std::endl;
            return true;
        }
        else if (type == ControlType::NACK)
        {
            std::cout << "Received NACK" << std::endl;
            return false;
        }

        type = ControlType::NOFRAME; // Ignore other reply types.
    }

    return false;
}

std::string WaitAndStopService::serviceRead(ControlType &type)
{
    type = ControlType::INVALID;
    std::string received = m_comm->read();

    if (received.empty())
    {
        type = ControlType::NOFRAME;
        return "";
    }

    char address = received[0];
    char control = received[1];
    char frameNumber = received[2];

    std::cout << "R\t" << address << "\t" << ControlType2Str[static_cast<ControlType>(control)] << "\t" << frameNumber
              << "\t" << std::flush;

    switch (control)
    {
    case static_cast<char>(ControlType::ACK):
    case static_cast<char>(ControlType::ENQ):
    case static_cast<char>(ControlType::EOT):
    case static_cast<char>(ControlType::NACK): {
        // Control frame. Doesn't have BCE nor data. Return empty.
        std::cout << std::endl;

        type = static_cast<ControlType>(control); // Only cast if it's a known type.
        return "";
    }

    case static_cast<char>(ControlType::STX): {
        // Data frame. Extract data, bce and check errors.
        // Only SLAVE can receive a frame of type STX.

        unsigned char dataSize = received[3];
        char *dataStart = (char *)received.data() + 4;
        unsigned char bce = *((char *)received.data() + 4 + dataSize);

        std::string data = std::string(dataStart, dataSize);
        unsigned char calculatedBce = calculateBCE(data); // Proper console format, if signed weird stuff happens.

        std::cout << int(bce) << "\t" << int(calculatedBce) << "\t" << std::endl;

        if (bce != calculatedBce)
        {
            std::cout << "Error on frame BCE" << std::endl;
            return "";
        }

        type = static_cast<ControlType>(control); // Only cast if it's a known type and frame content is valid.
        return data;
    }

    default: {
        type = ControlType::INVALID;
        break;
    }
    }

    std::cout << "Unknown frame control type" << std::endl;
    return "";
}
