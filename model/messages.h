//
// Created by honza on 19.3.20.
//

#ifndef PROJECT_MESSAGES_H
#define PROJECT_MESSAGES_H

namespace ns3 {
    enum MessageTypes{
        NEW_TRANSACTION,
        NEW_BLOCK,
        //protocol specific messages
        OUROBOROS_SEED,
    };
}

#endif //PROJECT_MESSAGES_H
