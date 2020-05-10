#include "network-helper.h"
#include "../model/constants.h"
#include <math.h>

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
        int sizePerOneMesh = 4;
        int countOfMeshs = ceil((double)constants.numberOfNodes / sizePerOneMesh);

        // craete localNetworks
        int i = 0;
        for (i = 0; i < countOfMeshs; i++) {
            NodeContainer lanNodes;
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
//            NS_LOG_INFO("endyyy " << i*sizePerOneMesh << " with " << ((i+1)*sizePerOneMesh));
            NodeContainer p2pNodes;
            //TODO check miminla size of loclal network
            p2pNodes.Add(nodes.Get(i*sizePerOneMesh));
            p2pNodes.Add(nodes.Get(((i+1)*sizePerOneMesh)));

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
        NS_LOG_INFO("end");

//        // create connection networks




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

