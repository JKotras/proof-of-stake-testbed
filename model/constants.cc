#include "constants.h"

namespace ns3 {
    Constants::Constants() {
        this->maxTransactionsPerBlock = 800;
        this->maxTransactionPoolSize = 1600;        //suppose 4*maxTransactionPerBlock
        this->numberOfNodes = 512;
        this->totalStack = 10000000;
        this->simulationTimeSeconds = 83.0;
        //transaction generation
        this->transactionGenerationType = T_RAND;
        this->transPoissonDistributionMeanMiliSeconds = 300.2;
        this->randMaxTransactionGenerationTimeMiliSeconds = 1000;       //advice cca number of nodes * 35
        //fee generation
        this->feeGenerationType = F_RAND;
        this->feePoissonDistributionMean = 6.2;
        this->randMaxFeeGeneration = 10;

        // network specific
        this->networkDecentralizedSizePerLocal = 4;
        this->networkDistributedCountOfConnections = 8;

        //ouroboros
        this->ouroborosSlotSizeSeconds = 4.0;
        this->ouroborosSecurityParameter = 2;
        //default epoch size is 20 seconds -> epoch = slogSize * 10 * securityParameter

        //algorand
        this->algorandProposeCommittePercenategeSize = 20.0;
        this->algorandVoteCommittePercenategeSize = 20.0;
        this->algorandWaitingProposedBlockSeconds = 5.0;
    }

    Constants constants;
}