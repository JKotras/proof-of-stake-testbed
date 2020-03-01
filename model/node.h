
#ifndef PROJECT_NODE_H
#define PROJECT_NODE_H

#include "../../network/model/application.h"
#include "../utils/rsa.h"

namespace ns3 {

    class BlockChainNodeApp : public Application {
    private:
        Keys keys;
    public:
        BlockChainNodeApp();
    };
}

#endif //PROJECT_NODE_H

