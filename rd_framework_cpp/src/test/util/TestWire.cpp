//
// Created by jetbrains on 25.07.2018.
//

#include <cassert>
#include "TestWire.h"

TestWire::TestWire(IScheduler const * const scheduler) : WireBase(scheduler) {
    this->connected.set(true);
}

void TestWire::send(RdId id, std::function<void(Buffer const &buffer)> writer) const {
    assert(!id.isNull());
    std::shared_ptr<Buffer> ostream(new Buffer(10));
    writer(*ostream);

    bytesWritten += ostream->get_position();

    ostream->set_position(0);

    msgQ.push(RdMessage(id, ostream));//todo move
    if (auto_flush) {
        process_all_messages();
    }
}

void TestWire::process_all_messages() const {
    while (!msgQ.empty()) {
        process_one_message();
    }
}

void TestWire::process_one_message() const {
    if (msgQ.empty()) {
        return;
    }
    auto msg = msgQ.front();
    msgQ.pop();
    counterpart->message_broker.dispatch(msg.id, msg.istream);//todo move
}

void TestWire::set_auto_flush(bool value) {
    auto_flush = value;
    if (value) {
        process_all_messages();
    }
}
