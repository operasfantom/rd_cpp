//
// Created by jetbrains on 02.08.2018.
//

#ifndef RD_CPP_RDLIST_H
#define RD_CPP_RDLIST_H

#include <ViewableList.h>
#include <RdReactiveBase.h>
#include "../serialization/Polymorphic.h"
#include "../serialization/SerializationCtx.h"
#include "util.h"

template<typename V, typename S = Polymorphic<V>>
class RdList : public RdReactiveBase, public IViewableList<V> {
private:
//    using list = typename ViewableList<V>;
    mutable ViewableList<V> list;
    mutable int64_t nextVersion = 1;

    using Event = typename IViewableList<V>::Event;
public:
    //region ctor/dtor

    RdList() = default;

    virtual ~RdList() = default;
    //endregion

    static std::string to_string(Op op);

    class Companion {
    public:
        static RdList<V, S> read(SerializationCtx const &ctx, Buffer const &buffer) {
            return RdList<V, S>::read(ctx, buffer);
        }

        static void write(SerializationCtx const &ctx, Buffer const &buffer, RdList<V, S> const &value) {
            value.write(ctx, buffer);
        }
    };

    static RdList<V> read(SerializationCtx ctx, Buffer const &buffer, ISerializer<V> *valSzr) {
//        return withId<RdList<V>>(RdList(valSzr, ViewableList(), buffer.read_pod<int64_t>()), RdId::read(buffer));
    }

    void write(SerializationCtx ctx, Buffer const &buffer) {
        buffer.write_pod<int64_t>(nextVersion);
        rd_id.write(buffer);
    }

    static const int32_t versionedFlagShift = 2; // update when changing Op

    bool optimizeNested = false;

    void init(Lifetime lifetime) const override {
        RdBindableBase::init(lifetime);

        local_change([this, lifetime]() {
            advise(lifetime, [this, lifetime](typename IViewableList<V>::Event const &e) {
                if (!is_local_change) return;

                if (!optimizeNested) {
                    V const *new_value = e.get_new_value();
                    if (new_value) {
                        const IProtocol *iProtocol = get_protocol();
                        identifyPolymorphic(*new_value, iProtocol->identity,
                                            iProtocol->identity.next(rd_id));
                    }
                }

                get_wire()->send(rd_id, [this, &e](Buffer const &buffer) {
                    Op op = static_cast<Op >(e.v.index());

                    buffer.write_pod<int64_t>(static_cast<int64_t>(op) | (nextVersion++ << versionedFlagShift));
                    buffer.write_pod<int32_t>(static_cast<const int32_t>(e.get_index()));

                    V const *new_value = e.get_new_value();
                    if (new_value) {
                        S::write(this->get_serialization_context(), buffer, *new_value);
                    }

                    this->logSend.trace("list " + location.toString() + " " + rd_id.toString() + to_string(op) +
                                  "::key = " + std::to_string(e.get_index()) +
                                  "::version = " + std::to_string(nextVersion - 1) +
                                  "::value = ${value.printToString()}");
                });
            });
        });

        get_wire()->advise(lifetime, this);

        if (!optimizeNested)
            this->view(lifetime, [this](Lifetime lf, size_t index, V const &value) {
                bindPolymorphic(value, lf, this, "[" + std::to_string(index) + "]");
            });
    }

    void on_wire_received(Buffer const &buffer) const override {
        int64_t header = (buffer.read_pod<int64_t>());
        int64_t version = header >> versionedFlagShift;
        Op op = static_cast<Op>((header & ((1 << versionedFlagShift) - 1L)));
        int32_t index = (buffer.read_pod<int32_t>());

        this->logReceived.trace("list " + location.toString() + " " + rd_id.toString() + to_string(op) +
                          "::key = ${key.printToString()}" +
                          "::version = " + std::to_string(version) +
                          "::value = ${value.printToString()}");


        MY_ASSERT_MSG(version == nextVersion, ("Version conflict for " + location.toString() + "}. Expected version " +
                                               std::to_string(nextVersion) +
                                               ", received " +
                                               std::to_string(version) +
                                               ". Are you modifying a list from two sides?"));

        nextVersion++;

        switch (op) {
            case Op::Add: {
                V value = S::read(this->get_serialization_context(), buffer);
                (index < 0) ? list.add(std::move(value)) : list.add(static_cast<size_t>(index), std::move(value));
                break;
            }
            case Op::Update: {
                V value = S::read(this->get_serialization_context(), buffer);
                list.set(static_cast<size_t>(index), std::move(value));
                break;
            }
            case Op::Remove: {
                list.removeAt(static_cast<size_t>(index));
                break;
            }
        }
    }

    void
    advise(Lifetime lifetime, std::function<void(typename IViewableList<V>::Event const &)> handler) const override {
        if (is_bound()) assert_threading();
        list.advise(lifetime, handler);
    }

    bool add(V element) const override { return local_change<bool>([&]() { return list.add(std::move(element)); }); }

    bool add(size_t index, V element) const override {
        return local_change<bool>([&]() { return list.add(index, std::move(element)); });
    }

    bool remove(V const &element) const override { return local_change<bool>([&]() { return list.remove(element); }); }

    V removeAt(size_t index) const override { return local_change<V>([&]() { return list.removeAt(index); }); }

    V const &get(size_t index) const override { return list.get(index); };

    V set(size_t index, V element) const override {
        return local_change<V>([&]() { return list.set(index, std::move(element)); });
    }

    void clear() const override { return local_change([&]() { list.clear(); }); }

    size_t size() const override { return list.size(); }

    bool empty() const override { return list.empty(); }

    std::vector<std::shared_ptr<V> > const &getList() const override { return list.getList(); }

    bool addAll(size_t index, std::vector<V> elements) const override {
        return local_change<bool>([&]() mutable { return list.addAll(index, std::move(elements)); });
    }

    bool addAll(std::vector<V> elements) const override {
        return local_change<bool>([&]() mutable { return list.addAll(std::move(elements)); });
    }

    bool removeAll(std::vector<V> elements) const override {
        return local_change<bool>([&]() mutable { return list.removeAll(std::move(elements)); });
    }
};

template<typename V, typename S>
std::string RdList<V, S>::to_string(Op op) {
    if (op == Op::Add) return "ADD";
    if (op == Op::Remove) return "REMOVE";
    if (op == Op::Update) return "Update";
    return "";
}


#endif //RD_CPP_RDLIST_H
