
#ifndef PROJECT_ALGORAND_HELPER_H
#define PROJECT_ALGORAND_HELPER_H

#include "node-helper.h"
#include <vector>

namespace ns3 {

    class AlgorandHelper : public NodeHelper {
    private:
        std::vector<int> blockProposals;
        std::vector<std::vector<int>> committeeMembers;
        double committeePercentageSize;
    protected:
        void CreateBlockProposal(int loopNumber);
        void CreateCommitteeMembers(int loopNumber);
    public:
        AlgorandHelper(double committeePercentageSize, int countOfNodes, long int totalStack);
        int NodeOfBlockProposal(int loopNumber);
        std::vector<int> ListOfCommitteeMembers(int loopNumber);
    };
}

#endif //PROJECT_ALGORAND_HELPER_H
