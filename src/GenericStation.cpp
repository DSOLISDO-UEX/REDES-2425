#include "GenericStation.h"

GenericStation::GenericStation(Adapter *adapter, char groupNumber) : m_interactiveServ(nullptr)
{
    // Declared this way so m_interactiveService is initialized properly. If constructor-like style is used,
    // there's no certainity of the order parameters are going to be constructed.
    m_working = false;
    m_groupNumber = groupNumber;
    m_communication = new Communication(adapter, groupNumber);
    m_interactiveServ = InteractiveService(m_communication);
}

GenericStation::~GenericStation()
{
    m_working = false;
    m_groupNumber = 0;
    delete m_communication;
}