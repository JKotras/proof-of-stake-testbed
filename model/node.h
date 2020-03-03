
#ifndef PROJECT_NODE_H
#define PROJECT_NODE_H

#include "ns3/application.h"
#include "../utils/rsa.h"
#include "ns3/event-id.h"
#include "ns3/ptr.h"
#include "ns3/address.h"
#include "blockchain.h"

namespace ns3 {

    class Socket;
    class EventId;

    class BlockChainNodeApp : public Application {
    protected:
        Keys keys;                           //node RSA
        BlockChain blockChain;               //node's blockchain
        Ptr<Socket> listenSocket;            //listening socket
        Address multicastLocal;              //local multicast address
//        EvenID nextEvent;                    // next event to process
        virtual void StartApplication (void);
        virtual void StopApplication (void);
        /**
         * Handle received packets
         * @param socket
         */
        void HandleRead (Ptr<Socket> socket);
        /**
         * Send messages by scheduler
         */
        void Send(void);
    public:
        BlockChainNodeApp();
        static TypeId GetTypeId (void);
        Ptr <Socket> GetListenPort(void) const;
    };
}

#endif //PROJECT_NODE_H

