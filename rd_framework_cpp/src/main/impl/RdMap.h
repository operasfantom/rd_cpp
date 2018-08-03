//
// Created by jetbrains on 02.08.2018.
//

#ifndef RD_CPP_RDMAP_H
#define RD_CPP_RDMAP_H


#include <main/Polymorphic.h>
#include <main/base/RdReactiveBase.h>
#include <viewable_collections.h>
#include <ViewableMap.h>

/*
template<typename K, typename V, typename S = Polymorphic>
class RdMap : RdReactiveBase, IViewableMap<K, V> {
private:
//    using list = typename ViewableMap<K, V>;
    ViewableMap<K, V> map;
    int64_t nextVersion = 0;
    std::map<K, int64_t> pendingForAck;

    using Event = typename IViewableMap<K, V>::Event;
public:
    RdMap() = default;

    explicit RdMap(const ViewableMap <K, V> &list, int64_t nextVersion) : map(map), nextVersion(nextVersion) {}

    enum class Op {
        Add, Update, Remove
    }; // update versionedFlagShift when changing

    static RdMap <K, V> read(SerializationCtx ctx, Buffer const &buffer, ISerializer<K, V> *valSzr) {
//        return withId<RdMap<K, V>>(RdMap(valSzr, ViewableMap(), buffer.read_pod<int64_t>()), RdId::read(buffer));
    }

    template<typename Y>
    static void write(SerializationCtx ctx, Buffer const &buffer, RdMap <Y> that) {
        buffer.write_pod<int64_t>(that.nextVersion);
        that.rd_id.write(buffer);
    }

    static const int32_t versionedFlagShift = 8; // update when changing Op

    bool optimizeNested = false;

    virtual void init(Lifetime lifetime) {
        RdBindableBase::init(lifetime);

        local_change([this, lifetime]() {
            advise(lifetime, [this, lifetime](Event e) {
                if (!is_local_change) return;

//            if (!optimizeNested) (e.newValueOpt)?.identifyPolymorphic(protocol.identity, protocol.identity.next(rdid))

                get_wire()->send(rd_id, [this, e](Buffer const &buffer) {
                    int32_t versionedFlag = ((master ? 1 : 0) 1 else 0) shl versionedFlagShift
                    Op op = (typeid(e) == typeid(typename Event::Add) ? Op::Add :
                             typeid(e) == typeid(typename Event::Update) ? Op::Update :
                             Op::Remove);

                    buffer.write_pod<int64_t>(static_cast<int64_t>(op) | (nextVersion++ << versionedFlagShift));
                    buffer.write_pod<int32_t>(e.index);

                    std::visit(overloaded{
                            [this, &buffer](typename Event::Add const &e) {
                                S::write(serialization_context, buffer, e.new_value);
                            },
                            [this, &buffer](typename Event::Update const &e) {
                                S::write(serialization_context, buffer, e.new_value);
                            },
                            [](typename Event::Remove const &e) {},
                    }, e.v);

//                logSend.trace { logmsg(op, nextVersion-1, it.index, it.newValueOpt) }
                });
            });
        });

        get_wire()->advise(lifetime, *this);

        if (!optimizeNested)
            this->view(lifetime,
                       [this](Lifetime lf, size_t index,
                              V const &value) { */
/*value.bindPolymorphic(lf, this, "[$index]");*//*
 });
    }

    virtual void on_wire_received(Buffer const &buffer) {
        int64_t header = (buffer.read_pod<int64_t>());
//        int64_t version = header >> versionedFlagShift;
        Op op = static_cast<Op>((header & ((1 << versionedFlagShift) - 1L)));
        int32_t index = (buffer.read_pod<int32_t>());

//            logReceived.trace { logmsg(op, version, index, value) }

        */
/*require(version == nextVersion) {
            "Version conflict for $location}. Expected version $nextVersion, received $version. Are you modifying a list from two sides?"
        }*//*


        nextVersion++;

        switch (op) {
            case Op::Add: {
                V value = S::read(serialization_context, buffer);
                (index < 0) ? list.add(value) : list.add(index, value);
                break;
            }
            case Op::Update: {
                V value = S::read(serialization_context, buffer);
                list.set(index, value);
                break;
            }
            case Op::Remove: {
                list.removeAt(index);
                break;
            }
        }
    }

    virtual void advise(Lifetime lifetime, std::function<void(typename IViewableMap<K, V>::Event)> handler) {
        if (is_bound()) assert_threading();
        map.advise(lifetime, handler);
    }

};
*/


#endif //RD_CPP_RDMAP_H
