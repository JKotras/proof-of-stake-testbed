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
#include "../model/node.h"
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
main (int argc, char *argv[])
{
    uint32_t numberOfNodes = 200;

    CommandLine cmd;
    cmd.Parse (argc,argv);

    LogComponentEnable ("BlockChainNodeApp", LOG_LEVEL_INFO);

    NodeContainer nodes;
    nodes.Create (numberOfNodes);

    CsmaHelper lanNet;
    lanNet.SetChannelAttribute ("DataRate", StringValue ("100Mbps"));
    lanNet.SetChannelAttribute ("Delay", StringValue ("2ms"));

    NetDeviceContainer netDevices;
    netDevices = lanNet.Install (nodes);

    InternetStackHelper stack;
    stack.Install (nodes);

    Ipv4AddressHelper address;
    address.SetBase ("192.168.1.0", "255.255.255.0");
    Ipv4InterfaceContainer netInterfaces;
    netInterfaces = address.Assign (netDevices);

    // now network is created

    //TODO create aplications
    Block block(0, 0, 0, nullptr, 0, 0, Ipv4Address("0.0.0.0"));
    std::cout << block.GetBlockHeight() << std::endl;
    BlockChainNodeApp app = BlockChainNodeApp();


    // end of impl


    UdpEchoClientHelper echoClient (netInterfaces.GetAddress(0), 9);
    echoClient.SetAttribute ("MaxPackets", UintegerValue (2));
    echoClient.SetAttribute ("Interval", TimeValue (Seconds (1.0)));
    echoClient.SetAttribute ("PacketSize", UintegerValue (1024));

    ApplicationContainer clientApps = echoClient.Install (nodes.Get (0));
    clientApps.Start (Seconds (2.0));
    clientApps.Stop (Seconds (10.0));

    ApplicationContainer clientApps1 = echoClient.Install (nodes.Get (2));
    clientApps1.Start (Seconds (3.0));
    clientApps1.Stop (Seconds (10.0));


    // routing in the network
    Ipv4GlobalRoutingHelper::PopulateRoutingTables ();

    // run simulator
    Simulator::Run ();
    Simulator::Destroy ();
    return 0;
}
