#include "constants.h"

namespace ns3 {
    Constants::Constants() {
        this->transactionGenerationPoissonParameter = 20.0;
        this->maxTransactionsPerBlock = 10;
        this->numberOfNodes = 3;
    }

    Constants constants;
}