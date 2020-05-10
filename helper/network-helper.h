#ifndef PROJECT_NETWORK_HELPER_H
#define PROJECT_NETWORK_HELPER_H

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/csma-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/ipv4-global-routing-helper.h"

namespace ns3 {

    class NetworkHelper {
    public:
        static Ipv4InterfaceContainer CreateBusNetwork(NodeContainer nodes);
        static Ipv4InterfaceContainer CreateMeshNetwork(NodeContainer nodes);
    };
}

#endif //PROJECT_NETWORK_HELPER_H
