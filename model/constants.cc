#include "constants.h"

namespace ns3 {
    Constants::Constants() {
        this->maxTransactionsPerBlock = 10;
        this->numberOfNodes = 6;
        this->transactionGenerationType = RAND;
        this->poissonDistributionMeanMiliSeconds = 300.2;
        this->randMaxTransactionGenerationTimeMiliSeconds = 10000;
    }

    Constants constants;
}