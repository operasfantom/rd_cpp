//
// Created by jetbrains on 25.07.2018.
//

#ifndef RD_CPP_TESTWIRE_H
#define RD_CPP_TESTWIRE_H


#include <main/base/WireBase.h>
#include <queue>
#include <main/UnsafeBuffer.h>

class RdMessage {
public:
    RdId id;
    AbstractBuffer const &istream;

    RdMessage(const RdId &id, AbstractBuffer const &istream) : id(id), istream(istream) {};
};

class TestWire : public WireBase {
protected:
    bool auto_flush = true;
public:
    TestWire *counterpart = nullptr;
    std::queue<RdMessage> msgQ;
    int64_t bytesWritten = 0;

    explicit TestWire(IScheduler *scheduler);

    virtual void send(RdId id, std::function<void(AbstractBuffer const &buffer)> writer);

    void process_all_messages();

    void process_one_message();

    void set_auto_flush(bool value);

    /*fun processAllMessages() {
        while (!msgQ.isEmpty()) processOneMessage()
    }

    fun processOneMessage() {
        val msg = msgQ.poll() ?: return
                counterpart.messageBroker.dispatch(msg.id, msg.istream)
    }*/
};


#endif //RD_CPP_TESTWIRE_H
