#include "WaitAndStop/WaitAndStopFrameBuilder.h"

WaitAndStopFrameBuilder::WaitAndStopFrameBuilder(char srcMac[6], char dstMac[6], char protocol[2])
    : FrameBuilder(srcMac, dstMac, protocol)
{
}

WaitAndStopFrameBuilder::WaitAndStopFrameBuilder(const WaitAndStopFrameBuilder &other)
    : m_data(other.m_data), FrameBuilder((char *)other.m_srcMac, (char *)other.m_dstMac, (char *)other.m_protocol)
{
    // Ensure DEEP copy of data.
    m_bodySize = other.m_bodySize; // Size of the body of the frame (header + dataSize)
    m_dataSize = other.m_dataSize; // Size of the data sent with frame's body.

    m_body = new char[m_bodySize];
    std::copy_n(other.m_body, m_bodySize, m_body);
}

bool WaitAndStopFrameBuilder::buildControl(char address, char control, char frameNumber)
{
    if (!m_data.empty())
        m_data = ""; // Clear if there was previous data. Ensures correct functionality when reusing this builder.

    m_data.resize(3); // Reserve to avoid reallocations.

    m_data[0] = address; // Append elements.
    m_data[1] = control;
    m_data[2] = frameNumber;

    return FrameBuilder::buildFrame(m_data);
}

bool WaitAndStopFrameBuilder::buildData(char address, char control, char *data, char frameNumber, char length, char bce)
{
    if (!m_data.empty())
        m_data = ""; // Clear if there was previous data. Ensures correct functionality when reusing this builder.

    m_data.reserve(size_t(5) + size_t(length));

    m_data += address; // Append elements.
    m_data += control;
    m_data += frameNumber;
    m_data += length;
    m_data.append(data, (unsigned char)length);
    m_data += bce;

    return FrameBuilder::buildFrame(m_data);
}

void WaitAndStopFrameBuilder::free()
{
    m_data = "";
    FrameBuilder::free();
}

void WaitAndStopFrameBuilder::hotChange(char ch)
{
    if (!m_data.empty())
    {
        m_data[4] = ch; // Modify data's content.

        if (m_body != nullptr)
        {
            char *internalData = FrameBuilder::getData(); // Gets internal frame buffer.
            internalData[4] = ch;                         // Modify first byte of the sequence.
        }
    }
}

const std::string &WaitAndStopFrameBuilder::getData()
{
    return m_data;
}

std::vector<WaitAndStopFrameBuilder> WaitAndStopFrameBuilder::fromFile(
    char address, char control, char initialFrameNumber, std::function<char(char *data, size_t dataSize)> bceCalculator,
    std::string filePath)
{
    std::ifstream input(filePath);
    if (!input.is_open())
    {
        std::cout << "Could not build frames from file because there isn't file!" << std::endl;
        return {};
    }

    std::vector<WaitAndStopFrameBuilder> result;
    while (!input.eof())
    {
        char data[254] = {0};

        size_t readSize = input.read(data, 254).gcount(); // Read 254 characters into the string(if possible).
        char bce = bceCalculator(data, readSize);

        WaitAndStopFrameBuilder frame(m_srcMac, m_dstMac, m_protocol);
        frame.buildData(address, control, data, initialFrameNumber, readSize, bce); // build this frame
        result.push_back(frame);                                                    // Append built frame to list.

        if (initialFrameNumber == '0')
            initialFrameNumber = '1'; // Affects parameter in stack, safe to use.
        else
            initialFrameNumber = '0';
    }

    return result;
}