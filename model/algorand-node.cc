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
        this->nodeHelper = nodeHelper;
        this->phaseCounter = 0;
        this->loopCounter = 0;
        this->secondsWaitingForBlockReceive = 5.0;
        this->secondsWaitingForStartSoftVote = 0.5;
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

        //start events
        this->blockProposeEvent = Simulator::Schedule(Seconds(this->secondsWaitingForBlockReceive), &AlgorandNodeApp::FinishReceiveTransaction, this);
    }

    void AlgorandNodeApp::StopApplication() {
        NS_LOG_FUNCTION(this);
        BlockChainNodeApp::StopApplication();
        Simulator::Cancel(this->blockProposeEvent);
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
        auto listOfMembers = this->nodeHelper->ListOfCommitteeMembers(this->loopCounter);
        for(int nodeId: listOfMembers) {
            if(nodeId == GetNode()->GetId()){
                return true;
            }
        }
        return false;
    }

    bool AlgorandNodeApp::HandleCustomRead(Ptr <Packet> packet, Address from, std::string receivedData) {
        NS_LOG_FUNCTION(this);
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
            case ALGORAND_SOFT_VOTE:
                this->ReceiveSoftVote(&d);
                return true;
        }
        return false;
    }

    void AlgorandNodeApp::ReceiveProposedBlock(rapidjson::Document *message) {
        NS_LOG_FUNCTION(this);
        double timeSeconds = Simulator::Now().GetSeconds();
//        NS_LOG_INFO("At time " << timeSeconds << " node " << GetNode()->GetId() << " receive proposed block");
        if(this->loopCounter >= this->receivedProposedBlocks.size()){
            int lastSize = this->receivedProposedBlocks.size();
            this->receivedProposedBlocks.resize(this->loopCounter+1);
            for(int i=lastSize; i <= this->loopCounter; i++){
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
        NS_LOG_FUNCTION(this);
//        NS_LOG_INFO("At time " << Simulator::Now().GetSeconds() << " node " << GetNode()->GetId() << " receive new Transaction");
        BlockChainNodeApp::ReceiveNewTransaction(message);
        // add transaction to the block
        Transaction *transaction = Transaction::FromJSON(message);
        this->createdBlock->AddTransaction(transaction);

    }

    void AlgorandNodeApp::FinishReceiveTransaction() {
        NS_LOG_FUNCTION(this);
        double timeSeconds = Simulator::Now().GetSeconds();
//        NS_LOG_INFO("At time " << timeSeconds << " node " << GetNode()->GetId() << " end with receive new transaction");

        //send new block
        Block* lastBlock = this->blockChain->GetTopBlock();
        int blockHeight =  this->blockChain->GetBlockchainHeight()+1;
        int validator = GetNode()->GetId();
        Block *newBlock = new Block(blockHeight, validator, lastBlock, timeSeconds, timeSeconds, Ipv4Address("0.0.0.0"));
        for(auto trans: this->createdBlock->GetTransactions()) {
            newBlock->AddTransaction(trans);
        }
        rapidjson::Document transactionDoc = newBlock->ToJSON();
        transactionDoc["type"]= ALGORAND_BLOCK_PROPOSAL;
        this->SendMessage(&transactionDoc, this->broadcastSocket);

        //create new block
        delete this->createdBlock;
        timeSeconds = Simulator::Now().GetSeconds();
        this->createdBlock = new Block(blockHeight, validator, lastBlock, timeSeconds, timeSeconds, Ipv4Address("0.0.0.0"));

        //plan next events
        this->AddPhaseNumber();
        this->blockProposeEvent = Simulator::Schedule(Seconds(this->secondsWaitingForStartSoftVote), &AlgorandNodeApp::SoftVotePhase, this);
    }


    void AlgorandNodeApp::SoftVotePhase() {
        NS_LOG_FUNCTION(this);
        if(!this->IsICommitteeMember()){
            //TODO plan next loop and phase
            return;
        }
        NS_LOG_INFO("At time " << Simulator::Now().GetSeconds() << " node " << GetNode()->GetId() << " start soft vote phase");

        //TODO  find block Id to vote
        int blockId = 1;
        int nodeId = GetNode()->GetId();
        const char *json = "{\"type\":\"1\",\"blockId\":\"1\", \"loopNum\":\"1\",\"senderId\":\"1\", \"senderStack\":\"1\"}";
        rapidjson::Document message;
        message.Parse(json);
        message["type"].SetInt(ALGORAND_SOFT_VOTE);
        message["blockId"].SetInt(blockId);
        message["loopNum"].SetInt(this->GetLoopNumber());
        message["senderId"].SetInt(GetNode()->GetId());
        message["senderStack"].SetInt(this->nodeHelper->GetNodeStack(nodeId));

        this->SendMessage(&message, this->broadcastSocket);

        //plan next
        //TODO
    }

    void AlgorandNodeApp::ReceiveSoftVote(rapidjson::Document *message) {
        NS_LOG_FUNCTION(this);
        NS_LOG_INFO("At time " << Simulator::Now().GetSeconds() << " node " << GetNode()->GetId() << " receive soft vote");

//        this->SendMessage(message, this->broadcastSocket);
    }
}