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
    std::string word;
    unsigned char srcMac[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
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

    std::cout << "Selected interface MAC:" << adp.getMac() << std::endl;
    bool working = true;

    while (working)
    {
        word = "";
        if (kbhit() == 1)
        {
            word += getch();
        }

        if (word == "c")
        {
            working = false;
            break;
        }

        if (word != "")
        {
            FrameBuilder builder;
            if (!builder.buildFrame(srcMac, dstMac, type, (unsigned char *)word.data()))
            {
                std::cout << "Could not build frame!" << std::endl;
                return 1;
            }

            if (!adp.sendFrame((unsigned char *)builder.getBody(), builder.getBodySize()))
            {
                std::cout << "Could not send packet" << std::endl;
                return 1;
            }

            std::cout << "Packet sent" << std::endl;
        }

        // RECEIVE
        apacket_t receivedPacket{};
        receivedPacket = adp.receiveFrame();

        if (receivedPacket.packet != nullptr)
        {

            std::cout << "Packet received: " << *(char *)(receivedPacket.packet + 14) << std::endl;
        }
        std::this_thread::yield();
    }

    adp.close();
    return 0;
}