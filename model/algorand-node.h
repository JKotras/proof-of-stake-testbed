#ifndef PROJECT_ALGORAND_NODE_H
#define PROJECT_ALGORAND_NODE_H

#include "blockchain-node.h"
#include "ns3/event-id.h"
#include "algorand-helper.h"

namespace ns3 {

    class AlgorandNodeApp : public BlockChainNodeApp {

    public:
        AlgorandNodeApp(AlgorandHelper *nodeHelper);
    };
}

#endif //PROJECT_ALGORAND_NODE_H
