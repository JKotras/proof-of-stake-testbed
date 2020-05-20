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

    void AlgorandHelper::CreateCommitteeMembers(int loopNumber) {
        if(loopNumber < this->committeeMembers.size()){
            return;
        }
        int lastSize = this->committeeMembers.size();
        int committeeSize = constants.numberOfNodes * (this->committeePercentageSize/100);
        this->committeeMembers.resize(loopNumber+1);
        for(int i=(lastSize-1); i <= loopNumber; i++){
            int counter = 0;
            do{
                //TODO make random that respect size of node stack
                int blockProposalNode = rand() % this->countOfNodes;
                this->blockProposals[i].push_back(blockProposalNode);
                counter++;
            } while(counter < committeeSize);
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

    std::vector<int> AlgorandHelper::ListOfCommitteeMembers(int loopNumber) {
        this->CreateCommitteeMembers(loopNumber);
        return this->committeeMembers[loopNumber];
    }
}