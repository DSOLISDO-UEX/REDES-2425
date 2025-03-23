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
 * about the conexion (src and dest mac, ...)
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
     * Tries to discover slaves. Must only be called by Master. Will block current thread until a slave is found.
     * @return True if found a slave.
     */
    bool discoverSlave();

    /**
     * Listens for packets sent by master, will block calling thread until a master sends the initial message.
     * @return True if found a master.
     */
    bool listenForMaster();

    /**
     * Sends data. Caller must ensure it's freed after calling write.
     * @param destinationMac Destination device for the packet.
     * @param protocolType Type of the protocol, look at m_protocolType.
     * @param data Data that's going to be sent. If empty no data will be sent, ONLY THE HEADER.
     * @param dataSize Size of the data that's going to be sent. If set to 0, no data will be sent, ONLY THE HEADER.
     * @return True if could write the frame onto destination, false other-ways.
     */
    bool write(char destinationMac[6], char protocolType[2], char *data, size_t dataSize);

    /**
     * Sends the built frame. WILL FREE DATA.
     * @param destinationMac Destination device for the packet.
     * @param protocolType Type of the protocol, look at m_protocolType.
     * @param data Built frame WITH DATA.
     * @return True if could write the frame onto destination, false other-ways.
     */
    bool write(char destinationMac[6], char protocolType[2], FrameBuilder &data);

    /**
     * Executes the main loop of the master station. Caller must ensure that slave and master have already stablished
     * connection. Important: This function will block calling threaad until there's an error or execution finishes.
     *
     * This function is responsible of:
     *  - Showing master's menu.
     *  - Receive/Sending characters from/to slave.
     *  - Sending file content (splitted) to slave.
     */
    void masterLoop();

    /**
     * Executes the main loop of the slave station. Caller must ensure that slave and master have already stablished
     * connection. Important: This function will block calling threaad until there's an error or execution finishes.
     *
     * This function is responsible of:
     *  - Showing slave's menu.
     *  - Continously receiving data from master.
     */
    void slaveLoop();

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