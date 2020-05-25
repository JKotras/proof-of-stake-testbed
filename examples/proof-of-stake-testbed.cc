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
#include "../model/ouroboros-node.h"
#include "../model/algorand-node.h"
#include "../model/constants.h"
#include "../helper/network-helper.h"
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

    //parse cmd params
    CommandLine cmd;
    cmd.Parse(argc, argv);

    //start logging
    LogComponentEnable("BlockChain", LOG_LEVEL_INFO);
    LogComponentEnable("BlockChainNodeApp", LOG_LEVEL_INFO);
    LogComponentEnable("OuroborosNodeApp", LOG_LEVEL_INFO);
    LogComponentEnable("OuroborosHelper", LOG_LEVEL_INFO);
    LogComponentEnable("AlgorandNodeApp", LOG_LEVEL_INFO);
    LogComponentEnable("AlgorandHelper", LOG_LEVEL_INFO);
    LogComponentEnable("NetworkHelper", LOG_LEVEL_INFO);
    LogComponentEnable("ProofOfStakeTestbed", LOG_LEVEL_INFO);

    //create nodes
    NodeContainer nodes;
    nodes.Create(constants.numberOfNodes);

    //create network
    auto netInterfaces = NetworkHelper::CreateBusNetwork(nodes);
//    std::vector <Ipv4Address> allAddress;
//    for(unsigned int i=0;i<constants.numberOfNodes;i++) {
//        allAddress.push_back(netInterfaces.GetAddress (i));
//    }
    // routing in the network
    Ipv4GlobalRoutingHelper::PopulateRoutingTables();

    // create applications
    AlgorandHelper nodeHelper(50, constants.numberOfNodes, 10000000);
    for(unsigned int i=0;i<constants.numberOfNodes;i++) {
        Ptr <AlgorandNodeApp> app = CreateObject<AlgorandNodeApp>(&nodeHelper);
//        app->SetNodesAddresses(allAddress);
        nodes.Get(i)->AddApplication(app);
        app->SetStartTime(Seconds(0.));
        app->SetStopTime(Seconds(10.));
    }

    // run simulator
    NS_LOG_INFO ("Run Simulation.");
    Simulator::Run();
    Simulator::Destroy();
    NS_LOG_INFO ("Done.");
    return 0;
}
