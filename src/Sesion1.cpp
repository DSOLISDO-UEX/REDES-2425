//============================================================================
// ----------- PRACTICAS DE FUNDAMENTOS DE REDES DE COMUNICACIONES -----------
// ---------------------------- CURSO 2024/25 --------------------------------
// ----------------------------- SESION1.CPP ---------------------------------
//============================================================================

#include <stdio.h>
#include <iostream>
#include "../include/Adapter.h"


int mainSesion1()
{
    Adapter adp;
    adp.openFromAvailable();  

    std::cout << "Chosen adapter MAC: " << adp.getMac() << std::endl;
    return 0;
}
