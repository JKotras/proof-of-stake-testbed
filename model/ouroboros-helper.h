
#ifndef PROJECT_OUROBOROS_HELPER_H
#define PROJECT_OUROBOROS_HELPER_H

#include "node-helper.h"
#include <vector>

namespace ns3 {

    class OuroborosHelper : public NodeHelper {
    protected:
        std::vector<std::vector<int>> slotLeaders;
        double slotSizeSeconds;    //basic epoch size is about 20 seconds
        int slotsInEpoch;       //10 * security parameter
    public:
        OuroborosHelper(double slotSizeSeconds, int securityParameter, int countOfNodes, long int totalStack);
        int GetSlotLeader(int slotNumber, int epochNumber);
        int GetSlotNumber();
        int GetEpochNumber();
        double GetSlotSizeSeconds() const;

    };
}

#endif //PROJECT_OUROBOROS_HELPER_H
