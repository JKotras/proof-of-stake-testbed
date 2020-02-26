#ifndef PROJECT_BLOCKCHAIN_H
#define PROJECT_BLOCKCHAIN_H

#include <vector>

namespace ns3 {
    // It is not need simulated content of block (I only define size)
    class Block {
    public:
        Block(int blockHeight, int blockSize, Block previousBlock);
    private:
        int blockHeight;
        int blockSize;
        Block previousBlock;
        double timeCreated;

    };

    class BlockChain {
    public:
        Blockchain(void);
    private:
        std::vector<Block> blocks;
    };
}

#endif //PROJECT_BLOCKCHAIN_H

