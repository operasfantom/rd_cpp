//
// Created by jetbrains on 25.07.2018.
//

#ifndef RD_CPP_TESTWIRE_H
#define RD_CPP_TESTWIRE_H


#include <main/base/WireBase.h>
#include <queue>

class RdMessage {
    RdId id;
//    AbstractBuffer& istream;
};

class TestWire : public WireBase {
protected:
    std::queue<RdMessage> msgQ;

    int64_t bytesWritten = 0;

public:
    TestWire *counterpart;

    TestWire(IScheduler *scheduler) : WireBase(scheduler) {
        this->connected.set(true);
    }

    void send(RdId id, std::function<void(AbstractBuffer &)> writer) {
//        require(!id.isNull)

//        AbstractBuffer& ostream = createAbstractBuffer();
//        writer(ostream);

//        bytesWritten += ostream.position

//        ostream.position = 0

//        msgQ.offer(RdMessage(id, ostream))
//        if (autoFlush) processAllMessages()
    }


    bool autoFlush = true;

    /*
    set(value) {
            field = value
            if (value) processAllMessages()
    }*/

    /*fun processAllMessages() {
        while (!msgQ.isEmpty()) processOneMessage()
    }

    fun processOneMessage() {
        val msg = msgQ.poll() ?: return
                counterpart.messageBroker.dispatch(msg.id, msg.istream)
    }*/
};


#endif //RD_CPP_TESTWIRE_H
