#include "ns3/log.h"
#include "ns3/internet-module.h"
#include <stdlib.h>
#include "ouroboros-helper.h"
#include "constants.h"

namespace ns3 {
    NS_LOG_COMPONENT_DEFINE ("OuroborosHelper");

    OuroborosHelper::OuroborosHelper(double slotSizeSeconds, int securityParameter, int countOfNodes, long int totalStack): NodeHelper(countOfNodes,totalStack) {
        this->slotSizeSeconds = slotSizeSeconds;
        this->slotsInEpoch = 10 * securityParameter;
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

    double OuroborosHelper::GetSlotSizeSeconds() const {
        return this->slotSizeSeconds;
    }

    double OuroborosHelper::GetEpochSizeSeconds() const {
        return (this->slotSizeSeconds * this->slotsInEpoch);
    }

    int OuroborosHelper::GetSlotNumber() {
        double timeSeconds = Simulator::Now().GetSeconds();
        return int(timeSeconds / this->slotSizeSeconds);
    }

    int OuroborosHelper::GetEpochNumber() {
        double timeSeconds = Simulator::Now().GetSeconds();
        double calc = timeSeconds / (this->slotSizeSeconds * this->slotsInEpoch);
        return int(calc);
    }

    int OuroborosHelper::GetSlotsInEpoch() const {
        return this->slotsInEpoch;
    }
}
