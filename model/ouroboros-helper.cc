#include "ns3/log.h"
#include "ns3/internet-module.h"
#include <stdlib.h>
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
            this->slotLeaders[epochNumber].resize(slotNumber+1,-1);
        }
        if(this->slotLeaders[epochNumber][slotNumber] == -1) {
            //randomly generate leader
            //TODo generate by stack
            this->slotLeaders[epochNumber][slotNumber] = (rand() % constants.numberOfNodes);
        }
        return this->slotLeaders[epochNumber][slotNumber];
    }
}
