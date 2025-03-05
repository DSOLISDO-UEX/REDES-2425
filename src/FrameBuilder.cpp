#include "../include/FrameBuilder.h"

bool FrameBuilder::buildFrame(char srcMAC[6], char dstMAC[6], char protocol[2], char *payload)
{
    m_body = (char *)BuildFrame((unsigned char *)srcMAC, (unsigned char *)dstMAC, (unsigned char *)protocol,
                                (unsigned char *)payload);
    m_dataSize = strlen((char *)payload);
    m_bodySize = m_dataSize + 6 + 6 + 2;

    return m_body != nullptr;
}

bool FrameBuilder::buildEmptyFrame(char *srcMAC, char *dstMAC, char *protocol)
{
    m_body = (char *)BuildHeader((unsigned char *)srcMAC, (unsigned char *)dstMAC, (unsigned char *)protocol);
    m_dataSize = 0;
    m_bodySize = m_dataSize + 6 + 6 + 2;
    
    return m_body != nullptr;
}

size_t FrameBuilder::getBodySize()
{
    return m_body == nullptr ? 0 : (m_bodySize);
}

size_t FrameBuilder::getDataSize()
{
    return m_dataSize;
}

char *FrameBuilder::getBody()
{
    return m_body;
}
