#ifndef INTERACTIVE_SERVICE_H
#define INTERACTIVE_SERVICE_H

#include "Communication.h"

/**
 * This class represents a service who's work is to communicate (1 character each time) Master and Slave. It assumes
 * given Communication object is already in a "good" state when is constructed.
 */
class InteractiveService
{
  public:
    /**
     * Creates the object and links it with the given connection.
     * @param comm
     */
    InteractiveService(Communication *comm);

    /**
     * Starts sending characters 1 by one to the slave and READING from slave. If exit is pressed this function will
     * return.
     * @return True if exited without any problems or false if there was an error.
     */
    bool startInteractiveMaster();

    /**
     * Starts sending content to Master. If exit is pressed this function will return.
     * @return True if exited without any problem or false if there was an error.
     */
    bool startInteractiveSlave();

    /**
     * Tries to read content from communication and shows it. This function is here because SLAVE is ALWAYS
     * listening, no matter what and not to dupplicate code within master's part.
     */
    void serviceRead();

    /**
     * This function tries to read something from the communication and discards it. Called from master to ensure
     * frames sent BEFORE starting the Interactive mode are discarded properly.
     */
    void readAndDiscard();

  private:
    Communication *m_comm;
};

#endif // INTERACTIVE_SERVICE_H