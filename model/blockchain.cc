

#include "ns3/application.h"
#include "ns3/event-id.h"
#include "ns3/ptr.h"
#include "ns3/traced-callback.h"
#include "ns3/address.h"
#include "ns3/log.h"
#include "ns3/network-module.h"
#include "ns3/applications-module.h"
#include "blockchain.h"
#include "messages.h"
#include "constants.h"
#include <vector>
#include "../../network/utils/ipv4-address.h"
#include "ns3/address.h"
#include "ns3/ipv4-address.h"

namespace ns3 {
    NS_LOG_COMPONENT_DEFINE ("BlockChain");

    class Ipv4Address;

    /*------------ Transaction ---------------*/

    Transaction::Transaction(int senderId, int receiverId) {
        this->id = rand(); //TODO: make it clever and better (long int)
        this->senderId = senderId;
        this->receiverId = receiverId;
    }

    Transaction::Transaction(long int id, int senderId, int receiverId) {
        this->id = id;
        this->senderId = senderId;
        this->receiverId = receiverId;
    }

    int Transaction::GetReceiverId() {
        return this->receiverId;
    }

    int Transaction::GetSenderId() {
        return this->senderId;
    }

    int Transaction::GetId(){
        return this->id;
    }

    rapidjson::Document Transaction::ToJSON() {
        rapidjson::Document message;
        message.SetObject();

        message.AddMember("type", NEW_TRANSACTION, message.GetAllocator());
        message.AddMember("id", this->id, message.GetAllocator());
        message.AddMember("senderId", this->senderId, message.GetAllocator());
        message.AddMember("receiverId", this->receiverId, message.GetAllocator());

        return message;
    }

    Transaction *Transaction::FromJSON(rapidjson::Document *document) {
        Transaction *transaction = new Transaction((*document)["id"].GetInt(), (*document)["senderId"].GetInt(), (*document)["receiverId"].GetInt());
        return transaction;
    }

    /*------------ BLOCK ---------------*/

    Block::Block(int blockHeight, int validatorId, Block *previousBlock, double timeCreated,
                 double timeReceived,
                 Ipv4Address receivedFrom) {
        this->id = rand(); //TODO: make it clever and better (long int)
        this->blockHeight = blockHeight;
        this->validatorId = validatorId;
        this->previousBlock = previousBlock;
        this->timeCreated = timeCreated;
        this->timeReceived = timeReceived;
        this->receivedFrom = receivedFrom;
        this->fullBlockCounter = 0;
    }


    int Block::GetBlockHeight() {
        return blockHeight;
    }

    int Block::GetBlockSize(){
        return this->transactions.size();
    }

    long int Block::GetId() {
        return this->id;
    }

    void Block::SetId(long int id) {
        this->id = id;
    }

    bool Block::IsBlockFull(){
        return this->GetBlockSize() >= constants.maxTransactionsPerBlock;
    }

    int Block::GetValidatorId() {
        return validatorId;
    }

    Block *Block::GetPreviousBlock() {
        return previousBlock;
    }

    double Block::GetTimeCreated() {
        return timeCreated;
    }

    double Block::GetTimeReceived() {
        return timeReceived;
    }

//    IPv4Address Block::GetReceivedFrom() {
//        return receivedFrom;
//    }

    void Block::AddTransaction(Transaction *transaction){
        if(this->IsBlockFull()){
            this->fullBlockCounter++;
            return;
        }
        this->transactions.push_back(transaction);
    }

    std::vector <Transaction*> Block::GetTransactions(){
        return this->transactions;
    }

    std::vector <Transaction*> Block::GetTransactionsByReceiver(int receiverId){
        std::vector <Transaction*> results;
        for(auto &trans: this->transactions) {
            if(trans->GetReceiverId() == receiverId){
                results.push_back(trans);
            }
        }
        return results;
    }

    std::vector <Transaction*> Block::GetTransactionsBySender(int senderId){
        std::vector <Transaction*> results;
        for(auto &trans: this->transactions) {
            if(trans->GetReceiverId() == senderId){
                results.push_back(trans);
            }
        }
        return results;
    }

    bool Block::IsSameAs(Block *block){
        if(this->GetBlockHeight() == block->GetBlockHeight() && this->GetValidatorId() == block->GetValidatorId()){
            return true;
        }
        return false;
    }

    bool Block::IsExactSameAs(Block *block){
        if(this->GetBlockHeight() == block->GetBlockHeight()
                && this->GetValidatorId() == block->GetValidatorId()
                && this->GetId() == block->GetId()){
            return true;
        }
        return false;
    }

