#ifndef GENERIC_STATION_H
#define GENERIC_STATION_H


#include "Adapter.h"
#include "Communication.h"
#include "FrameBuilder.h"

/**
 * TODO: Make GenericStation use the new Service for the communication.
 */

/**
 * This class represents a generic station. It implements some methods and let child classes define others.
 * The behaviour varies depending on if the station is Slave or Master.
 */
class GenericStation
{
  public:
    /**
     * Creates the station with the given (opened) adapter and group number.
     * @param adapter
     * @param groupNumber
     */
    GenericStation(Adapter *adapter, char groupNumber);

    /**
     * Destroys the object and frees communication.
     */
    ~GenericStation();

    /**
     * Executes the main loop of the station.
     * Important: Will block calling thread until execution finishes.
     * @return True if station finished without errors.
     */
    virtual bool loop() = 0;

    /**
     * Tries discovering either Slaves (if caller is Master) or Master (if caller is Slave).
     * Important: This function will block calling thread until an error is thrown or a device is
     * discovered.
     * @return True if discovered and false if there was any error.
     */
    virtual bool tryDiscovery() = 0;

    /**
     * Shows the menu of the station.
     */
    virtual void showMenu() = 0;

  protected:
    char m_groupNumber;
    std::unique_ptr<Communication> m_communication;
};

#endif // GENERIC_STATION_H