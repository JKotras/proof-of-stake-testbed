#ifndef PROJECT_CONSTANSTS_H
#define PROJECT_CONSTANSTS_H
#include <stdint.h>

namespace ns3 {
    enum TransactionGenerationType{
        RAND,
        POISSON,
    };

    class Constants{
    public:
        Constants();
        int maxTransactionsPerBlock;
        uint32_t numberOfNodes;
        TransactionGenerationType transactionGenerationType;
        double poissonDistributionMeanMiliSeconds;
        int randMaxTransactionGenerationTimeMiliSeconds;
        long int totalStack;
        double simulationTimeSeconds;

        //network specific
        int networkDecentralizedSizePerLocal;
        int networkDistributedCountOfConnections;

        //ouroboros
        double ouroborosSlotSizeSeconds;
        int ouroborosSecurityParameter;

        //algorand
        double algorandProposeCommittePercenategeSize;
        double algorandVoteCommittePercenategeSize;
        double algorandWaitingProposedBlockSeconds;
    };

    extern Constants constants;
}

#endif //PROJECT_CONSTANSTS_H
