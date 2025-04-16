#ifndef WAIT_AND_STOP_FRAME_BUILDER_H
#define WAIT_AND_STOP_FRAME_BUILDER_H

#include "FrameBuilder.h"

/**
 * This class represents a child of FrameBuilder, it offers functionality to build complex Frames for Wait&Stop
 * protocol.
 */
class WaitAndStopFrameBuilder : public FrameBuilder
{
  public:
    /**
     * Creates the object with the given information.
     * @param srcMac
     * @param dstMac
     * @param protocol
     */
    WaitAndStopFrameBuilder(char srcMac[6], char dstMac[6], char protocol[2]);

    /**
     * Copies other into this. Makes a DEEP copy.
     * @param other
     */
    WaitAndStopFrameBuilder(const WaitAndStopFrameBuilder &other);

    /**
     * Builds a control frame. Address will be 'R' and control one of the ControlTypes enum. Frame number will flip from
     * 0-1 and 1-0.
     * @param address
     * @param control
     * @param frameNumber
     * @return Returns true if could build the frame successfully.
     */
    bool buildControl(char address, char control, char frameNumber);

    /**
     * Builds a data frame. Address will be 'R' and control one of the ControlTypes enum. Frame number will flip from
     * 0-1 and 1-0. And BCE represents a "hash" used to control erros in the transmision.
     * @param address
     * @param control
     * @param data
     * @param frameNumber
     * @param length
     * @param bce
     * @return Returns true if could build the frame successfully.
     */
    bool buildData(char address, char control, char *data, char frameNumber, char length, char bce);

    /**
     * Frees data and internal frame, if not already freed.
     */
    void free() override;

    /**
     * Changes the first character of the data to the one given. If frame was not built, IT WON'T DO ANYTHING. It will
     * also affect built's frame buffer. Used to introduce errors on Wait&Stop's protocol.
     * @param ch
     */
    void hotChange(char ch);

    /**
     * @return Returns built data of this frame.
     */
    const std::string &getData();

    /**
     * Opens the input file and tries to read its content into frames of 254 characters. If a frame
     * is less that 254, its real size will be used (for the last or first element, depending on the
     * input file).
     * @param address
     * @param control
     * @param initialFrameNumber
     * @param bceCalculator A lambda expression that calculates the BCE for a frame. Allows rapid BCE-calculation
     * changes.
     * @param filePath
     * @return A vector of FrameBUilder with each frame of the file. Delimiter is Trama XX- where
     * XX is a number. RESULTING FRAMES ARE OF DATA TYPE!
     */
    std::vector<WaitAndStopFrameBuilder> fromFile(char address, char control, char initialFrameNumber,
                                                  std::function<char(char *data, size_t dataSize)> bceCalculator,
                                                  std::string filePath);

  private:
    std::string m_data;
};

#endif // WAIT_AND_STOP_FRAME_BUILDER_H