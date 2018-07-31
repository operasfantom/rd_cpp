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
        process_all_messages();
    }
}

void TestWire::process_all_messages() {
    while (!msgQ.empty()) {
        process_one_message();
    }
}

void TestWire::process_one_message() {
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
        process_all_messages();
    }
}
