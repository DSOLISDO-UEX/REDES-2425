#include "../include/FrameBuilder.h"

bool FrameBuilder::buildFrame(char srcMAC[6], char dstMAC[6], char protocol[2], char *payload)
{
    m_body = (char *)BuildFrame((unsigned char *)srcMAC, (unsigned char *)dstMAC, (unsigned char *)protocol,
                                (unsigned char *)payload);
    m_dataSize = strlen((char *)payload);
    m_bodySize = m_dataSize + 6 + 6 + 2;

    return m_body != nullptr;
}

bool FrameBuilder::buildEmptyFrame(char srcMAC[6], char dstMAC[6], char protocol[2])
{
    m_body = (char *)BuildHeader((unsigned char *)srcMAC, (unsigned char *)dstMAC, (unsigned char *)protocol);
    m_dataSize = 0;
    m_bodySize = m_dataSize + 6 + 6 + 2;

    return m_body != nullptr;
}

char *FrameBuilder::getData() const
{
    if (!m_body)
        return nullptr;

    return m_body + 14;
}

char *FrameBuilder::getBody() const
{
    return m_body;
}

size_t FrameBuilder::getBodySize() const
{
    return m_body == nullptr ? 0 : (m_bodySize);
}

size_t FrameBuilder::getDataSize() const
{
    return m_dataSize;
}

std::vector<FrameBuilder> FrameBuilder::fromFile(char srcMAC[6], char dstMAC[6], char protocol[2], std::string filePath)
{
    std::ifstream input(filePath);

    if (!input.is_open())
    {
        std::cout << "Could not build frames from file because there isn't file!" << std::endl;
        return {};
    }

    std::string line;
    std::vector<FrameBuilder> result;

    while (!input.eof())
    {
        line.clear();
        line = ""; // Ensure buffer capacity is resetted between each iteration not to have a line of 510bytes.

        line.reserve(254);                    // Maximum of 254 characters.
        input.read((char *)line.data(), 254); // Read 254 characters into the string.

        FrameBuilder frame;
        frame.buildFrame(srcMAC, dstMAC, protocol, (char *)line.data()); // build this frame

        result.push_back(frame); // Append built frame to list.
    }

    return result;
}