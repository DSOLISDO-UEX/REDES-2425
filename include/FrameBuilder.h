#ifndef FRAME_BUILDER_H
#define FRAME_BUILDER_H

#include "linkLayer.h"

/**
 * Like Adapter, this class is a bridge between C (LinkLayer) and C++.
 */
class FrameBuilder
{
public:
    /** 
     * Builds a frame with the given information and payload.
     * @param srcMac MAC of the source device.
     * @param dstMac MAC of the destination device.
     * @param protocol Protocol for the frame.
     * @param payload Data that will be in the frame.
     * @return True if there wasn't any error.
     */
    bool buildFrame(unsigned char *srcMAC, unsigned char *dstMAC, unsigned char* protocol, unsigned char *payload);

    /** 
     * Builds the header of the frame.
     * @param srcMac MAC of the source device.
     * @param dstMac MAC of the destination device.
     * @param protocol Protocol for the frame.
     * @return True if there wasn't any error.
     */
    bool buildFrameHeader(unsigned char *srcMAC, unsigned char *dstMAC, unsigned char* protocol);

    /**
     * @return The size of the body (it uses strlen). TODO Change this, at the momment teachers have 
     * not mentioned how to get the size of a frame. 
     */
    size_t getBodySize();

    /**
     * @return The size of the header (it uses strlen). TODO Change this, at the momment teachers have 
     * not mentioned how to get the size of a header. 
     */
    size_t getHeaderSize();

    /**
     * @return Returns the body of the frame (unsigned char*).
     */
    unsigned char* getBody();

    /**
     * @return Returns the header of the frame (unsigned char*).
     */
    unsigned char* getHeader();

private:
    unsigned char* m_body; // Body of the frame.
    unsigned char* m_header; // Header of the frame.
};

#endif