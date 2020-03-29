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

    bool OuroborosNodeApp::HandleCustomRead(Ptr <Packet> packet, Address from, std::string receivedData) {
        rapidjson::Document d;
        d.Parse(receivedData.c_str());
        if (!d.IsObject()) {
            NS_LOG_WARN("The parsed packet is corrupted: " << receivedData);
            return false;
        }
        int messageType = d["type"].GetInt();
        switch (messageType) {
            case OUROBOROS_SEED:
                this->ReceiveEpochSeed(receivedData);
                return true;
        }
        return false;
    }

    int OuroborosNodeApp::GetSlotLeader(int slotNumber){
        long int count;
        for(auto const& item: this->receivedSeeds[slotNumber]) {
            count+=item;
        }
        auto allSatoshis = this->blockChain.GetAllSatoshis();
        NS_LOG_INFO("All satoshis " << allSatoshis.size());
    }

    void OuroborosNodeApp::SendEpochSeed() {
        NS_LOG_FUNCTION(this);
        this->GetSlotLeader(this->GetSlotNumber());

        double timeSeconds = Simulator::Now().GetSeconds();
        int secret = this->CreateSecret();
        NS_LOG_INFO("At time " << timeSeconds << "s Epoch:  " << this->GetEpochNumber() << " sending seed: " << secret);

        const char *json = "{\"type\":\"1\",\"value\":1}";
        rapidjson::Document message;
        message.Parse(json);
        message["type"].SetInt(OUROBOROS_SEED);
        message["value"].SetInt(secret);

        this->SendMessage(message, this->broadcastSocket);

        //plan next sending
        this->sendingSeedNextEvent = Simulator::Schedule(Seconds(double(this->slotSizeSeconds)),
                                                         &OuroborosNodeApp::SendEpochSeed, this);
    }

    void OuroborosNodeApp::ReceiveEpochSeed(std::string receivedData) {
        auto slotNumber = this->GetSlotNumber();
        if (slotNumber+1 > this->receivedSeeds.size()) {
            int diff = slotNumber+1 - this->receivedSeeds.size();
            for (int i = 0; i < diff; i++) {
                std::vector<int> myints;
                this->receivedSeeds.push_back(myints);
            }
        }

        rapidjson::Document d;
        d.Parse(receivedData.c_str());
        int seed = d["value"].GetInt();
        NS_LOG_INFO("Num " << seed);
        this->receivedSeeds[slotNumber].push_back(seed);
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
        return int(timeSeconds / (this->slotSizeSeconds * this->slotsInEpoch));
    }

}