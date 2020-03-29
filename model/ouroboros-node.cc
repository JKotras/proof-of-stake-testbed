#include "ns3/internet-module.h"
#include <stdlib.h>
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
#include "ouroboros-node.h"

namespace ns3 {
    NS_LOG_COMPONENT_DEFINE ("OuroborosNodeApp");
    NS_OBJECT_ENSURE_REGISTERED (OuroborosNodeApp);

    OuroborosNodeApp::OuroborosNodeApp(int slotSizeSeconds, int securityParameter) {
        this->slotSizeSeconds = slotSizeSeconds;
        this->slotsInEpoch = 10 * securityParameter;
    }

    void OuroborosNodeApp::StartApplication() {
        NS_LOG_FUNCTION(this);
        NS_LOG_INFO("Starting Ouroboros App " << GetNode()->GetId());
        BlockChainNodeApp::StartApplication();
        this->sendingSeedNextEvent = Simulator::Schedule(Seconds(0.0), &OuroborosNodeApp::SendEpochSeed, this);
    }

    void OuroborosNodeApp::StopApplication() {
        NS_LOG_FUNCTION(this);

        BlockChainNodeApp::StopApplication();
        Simulator::Cancel(this->sendingSeedNextEvent);
    }

    bool OuroborosNodeApp::HandleCustomRead(Ptr <Packet> packet, Address from, std::string receivedData){
        return false;
    }

    void OuroborosNodeApp::SendEpochSeed() {
        NS_LOG_FUNCTION(this);

        double timeSeconds = Simulator::Now().GetSeconds();
        int secret = this->CreateSecret();
        NS_LOG_INFO("At time " << timeSeconds  << "s Epoch:  " << this->GetEpochNumber() << " sending seed: " << secret);

        const char* json = "{\"type\":\"1\",\"value\":1}";
        rapidjson::Document message;
        message.Parse(json);
        message["type"].SetInt(OUROBOROS_SEED);
        message["value"].SetInt(secret);
        NS_LOG_INFO("HERE " << message["value"].GetInt());

        this->SendMessage(message, this->broadcastSocket);

        //plan next sending
        this->sendingSeedNextEvent = Simulator::Schedule (Seconds(double(this->slotSizeSeconds)), &OuroborosNodeApp::SendEpochSeed, this);
    }

    void OuroborosNodeApp::ReceiveEpochSeed(std::string receivedData){
        //parse number
    }

    int OuroborosNodeApp::CreateSecret() {
        return rand();
    }

    int OuroborosNodeApp::GetSlotNumber() {
        double timeSeconds = Simulator::Now().GetSeconds();
        return int(timeSeconds / this->slotSizeSeconds);
    }

    int OuroborosNodeApp::GetEpochNumber() {
        double timeSeconds = Simulator::Now().GetSeconds();
        return int(timeSeconds / (this->slotSizeSeconds*this->slotsInEpoch));
    }

}