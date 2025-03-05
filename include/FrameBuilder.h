#ifndef FRAME_BUILDER_H
#define FRAME_BUILDER_H

#include "linkLayer.h"
#include <iostream>

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
    bool buildFrame(char srcMAC[6], char dstMAC[6], char protocol[2], char *payload);

    /**
     * Builds the header of the frame.
     * @param srcMac MAC of the source device.
     * @param dstMac MAC of the destination device.
     * @param protocol Protocol for the frame.
     * @return True if there wasn't any error.
     */
    bool buildEmptyFrame(char srcMAC[6], char dstMAC[6], char protocol[2]);

    /**
     * @return The size of the body of the frame.
     */
    size_t getBodySize();

    /**
     * @return The size of the data that's inside the body of the frame.
     */
    size_t getDataSize();

    /**
     * @return Returns the body of the frame (unsigned char*).
     */
    char *getBody();

  private:
    size_t m_bodySize{0};  // Size of the body of the frame (header + dataSize)
    size_t m_dataSize{0};  // Size of the data sent with frame's body.
    char *m_body{nullptr}; // Body of the frame.
};

#endif