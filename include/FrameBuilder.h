#ifndef FRAME_BUILDER_H
#define FRAME_BUILDER_H

#include "linkLayer.h"
#include <iostream>
#include <vector>
#include <fstream>

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
     * @return Returns the payload of the built frame. If not built returns nullptr.
     */
    char *getData() const;

    /**
     * @return Returns the body of the frame (unsigned char*).
     */
    char *getBody() const;

    /**
     * @return The size of the body of the frame.
     */
    size_t getBodySize() const;

    /**
     * @return The size of the data that's inside the body of the frame.
     */
    size_t getDataSize() const;

    /**
     * Opens the input file and tries to read its content into frames of 254 characters. If a frame
     * is less that 254, its real size will be used (for the last or first element, depending on the
     * input file).
     * @param srcMac
     * @param dstMac
     * @param protocol
     * @param filePath
     * @return A vector of FrameBuilders with each frame of the file. Delimiter is Trama XX- where
     * XX is a number.
     */
    std::vector<FrameBuilder> fromFile(char srcMAC[6], char dstMAC[6], char protocol[2], std::string filePath);

  private:
    size_t m_bodySize{0};  // Size of the body of the frame (header + dataSize)
    size_t m_dataSize{0};  // Size of the data sent with frame's body.
    char *m_body{nullptr}; // Body of the frame.
};

#endif