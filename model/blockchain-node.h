
#ifndef PROJECT_NODE_H
#define PROJECT_NODE_H

#include "ns3/core-module.h"
#include "ns3/internet-module.h"
#include "ns3/application.h"
#include "ns3/event-id.h"
#include "ns3/ptr.h"
#include "ns3/address.h"
#include "blockchain.h"
#include "../utils/rsa.h"

namespace ns3 {

    class Socket;
    class EventId;

    class BlockChainNodeApp : public Application {
    protected:
        Keys keys;                           //node RSA
        BlockChain blockChain;               //node's blockchain
        Ptr<Socket> listenSocket;            //listening socket
        Ptr<Socket> broadcastSocket;             //broad socket
        Address multicastLocal;              //local multicast address
        EventId nextEvent;                    // next event to process
        Ipv4InterfaceContainer netContainer;  // container of whole network
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
        /**
         * Schedule future Send
         * @param dt
         */
        void ScheduleSend (Time dt);
    public:
        BlockChainNodeApp();
        BlockChainNodeApp(Ipv4InterfaceContainer netContainer);
        static TypeId GetTypeId (void);
        Ptr <Socket> GetListenPort(void) const;
    };
}

#endif //PROJECT_NODE_H

