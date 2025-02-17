//============================================================================
// ----------- PRACTICAS DE FUNDAMENTOS DE REDES DE COMUNICACIONES -----------
// ---------------------------- CURSO 2024/25 --------------------------------
// ----------------------------- SESION2.CPP ---------------------------------
//============================================================================
#include "../include/Adapter.h"
#include "../include/FrameBuilder.h"

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

    std::cin.ignore(); // Clears the input buffer.
    std::cout << "Enter a word to send: " << std::endl;
    std::cin >> word; // Ask for a word.

    FrameBuilder builder;
    if (!builder.buildFrame(srcMac, dstMac, type, (unsigned char*)word.data()))
    {
      std::cout << "Could not build frame!" << std::endl;
      return 1;
    }

    if (!adp.sendFrame((unsigned char*)builder.getBody(), builder.getBodySize()))
    {
      std::cout << "Could not send packet" << std::endl;
      return 1;
    }

    std::cout << "Packet sent" << std::endl;
    adp.close();

    return 0;
}