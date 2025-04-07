#include "FrameBuilder.h"

FrameBuilder::FrameBuilder(char srcMac[6], char dstMac[6], char protocol[2])
{
    std::copy_n(srcMac, 6, m_srcMac);
    std::copy_n(dstMac, 6, m_dstMac);
    std::copy_n(protocol, 2, m_protocol);
}

FrameBuilder::~FrameBuilder()
{
}

bool FrameBuilder::buildFrame(char *payload, size_t payloadSize)
{
    free(); // If multiple frames are being built, this is required not to have a memory leak.

    m_body = (char *)BuildFrame((unsigned char *)m_srcMac, (unsigned char *)m_dstMac, (unsigned char *)m_protocol,
                                (unsigned char *)payload);
    m_dataSize = payloadSize;
    m_bodySize = m_dataSize + 6 + 6 + 2;

    return m_body != nullptr;
}

bool FrameBuilder::buildEmptyFrame()
{
    free(); // If multiple frames are being built, this is required not to have a memory leak.

    m_body = (char *)BuildHeader((unsigned char *)m_srcMac, (unsigned char *)m_dstMac, (unsigned char *)m_protocol);
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

void FrameBuilder::free()
{
    if (m_body != nullptr && m_dataSize != 0)
    {
        m_bodySize = 0;
        m_dataSize = 0;
        delete m_body;
    }
}

std::vector<FrameBuilder> FrameBuilder::fromFile(std::string filePath)
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
        char data[254] = {0};

        size_t readSize = input.read(data, 254).gcount(); // Read 254 characters into the string(if possible).

        FrameBuilder frame(m_srcMac, m_dstMac, m_protocol);
        frame.buildFrame(data, readSize); // build this frame

        result.push_back(frame); // Append built frame to list.
    }

    return result;
}