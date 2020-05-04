#include "network-helper.h"

namespace ns3 {
    NS_LOG_COMPONENT_DEFINE ("NetworkHelper");

    Ipv4InterfaceContainer NetworkHelper::CreateBusNetwork(NodeContainer nodes) {
        CsmaHelper lanNet;
        lanNet.SetChannelAttribute("DataRate", StringValue("100Mbps"));
        lanNet.SetChannelAttribute("Delay", StringValue("2ms"));

        NetDeviceContainer netDevices;
        netDevices = lanNet.Install(nodes);

        InternetStackHelper stack;
        stack.Install(nodes);

        Ipv4AddressHelper address;
        address.SetBase("192.168.1.0", "255.255.255.0");
        Ipv4InterfaceContainer netInterfaces;
        netInterfaces = address.Assign(netDevices);
        return netInterfaces;
    }
}

