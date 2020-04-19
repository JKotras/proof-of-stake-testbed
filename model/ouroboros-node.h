//
// Created by honza on 15.3.20.
//

#ifndef PROJECT_OUROBOROS_NODE_H
#define PROJECT_OUROBOROS_NODE_H

#include "blockchain-node.h"
#include "ns3/event-id.h"
#include "node-helper.h"

namespace ns3 {

    class OuroborosNodeApp : public BlockChainNodeApp {
    private:
        int slotSizeSeconds;    //basic epoch size is about 20 seconds
        int slotsInEpoch;       //10 * security parameter
        EventId sendingSeedNextEvent;
        std::vector<Ipv4Address> nodesAddresses;
        std::vector<std::vector<int>> receivedSeeds;   //vector of epochs (vector indexed by nodes contain received epoch num)

    protected:
        int CreateSecret();

        int GetSlotNumber();

        int GetEpochNumber();

        int GetSlotLeader(int slotNumber);

        void StartApplication(void);

        void StopApplication(void);

        bool HandleCustomRead(Ptr <Packet> packet, Address from, std::string receivedData) override;

    public:
        OuroborosNodeApp(int slotSizeSeconds, int securityParameter, NodeHelper *nodeHelper);

        void SendEpochSeed();

        void ReceiveEpochSeed(std::string receivedData);
    };
}

#endif //PROJECT_OUROBOROS_NODE_H
