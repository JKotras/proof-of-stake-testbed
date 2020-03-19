

#include "ns3/application.h"
#include "ns3/event-id.h"
#include "ns3/ptr.h"
#include "ns3/traced-callback.h"
#include "ns3/address.h"
#include "ns3/log.h"
#include "ns3/network-module.h"
#include "ns3/applications-module.h"
#include "blockchain.h"
#include <vector>
#include "../../network/utils/ipv4-address.h"
#include "ns3/address.h"
#include "ns3/ipv4-address.h"

namespace ns3 {

    class Ipv4Address;

    int maxBlockSize = 100;

    Satoshi::Satoshi(int owner) {
        this->owner = owner;
    }

    bool Satoshi::IsOwner(int ownerId) const {
        return this->owner == ownerId;
    }

    /*------------ BLOCK ---------------*/

    Block::Block(int blockHeight, int validatorId, Block *previousBlock, double timeCreated,
                 double timeReceived,
                 Ipv4Address receivedFrom) {
        this->blockHeight = blockHeight;
        this->validatorId = validatorId;
        this->previousBlock = previousBlock;
        this->timeCreated = timeCreated;
        this->timeReceived = timeReceived;
        this->receivedFrom = receivedFrom;
    }


    int Block::GetBlockHeight() const {
        return blockHeight;
    }

    int Block::GetBlockSize() const {
        return this->satoshis.size();
    }

    bool Block::IsBlockFull() const {
        return this->satoshis.size() >= maxBlockSize;
    }

    int Block::GetValidatorId() const {
        return validatorId;
    }

    Block *Block::GetPreviousBlock() const {
        return previousBlock;
    }

    double Block::GetTimeCreated() const {
        return timeCreated;
    }

    double Block::GetTimeReceived() const {
        return timeReceived;
    }

//    IPv4Address Block::GetReceivedFrom() {
//        return receivedFrom;
//    }

    void Block::SetSatoshis(std::vector <Satoshi> satoshis){
        this->satoshis = satoshis;
    }

    void Block::AddSahoshi(Satoshi &satoshi){
        if(this->IsBlockFull()){
            //TODO exception
            return;
        }
        this->satoshis.push_back(satoshi);
    }

    std::vector <Satoshi> Block::GetSatoshis() const {
        return this->satoshis;
    }

    std::vector <Satoshi> Block::GetSatoshisByOwner(int ownerId) const{
        std::vector <Satoshi> results;
        for(auto const& satoshi: this->satoshis) {
            if(satoshi.IsOwner(ownerId)){
                results.push_back(satoshi);
            }
        }
        return results;
    }

    std::vector <Satoshi> Block::GetAllChainSatoshisByOwner(int ownerId) const{
        if(!previousBlock){
            std::vector <Satoshi> vector;
            auto res = this->GetAllChainSatoshisByOwner(ownerId);
            vector.insert(vector.begin(), res.begin(), res.end());
            auto local = this->GetSatoshisByOwner(ownerId);
            vector.insert(vector.begin(), local.begin(), local.end());
            return vector;
        } else {
            return this->GetAllChainSatoshisByOwner(ownerId);
        }
    }

    bool operator==(const Block &block1, const Block &block2) {
        if(block1.GetBlockHeight() == block2.GetBlockHeight() && block1.GetValidatorId() == block2.GetValidatorId()){
            return true;
        }
        return false;
    }

    /*------------ BLOCKChain ---------------*/


    BlockChain::BlockChain() {
        Block block(0, 0, nullptr, 0, 0, Ipv4Address("0.0.0.0"));
        this->AddBlock(block);
    }

    int BlockChain::GetTotalCountOfBlocks() const {
        return this->totalCountOfBlocks;
    }

    const Block *BlockChain::GetTopBlock() const {
        return &(this->blocks[this->blocks.size()-1][0]);
    }

    int BlockChain::GetBlockchainHeight() const {
        return this->GetTopBlock()->GetBlockHeight();
    }

    bool BlockChain::HasBlock(Block &block) const {
        if (block.GetBlockHeight() > this->GetBlockchainHeight()) {
            return false;
        }
        std::vector <Block> column = this->blocks[block.GetBlockHeight()];
        for (auto const &value: column) {
            if (block == value) {
                return true;
            }
        }
        return false;
    }

    void BlockChain::AddBlock(Block &block) {
        if (this->blocks.size() == 0) {
            // add genesis block
            std::vector <Block> newColumn(1, block);
            this->blocks.push_back(newColumn);
        } else if (block.GetBlockHeight() > this->GetBlockchainHeight()) {
            //add block to the end of rows
            int emptyColums = this->GetBlockchainHeight() - block.GetBlockHeight() - 1;
            for (int i = 0; i < emptyColums; i++) {
                std::vector <Block> newColumn;
                this->blocks.push_back(newColumn);
            }
            std::vector <Block> newColumn(1, block);
            this->blocks.push_back(newColumn);
        } else {
            // add to existing column
            this->blocks[block.GetBlockHeight()].push_back(block);
        }
        this->totalCountOfBlocks++;
    }


}

