#include "constants.h"

namespace ns3 {
    Constants::Constants() {
        this->maxTransactionsPerBlock = 10;
        this->numberOfNodes = 3;
        this->transactionGenerationType = RAND;
        this->poissonDistributionMeanMiliSeconds = 300.2;
        this->randMaxTransactionGenerationTimeMiliSeconds = 300;
    }

    Constants constants;
}