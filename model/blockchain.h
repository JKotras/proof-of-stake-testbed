
#ifndef PROJECT_BLOCKCHAIN_H
#define PROJECT_BLOCKCHAIN_H

#include <vector>
#include "ns3/address.h"
#include "ns3/ipv4-address.h"
#include <algorithm>
#include "../../../rapidjson/document.h"
#include "../../../rapidjson/writer.h"
#include "../../../rapidjson/stringbuffer.h"


namespace ns3 {
    class Ipv4Address;

    class Transaction{
    private:
        long int id;
        int senderId;
        int receiverId;
    public:
        Transaction(int senderId, int receiverId);
        Transaction(long int id, int senderId, int receiverId);
        int GetReceiverId() const;
        int GetSenderId() const;
        int GetId() const;
        rapidjson::Document ToJSON();
        static Transaction FromJSON(std::string data);
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
        std::vector <Transaction> transactions;
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
        void AddTransaction(Transaction &transaction);
        std::vector <Transaction> GetTransactions() const;
        std::vector <Transaction> GetTransactionsByReceiver(int receiverId) const;
        std::vector <Transaction> GetTransactionsBySender(int senderId) const;
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
