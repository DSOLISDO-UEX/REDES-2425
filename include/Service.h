#ifndef SERVICE_H
#define SERVICE_H

#include "Communication.h"
#include "WaitAndStopFrameBuilder.h"

enum class Control
{
    ENQ = 05,   //Select salve station
    EOT = 04,   //Free communication
    ACK = 06,   //Confirm recieved - Accept the frame
    NACK = 21,  //Confirm recieved but there are mistakes - Reject the frame
    STX = 02    //Data frame
};

class Service
{
    public:
        
        /**
         * Given the data from file, calculates the value of the BCE
         * @param data String with the data prepared to be sent
         * @return char with the value of bce
         */
        char calculateBCE(std::string &data);


        Communication* getCommunication();

        char sendAckOrNack(const std::string &data, char receivedBCE);


    private:
        Communication* m_comm;
};

#endif