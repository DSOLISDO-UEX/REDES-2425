#ifndef WAIT_AND_STOP_FRAME_BUILDER_H
#define WAIT_AND_STOP_FRAME_BUILDER_H

#include "FrameBuilder.h"

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

  private:
    std::string m_data;
};

#endif // WAIT_AND_STOP_FRAME_BUILDER_H