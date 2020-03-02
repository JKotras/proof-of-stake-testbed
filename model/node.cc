
#include "node.h"
#include "../utils/rsa.cc"
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
        this->keys = generate_keys();
    }

    TypeId BlockChainNodeApp::GetTypeId() {
        static TypeId tid = TypeId("ns3::BlockChainNodeApp")
                .SetParent<Application>()
                .SetGroupName("Applications")
                .AddConstructor<BlockChainNodeApp>()
                .AddAttribute("Port", "Port on which we listen for incoming packets.",
                              UintegerValue(135),
                              MakeUintegerAccessor(&BlockChainNodeApp::listenPort),
                              MakeUintegerChecker<uint16_t>());
        return tid;
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

