
#ifndef PROJECT_OUROBOROS_HELPER_H
#define PROJECT_OUROBOROS_HELPER_H

#include "node-helper.h"
#include <vector>

namespace ns3 {

    class OuroborosHelper : public NodeHelper {
    protected:
        std::vector<std::vector<int>> slotLeaders;
    public:
        OuroborosHelper(int countOfNodes, long int totalStack);
        int GetSlotLeader(int slotNumber, int epochNumber);

    };
}

#endif //PROJECT_OUROBOROS_HELPER_H
