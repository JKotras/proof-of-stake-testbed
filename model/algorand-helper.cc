#include "ns3/log.h"
#include "ns3/internet-module.h"
#include <stdlib.h>
#include "algorand-helper.h"
#include "constants.h"

namespace ns3 {
    NS_LOG_COMPONENT_DEFINE ("AlgorandHelper");

    AlgorandHelper::AlgorandHelper(int countOfNodes, long int totalStack): NodeHelper(countOfNodes,totalStack) {

    }

    int AlgorandHelper::GetLoopNumber() {
        return this->loopCounter;
    }

    int AlgorandHelper::GetPhaseNumber() {
        return this->phaseCounter;
    }

    void AlgorandHelper::AddPhaseNumber() {
        int result = this->phaseCounter++;
        this->phaseCounter = (int)result%3;
        if((result/3) >= 1){
            this->loopCounter++;
        }
    }

    void AlgorandHelper::CreateBlockProposal(int loopNumber) {
        if(loopNumber < this->blockProposals.size()){
            return;
        }
        int lastSize = this->blockProposals.size();
        this->blockProposals.resize(loopNumber+1);
        for(int i=(lastSize-1); i <= loopNumber; i++){
            //TODO make more blockProposals per one phase
            //TODO make random that respect size of node stack
            int blockProposalNode = rand() % this->countOfNodes;
            this->blockProposals[i].push_back(blockProposalNode);
        }
    }

    bool AlgorandHelper::IsBlockProposal(int NoneId, int loopNumber) {
        std::vector<int> listOfBlockProposal = this->ListOfBlockProposal(loopNumber);
        for(auto value: listOfBlockProposal){
            if(value == NoneId){
                return true;
            }
        }
        return false;
    }

    std::vector<int> AlgorandHelper::ListOfBlockProposal(int loopNumber) {
        this->CreateBlockProposal(loopNumber);
        return this->blockProposals[loopNumber];
    }
}