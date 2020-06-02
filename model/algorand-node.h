#ifndef PROJECT_ALGORAND_NODE_H
#define PROJECT_ALGORAND_NODE_H

#include "blockchain-node.h"
#include "ns3/event-id.h"
#include "algorand-helper.h"

namespace ns3 {

    class AlgorandNodeApp : public BlockChainNodeApp {
        int phaseCounter;
        int loopCounter;
        int loopCounterProposedBlock;
        Block *createdBlock;
        double secondsWaitingForBlockReceive;
        double secondsWaitingForStartSoftVote;
        double secondsWaitingForStartCertify;
        EventId blockProposeEvent;
        EventId SoftVoteEvent;
        EventId CertifyVoteEvent;
        std::vector <std::vector<Block *>> receivedProposedBlocks;
        std::vector <std::vector<int>> receivedSoftVoteBlockIds;
        std::vector <int> receivedCertifyMessageIds;
        std::vector <int> receivedCertifyVoteBlockIds;
    protected:
        AlgorandHelper *nodeHelper;

        bool IsIBlockProposalMember();

        bool IsICommitteeMember();

        void ProposeBlock();

        bool HandleCustomRead(Ptr <Packet> packet, Address from, std::string receivedData) override;

        void StartApplication(void);

        void StopApplication(void);

        void FinishReceiveTransaction();    //finish first phase

        void SoftVotePhase();

        void CertifyVotePhase();

        Block *GetLowerReceivedProposedBlock(int loopCounter);

    public:
        AlgorandNodeApp(AlgorandHelper *nodeHelper);

        int GetPhaseNumber();

        int GetLoopNumber();

        void AddLoopNumber();

        void AddPhaseNumber();

        void ReceiveProposedBlock(rapidjson::Document *message);

        void ReceiveSoftVote(rapidjson::Document *message);

        void ReceiveCertifyVote(rapidjson::Document *message);

        /**
         * Handle new received transaction
         * @param receivedData
         */
        void ReceiveNewTransaction(rapidjson::Document *message) override;
    };
}

#endif //PROJECT_ALGORAND_NODE_H
