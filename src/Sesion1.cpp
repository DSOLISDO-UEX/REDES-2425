//============================================================================
// ----------- PRACTICAS DE FUNDAMENTOS DE REDES DE COMUNICACIONES -----------
// ---------------------------- CURSO 2024/25 --------------------------------
// ----------------------------- SESION1.CPP ---------------------------------
//============================================================================


#include <stdio.h>
#include <iostream>
#include "../include/Adapter.h"




using namespace std;


int main()
{
    printf("\n----------------------------\n");
    printf("------ SESION 1 - FRC ------\n");
    printf("----------------------------\n");

    pcap_if_t *avail_ifaces = GetAvailAdapters(); 
    pcap_if_t *current = avail_ifaces;
    int i = 0;

    while(current)
    {
        printf("[%d] %s\n", i++, current->name);
        current = current->next; 
    }   

    printf("Seleccione interfaz: ");
    
    int option = -1;
    scanf("%d", &option);

    pcap_if_t* selected = avail_ifaces;
    i = 0;
    while(i != option)
    {
        selected = selected->next;
        i++;            
    }

    printf("Interfaz elegida: %s\n", selected->name);

    interface_t iface;
    setDeviceName(&iface, selected->name);
    GetMACAdapter(&iface);

	for(size_t j = 0; j < sizeof(iface.MACaddr); j++)
	{
		printf("%02x", iface.MACaddr[j]);

		if (j != (sizeof(iface.MACaddr) - 1))
		{
			printf(":");
		}
	}

	printf("\n");

 return 0;
}
