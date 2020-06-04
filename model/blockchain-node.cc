#include "ns3/internet-module.h"
#include <stdlib.h>
#include <random>
#include <chrono>
#include "ns3/log.h"
#include "ns3/ipv4-address.h"
#include "ns3/ipv6-address.h"
#include "ns3/address-utils.h"
#include "ns3/nstime.h"
#include "ns3/inet-socket-address.h"
#include "ns3/inet6-socket-address.h"
#include "ns3/socket.h"
#include "ns3/udp-socket.h"
#include "ns3/simulator.h"
#include "ns3/socket-factory.h"
#include "ns3/packet.h"
#include "ns3/uinteger.h"
#include "ns3/event-id.h"
#include "blockchain-node.h"
#include "constants.h"
#include "../utils/rsa.h"

namespace ns3 {
    NS_LOG_COMPONENT_DEFINE ("BlockChainNodeApp");
    NS_OBJECT_ENSURE_REGISTERED (BlockChainNodeApp);

    BlockChainNodeApp::BlockChainNodeApp(Ipv4InterfaceContainer netContainer, NodeHelper *nodeHelper) {
        this->netContainer = netContainer;
        this->Init(nodeHelper);
    }

    BlockChainNodeApp::BlockChainNodeApp(NodeHelper *nodeHelper) {
        this->Init(nodeHelper);
    }

    void BlockChainNodeApp::Init(NodeHelper *nodeHelper) {
        this->nodeHelper = nodeHelper;
        this->blockChain = new BlockChain();
        this->listenSocket = 0;
        this->broadcastSocket = 0;
        this->keys = generate_keys();

        //rnd generator
        unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
        std::default_random_engine generator (seed);
        this->generator = generator;
        //poisson
        std::poisson_distribution<int> d(constants.poissonDistributionMeanMiliSeconds);
        this->transactionGenerationDistribution = d;

    }

    TypeId BlockChainNodeApp::GetTypeId() {
        static TypeId tid = TypeId("ns3::BlockChainNodeApp")
                .SetParent<Application>()
                .SetGroupName("Applications");
        return tid;
    }

    Ptr <Socket> BlockChainNodeApp::GetListenPort() const {
        return this->listenSocket;
    };

    void BlockChainNodeApp::StartApplication() {
        NS_LOG_FUNCTION(this);
        NS_LOG_INFO("Starting App " << GetNode()->GetId() << " stack: " << this->nodeHelper->GetNodeStack(GetNode()->GetId()));

        // listen Socket
        if (!this->listenSocket) {
            TypeId tid = TypeId::LookupByName("ns3::UdpSocketFactory");     //udp
            this->listenSocket = Socket::CreateSocket(GetNode(), tid);
            InetSocketAddress local = InetSocketAddress(Ipv4Address::GetAny(), 655);
            this->listenSocket->Bind(local);
            this->listenSocket->Listen();
            this->listenSocket->ShutdownSend();
            if (addressUtils::IsMulticast(this->multicastLocal)) {
                Ptr <UdpSocket> udpSocket = DynamicCast<UdpSocket>(this->listenSocket);
                if (udpSocket) {
                    // equivalent to setsockopt (MCAST_JOIN_GROUP)
                    udpSocket->MulticastJoinGroup(0, this->multicastLocal);
                } else {
                    NS_FATAL_ERROR("Error: Failed to join multicast group");
                }
            }
        }
        this->listenSocket->SetRecvCallback(MakeCallback(&BlockChainNodeApp::HandleRead, this));
        this->listenSocket->SetAcceptCallback(MakeNullCallback < bool, Ptr < Socket > ,const Address &> (),
                MakeCallback(&BlockChainNodeApp::HandleConnectionAccept, this));
        this->listenSocket->SetCloseCallbacks(
                MakeCallback(&BlockChainNodeApp::HandleConnectionEnd, this),
                MakeCallback(&BlockChainNodeApp::HandleConnectionEnd, this)
        );
        this->listenSocket->SetAllowBroadcast (true);

        //broadcast socket
        if (!this->broadcastSocket) {
            TypeId tid = TypeId::LookupByName("ns3::UdpSocketFactory");
            this->broadcastSocket = Socket::CreateSocket (GetNode (), tid);
            static const InetSocketAddress broadcastAddress = InetSocketAddress(Ipv4Address("255.255.255.255"), 655);
            this->broadcastSocket->Connect (broadcastAddress);
            this->broadcastSocket->SetAllowBroadcast (true);
            this->broadcastSocket->ShutdownRecv ();
        }


        //socket to all nodes
        for (std::vector<Ipv4Address>::const_iterator i = this->nodesAddresses.begin(); i != this->nodesAddresses.end(); ++i) {
            TypeId tid = TypeId::LookupByName("ns3::UdpSocketFactory");
            this->nodesSockets[*i] = Socket::CreateSocket(GetNode(), tid);
            this->nodesSockets[*i]->Connect(InetSocketAddress(*i, 655));
        }

        //start transactionGenerator
        // TODO make better random
        int random = rand() % 5;
        this->nextNewTransactionsEvent = Simulator::Schedule(Seconds(random), &BlockChainNodeApp::GenerateSendTransactions, this);

    }

