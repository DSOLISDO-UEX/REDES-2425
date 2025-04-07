#include "../include/Service.h"


char Service::calculateBCE(std::string &data){
    char bce = 0x55;
    for(size_t i = 0; i<data.size(); i++)
    {
        bce ^= data[i];
    }

    if(bce == 0 || bce == 255){
        bce = 1;
    }
    return bce;
}





