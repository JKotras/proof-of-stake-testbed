
#include "node.h"
#include "../utils/rsa.cc"

namespace ns3 {
    BlockChainNodeApp::BlockChainNodeApp() {
        this->keys = generate_keys();
        std::cout << this->keys.public_key.first << std::endl;
    }

}

