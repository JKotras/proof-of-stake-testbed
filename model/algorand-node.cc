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
#include "algorand-node.h"
#include "constants.h"

namespace ns3 {
    NS_LOG_COMPONENT_DEFINE ("AlgorandNodeApp");
    NS_OBJECT_ENSURE_REGISTERED (AlgorandNodeApp);

    AlgorandNodeApp::AlgorandNodeApp (AlgorandHelper *nodeHelper) : BlockChainNodeApp(nodeHelper) {
        this->phaseCounter = 0;
        this->loopCounter = 0;
        this->secondsWaitingForBlockReceive = 5.0;
    }

    void AlgorandNodeApp::StartApplication() {
        NS_LOG_FUNCTION(this);
        NS_LOG_INFO("Starting Algorand App " << GetNode()->GetId());
        BlockChainNodeApp::StartApplication();
        double time = Simulator::Now().GetSeconds();

        //init created block
        int blockHeight =  this->blockChain->GetBlockchainHeight()+1;
        int validator = GetNode()->GetId();
        Block* lastBlock = this->blockChain->GetTopBlock();
        this->createdBlock = new Block(blockHeight, validator, lastBlock, time, time, Ipv4Address("0.0.0.0"));
    }

    void AlgorandNodeApp::StopApplication() {
        NS_LOG_FUNCTION(this);
        BlockChainNodeApp::StopApplication();
    }

    int AlgorandNodeApp::GetLoopNumber() {
        return this->loopCounter;
    }

    int AlgorandNodeApp::GetPhaseNumber() {
        return this->phaseCounter;
    }

    void AlgorandNodeApp::AddPhaseNumber() {
        int result = this->phaseCounter++;
        this->phaseCounter = (int)result%3;
        if((result/3) >= 1){
            this->loopCounter++;
        }
    }

    bool AlgorandNodeApp::IsICommitteeMember() {
        for(int nodeId: this->nodeHelper->ListOfCommitteeMembers(this->loopCounter)) {
            if(nodeId == GetNode()->GetId()){
                return true;
            }
        }
        return false;
    }

    bool AlgorandNodeApp::HandleCustomRead(Ptr <Packet> packet, Address from, std::string receivedData) {
//        NS_LOG_INFO("Node " << GetNode()->GetId() << " Total Received Data: " << receivedData);
        rapidjson::Document d;
        d.Parse(receivedData.c_str());
        if (!d.IsObject()) {
            NS_LOG_WARN("The parsed packet is corrupted: " << receivedData);
            return false;
        }
        int messageType = d["type"].GetInt();
        switch (messageType) {
            case ALGORAND_BLOCK_PROPOSAL:
                this->ReceiveProposedBlock(&d);
                return true;
        }
        return false;
    }

    void AlgorandNodeApp::ReceiveProposedBlock(rapidjson::Document *message) {
        if(this->loopCounter > this->receivedProposedBlocks.size()){
            int lastSize = this->receivedProposedBlocks.size();
            this->receivedProposedBlocks.resize(this->loopCounter+1);
            for(int i=(lastSize-1); i <= this->loopCounter; i++){
                std::vector <Block *> vector;
                this->receivedProposedBlocks[i] = vector;
            }
        }
        Block *previousBlock = this->blockChain->GetTopBlock();
        //TODO beter receive FROM address
        Block *proposedBlock = Block::FromJSON(message,previousBlock,Ipv4Address("0.0.0.0"));

        //check if node has not already received this proposed block
        for(auto block: this->receivedProposedBlocks[this->loopCounter]){
            if(proposedBlock->IsSameAs(block)){
                //already receive
                return;
            }
        }

        //section where first time proposed block come
        this->receivedProposedBlocks[this->loopCounter].push_back(proposedBlock);
        this->SendMessage(message, this->broadcastSocket);
    }

    void AlgorandNodeApp::ReceiveNewTransaction(rapidjson::Document *message){
        BlockChainNodeApp::ReceiveNewTransaction(message);
        // add transaction to the block
        Transaction *transaction = Transaction::FromJSON(message);
        this->createdBlock->AddTransaction(transaction);

    }
}