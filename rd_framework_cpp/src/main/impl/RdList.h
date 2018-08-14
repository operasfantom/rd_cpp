//
// Created by jetbrains on 02.08.2018.
//

#ifndef RD_CPP_RDLIST_H
#define RD_CPP_RDLIST_H

#include <viewable_collections.h>
#include <ViewableList.h>
#include <RdReactiveBase.h>
#include "../serialization/Polymorphic.h"
#include "../serialization/SerializationCtx.h"

template<typename V, typename S = Polymorphic<V>>
class RdList : public RdReactiveBase, public IViewableList<V> {
private:
//    using list = typename ViewableList<V>;
    mutable ViewableList<V> list;
    mutable int64_t nextVersion;

    using Event = typename IViewableList<V>::Event;
public:
    //region ctor/dtor

    RdList() = default;

    explicit RdList(const ViewableList<V> &list, int64_t nextVersion) : list(list), nextVersion(nextVersion) {}

    virtual ~RdList() = default;
    //endregion

    enum class Op {
        Add, Update, Remove
    }; // update versionedFlagShift when changing

    static RdList<V> read(SerializationCtx ctx, Buffer const &buffer, ISerializer<V> *valSzr) {
//        return withId<RdList<V>>(RdList(valSzr, ViewableList(), buffer.read_pod<int64_t>()), RdId::read(buffer));
    }

    template<typename Y>
    static void write(SerializationCtx ctx, Buffer const &buffer, RdList<Y> that) {
        buffer.write_pod<int64_t>(that.nextVersion);
        that.rd_id.write(buffer);
    }

    static const int32_t versionedFlagShift = 2; // update when changing Op

    bool optimizeNested = false;

    virtual void init(Lifetime lifetime) const {
        RdBindableBase::init(lifetime);

        local_change([this, lifetime]() {
            advise(lifetime, [this, lifetime](typename IViewableList<V>::Event const& e) {
                if (!is_local_change) return;

                if (!optimizeNested) {
                    V const* new_value = e.get_new_value();
                    if (new_value) {
                        identifyPolymorphic(e, get_protocol()->identity, get_protocol()->identity->next(rd_id));
                    }
                }

                get_wire()->send(rd_id, [this, &e](Buffer const &buffer) {
                    Op op = static_cast<Op >(e.v.index());

                    buffer.write_pod<int64_t>(static_cast<int64_t>(op) | (nextVersion++ << versionedFlagShift));
                    buffer.write_pod<int32_t>(e.get_index());

                    V const* new_value = e.get_new_value();
                    if (new_value) {
                        S::write(this->get_serialization_context(), buffer, *new_value);
                    }

//                logSend.trace { logmsg(op, nextVersion-1, it.index, it.newValueOpt) }
                });
            });
        });

        get_wire()->advise(lifetime, *this);

        if (!optimizeNested)
            this->view(lifetime, [this](Lifetime lf, size_t index, V const *value) {
                bindPolymorphic(*value, lf, this, "[" + std::to_string(index) + "]");
            });
    }

    virtual void on_wire_received(Buffer const &buffer) const {
        int64_t header = (buffer.read_pod<int64_t>());
//        int64_t version = header >> versionedFlagShift;
        Op op = static_cast<Op>((header & ((1 << versionedFlagShift) - 1L)));
        int32_t index = (buffer.read_pod<int32_t>());

//            logReceived.trace { logmsg(op, version, index, value) }

        /*require(version == nextVersion) {
            "Version conflict for $location}. Expected version $nextVersion, received $version. Are you modifying a list from two sides?"
        }*/

        nextVersion++;

        switch (op) {
            case Op::Add: {
                V value = std::move(S::read(this->get_serialization_context(), buffer));
                (index < 0) ? list.add(std::move(value)) : list.add(index, std::move(value));
                break;
            }
            case Op::Update: {
                V value = std::move(S::read(this->get_serialization_context(), buffer));
                list.set(index, std::move(value));
                break;
            }
            case Op::Remove: {
                list.removeAt(index);
                break;
            }
        }
    }

    virtual void
    advise(Lifetime lifetime, std::function<void(typename IViewableList<V>::Event const &)> handler) const {
        if (is_bound()) assert_threading();
        list.advise(lifetime, handler);
    }

    virtual bool add(V element) const { return local_change<bool>([&]() { return list.add(std::move(element)); }); }

    virtual bool add(size_t index, V element) const {
        return local_change<bool>([&]() { return list.add(index, std::move(element)); });
    }

    virtual bool remove(V const &element) const { return local_change<bool>([&]() { return list.remove(element); }); }

//    virtual bool removeAll(elements: Collection<V>): Boolean = local_change { list.removeAll(elements) }
    virtual V removeAt(size_t index) const { return local_change<V>([&]() { return list.removeAt(index); }); }

//    virtual bool retainAll(elements: Collection<V>): Boolean = local_change { list.retainAll(elements) }

    virtual V const& get(size_t index) const { return list.get(index); };

    virtual V set(size_t index, V element) const {
        return local_change<V>([&]() { return list.set(index, std::move(element)); });
    }

    virtual void clear() const { return local_change([&]() { list.clear(); }); }

    virtual size_t size() const { return list.size(); }

    virtual bool empty() const { return list.empty(); }

    std::vector<V> toList() const { return list.toList(); }
};


#endif //RD_CPP_RDLIST_H
