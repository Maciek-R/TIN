#include "network_object.h"

#ifdef BUILD_WLAN
InterfaceType NetworkObject::interfaceType = InterfaceType::WLAN;
#else
InterfaceType NetworkObject::interfaceType = InterfaceType::ETH;
#endif
