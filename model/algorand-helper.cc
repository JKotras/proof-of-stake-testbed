#include "ns3/log.h"
#include "ns3/internet-module.h"
#include <stdlib.h>
#include "algorand-helper.h"
#include "constants.h"

namespace ns3 {
    NS_LOG_COMPONENT_DEFINE ("AlgorandHelper");

    AlgorandHelper::AlgorandHelper(int countOfNodes, long int totalStack): NodeHelper(countOfNodes,totalStack) {

    }
}