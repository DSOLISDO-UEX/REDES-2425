#include "../include/FrameBuilder.h"

bool FrameBuilder::buildFrame(unsigned char *srcMAC, unsigned char *dstMAC, unsigned char* protocol, unsigned char *payload)
{
    m_body = BuildFrame(srcMAC, dstMAC, protocol, payload);
    m_payloadSize = strlen((char*)payload);

    return m_body != nullptr;
}

bool FrameBuilder::buildFrameHeader(unsigned char *srcMAC, unsigned char *dstMAC, unsigned char* protocol)
{
    m_header = BuildHeader(srcMAC, dstMAC, protocol);
    return m_header == nullptr;
}

size_t FrameBuilder::getBodySize()
{
    return m_body == nullptr ? 0 : (m_payloadSize);
}

size_t FrameBuilder::getHeaderSize()
{
    return m_header == nullptr ? 0 : (6 + 6 + 2);
}

unsigned char* FrameBuilder::getBody()
{
    return m_body;
}

unsigned char* FrameBuilder::getHeader()
{
    return m_header;
}