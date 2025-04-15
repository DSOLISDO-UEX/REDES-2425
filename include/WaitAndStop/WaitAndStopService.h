#ifndef WAIT_AND_STOP_SERVICE_H
#define WAIT_AND_STOP_SERVICE_H

#include "Communication.h"
#include "WaitAndStop/WaitAndStopFrameBuilder.h"
#include <map>

enum class ControlType : char
{
    ACK = 06,      // Confirm recieved - Accept the frame.
    ENQ = 05,      // Select slave station.
    EOT = 04,      // Free communication.
    NOFRAME = 100, // Return by serviceRead if there was nothing to read. Used to distinguish between erroneous frames
                   // and empty reads.
    NACK = 21,     // Ask master to resend a last frame because there was an error while receiving.
    STX = 02,      // Data frame.
    INVALID = 101  // Returned by serviceRead if there was any error on the frame.
};

inline std::map<ControlType, const char *> ControlType2Str = {
    {ControlType::ACK, "ACK"},         {ControlType::ENQ, "ENQ"},   {ControlType::EOT, "EOT"},
    {ControlType::NOFRAME, "NOFRAME"}, {ControlType::NACK, "NACK"}, {ControlType::STX, "STX"},
    {ControlType::INVALID, "INVALID"}};

class WaitAndStopService
{
  public:
    /**
     * Craetes the object linked to a Communication. It must has already been opened and devices must be paired.
     */
    WaitAndStopService(Communication *comm);

    /**
     * Closes the connection with slave's wait and stop service. Must be called by master.
     * @return True if there was no problem while closing the connection.
     */
    bool closeMasterConnection();

    /**
     * Closes the connection with master's wait and stop service. Called after master asked to finish.
     * Must be called by slave
     * @return True if there was no problem while closing the connection.
     */
    bool closeSlaveConnection();

    /**
     * Starts the stablishment process for Wait & Stop protocol. This must be called by master.
     * Will send a control frame with ENQ to the slave. Will block calling thread until finish.
     * @return Returns true if connection was sucessfull.
     */
    bool startMasterStablishment();

    /**
     * Starts the transfer phase. startMasterStablishment must have been called previously and must have returned true.
     * @return True if transfer was successfull.
     */
    bool startMasterTransfer();

    /**
     * Starts the stablishment process for Wait & Stop protocol. This must be called by slave. Will block calling thread
     * until an ENQ frame is received from the master.
     * @return Returns true if connection was sucessfull.
     */
    bool startSlaveStablishment();

    /**
     * Starts the transfer phase. startSlaveStablishment must have been called previously and must have returned true.
     * @return True if transfer was successfull.
     */
    bool startSlaveTransfer();

    /**
     * Given the data from file, calculates the value of the BCE
     * @param data String with the data prepared to be sent
     * @return char with the value of bce
     */
    char calculateBCE(const std::string &data);

  private:
    /**
     * Sends an ACK frame to the destination device.
     */
    bool sendAck();

    /**
     * Sends a NACK frame to the destination device.
     */
    bool sendNAck();

    /**
     * Sends the content to the pair and print frame's information. Important! frame's real type must be
     * WaitAndStopFrameBuilder!
     * @param frame
     * @return Returns true if frame was sent successfully.
     */
    bool serviceWrite(FrameBuilder &frame);

    /**
     * Waits for a repply of the type ACK or NACK. Blocks calling thread until one is received.
     * @return True if ACK and false if NACK.
     */
    bool waitForReply();

    /**
     * Performs a read operation on the Communication object and sanitizes it to control errors. Reads control value and
     * returns it casted to ControlType in type. Will also print frame's information if not empty.
     * @param type
     * @return Resulting DATA of the frame or empty if there was any error / there was nothing to read.
     */
    std::string serviceRead(ControlType &type);

  private:
    char m_currentFrameNumber;
    Communication *m_comm;
};

#endif