#include "ns3/log.h"
#include "ns3/internet-module.h"
#include <stdlib.h>
#include "algorand-helper.h"
#include "constants.h"

namespace ns3 {
    NS_LOG_COMPONENT_DEFINE ("AlgorandHelper");

    AlgorandHelper::AlgorandHelper(double committeePercentageSize, int countOfNodes, long int totalStack): NodeHelper(countOfNodes,totalStack) {
        if(this->committeePercentageSize > 100){
            NS_FATAL_ERROR("Invalid committee percentage size. Max number is 100");
        }
        this->committeePercentageSize = committeePercentageSize;
        int committeeSize = constants.numberOfNodes * (this->committeePercentageSize/100);
        if(committeeSize <= 0){
            NS_FATAL_ERROR("Calculated committee size size 0 - invalid");
        }
    }

    int AlgorandHelper::NodeOfBlockProposal(int loopNumber) {
        if(loopNumber < this->blockProposals.size()){
            return this->blockProposals[loopNumber];
        }
        int lastSize = this->blockProposals.size();
        this->blockProposals.resize(loopNumber+1);
        for(int i=lastSize; i <= loopNumber; i++){
            int blockProposalNode = rand() % this->countOfNodes;
            this->blockProposals[i] = blockProposalNode;
        }
        return this->blockProposals[loopNumber];
    }

    void AlgorandHelper::CreateCommitteeMembers(int loopNumber) {
        if(loopNumber < this->committeeMembers.size()){
            return;
        }
        int lastSize = this->committeeMembers.size();
        int committeeSize = constants.numberOfNodes * (this->committeePercentageSize/100);
        this->committeeMembers.resize(loopNumber+1);
        for(int i=lastSize; i <= loopNumber; i++){
            int counter = 0;
            do{
                //TODO make random that respect size of node stack
                int committeeMemberNode = rand() % this->countOfNodes;
                this->committeeMembers[i].push_back(committeeMemberNode);
                counter++;
            } while(counter < committeeSize);
        }
    }

    std::vector<int> AlgorandHelper::ListOfCommitteeMembers(int loopNumber) {
        this->CreateCommitteeMembers(loopNumber);
        return this->committeeMembers[loopNumber];
    }
}