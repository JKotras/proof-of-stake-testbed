#ifndef PROJECT_CONSTANSTS_H
#define PROJECT_CONSTANSTS_H
#include <stdint.h>

namespace ns3 {
    class Constants{
    public:
        Constants();
        int maxTransactionsPerBlock;
        double transactionGenerationPoissonParameter;
        uint32_t numberOfNodes;
    };

    extern Constants constants;
}

#endif //PROJECT_CONSTANSTS_H
