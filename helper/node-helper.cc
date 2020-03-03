
#include "node-helper.h"

namespace ns3 {

    BlockChainNodeAppHelper::BlockChainNodeAppHelper() {

    }

    ApplicationContainer BlockChainNodeAppHelper::Install(NodeContainer c) {
        ApplicationContainer apps;
        for (NodeContainer::Iterator i = c.Begin(); i != c.End(); ++i) {
            Ptr <Node> node = *i;
            Ptr <BlockChainNodeApp> client = this->objectFactory.Create<BlockChainNodeApp>();
            node->AddApplication(client);
            apps.Add(client);
        }
        return apps;
    }
}
