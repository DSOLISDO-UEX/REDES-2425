#ifndef SLAVE_STATION_H
#define SLAVE_STATION_H

#include "GenericStation.h"
#include "WaitAndStop/WaitAndStopService.h"

/**
 * This class represents the Slave station.
 */
class SlaveStation : public GenericStation
{
  public:
    /**
     * Creates the slave station with the given (opened) adapter and group number.
     */
    SlaveStation(Adapter *adapter, char groupNumber);

    /**
     * Checks for input and does actions based on it.
     * @return True if there wasn't any error.
     */
    bool checkInput() override;

    /**
     * Executes the main loop of the station.
     * Important: Will block calling thread until execution finishes.
     * @return True if station finished without errors.
     */
    bool loop() override;

    /**
     * Tries discovering either Slaves (if caller is Master) or Master (if caller is Slave).
     * Important: This function will block calling thread until an error is thrown or a device is
     * discovered.
     * @return True if discovered and false if there was any error.
     */
    bool tryDiscovery() override;

    /**
     * Shows the menu of the station.
     */
    void showMenu() override;
};

#endif // SLAVE_STATION_H