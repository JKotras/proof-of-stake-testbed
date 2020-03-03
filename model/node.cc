
#include "node.h"
#include "../utils/rsa.h"
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

namespace ns3 {
    NS_LOG_COMPONENT_DEFINE ("BlockChainNodeApp");
    NS_OBJECT_ENSURE_REGISTERED (BlockChainNodeApp);

    BlockChainNodeApp::BlockChainNodeApp() {
        this->listenSocket = 0;
        this->keys = generate_keys();
    }

    TypeId BlockChainNodeApp::GetTypeId() {
        static TypeId tid = TypeId("ns3::BlockChainNodeApp")
                .SetParent<Application>()
                .SetGroupName("Applications")
                .AddConstructor<BlockChainNodeApp>();
        return tid;
    }

    Ptr <Socket> BlockChainNodeApp::GetListenPort() const {
        return this->listenSocket;
    };

    void BlockChainNodeApp::StartApplication() {
        NS_LOG_FUNCTION(this);
        NS_LOG_INFO("Starting App");

        if (!this->listenSocket) {
            TypeId tid = TypeId::LookupByName("ns3::UdpSocketFactory");     //udp
            this->listenSocket = Socket::CreateSocket(GetNode(), tid);
            InetSocketAddress local = InetSocketAddress(Ipv4Address::GetAny(), 655);
            this->listenSocket->Bind(local);
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
        this->listenSocket->SetAllowBroadcast (true);
//        this->nextEvent = Simulator::Schedule(Seconds(0.0), &BlockChainNodeApp::Send, this);
    }

    void BlockChainNodeApp::StopApplication() {
        NS_LOG_FUNCTION(this);

        if (this->listenSocket != 0) {
            this->listenSocket->Close();
            this->listenSocket->SetRecvCallback(MakeNullCallback < void, Ptr < Socket > > ());
        }
//        Simulator::Cancel(this->nextEvent);
    }

    void BlockChainNodeApp::Send() {
        NS_LOG_FUNCTION(this);
        NS_LOG_INFO("sending");
    }

    void BlockChainNodeApp::HandleRead(Ptr <Socket> socket) {
        NS_LOG_FUNCTION(this << socket);

        Ptr <Packet> packet;
        Address from;
        while ((packet = socket->RecvFrom(from))) {
            if (InetSocketAddress::IsMatchingType(from)) {
                NS_LOG_INFO("At time " << Simulator::Now().GetSeconds() << "s server received " << packet->GetSize()
                                       << " bytes from " <<
                                       InetSocketAddress::ConvertFrom(from).GetIpv4() << " port " <<
                                       InetSocketAddress::ConvertFrom(from).GetPort());
            } else if (Inet6SocketAddress::IsMatchingType(from)) {
                NS_LOG_INFO("At time " << Simulator::Now().GetSeconds() << "s server received " << packet->GetSize()
                                       << " bytes from " <<
                                       Inet6SocketAddress::ConvertFrom(from).GetIpv6() << " port " <<
                                       Inet6SocketAddress::ConvertFrom(from).GetPort());
            }

            packet->RemoveAllPacketTags();
            packet->RemoveAllByteTags();

            NS_LOG_LOGIC("Echoing packet");
            socket->SendTo(packet, 0, from);

            if (InetSocketAddress::IsMatchingType(from)) {
                NS_LOG_INFO("At time " << Simulator::Now().GetSeconds() << "s server sent " << packet->GetSize()
                                       << " bytes to " <<
                                       InetSocketAddress::ConvertFrom(from).GetIpv4() << " port " <<
                                       InetSocketAddress::ConvertFrom(from).GetPort());
            } else if (Inet6SocketAddress::IsMatchingType(from)) {
                NS_LOG_INFO("At time " << Simulator::Now().GetSeconds() << "s server sent " << packet->GetSize()
                                       << " bytes to " <<
                                       Inet6SocketAddress::ConvertFrom(from).GetIpv6() << " port " <<
                                       Inet6SocketAddress::ConvertFrom(from).GetPort());
            }
        }
    }

}

