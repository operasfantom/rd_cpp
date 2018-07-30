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

    explicit TestWire(IScheduler *scheduler) : WireBase(scheduler) {
        this->connected.set(true);
    }

    void send(RdId id, std::function<void(AbstractBuffer const &buffer)> writer) {
//        require(!id.isNull)
        UnsafeBuffer buffer(std::vector<unsigned char>(10));
        AbstractBuffer &ostream = buffer;
        writer(ostream);

        bytesWritten += ostream.get_position();

        ostream.set_position(0);

        msgQ.push(RdMessage(id, ostream));
        if (auto_flush) {
            processAllMessages();
        }
    }

    void processAllMessages() {
        while (!msgQ.empty()) {
            processOneMessage();
        }
    }

    void processOneMessage() {
        if (msgQ.empty()){
            return;
        }
        auto msg = msgQ.front();
        msgQ.pop();
        counterpart->message_broker.dispatch(msg.id, msg.istream);
    }

    void set_auto_flush(bool value) {
            auto_flush = value;
            if (value) {
//                process_all_messages();
            }
    }

    /*fun processAllMessages() {
        while (!msgQ.isEmpty()) processOneMessage()
    }

    fun processOneMessage() {
        val msg = msgQ.poll() ?: return
                counterpart.messageBroker.dispatch(msg.id, msg.istream)
    }*/
};


#endif //RD_CPP_TESTWIRE_H
