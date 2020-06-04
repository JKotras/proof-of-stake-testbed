#include "node-helper.h"
#include "ns3/log.h"
#include "ns3/internet-module.h"
#include <stdlib.h>

namespace ns3 {
    NS_LOG_COMPONENT_DEFINE ("NodeHelper");

    NodeHelper::NodeHelper(int countOfNodes, long int totalStack) {
        this->countOfNodes = countOfNodes;
        this->totalStack = totalStack;
        this->CreateAndSetStack();
    }

    void NodeHelper::CreateAndSetStack() {
        NS_LOG_FUNCTION(this);
        //basic regular division of stack
//        for(int i=0;i<countOfNodes;i++){
//            this->stackSizes[i] = (long int)(totalStack/countOfNodes);
//        }
        // divide random, based on the percentage diff
        long int basicStack = (long int)(totalStack/countOfNodes);
        double percentageDiff = 68.0;                       // how much (in percentage) it can be stack difference between nodes
        double numberPercentageDiff = percentageDiff/100;
        long int maxDiff = (long int)(basicStack*numberPercentageDiff);
        long int minStack = basicStack - maxDiff;
        if(minStack < 0){
            minStack = 0;
        }
        long int maxStack = basicStack + maxDiff;
        if(maxStack > totalStack){
            maxStack = totalStack;
        }
        long int minMaxDiff = maxStack - minStack;
        if(minMaxDiff == 0){
            minMaxDiff = 1;
        }

        long int restOfTotalStack = totalStack;
        this->stackSizes.reserve(countOfNodes);
        for(int i=0;i<countOfNodes;i++){
            //TODO be carefull - random do not generate long int
            long int nodeStack = rand() % minMaxDiff + minStack;
            if(nodeStack > restOfTotalStack){
                nodeStack = restOfTotalStack;
            }
            //manual setting of node stack
//            if(i == 0){
//                nodeStack = 10000000;
//            }
            this->stackSizes[i] = nodeStack;
            restOfTotalStack = restOfTotalStack - nodeStack;
        }
        if(restOfTotalStack > 0) {
            int rest = restOfTotalStack / countOfNodes;
            for (int i = 0; i < countOfNodes; i++) {
                this->stackSizes[i] = this->stackSizes[i] + rest;
            }
        }
    }

    long int NodeHelper::GetNodeStack(int nodeId) {
        return this->stackSizes[nodeId];
    }

    long int NodeHelper::GetTotalStack() {
        return this->totalStack;
    }

    int NodeHelper::GetStackCoinOwner(long int stackCoin) {
        //TODO implement
        return 0;
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
