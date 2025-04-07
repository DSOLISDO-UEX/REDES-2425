#ifndef MASTER_STATION_H
#define MASTER_STATION_H

#include "GenericStation.h"

/**
 * This class represents the Master station.
 */
class MasterStation : public GenericStation
{
  public:
    /**
     * Creates the Master station with the given (opened) adapter and group number.
     */
    MasterStation(Adapter *adapter, char groupNumber);

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

#endif // MASTER_STATION_H