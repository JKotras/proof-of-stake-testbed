
#ifndef PROJECT_BLOCKCHAIN_H
#define PROJECT_BLOCKCHAIN_H

#include <vector>
#include "ns3/address.h"
#include "ns3/ipv4-address.h"
#include <algorithm>


namespace ns3 {
    class Ipv4Address;

    class Satoshi{
    protected:
        int owner;
    public:
        Satoshi(int owner);
        bool IsOwner(int ownerId) const;
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
        std::vector <Satoshi> satoshis;
    public:
        Block(int blockHeight, int validatorId, Block *previousBlock, double timeCreated, double timeReceived, Ipv4Address receivedFrom);
        int GetBlockHeight() const ;
        int GetBlockSize() const ;
        bool IsBlockFull() const;
        int GetValidatorId() const ;
        Block *GetPreviousBlock() const ;
        double GetTimeCreated() const ;
        double GetTimeReceived() const ;
//        IPv4Address GetReceivedFrom() const;
        void SetSatoshis(std::vector <Satoshi> satoshis);
        void AddSahoshi(Satoshi &satoshi);
        std::vector <Satoshi> GetSatoshis() const;
        std::vector <Satoshi> GetSatoshisByOwner(int ownerId) const;
        std::vector <Satoshi> GetAllChainSatoshisByOwner(int ownerId) const;
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
        std::vector <Satoshi> GetAllSatoshis() const;
    };
}

#endif //PROJECT_BLOCKCHAIN_H
