//============================================================================
// ----------- PRACTICAS DE FUNDAMENTOS DE REDES DE COMUNICACIONES -----------
// ---------------------------- CURSO 2024/25 --------------------------------
// ----------------------------- SESION3.CPP ---------------------------------
//============================================================================
#include "../include/Adapter.h"
#include "../include/Communication.h"
#include <thread>

int main()
{
    printf("\n----------------------------\n");
    printf("------ SESION 3 - FRC ------\n");
    printf("----------------------------\n");

    Adapter adp;
    interface_t iface;
    int groupNum = 0, stationMode = 0;

    if (!adp.openFromAvailable())
    {
        std::cout << "Could not select an adapter!" << std::endl;
        return 1;
    }

    std::cout << "Source MAC: " << adp.getMacAsStr(adp.getAdapterMac()) << std::endl;
    
    std::cout << "Enter group number: " << std::endl;
    std::cin >> groupNum;

    Communication communication(&adp, groupNum);

    std::cout << "Select station" << std::endl;
    std::cout << "[1] Mode slave" << std::endl;
    std::cout << "[2] Mode master" << std::endl;

    std::cin >> stationMode;
    switch(stationMode)
    {
        case 1: // Slave
        {
            communication.listenForMaster();
            communication.slaveLoop();

            break;
        }
        case 2: // Master--Exclusivamente envia info, 2 tipos
        {
            communication.discoverSlave();
            communication.masterDoing();

            break;
        }

        default:
        {
            std::cout << "Invalid option!" << std::endl;
            return 1;
        }
    }

    adp.close();
    return 0;
}