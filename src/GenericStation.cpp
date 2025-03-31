#include "../include/GenericStation.h"

GenericStation::GenericStation(Adapter *adapter, char groupNumber)
    : m_groupNumber(groupNumber), m_communication(std::make_unique<Communication>(adapter, groupNumber))
{
}

GenericStation::~GenericStation()
{
    m_groupNumber = 0;
    m_communication.reset();
}