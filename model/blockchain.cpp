#include "ns3/application.h"
#include "ns3/event-id.h"
#include "ns3/ptr.h"
#include "ns3/traced-callback.h"
#include "ns3/address.h"
#include "ns3/log.h"
#include <vector>

namespace ns3 {
    class Block {
    private:
        int blockHeight;
        int blockSize;
        int validatorId;
        Block *previousBlock;
        double timeCreated;
        double timeReceived;
        Ipv4Address receivedFrom;

    public:
        Block(int blockHeight, int blockSize, int validatorId, Block previousBlock, double timeCreated,
              double timeReceived,
              Ipv4Address receivedFrom) {
            this->blockHeight = blockHeight;
            this->blockSize = blockSize;
            this->validatorId = validatorId;
            this->previousBlock = previousBlock;
            this->timeCreated = timeCreated;
            this->timeReceived = timeReceived;
            this->receivedFrom = receivedFrom;
        }


        int GetBlockHeight() {
            return blockHeight;
        }

        int GetBlockSize() {
            return blockSize;
        }

        int GetValidatorId() {
            return validatorId;
        }

        Block GetPreviousBlock() {
            return previousBlock;
        }

        double GetTimeCreated() {
            return timeCreated;
        }

        double GetTimeReceived() {
            return timeReceived;
        }

        IPv4Address GetReceivedFrom() {
            return receivedFrom;
        }

    };

    class BlockChain {
    private:
        std::vector <std::vector<Block>> blocks;
        int totalCountOfBlocks;
    public:
        BlockChain() {
            Block block(0,0,0, nullptr, 0,0,Ipv4Address("0.0.0.0"));
            this->AddBlock(block);
            this->totalCountOfBlocks;
        }

        int GetTotalCountOfBlocks(){
            return this->totalCountOfBlocks;
        }

        Block *GetTopBlock(){
            return this->blocks.back()[0];
        }

        int GetBlockchainHeight(){
            return this->GetTopBlock()->GetBlockHeight();
        }

        bool HasBlock(Block &block){
            if(block.GetBlockHeight() > this->GetBlockchainHeight()){
                return false;
            }
            std::vector<Block> column = this->blocks[block.GetBlockHeight()];
            for(auto const& value: column) {
                if(block == value){
                    return true;
                }
            }
            return false;
        }

        void AddBlock(Block &block){
            if (this->blocks.size() == 0) {
                // add genesis block
                std::vector <Block> newColumn(1, block);
                this->blocks.push_back(newColumn);
            } else if (block.GetBlockHeight() > this->GetBlockchainHeight()){
                //add block to the end of rows
                int emptyColums = this->GetBlockchainHeight() - block.GetBlockHeight() - 1;
                for(int i = 0; i < emptyColums; i++){
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

    };
}

