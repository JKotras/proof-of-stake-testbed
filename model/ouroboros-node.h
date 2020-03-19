//
// Created by honza on 15.3.20.
//

#ifndef PROJECT_OUROBOROS_NODE_H
#define PROJECT_OUROBOROS_NODE_H

#include "blockchain-node.h"
#include "ns3/event-id.h"


namespace ns3 {

    class OuroborosNodeApp : public BlockChainNodeApp {
    private:
        int slotSizeSeconds;
        int slotsInEpoch;       //10 * security parameter
        EventId sendingSeedNextEvent;

    protected:
        int CreateSecret();

        int GetSlotNumber();

        int GetEpochNumber();

        void StartApplication(void);

        void StopApplication(void);

    public:
        OuroborosNodeApp(int slotSizeSeconds, int securityParameter);

        void SendEpochSeed();

        void ReceiveEpochSeed();
    };
}

#endif //PROJECT_OUROBOROS_NODE_H
