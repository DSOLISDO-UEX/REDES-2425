#ifndef WAIT_AND_STOP_SERVICE_H
#define WAIT_AND_STOP_SERVICE_H

#include "Communication.h"
#include "WaitAndStop/WaitAndStopFrameBuilder.h"
#include <map>

/**
 * A simple enumeration that contains types that can be linked to the Control field on a generic Wait&Stop Frame.
 */
enum class ControlType : unsigned char
{
    ACK = 0x06,      // Frame successfully received. Sent by dest to source.
    ENQ = 0x05,      // Select slave station, sent by Master to start communication with Wait&Stop protocol.
    EOT = 0x04,      // Free communication. Sent by Master.
    NOFRAME = 0xBE, // Return by serviceRead if there was nothing to read. Used to distinguish between erroneous frames
                   // and empty reads.
    NACK = 0x21,     // Ask master to resend a last frame because there was an error while receiving.
    STX = 0x02,      // Data frame.
    INVALID = 0xBB  // Returned by serviceRead if there was any error on the frame (BCE mismatch).
};

/**
 * This variable maps each ControlType to a string.
 */
inline std::map<ControlType, const char *> ControlType2Str = {
    {ControlType::ACK, "ACK"},         {ControlType::ENQ, "ENQ"},   {ControlType::EOT, "EOT"},
    {ControlType::NOFRAME, "NOFRAME"}, {ControlType::NACK, "NACK"}, {ControlType::STX, "STX"},
    {ControlType::INVALID, "INVALID"}};

/**
 * This class represents a service who's work is to communicate (following a Wait&Stop protocol's guidelines) Master and
 * Slave. It assumes given Communication object is already in a "good" state when is constructed.
 */
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

    /**
     * Given the data from file, calculates the value of the BCE
     * @param data String with the data prepared to be sent
     * @param dataSize
     * @return char with the value of bce
     */
    char calculateBCE(char *data, size_t dataSize);

  private:
    /**
     * Sends an ACK frame to the destination device.
     * @return True if ACK was sent successfully, false other ways.
     */
    bool sendAck();

    /**
     * Sends a NACK frame to the destination device.
     * @return True if NACK was sent successfully, false other ways.
     */
    bool sendNAck();

    /**
     * Sends the content to the pair and print frame's information. Important! frame's real type must be
     * WaitAndStopFrameBuilder! Other ways undefined behaviout is assured!
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
    size_t m_errorsLeft; // Left erros to introduce in the transfer phase. Used by Master.
};

#endif