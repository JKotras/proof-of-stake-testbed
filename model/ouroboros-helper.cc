#include "ns3/internet-module.h"
#include <stdlib.h>
#include "ns3/log.h"
#include "ns3/ipv4-address.h"
#include "ns3/ipv6-address.h"
#include "ns3/address-utils.h"
#include "ns3/nstime.h"
#include "ns3/inet-socket-address.h"
#include "ns3/inet6-socket-address.h"
#include "ns3/socket.h"
#include "ns3/udp-socket.h"
#include "ns3/simulator.h"
#include "ns3/socket-factory.h"
#include "ns3/packet.h"
#include "ns3/uinteger.h"
#include "ns3/event-id.h"
#include "ouroboros-helper.h"
#include "constants.h"

namespace ns3 {
    NS_LOG_COMPONENT_DEFINE ("OuroborosHelper");

    OuroborosHelper::OuroborosHelper(int countOfNodes, long int totalStack): NodeHelper(countOfNodes,totalStack) {

    }

    int OuroborosHelper::GetSlotLeader(int slotNumber, int epochNumber) {
        if(epochNumber >= this->slotLeaders.size()){
            this->slotLeaders.resize(epochNumber+1);
        }
        if(slotNumber >= this->slotLeaders[epochNumber].size()){
            this->slotLeaders[epochNumber].resize(slotNumber,-1);
        }
        if(this->slotLeaders[epochNumber][slotNumber] != -1) {
            //randomly generate leader
            this->slotLeaders[epochNumber][slotNumber] = rand() % constants.numberOfNodes;
        }
        return this->slotLeaders[epochNumber][slotNumber];
    }
}
