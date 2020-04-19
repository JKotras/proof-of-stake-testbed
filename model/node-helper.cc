#include "node-helper.h"
#include "ns3/log.h"
#include "ns3/internet-module.h"
#include <stdlib.h>

namespace ns3 {
    NS_LOG_COMPONENT_DEFINE ("NodeHelper");

    NodeHelper::NodeHelper(int countOfNodes, long int totalStack) {
        this->totalStack = totalStack;
        this->stackSizes.reserve(countOfNodes);
        for(int i=0;i<countOfNodes;i++){
            this->stackSizes[i] = (long int)(totalStack/countOfNodes);
        }
    }

    long int NodeHelper::GetNodeStack(int nodeId) {
        return this->stackSizes[nodeId];
    }

    long int NodeHelper::GetTotalStack() {
        return this->totalStack;
    }

    void NodeHelper::SendStack(int senderNodeId, int receiverNodeId, long int size) {
        long int senderResult = this->stackSizes[senderNodeId] - size;
        if(senderResult < 0){
            NS_FATAL_ERROR("Can not send stack. Not enought of sender stack");
            return;
        }
        // Todo transaction
        this->stackSizes[senderNodeId] = senderResult;
        this->stackSizes[receiverNodeId] = this->stackSizes[receiverNodeId] + size;
    }
}
