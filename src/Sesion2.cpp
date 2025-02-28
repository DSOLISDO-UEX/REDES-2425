//============================================================================
// ----------- PRACTICAS DE FUNDAMENTOS DE REDES DE COMUNICACIONES -----------
// ---------------------------- CURSO 2024/25 --------------------------------
// ----------------------------- SESION2.CPP ---------------------------------
//============================================================================
#include "../include/Adapter.h"
#include "../include/FrameBuilder.h"
#include <thread>

int main()
{
    char ch[2] = {'\x00', '\x00'};
    unsigned char dstMac[6] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05};
    unsigned char type[2] = {0x30, 0x00};

    interface_t iface;

    printf("\n----------------------------\n");
    printf("------ SESION 2 - FRC ------\n");
    printf("----------------------------\n");

    Adapter adp;
    if (!adp.openFromAvailable())
    {
        std::cout << "Could not select an adapter!" << std::endl;
        return 1;
    }

    std::cout << "Source MAC: " << adp.getMacAsStr(adp.getAdapterMac()) << std::endl;
    std::cout << "Destination MAC:" << adp.getMacAsStr(dstMac) << std::endl;

    bool working = true;
    while (working)
    {
        if (kbhit() == 1)
        {
            ch[0] = getch();
            std::cout << std::endl; // Ensure newline after writing a character.
        }

        if (ch[0] == 27)
        {
            working = false;
            std::cout << "Exiting..." << std::endl;

            continue; // Next iteration should skip the WHILE.
        }

        // SEND
        if (ch[0] != '\x00')
        {
            // There's a character pending to be sent.
            FrameBuilder builder;
            if (!builder.buildFrame(adp.getAdapterMac(), dstMac, type, (unsigned char *)ch))
            {
                std::cout << "Could not build frame!" << std::endl;
                return 1;
            }

            if (!adp.sendFrame((unsigned char *)builder.getBody(), builder.getBodySize()))
            {
                std::cout << "Could not send packet" << std::endl;
                return 1;
            }

            ch[0] = '\x00';                          // Reset character.
            std::cout << "Packet sent" << std::endl; // Log
        }

        // RECEIVE
        apacket_t receivedPacket{};
        receivedPacket = adp.receiveFrame();

        if (receivedPacket.packet != nullptr)
        {
            unsigned char *packetContent = (unsigned char *)(receivedPacket.packet + 14); // srcMac + dstMac + protocol
            std::cout << "Received packet length: " << strlen((const char *)packetContent) << std::endl;
            std::cout << "Packet received: " << *packetContent << std::endl;
        }

        std::this_thread::yield(); /* 
                                    * Ensure this thread doesn't keep running on CPU without letting other threads execute.
                                    */
    }

    adp.close();
    return 0;
}