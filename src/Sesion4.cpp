//============================================================================
// ----------- PRACTICAS DE FUNDAMENTOS DE REDES DE COMUNICACIONES -----------
// ---------------------------- CURSO 2024/25 --------------------------------
// ----------------------------- SESION4.CPP ---------------------------------
//============================================================================
#include "../include/SlaveStation.h"
#include "../include/MasterStation.h"
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

    std::cout << "Select station" << std::endl;
    std::cout << "[1] Mode slave" << std::endl;
    std::cout << "[2] Mode master" << std::endl;

    std::cin >> stationMode;
    GenericStation* station = nullptr;

    switch(stationMode)
    {
        case 1: // Slave
        {
            station = new SlaveStation(&adp, groupNum);
            break;
        }
        case 2: // Master
        {
            station = new MasterStation(&adp, groupNum);
            break;
        }

        default:
        {
            std::cout << "Invalid option!" << std::endl;
            return 1;
        }
    }

    station->tryDiscovery();
    station->loop();

    adp.close();
    return 0;
}