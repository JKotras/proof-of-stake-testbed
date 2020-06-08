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
        this->loopCounterProposedBlock = 0;
        this->secondsWaitingForBlockReceive = constants.algorandWaitingProposedBlockSeconds;
        this->secondsWaitingForStartSoftVote = 0.5;
        this->secondsWaitingForStartCertify = 0.5;
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
        Simulator::Cancel(this->SoftVoteEvent);
        Simulator::Cancel(this->CertifyVoteEvent);
    }

    int AlgorandNodeApp::GetLoopNumber() {
        return this->loopCounter;
    }

    int AlgorandNodeApp::GetPhaseNumber() {
        return this->phaseCounter;
    }

    void AlgorandNodeApp::AddLoopNumber() {
        this->phaseCounter=0;
        this->loopCounter++;
    }

    void AlgorandNodeApp::AddPhaseNumber() {
        this->phaseCounter++;
    }

    bool AlgorandNodeApp::IsIBlockProposalMember() {
        auto listOfMembers = this->nodeHelper->ListOfBlockProposals(this->loopCounter);
        for(int nodeId: listOfMembers) {
            if(nodeId == GetNode()->GetId()){
                return true;
            }
        }
        return false;
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
//        NS_LOG_INFO("Node " << GetNode()->GetId() << " time " << Simulator::Now().GetSeconds() << " Total Received Data: " << receivedData);
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
            case ALGORAND_CERTIFY_VOTE:
                this->ReceiveCertifyVote(&d);
                return true;
        }
        return false;
    }

    void AlgorandNodeApp::ReceiveProposedBlock(rapidjson::Document *message) {
        NS_LOG_FUNCTION(this);
        double timeSeconds = Simulator::Now().GetSeconds();
//        NS_LOG_INFO("At time " << timeSeconds << " node " << GetNode()->GetId() << " receive proposed block");
        if(this->loopCounterProposedBlock >= this->receivedProposedBlocks.size()){
            int lastSize = this->receivedProposedBlocks.size();
            this->receivedProposedBlocks.resize(this->loopCounterProposedBlock+1);
            for(int i=lastSize; i <= this->loopCounter; i++){
                std::vector <Block *> vector;
                this->receivedProposedBlocks[i] = vector;
            }
        }

        Block *previousBlock = this->blockChain->GetTopBlock();
        //TODO beter receive FROM address
        Block *proposedBlock = Block::FromJSON(message,previousBlock,Ipv4Address("0.0.0.0"));

        //check if node has not already received this proposed block
        for(auto block: this->receivedProposedBlocks[this->loopCounterProposedBlock]){
            if(proposedBlock->IsSameAs(block)){
                //already receive
                return;
            }
        }
        //section where first time proposed block come
        this->receivedProposedBlocks[this->loopCounterProposedBlock].push_back(proposedBlock);
        this->SendMessage(message, this->broadcastSocket);
    }

    Block* AlgorandNodeApp::GetLowerReceivedProposedBlock(int loopCounter) {
        Block *selectedBlock;
        long int lowerNum;
        if(this->receivedProposedBlocks.size() <= loopCounter){
            return NULL;
        }
        if(this->receivedProposedBlocks[loopCounter].size() == 0){
            return NULL;
        }
        selectedBlock = this->receivedProposedBlocks[loopCounter][0];
        lowerNum = selectedBlock->GetId();
        for(auto block: this->receivedProposedBlocks[loopCounter]){
            if(block->GetId() < lowerNum){
                selectedBlock = block;
                lowerNum = selectedBlock->GetId();
            }
        }

        return selectedBlock;
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
//        NS_LOG_INFO("At time " << timeSeconds << " node " << GetNode()->GetId() << " loop " << this->loopCounterProposedBlock << " end with receive new transaction ");

        //create new block
        Block* lastBlock = this->blockChain->GetTopBlock();
        int blockHeight =  this->blockChain->GetBlockchainHeight()+1;
        int validator = GetNode()->GetId();
        Block *createdBlock = this->createdBlock;
        timeSeconds = Simulator::Now().GetSeconds();
        this->createdBlock = new Block(blockHeight, validator, lastBlock, timeSeconds, timeSeconds, Ipv4Address("0.0.0.0"));

        //send new block
        if(this->IsIBlockProposalMember()) {
//            NS_LOG_INFO("At time " << timeSeconds << " node " << GetNode()->GetId() << " loop " << this->loopCounterProposedBlock << " propose block");
            Block *newBlock = new Block(blockHeight, validator, lastBlock, timeSeconds, timeSeconds,
                                        Ipv4Address("0.0.0.0"));
            for (auto trans: createdBlock->GetTransactions()) {
                newBlock->AddTransaction(trans);
            }
            rapidjson::Document transactionDoc = newBlock->ToJSON();
            transactionDoc["type"] = ALGORAND_BLOCK_PROPOSAL;
            this->SendMessage(&transactionDoc, this->broadcastSocket);
        }

        //delete old block
        delete createdBlock;

        //plan next events
        this->loopCounterProposedBlock++;
        this->blockProposeEvent = Simulator::Schedule(Seconds(this->secondsWaitingForBlockReceive), &AlgorandNodeApp::FinishReceiveTransaction, this);
        this->loopCounter = this->loopCounterProposedBlock-1;
        this->phaseCounter = 0;
        this->SoftVoteEvent = Simulator::Schedule(Seconds(this->secondsWaitingForStartSoftVote), &AlgorandNodeApp::SoftVotePhase, this);
    }


    void AlgorandNodeApp::SoftVotePhase() {
        NS_LOG_FUNCTION(this);
        if(!this->IsICommitteeMember()){
            return;
        }
//        NS_LOG_INFO("At time " << Simulator::Now().GetSeconds() << " node " << GetNode()->GetId() << " loop " << this->loopCounter << " start soft vote phase");

        Block *block = this->GetLowerReceivedProposedBlock(this->loopCounter);
        if(block == NULL){
            return;
        }

        int blockId = block->GetId();
        int nodeId = GetNode()->GetId();
//        NS_LOG_INFO("At time " << Simulator::Now().GetSeconds() << " node " << GetNode()->GetId() << " start soft vote phase with " << blockId);
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
        this->AddPhaseNumber();
        this->CertifyVoteEvent = Simulator::Schedule(Seconds(this->secondsWaitingForStartCertify), &AlgorandNodeApp::CertifyVotePhase, this);
    }

    void AlgorandNodeApp::ReceiveSoftVote(rapidjson::Document *message) {
        NS_LOG_FUNCTION(this);
//        NS_LOG_INFO("At time " << Simulator::Now().GetSeconds() << " node " << GetNode()->GetId() << " loop " << this->loopCounter << " receive soft vote");

        if(this->loopCounter >= this->receivedSoftVoteBlockIds.size()){
            int lastSize = this->receivedSoftVoteBlockIds.size();
            this->receivedSoftVoteBlockIds.resize(this->loopCounter+1);
            for(int i=lastSize; i <= this->loopCounter; i++){
                std::vector <int> vector;
                this->receivedSoftVoteBlockIds[i] = vector;
            }
        }

        int recBlockId = (*message)["blockId"].GetInt();

        //check if node has not already received this soft vote
        for(auto blockId: this->receivedSoftVoteBlockIds[this->loopCounter]){
            if(blockId == recBlockId){
                //already receive
                return;
            }
        }

        this->receivedSoftVoteBlockIds[this->loopCounter].push_back(recBlockId);
        this->SendMessage(message, this->broadcastSocket);
    }

    void AlgorandNodeApp::CertifyVotePhase() {
        NS_LOG_FUNCTION(this);
        if(!this->IsICommitteeMember()){
            return;
        }
//        NS_LOG_INFO("At time " << Simulator::Now().GetSeconds() << " node " << GetNode()->GetId() << " loop " << this->loopCounter << " start certify vote phase");

        Block *block = this->GetLowerReceivedProposedBlock(this->loopCounter);
        if(block == NULL){
            return;
        }
        int blockId = block->GetId();
        int nodeId = GetNode()->GetId();
//        NS_LOG_INFO("At time " << Simulator::Now().GetSeconds() << " node " << GetNode()->GetId() << " loop " << this->loopCounter << " start certify vote phase with " << blockId);

        //create messasge
        rapidjson::Document message;
        message.SetObject();
        int messageId = rand();
        message.AddMember("type", ALGORAND_CERTIFY_VOTE, message.GetAllocator());
        message.AddMember("messageId", messageId, message.GetAllocator());
        message.AddMember("blockId", blockId, message.GetAllocator());
        message.AddMember("loopNum", this->GetLoopNumber(), message.GetAllocator());
        message.AddMember("senderId", GetNode()->GetId(), message.GetAllocator());
        message.AddMember("senderStack", this->nodeHelper->GetNodeStack(nodeId), message.GetAllocator());
        message.AddMember("block", block->ToJSON(), message.GetAllocator());

        this->SendMessage(&message, this->broadcastSocket);
    }

    void AlgorandNodeApp::ReceiveCertifyVote(rapidjson::Document *message) {
        NS_LOG_FUNCTION(this);
//        NS_LOG_INFO("At time " << Simulator::Now().GetSeconds() << " node " << GetNode()->GetId() << " loop " << this->loopCounter <<  " receive certify vote");

        if(this->loopCounter >= this->receivedCertifyMessageIds.size()){
            int lastSize = this->receivedCertifyMessageIds.size();
            this->receivedCertifyMessageIds.resize(lastSize+1);
        }

        //check if node has not already received this soft vote
        int recMessageId = (*message)["messageId"].GetInt();
        for(auto messageId: this->receivedCertifyMessageIds){
            if(messageId == recMessageId){
                //already receive
                return;
            }
        }
        this->receivedCertifyMessageIds.push_back(recMessageId);

        //send to others
        this->SendMessage(message, this->broadcastSocket);



        //check status
        int recBlockId = (*message)["blockId"].GetInt();
        this->receivedCertifyVoteBlockIds.push_back(recBlockId);
        int loopNum =  (*message)["loopNum"].GetInt();
        int committeeSize = (this->nodeHelper->ListOfCommitteeMembers(loopNum)).size();
        int counter = 0;
        int minimalCount = round(committeeSize * 0.6);

        for(auto blockId: this->receivedCertifyVoteBlockIds){
            if(blockId == recBlockId){
                counter++;
            }
        }

        if(counter >= minimalCount){
            //add block
            rapidjson::Value &blockObj = (*message)["block"];
            rapidjson::StringBuffer sb;
            rapidjson::Writer<rapidjson::StringBuffer> writer(sb);
            blockObj.Accept(writer);
            std::string s = sb.GetString();
            rapidjson::Document messageBlock;
            messageBlock.Parse(s.c_str());
            Block *previousBlock = this->blockChain->GetTopBlock();
            Block *block = Block::FromJSON(&messageBlock,previousBlock,Ipv4Address("0.0.0.0"));
//            NS_LOG_INFO("At time " << Simulator::Now().GetSeconds() << " node " << GetNode()->GetId() << " loop " << this->loopCounter <<  " adding new block");
            if(this->blockChain->HasBlock(block)){
                return;
            }
            this->blockChain->AddBlock(block);
        }
    }

    void AlgorandNodeApp::PrintProcessInfo() {
        int myId = GetNode()->GetId();
        int recPropBlockCounter = 0;
        for(auto item: this->receivedProposedBlocks){
            recPropBlockCounter += item.size();
        }
        int recSoftVoteCounter = 0;
        for(auto item: this->receivedSoftVoteBlockIds){
            recSoftVoteCounter += item.size();
        }
        int recCertifyVoteCounter = 0;
        std::vector<int> seen;
        int size = this->receivedCertifyVoteBlockIds.size();
        seen.resize(size);
        for(auto item: this->receivedCertifyVoteBlockIds){
            bool seenBool = false;
            for(auto seenItem: seen){
                if(seenItem == item){
                    seenBool = true;
                    break;
                }
            }
            if(!seenBool) {
                seen.push_back(item);
                recCertifyVoteCounter++;
            }
        }
        int inPropseCommite = 0;
        for(int i = 0;i<=this->loopCounterProposedBlock;i++){
            for(auto item: this->nodeHelper->ListOfBlockProposals(i)){
                if(item == myId){
                    inPropseCommite++;
                    break;
                }
            }
        }
        int inVoteCommite = 0;
        for(int i = 0;i<=this->loopCounter;i++){
            for(auto item: this->nodeHelper->ListOfCommitteeMembers(i)){
                if(item == myId){
                    inVoteCommite++;
                    break;
                }
            }
        }

        NS_LOG_FUNCTION(this);
        NS_LOG_INFO("");
        NS_LOG_INFO(" Algorand App " << GetNode()->GetId());
        NS_LOG_INFO("----------------------------------------------------------------------------------------   ");
        NS_LOG_INFO(" Algorand Info: ");
        NS_LOG_INFO(" Loop count (prop. blocks)    |  Loop count (vote)      |");
        NS_LOG_INFO("            " << this->loopCounterProposedBlock+1 << "                |           " << this->loopCounter+1 << "            | ");
        NS_LOG_INFO(" Rec. proposed blocks count   |  Rec. soft vote count   |  Rec. certify vote count  |");
        NS_LOG_INFO("           " << recPropBlockCounter << "               |            " << recSoftVoteCounter << "         |             " << recCertifyVoteCounter << "        | ");
        NS_LOG_INFO(" In propose (committee) count |  In vote committee count |  ");
        NS_LOG_INFO("                " << inPropseCommite << "             |           " << inVoteCommite << "             | ");
        NS_LOG_INFO(" Base Info: ");
        BlockChainNodeApp::PrintProcessInfo();
        NS_LOG_INFO("");
    }
}