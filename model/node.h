
#ifndef PROJECT_NODE_H
#define PROJECT_NODE_H

#include "../../network/model/application.h"
#include "../utils/rsa.h"
#include "ns3/event-id.h"
#include "ns3/ptr.h"
#include "ns3/address.h"

namespace ns3 {

    class Socket;

    class BlockChainNodeApp : public Application {
    private:
        Keys keys;
    public:
        BlockChainNodeApp();
        static TypeId GetTypeId (void);
        void HandleRead(Ptr<Socket> socket);
        uint16_t listenPort;
    };
}

#endif //PROJECT_NODE_H

