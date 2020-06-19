#ifndef PROJECT_NODE_HELPER_H
#define PROJECT_NODE_HELPER_H

#include <vector>

namespace ns3 {

    class NodeHelper {
    protected:
        std::vector<long int> stackSizes;
        long int totalStack;
        int countOfNodes;
        void CreateAndSetStack();
    public:
        NodeHelper(int countOfNodes, long int totalStack);
        long int GetTotalStack();
        long int GetNodeStack(int nodeId);
        int GetStackCoinOwner(long int stackCoin);
        int GetNodeRandomByStack();
        void SendStack(int senderNodeId, int receiverNodeId, long int size);
    };
}
#endif //PROJECT_NODE_HELPER_H
