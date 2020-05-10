#include "network-helper.h"
#include "../model/constants.h"

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

    Ipv4InterfaceContainer NetworkHelper::CreateMeshNetwork(NodeContainer nodes) {
        int sizePerOneMesh = 6;
        int countOfMeshs = (constants.numberOfNodes / sizePerOneMesh) + 1;

        // craete localNetworks
        int i = 0;
        std::vector<NodeContainer *> localNetworkNodeContainers;
        for (i = 0; i < countOfMeshs; i++) {
            NodeContainer lanNodes;
            localNetworkNodeContainers.push_back(&lanNodes);
            for(int j = 0; j < sizePerOneMesh; j++){
                int index = (i*sizePerOneMesh) + j;
                if(index > (constants.numberOfNodes - 1)){
                    break;
                }
                lanNodes.Add(nodes.Get(index));
            }

            CsmaHelper lanNet;
            lanNet.SetChannelAttribute("DataRate", StringValue("100Mbps"));
            lanNet.SetChannelAttribute("Delay", StringValue("2ms"));

            NetDeviceContainer netDevices;
            netDevices = lanNet.Install(lanNodes);

            InternetStackHelper stack;
            stack.Install(lanNodes);

            //TODO be avare with i size
            std::string localipv4address = "192.168." + std::to_string(i) + ".0";
            Ipv4AddressHelper address;
            address.SetBase(localipv4address.c_str(), "255.255.255.0");
            Ipv4InterfaceContainer netInterfaces;
            netInterfaces = address.Assign(netDevices);
        }

        //mesh connection
        int lastIPv4address = i;
        for (i = 0; i < (countOfMeshs-1); i++) {
            NodeContainer p2pNodes;
            p2pNodes.Add(localNetworkNodeContainers[i]->Get(1));
            p2pNodes.Add(localNetworkNodeContainers[i+1]->Get(0));

            PointToPointHelper pointToPoint;
            pointToPoint.SetDeviceAttribute ("DataRate", StringValue("100Mbps"));
            pointToPoint.SetChannelAttribute ("Delay", StringValue ("2ms"));

            NetDeviceContainer p2pDevices;
            p2pDevices = pointToPoint.Install(p2pNodes);

            //maybe it alreay installed
//            InternetStackHelper stack;
//            stack.Install(p2pNodes);

            std::string localipv4address = "192.168." + std::to_string(i+lastIPv4address) + ".0";
            Ipv4AddressHelper address;
            address.SetBase(localipv4address.c_str(), "255.255.255.0");
            Ipv4InterfaceContainer p2pInterfaces;
            p2pInterfaces = address.Assign (p2pDevices);
        }
        // create connection networks




//        CsmaHelper lanNet;
//        lanNet.SetChannelAttribute("DataRate", StringValue("100Mbps"));
//        lanNet.SetChannelAttribute("Delay", StringValue("2ms"));
//
//        NetDeviceContainer netDevices;
//        netDevices = lanNet.Install(nodes);
//
//        InternetStackHelper stack;
//        stack.Install(nodes);
//
//        Ipv4AddressHelper address;
//        address.SetBase("192.168.1.0", "255.255.255.0");
//        Ipv4InterfaceContainer netInterfaces;
//        netInterfaces = address.Assign(netDevices);
//        return netInterfaces;
    }
}

