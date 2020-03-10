
#ifndef PROJECT_BLOCKCHAIN_H
#define PROJECT_BLOCKCHAIN_H

#include <vector>
#include "ns3/address.h"
#include "ns3/ipv4-address.h"
#include <algorithm>


namespace ns3 {
    class Ipv4Address;

    enum MessagesType {
        INV,              //0
        GET_HEADERS,      //1
        HEADERS,          //2
        GET_BLOCKS,       //3
        BLOCK,            //4
        GET_DATA,         //5
        NO_MESSAGE,       //6
        EXT_INV,          //7
        EXT_GET_HEADERS,  //8
        EXT_HEADERS,      //9
        EXT_GET_BLOCKS,   //10
        CHUNK,            //11
        EXT_GET_DATA,     //12
    };

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
        Block(int blockHeight, int blockSize, int validatorId, Block *previousBlock, double timeCreated, double timeReceived, Ipv4Address receivedFrom);
        int GetBlockHeight() const ;
        int GetBlockSize() const ;
        int GetValidatorId() const ;
        Block *GetPreviousBlock() const ;
        double GetTimeCreated() const ;
        double GetTimeReceived() const ;
//        IPv4Address GetReceivedFrom() const;
        friend bool operator==(const Block &block1, const Block &block2);
    };

    class BlockChain {
    private:
        std::vector <std::vector<Block>> blocks;
        int totalCountOfBlocks;
    public:
        BlockChain();
        int GetTotalCountOfBlocks() const ;
        const Block *GetTopBlock() const ;
        int GetBlockchainHeight() const ;
        bool HasBlock(Block &block) const ;
        void AddBlock(Block &block);
    };
}

#endif //PROJECT_BLOCKCHAIN_H
