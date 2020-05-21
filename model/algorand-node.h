#ifndef PROJECT_ALGORAND_NODE_H
#define PROJECT_ALGORAND_NODE_H

#include "blockchain-node.h"
#include "ns3/event-id.h"
#include "algorand-helper.h"

namespace ns3 {

    class AlgorandNodeApp : public BlockChainNodeApp {
        int phaseCounter;
        int loopCounter;
        Block* createdBlock;
        double secondsWaitingForBlockReceive;
        EventId blockProposeEvent;
        std::vector<std::vector <Block *>> receivedProposedBlocks;
    protected:
        AlgorandHelper *nodeHelper;
        bool IsICommitteeMember();
        void ProposeBlock();
        bool HandleCustomRead(Ptr <Packet> packet, Address from, std::string receivedData) override;
    public:
        AlgorandNodeApp(AlgorandHelper *nodeHelper);
        int GetPhaseNumber();
        int GetLoopNumber();
        void AddPhaseNumber();
        void ReceiveProposedBlock(rapidjson::Document *message);
        /**
         * Handle new received transaction
         * @param receivedData
         */
        void ReceiveNewTransaction(rapidjson::Document *message) override ;
    };
}

#endif //PROJECT_ALGORAND_NODE_H