    void BlockChainNodeApp::StopApplication() {
        NS_LOG_FUNCTION(this);
        this->PrintProcessInfo();
        if (this->listenSocket != 0) {
            this->listenSocket->Close();
            this->listenSocket->SetRecvCallback(MakeNullCallback < void, Ptr < Socket > > ());
        }
        Simulator::Cancel(this->nextEvent);
        Simulator::Cancel(this->nextNewTransactionsEvent);
    }

    void BlockChainNodeApp::SetNodesAddresses(std::vector <Ipv4Address> &addresses) {
        this->nodesAddresses = addresses;
    }

    void BlockChainNodeApp::HandleRead(Ptr <Socket> socket) {
        NS_LOG_FUNCTION(this << socket);

        Ptr <Packet> packet;
        Address from;
        while ((packet = socket->RecvFrom(from))) {
            double receiveTimeSeconds = Simulator::Now().GetSeconds();
            if(Inet6SocketAddress::IsMatchingType(from)){
                NS_FATAL_ERROR("Error: IPv6 not support");
                continue;
            } else if(InetSocketAddress::IsMatchingType(from)) {
//                NS_LOG_INFO("At time " << receiveTimeSeconds  << "s NODE " << GetNode()->GetId() << " received " << packet->GetSize()
//                                       << " bytes from " <<
//                                       InetSocketAddress::ConvertFrom(from).GetIpv4() << " port " <<
//                                       InetSocketAddress::ConvertFrom(from).GetPort());
            } else {
                NS_FATAL_ERROR("Error: Received unsupported bytes");
                continue;
            }
            char *packetInfo = new char[packet->GetSize() + 1];
            std::ostringstream totalStream;
            packet->CopyData(reinterpret_cast<uint8_t *>(packetInfo), packet->GetSize());
            packetInfo[packet->GetSize()] = '\0';
            totalStream << packetInfo;
            std::string totalReceivedData(totalStream.str());

            //delimiter
            if(totalReceivedData == "#"){
                continue;
            }

            //handle data
            bool handled = this->HandleCustomRead(packet, from, totalReceivedData);
            if(handled){
                continue;
            }
            this->HandleGeneralRead(packet, from, totalReceivedData);
        }
    }

    bool BlockChainNodeApp::HandleCustomRead(Ptr <Packet> packet, Address from, std::string receivedData){
        // by default - not handled
        return false;
    }

    void BlockChainNodeApp::HandleGeneralRead(Ptr <Packet> packet, Address from, std::string receivedData){
//        NS_LOG_INFO("Node " << GetNode()->GetId() << " Total Received Data: " << receivedData);

        rapidjson::Document document;
        document.Parse(receivedData.c_str());

        if(document["type"] == NEW_TRANSACTION){
            this->ReceiveNewTransaction(&document);
        } else if (document["type"] == NEW_BLOCK){
            this->ReceiveBlock(&document);
        }

    }

    void BlockChainNodeApp::HandleConnectionAccept(Ptr<Socket> socket, const Address& address){
        NS_LOG_FUNCTION(this);
        double timeSeconds = Simulator::Now().GetSeconds();
        NS_LOG_INFO("At time " << timeSeconds  << "s node " << GetNode()->GetId() << " accepting conenction");
    }

    void BlockChainNodeApp::HandleConnectionEnd(Ptr<Socket> socket){
        NS_LOG_FUNCTION(this);
        double timeSeconds = Simulator::Now().GetSeconds();
        NS_LOG_INFO("At time " << timeSeconds  << "s node " << GetNode()->GetId() << " connection end");
    }

    void BlockChainNodeApp::ReceiveBlocks(std::vector <Block> &blocks){

    }