    rapidjson::Document Block::ToJSON() {
        rapidjson::Document message;
        message.SetObject();

        message.AddMember("type", NEW_BLOCK, message.GetAllocator());
        message.AddMember("id", this->id, message.GetAllocator());
        message.AddMember("blockHeight", this->blockHeight, message.GetAllocator());
        message.AddMember("blockSize", this->GetBlockSize(), message.GetAllocator());
        message.AddMember("validatorId", this->validatorId, message.GetAllocator());
        message.AddMember("timeCreated", this->timeCreated, message.GetAllocator());

        rapidjson::Value array(rapidjson::kArrayType);
        for(auto trans: this->transactions) {
            rapidjson::Document transDoc = trans->ToJSON();
            array.PushBack(transDoc, message.GetAllocator());
        }
        message.AddMember("transactions", array, message.GetAllocator());

        return message;
    }


    Block *Block::FromJSON(rapidjson::Document *document, Block *previousBlock, Ipv4Address receivedFrom) {
        double timeSeconds = Simulator::Now().GetSeconds();
        Block *block = new Block(
                (*document)["blockHeight"].GetInt(),
                (*document)["validatorId"].GetInt(),
                previousBlock,
                (*document)["validatorId"].GetDouble(),
                timeSeconds,
                receivedFrom
        );
        block->SetId((*document)["id"].GetInt());
        for(int i=0; i<(*document)["transactions"].Size(); i++){
            auto docTrans = (*document)["transactions"][i].GetObject();
            rapidjson::Document message;
            message.SetObject();
            message.AddMember("type", NEW_TRANSACTION, message.GetAllocator());
            message.AddMember("id", docTrans["id"].GetInt(), message.GetAllocator());
            message.AddMember("senderId", docTrans["senderId"].GetInt(), message.GetAllocator());
            message.AddMember("receiverId", docTrans["receiverId"].GetInt(), message.GetAllocator());

            Transaction *transaction = Transaction::FromJSON(&message);
            block->AddTransaction(transaction);
        }
        return block;
    }

    int Block::GetFullBlockCounter() {
        return this->fullBlockCounter;
    }

    void Block::SetFullBlockCounter(int counter) {
        this->fullBlockCounter = counter;
    }

    /*------------ BLOCKChain ---------------*/


    BlockChain::BlockChain() {
        this->totalCountOfBlocks = 0;
        Block* block = new Block(0, 0, nullptr, 0, 0, Ipv4Address("0.0.0.0"));
        this->AddBlock(block);
    }

    int BlockChain::GetTotalCountOfBlocks(){
        return this->totalCountOfBlocks;
    }

    Block *BlockChain::GetTopBlock() {
        if(this->blocks.size() < 1){
            return NULL;
        }
        return this->blocks[this->blocks.size()-1][0];
    }

    int BlockChain::GetBlockchainHeight() {
        Block *topBlock = this->GetTopBlock();
        if(topBlock){
            return topBlock->GetBlockHeight();
        }
        return 0;
    }

    bool BlockChain::HasBlock(Block *block) {
        if (block->GetBlockHeight() > this->GetBlockchainHeight()) {
            return false;
        }
        auto column = this->blocks[block->GetBlockHeight()];
        for (auto &value: column) {
            if (block->IsSameAs(value)) {
                return true;
            }
        }
        return false;
    }

    void BlockChain::AddBlock(Block *block) {
        if(block->GetBlockHeight() < 0){
            NS_FATAL_ERROR("Height of block is not positive");
            return;
        }
        if (this->blocks.size() == 0) {
            // add genesis block
            std::vector<Block*> newColumn;
            newColumn.push_back(block);
            this->blocks.push_back(newColumn);
        } else if (block->GetBlockHeight() > this->GetBlockchainHeight()) {
            //add block to the end of rows
            int emptyColums = this->GetBlockchainHeight() - block->GetBlockHeight() - 1;
            for (int i = 0; i < emptyColums; i++) {
                std::vector<Block*> newColumn;
                this->blocks.push_back(newColumn);
            }
            std::vector<Block*> newColumn;
            newColumn.push_back(block);
            this->blocks.push_back(newColumn);
        } else {
            // add to existing column
            this->blocks[block->GetBlockHeight()].push_back(block);
        }
        this->totalCountOfBlocks++;
    }

    void BlockChain::PrintInfo() {
        NS_LOG_FUNCTION(this);
        NS_LOG_INFO("Count of blocks: " << this->GetTotalCountOfBlocks());
        NS_LOG_INFO("BlockChain lenght: " << this->blocks.size());
        int transactionCount = 0;
        int nonAddedTransactionsCount = 0;
        for(auto blockHItem: this->blocks) {
            for(Block *block: blockHItem){
                transactionCount = transactionCount + block->GetBlockSize();
                nonAddedTransactionsCount = nonAddedTransactionsCount + block->GetFullBlockCounter();
            }
        }
        NS_LOG_INFO("Count of transactions: " << transactionCount);
        NS_LOG_INFO("Count of transactions (over of block size): " << nonAddedTransactionsCount);
    }
}

