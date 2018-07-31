//
// Created by jetbrains on 25.07.2018.
//

#include "TestWire.h"

TestWire::TestWire(IScheduler *scheduler) : WireBase(scheduler) {
    this->connected.set(true);
}

void TestWire::send(RdId id, std::function<void(AbstractBuffer const &buffer)> writer) {
//        require(!id.isNull)
    UnsafeBuffer buffer(10);
    AbstractBuffer &ostream = buffer;
    writer(ostream);

    bytesWritten += ostream.get_position();

    ostream.set_position(0);

    msgQ.push(RdMessage(id, ostream));
    if (auto_flush) {
        processAllMessages();
    }
}

void TestWire::processAllMessages() {
    while (!msgQ.empty()) {
        processOneMessage();
    }
}

void TestWire::processOneMessage() {
    if (msgQ.empty()) {
        return;
    }
    auto msg = msgQ.front();
    msgQ.pop();
    counterpart->message_broker.dispatch(msg.id, msg.istream);
}

void TestWire::set_auto_flush(bool value) {
    auto_flush = value;
    if (value) {
        processAllMessages();
    }
}
