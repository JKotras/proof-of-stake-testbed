
#ifndef PROJECT_NODE_H
#define PROJECT_NODE_H

#include "ns3/core-module.h"
#include "ns3/internet-module.h"
//#include "ns3/application.h"
#include "../../network/model/application.h"
#include "ns3/event-id.h"
#include "ns3/ptr.h"
#include "ns3/address.h"
#include "blockchain.h"
#include "messages.h"
#include "../utils/rsa.h"
#include "../../../rapidjson/document.h"
#include "../../../rapidjson/writer.h"
#include "../../../rapidjson/stringbuffer.h"

namespace ns3 {

    class Socket;
    class EventId;

    class BlockChainNodeApp : public Application {
    protected:
        int id;
        Keys keys;                                                  //node RSA
        BlockChain blockChain;                                      //node's blockchain
        Ptr<Socket> listenSocket;                                   //listening socket
        Ptr<Socket> broadcastSocket;                                //broadcastfa socket
        std::vector<Ipv4Address> nodesAddresses;                    //list of all nodes addresses
        std::map<Ipv4Address, Ptr<Socket>> nodesSockets;            //sockets to all nodes
        Address multicastLocal;                                     //local multicast address
        EventId nextEvent;                                          // next event to process
        Ipv4InterfaceContainer netContainer;                        // container of whole network
        virtual void StartApplication (void);
        virtual void StopApplication (void);

        /**
         * Handle received packets
         * @param socket
         */
        void HandleRead (Ptr<Socket> socket);

        /**
         * Handle an incomming conneciton
         * @param socket
         * @param address
         */
        void HandleConnectionAccept(Ptr<Socket> socket, const Address& address);

        /**
         * handle conneciton end or error
         * @param socket
         */
        void HandleConnectionEnd(Ptr<Socket> socket);

        /**
         * Handle when receive new block
         * @param block
         */
        void ReceiveBlock(const Block &block);

        /**
         * Handle when receive more new blocks
         * @param blocks
         */
        void ReceiveBlocks(std::vector <Block> &blocks);
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
        /**
         *
         * @param peers
         */
        void SetNodesAddresses(std::vector <Ipv4Address> &peers);
        /**
         *
         * @param message
         * @param outgoingSocket
         */
        void SendMessage(rapidjson::Document &message, Ptr<Socket> outgoingSocket);
        /**
         *
         * @param message
         * @param outgoingAddress
         */
        void SendMessage(rapidjson::Document &message, Address &outgoingAddress);
    };
}

#endif //PROJECT_NODE_H

