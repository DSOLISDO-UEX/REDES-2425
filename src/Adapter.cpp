#include "../include/Adapter.h"

bool Adapter::openHandle()
{
    return OpenAdapter(&m_selectedInterface) == 0;
}

bool Adapter::selectAdapter(const std::string &adapterName)
{
    return setDeviceName(&m_selectedInterface, (char*)adapterName.c_str()) == 0;
}

pcap_if_t *Adapter::GetAdapters()
{
    return GetAvailAdapters();
}