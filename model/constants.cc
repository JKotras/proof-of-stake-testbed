#include "constants.h"

namespace ns3 {
    Constants::Constants() {
        this->maxTransactionsPerBlock = 955;
        this->numberOfNodes = 20;
        this->transactionGenerationType = RAND;
        this->poissonDistributionMeanMiliSeconds = 300.2;
        this->randMaxTransactionGenerationTimeMiliSeconds = 500;
        this->totalStack = 10000000;
        this->simulationTimeSeconds = 120.0;
    }

    Constants constants;
}