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

Communication* Service::getCommunication(){
    return m_comm;
}

char Service::sendAckOrNack(const std::string &data, char receivedBCE){
    std::string tempData = data;
    char calculatedBCE = calculateBCE(tempData);

    if (calculatedBCE == receivedBCE) {
        //If same, send ACK
        return '0x06'; //ACK
    } else{
        //Otherwise, need to send NACK
        return '0x21'; //NACK
    }
}



