#include "WaitAndStop/WaitAndStopFrameBuilder.h"

WaitAndStopFrameBuilder::WaitAndStopFrameBuilder(char srcMac[6], char dstMac[6], char protocol[2])
    : FrameBuilder(srcMac, dstMac, protocol)
{
}

bool WaitAndStopFrameBuilder::buildControl(char address, char control, char frameNumber)
{
    if (!m_data.empty())
        m_data = ""; // Clear if there was previous data. Ensures correct functionality when reusing this builder.

    m_data.reserve(3); // Reserve to avoid reallocations.

    m_data += address; // Append elements.
    m_data += control;
    m_data += frameNumber;

    return FrameBuilder::buildFrame((char *)m_data.data(), m_data.size());
}

bool WaitAndStopFrameBuilder::buildData(char address, char control, char *data, char frameNumber, char length, char bce)
{
    if (!m_data.empty())
        m_data = ""; // Clear if there was previous data. Ensures correct functionality when reusing this builder.

    m_data.reserve(5 + length); // Reserve to avoid reallocations.

    m_data += address; // Append elements.
    m_data += control;
    m_data += frameNumber;
    m_data += length;
    m_data += data;
    m_data += bce;

    return FrameBuilder::buildFrame((char *)m_data.data(), m_data.size());
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
        result.push_back(frame); // Append built frame to list.

        if (initialFrameNumber == '0')
            initialFrameNumber = '1'; // Affects parameter in stack, safe to use.
        else
            initialFrameNumber = '0';
    }

    return result;
}