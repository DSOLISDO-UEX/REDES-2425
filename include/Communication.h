#ifndef COMMUNICATION_H
#define COMMUNICATION_H

#include "Adapter.h"
#include "FrameBuilder.h"
#include <algorithm>
#include <chrono>
#include <thread>

enum class PacketType : uint8_t
{
    Common = 0,        // Once Communication has been established, packets are ALL of this type.
    AskForSlave = 1,   // Sent by Master. Asks every device on the network if they are the Slave.
    RepplyToMaster = 2 // Sent by Slave to the Master once asked to establish the Communication.
};

/**
 * This class represents a communication. It holds a pointer to the adapter and information
 * about the conection (src and dest mac, ...)
 */
class Communication
{
  public:
    /**
     * Constructs the object and links it to an adapter. Caller must ensure that at the time of constructing
     * this object the adapter is opened and can send / receive data.
     * @param adapter Adapter used to send and receive packets.
     * @param groupNumber Number of the group.
     */
    Communication(Adapter *adapter, char groupNumber);

    /**
     * Destroys the object. Won't delete linked adapter.
     */
    ~Communication();

    /**
     * Sends data. Caller must ensure it's freed after calling write and that this Communication object
     * has a valid destination MAC.
     * @param protocolType Type of the protocol, look at m_protocolType.
     * @param data Data that's going to be sent. If empty no data will be sent, ONLY THE HEADER.
     * @param dataSize Size of the data that's going to be sent. If set to 0, no data will be sent, ONLY THE HEADER.
     * @return True if could write the frame onto destination, false other-ways.
     */
    bool write(char protocolType[2], char *data, size_t dataSize);

    /**
     * Sends the built frame. WILL FREE DATA.
     * @param destinationMac Destination device for the packet.
     * @param protocolType Type of the protocol, look at m_protocolType.
     * @param data Built frame WITH DATA.
     * @return True if could write the frame onto destination, false other-ways.
     */
    bool write(char protocolType[2], FrameBuilder &data);

    /**
     * Caller must ensure that setDestinationMac was called at some point previously.
     * @return A pointer to the buffer that contains destination MAC.
     */
    char* getDestinationMac();

    /**
     * Caller must ensure that adapter was opened previously.
     * @return A pointer to the buffer that contains destination MAC.
     */
    char* getSourceMac();

    /**
     * Sets the destination MAC for this communication.
     * @param dstMac
     */
    void setDestinationMac(char dstMac[6]);

    /**
     * @return Returns the ENTIRE packet read. If there was nothing to read, returns an empty string.
     */
    std::string read();

  private:
    Adapter *m_adapter;       // Adapter used to send and receive packets throught the network interface.
    char m_sourceMac[6];      // Buffer than contains the source mac of the destination (retrieved from the adapter)
    char m_destinationMac[6]; // Buffer that contains the destination mac (retrieved throught discoverMac())
    char m_groupNumber;
};

#endif // COMMUNICATION_H