
#ifndef PROJECT_ALGORAND_HELPER_H
#define PROJECT_ALGORAND_HELPER_H

#include "node-helper.h"
#include <vector>

namespace ns3 {

    class AlgorandHelper : public NodeHelper {
    private:
        int phaseCounter;
        int loopCounter;
        std::vector<std::vector<int>> blockProposals;
    protected:
        void CreateBlockProposal(int loopNumber);
    public:
        AlgorandHelper(int countOfNodes, long int totalStack);
        int GetPhaseNumber();
        int GetLoopNumber();
        void AddPhaseNumber();
        bool IsBlockProposal(int NoneId, int loopNumber);
        std::vector<int> ListOfBlockProposal(int loopNumber);
    };
}

#endif //PROJECT_ALGORAND_HELPER_H
