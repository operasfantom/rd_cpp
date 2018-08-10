//
// Created by jetbrains on 25.07.2018.
//

#ifndef RD_CPP_TESTWIRE_H
#define RD_CPP_TESTWIRE_H


#include <queue>
#include <utility>
#include <WireBase.h>
#include "../../main/RdId.h"
#include "../../main/Buffer.h"

class RdMessage {
public:
    RdId id;
    std::shared_ptr<Buffer> istream;

    RdMessage(const RdId &id, std::shared_ptr<Buffer> istream) : id(id), istream(std::move(istream)) {};
};

class TestWire : public WireBase {
protected:
    bool auto_flush = true;
public:
    mutable TestWire const *counterpart = nullptr;
    mutable std::queue<RdMessage> msgQ;
    mutable int64_t bytesWritten = 0;

    explicit TestWire(IScheduler const *const scheduler);

    virtual void send(RdId id, std::function<void(Buffer const &buffer)> writer) const;

    void process_all_messages() const;

    void process_one_message() const;

    void set_auto_flush(bool value);
};


#endif //RD_CPP_TESTWIRE_H
