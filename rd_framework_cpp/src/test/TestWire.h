//
// Created by jetbrains on 25.07.2018.
//

#ifndef RD_CPP_TESTWIRE_H
#define RD_CPP_TESTWIRE_H


#include <main/base/WireBase.h>
#include <main/Buffer.h>
#include <queue>
#include <utility>

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
    TestWire *counterpart = nullptr;
    std::queue<RdMessage> msgQ;
    int64_t bytesWritten = 0;

    explicit TestWire(IScheduler *scheduler);

    virtual void send(RdId id, std::function<void(Buffer const &buffer)> writer);

    void process_all_messages();

    void process_one_message();

    void set_auto_flush(bool value);
};


#endif //RD_CPP_TESTWIRE_H
