//
// Created by jetbrains on 02.08.2018.
//

#ifndef RD_CPP_RDMAP_H
#define RD_CPP_RDMAP_H


#include <viewable_collections.h>
#include <ViewableMap.h>
#include <cstdint>
#include <RdReactiveBase.h>
#include "../serialization/Polymorphic.h"

template<typename K, typename V, typename KS = Polymorphic<K>, typename VS = Polymorphic<V>>
class RdMap : public RdReactiveBase, public IViewableMap<K, V> {
private:
//    using list = typename ViewableMap<K, V>;
    ViewableMap<K, V> map;
    mutable int64_t nextVersion = 0;
    mutable std::map<K, int64_t> pendingForAck;

    bool is_master() const {
        return manualMaster.has_value() ? *manualMaster : !optimizeNested;
    }

public:

    std::optional<bool> manualMaster;

    bool optimizeNested = false;

    using Event = typename IViewableMap<K, V>::Event;

    RdMap() = default;

    explicit RdMap(const ViewableMap<K, V> &list, int64_t nextVersion) : map(map), nextVersion(nextVersion) {}

    virtual ~RdMap() = default;

    enum class Op {
        Add, Update, Remove, Ack
    }; // update versionedFlagShift when changing



/*    static RdMap<K, V> read(SerializationCtx ctx, Buffer const &buffer) {
//        return withId<RdMap<K, V>>(RdMap(valSzr, ViewableMap(), buffer.read_pod<int64_t>()), RdId::read(buffer));
    }

    template<typename X, typename Y>
    static void write(SerializationCtx ctx, Buffer const &buffer, RdMap<X, Y> const &that) {
        buffer.write_pod<int64_t>(that.nextVersion);
        that.rd_id.write(buffer);
    }*/

    static const int32_t versionedFlagShift = 8; // update when changing Op

    virtual void init(Lifetime lifetime) const {
        RdBindableBase::init(lifetime);

        local_change([this, lifetime]() {
            advise(lifetime, [this, lifetime](Event e) {
                if (!is_local_change) return;

                std::optional<V> new_value = e.get_new_value();
                if (new_value.has_value()) {
                    identifyPolymorphic(e, get_protocol()->identity, get_protocol()->identity->next(rd_id));
                }

                get_wire()->send(rd_id, [this, e](Buffer const &buffer) {
                    int32_t versionedFlag = ((is_master() ? 1 : 0)) << versionedFlagShift;
                    Op op = static_cast<Op>(e.v.index());

                    buffer.write_pod<int32_t>(static_cast<int32_t>(op) | versionedFlag);

                    int64_t version = is_master() ? ++nextVersion : 0L;

                    if (is_master()) {
                        pendingForAck.insert(std::make_pair(e.get_key(), version));
                        buffer.write_pod(version);
                    }

                    KS::write(this->get_serialization_context(), buffer, e.get_key());

                    std::optional<V> new_value = e.get_new_value();
                    if (new_value.has_value()) {
                        VS::write(this->get_serialization_context(), buffer, *new_value);
                    }

//                logSend.trace { logmsg(op, nextVersion-1, it.index, it.newValueOpt) }
                });
            });
        });

        get_wire()->advise(lifetime, *this);

        if (!optimizeNested)
            this->view(lifetime, [this](Lifetime lf, std::pair<K, V> const &entry) {
                bindPolymorphic(entry.second, lf, this, "[" + std::to_string(entry.first) + "]");
            });
    }

    virtual void on_wire_received(Buffer const &buffer) const {
        int32_t header = buffer.read_pod<int32_t>();
        bool msgVersioned = (header >> versionedFlagShift) != 0;
        Op op = static_cast<Op>(header & ((1 << versionedFlagShift) - 1));

        int64_t version = msgVersioned ? buffer.read_pod<int64_t>() : 0;

        K const &key = KS::read(this->get_serialization_context(), buffer);

        if (op == Op::Ack) {
            /*val errmsg =
            if (!msgVersioned) "Received ${Op.Ack} while msg hasn't versioned flag set"
            else if (!master) "Received ${Op.Ack} when not a Master"
            else pendingForAck[key]?.let { pendingVersion ->
                    if (pendingVersion < version) "Pending version `$pendingVersion` < ${Op.Ack} version `$version`"
                    else {
                        //side effect
                        if (pendingVersion == version) pendingForAck.remove(key) //else we don't need to remove, silently drop
                        "" //return good result
                    }
            } ?: "No pending for ${Op.Ack}"

            if (errmsg.isEmpty())
                logReceived.trace  { logmsg(Op.Ack, version, key) }
            else
            logReceived.error {  logmsg(Op.Ack, version, key) + " >> $errmsg"}*/

        } else {
            bool isPut = (op == Op::Add || op == Op::Update);
            std::optional<V> value = isPut ? VS::read(this->get_serialization_context(), buffer) : nullptr;

            if (msgVersioned || !is_master() || pendingForAck.count(key) == 0) {
//                    logReceived.trace { logmsg(op, version, key, value) }

                if (value.has_value()) {
                    map.set(key, *value);
                } else {
                    map.remove(key);
                }
            } else {
//                    logReceived.trace { logmsg(op, version, key, value) + " >> REJECTED" }
            }


            if (msgVersioned) {
                get_wire()->send(rd_id, [this, version, key](Buffer const &innerBuffer) {
                    innerBuffer.write_pod<int32_t>((1 << versionedFlagShift) | static_cast<int32_t>(Op::Ack));
                    innerBuffer.write_pod<int64_t>(version);
                    KS::write(this->get_serialization_context(), innerBuffer, key);

//                    logSend.trace { logmsg(Op.Ack, version, key) }
                });

//                if (is_master()) logReceived.error { "Both ends are masters: $location" }
            }

        }
    }

    virtual void advise(Lifetime lifetime, std::function<void(typename IViewableMap<K, V>::Event)> handler) const {
        if (is_bound()) assert_threading();
        map.advise(lifetime, handler);
    }

    virtual V get(K const &key) const {
        return local_change<V>([&]() { return map.get(key); });
    }

    virtual std::optional<V> set(K const &key, V const &value) const {
        return local_change<std::optional<V>>([&]() { return map.set(key, value); });
    }

    virtual std::optional<V> remove(K const &key) const {
        return local_change<std::optional<V>>([&]() { return map.remove(key); });
    }

    virtual void clear() const {
        return local_change([&]() { return map.clear(); });
    }

    virtual size_t size() const {
        return local_change<size_t>([&]() { return map.size(); });
    }

};


#endif //RD_CPP_RDMAP_H
