#ifndef FRAME_BUILDER_H
#define FRAME_BUILDER_H

#include "linkLayer.h"
#include <fstream>
#include <iostream>
#include <vector>
#include <algorithm>

/**
 * Like Adapter, this class is a bridge between C (LinkLayer) and C++ and allows quick-frame-building.
 */
class FrameBuilder
{
  public:
    /** Creates the object with the information given. Can be reused to build multiple frames.
     * @param srcMac
     * @param dstMac
     * @param protocol
     */
    FrameBuilder(char srcMac[6], char dstMac[6], char protocol[2]);

    /**
     * Won't free inside buffer, caller must ensure it is freed.
     */
    ~FrameBuilder();

    /**
     * Builds a frame with the given payload. 
     * WARNING: If multiple frames are built, the last one will be released.  
     * @param payload Data that will be in the frame.
     * @return True if there wasn't any error.
     */
    bool buildFrame(const std::string &payload);

    /**
     * Builds the header of the frame.
     * WARNING: If multiple frames are built, the last one will be released.
     * @return True if there wasn't any error.
     */
    bool buildEmptyFrame();

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
     * Frees the built frame if it's not already free.
     */
    virtual void free();

    /**
     * Opens the input file and tries to read its content into frames of 254 characters. If a frame
     * is less that 254, its real size will be used (for the last or first element, depending on the
     * input file).
     * @param filePath
     * @return A vector of FrameBuilders with each frame of the file. Delimiter is Trama XX- where
     * XX is a number.
     */
    std::vector<FrameBuilder> fromFile(std::string filePath);

  protected:
    char m_srcMac[6]{};
    char m_dstMac[6]{};
    char m_protocol[2]{};
    char *m_body{nullptr}; // Body of the frame.
    size_t m_bodySize{0};  // Size of the body of the frame (header + dataSize)
    size_t m_dataSize{0};  // Size of the data sent with frame's body.
};

#endif