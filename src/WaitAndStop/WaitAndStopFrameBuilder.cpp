#include "WaitAndStop/WaitAndStopFrameBuilder.h"

WaitAndStopFrameBuilder::WaitAndStopFrameBuilder(char srcMac[6], char dstMac[6], char protocol[2])
    : FrameBuilder(srcMac, dstMac, protocol)
{
}

bool WaitAndStopFrameBuilder::buildControl(char address, char control, char frameNumber)
{
    m_data.reserve(3); // Reserve to avoid reallocations.

    m_data += address; // Append elements.
    m_data += control;
    m_data += frameNumber;


    return FrameBuilder::buildFrame((char*)m_data.data(), m_data.size());
}

bool WaitAndStopFrameBuilder::buildData(char address, char control, char *data, char frameNumber, char length, char bce)
{
    m_data.reserve(5 + length); // Reserve to avoid reallocations.
    
    m_data += address; // Append elements.
    m_data += control;
    m_data += frameNumber;
    m_data += length;
    m_data += data;
    m_data += bce;

    return FrameBuilder::buildFrame((char*)m_data.data(), m_data.size());
}