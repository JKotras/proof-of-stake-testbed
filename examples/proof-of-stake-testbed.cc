/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/csma-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/ipv4-global-routing-helper.h"
#include "../model/blockchain.h"
#include "../model/blockchain-node.h"
#include "../helper/blockchain-node-helper.h"
#include <iostream>
#include <string>

// Default Network Topology
//
//       10.1.1.0
//    n1   n2   n3   n4     n*
//    |    |    |    |      |
//    ================ *** === ***
//           LAN


using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("ProofOfStakeTestbed");

int
main(int argc, char *argv[]) {
    uint32_t numberOfNodes = 20;

    CommandLine cmd;
    cmd.Parse(argc, argv);

    LogComponentEnable("BlockChainNodeApp", LOG_LEVEL_INFO);

    NodeContainer nodes;
    nodes.Create(numberOfNodes);

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
    std::vector <Ipv4Address> allAddress;

    for(unsigned int i=0;i<numberOfNodes;i++) {
        allAddress.push_back(netInterfaces.GetAddress (i));
    }

    // now network is created
    for(unsigned int i=0;i<numberOfNodes;i++) {
        Ptr <BlockChainNodeApp> app = CreateObject<BlockChainNodeApp>(netInterfaces);
        app->SetNodesAddresses(allAddress);
        nodes.Get(i)->AddApplication(app);
        app->SetStartTime(Seconds(1.));
        app->SetStopTime(Seconds(20.));
    }

    // end of impl

    // routing in the network
    Ipv4GlobalRoutingHelper::PopulateRoutingTables();

    // run simulator
    NS_LOG_INFO ("Run Simulation.");
    Simulator::Run();
    Simulator::Destroy();
    NS_LOG_INFO ("Done.");
    return 0;
}
