//
// Created by honza on 3.3.20.
//

#ifndef PROJECT_NODE_HELPER_H
#define PROJECT_NODE_HELPER_H

#include "ns3/application-container.h"
#include "ns3/node-container.h"
#include "ns3/object-factory.h"
#include "../model/node.h"

namespace ns3 {
    class BlockChainNodeAppHelper {
    public:
        BlockChainNodeAppHelper();
        ApplicationContainer Install (NodeContainer c);

    protected:
    private:
        ObjectFactory objectFactory;
    };
}

#endif //PROJECT_NODE_HELPER_H