    void BlockChainNodeApp::ReceiveBlock(rapidjson::Document *message) {
        double timeSeconds = Simulator::Now().GetSeconds();
        Block *previousBlock = this->blockChain->GetTopBlock();
        //TODO beter receive FROM address
        Block *block = Block::FromJSON(message,previousBlock,Ipv4Address("0.0.0.0"));
        if(this->blockChain->HasBlock(block)){
            //already received
            return;
        }
        this->blockChain->AddBlock(block);
        this->SendMessage(message, this->broadcastSocket);
    }

    void BlockChainNodeApp::ReceiveNewTransaction(rapidjson::Document *message){
        Transaction *transaction = Transaction::FromJSON(message);
        double timeSeconds = Simulator::Now().GetSeconds();
        if(std::count(this->receivedTransactionsIds.begin(), this->receivedTransactionsIds.end(), transaction->GetId())){
            //already received
            return;
        }
//        NS_LOG_INFO("At time " << timeSeconds  << "s node " << GetNode()->GetId() << " receive transaction " << transaction->GetId());
        this->receivedTransactionsIds.push_back(transaction->GetId());
        this->SendMessage(message, this->broadcastSocket);
    }

    void BlockChainNodeApp::SendMessage(rapidjson::Document *message, Ptr<Socket> outgoingSocket) {
        NS_LOG_FUNCTION(this);

        const uint8_t delimiter[] = "#";
        rapidjson::StringBuffer buffer;

        rapidjson::Writer <rapidjson::StringBuffer> writer(buffer);

        message->Accept(writer);
        double timeSeconds = Simulator::Now().GetSeconds();
//        NS_LOG_INFO("At time " << timeSeconds  << "s node " << GetNode()->GetId()
//                            << " and sent a "
//                            << " message: T" /*<< buffer.GetString()*/);

        outgoingSocket->Send(reinterpret_cast<const uint8_t *>(buffer.GetString()), buffer.GetSize(), 0);
        outgoingSocket->Send(delimiter, 1, 0);
    }

    void BlockChainNodeApp::SendMessage(rapidjson::Document *message, Address &outgoingAddress) {
        NS_LOG_FUNCTION(this);

        const uint8_t delimiter[] = "#";
        rapidjson::StringBuffer buffer;
        rapidjson::Writer <rapidjson::StringBuffer> writer(buffer);

        message->Accept(writer);
        double timeSeconds = Simulator::Now().GetSeconds();

        Ipv4Address address = InetSocketAddress::ConvertFrom(outgoingAddress).GetIpv4();
        auto it = this->nodesSockets.find(address);

        if(it == this->nodesSockets.end()){
            NS_FATAL_ERROR("Node address not found");
        }

        this->nodesSockets[address]->Send(reinterpret_cast<const uint8_t *>(buffer.GetString()), buffer.GetSize(), 0);
        this->nodesSockets[address]->Send(delimiter, 1, 0);
    }

    void BlockChainNodeApp::GenerateSendTransactions(){
        NS_LOG_FUNCTION(this);

        double timeSeconds = Simulator::Now().GetSeconds();
//        NS_LOG_INFO("At time " << timeSeconds << "s node " << GetNode()->GetId() << " sending transactions next:num " << this->transactionGenerationDistribution(this->generator));

        //send transaction to all nodes
        Transaction transaction(GetNode()->GetId(), 1);
        rapidjson::Document message = transaction.ToJSON();
        message["type"].SetInt(NEW_TRANSACTION);
//        NS_LOG_INFO("At time " << timeSeconds  << "s node " << GetNode()->GetId() << " sended transaction " << transaction.GetId());

        this->SendMessage(&message, this->broadcastSocket);
        this->countOfGeneratedTransactions++;

        //plan next sending
        int random = 500;
        if(constants.transactionGenerationType == RAND) {
            random = rand() % constants.randMaxTransactionGenerationTimeMiliSeconds;
        } else if(constants.transactionGenerationType == POISSON){
            random = this->transactionGenerationDistribution(this->generator);
        }
//        NS_LOG_INFO("At time " << timeSeconds  << "s node " << GetNode()->GetId() << " planing generation " << random);
        this->nextNewTransactionsEvent = Simulator::Schedule(MilliSeconds(random), &BlockChainNodeApp::GenerateSendTransactions, this);
    }

    void BlockChainNodeApp::PrintProcessInfo() {
        NS_LOG_INFO(" Stack    |Count of generated transactions  | ");
        NS_LOG_INFO(" " << this->nodeHelper->GetNodeStack(GetNode()->GetId())<< "  |                 " << this->countOfGeneratedTransactions << "            | ");
        NS_LOG_INFO(" BlockChain log: ");
        this->blockChain->PrintInfo();
    }
}

