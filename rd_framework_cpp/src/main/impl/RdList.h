//
// Created by jetbrains on 02.08.2018.
//

#ifndef RD_CPP_RDLIST_H
#define RD_CPP_RDLIST_H

#include <main/base/RdReactiveBase.h>
#include <viewable_collections.h>
#include <main/base/ISerializer.h>
#include <ViewableList.h>
#include <main/Polymorphic.h>

template<typename V>
class RdList : public RdReactiveBase, public IViewableList<V> {
private:
    ISerializer<V> *valSzr;
    ViewableList<V> list;//todo IViewableList<V> by list
    int64_t nextVersion;

    using Event = typename IViewableList<V>::Event;
public:
    RdList(ISerializer<V> *valSzr, const ViewableList<V> &list, int64_t nextVersion) : valSzr(valSzr), list(list),
                                                                                       nextVersion(nextVersion) {}

    RdList(ISerializer<V> *valSzr = new Polymorphic<V>()) : valSzr(valSzr) {};
//    RdList(valSzr: ISerializer<V> = Polymorphic<V>()) : this(valSzr, ViewableList())
    enum class Op {
        Add, Update, Remove
    }; // update versionedFlagShift when changing
    /*static RdList<V> read(SerializationCtx ctx, AbstractBuffer const& buffer, ISerializer<V>* valSzr)  {
                return RdList(valSzr, ViewableList(), read_pod<int64_t>(buffer)).withId(RdId.read(buffer));
            }*/

    /*void write(SerializationCtx ctx, AbstractBuffer const& buffer, RdList<*> value)= value.run {
        buffer.writeLong(nextVersion);
        rdid.write(buffer)
    }*/

    static const int32_t versionedFlagShift = 2; // update when changing Op

    bool optimizeNested = false;

    virtual void init(Lifetime lifetime) {
        RdBindableBase::init(lifetime);

        local_change([this, lifetime]() {
            advise(lifetime, [this, lifetime](typename IViewableList<V>::Event e) {
                if (!is_local_change) return;

//            if (!optimizeNested) (e.newValueOpt)?.identifyPolymorphic(protocol.identity, protocol.identity.next(rdid))

                get_wire()->send(rd_id, [this, e](Buffer const &buffer) {
                    Op op = (typeid(e) == typeid(typename Event::Add) ? Op::Add :
                             typeid(e) == typeid(typename Event::Update) ? Op::Update :
                             Op::Remove);

                    buffer.write_pod<int64_t>(static_cast<int64_t>(op) | (nextVersion++ << versionedFlagShift));
                    buffer.write_pod<int32_t>(e.index);

                    /*if (typeid(e) == typeid(typename Event::Add) || typeid(e) == typeid(typename Event::Update)) {
                        valSzr->write(serialization_context, buffer, e.v.new_value);
                    }
                    */
                    std::visit(overloaded{
                            [this, &buffer](typename Event::Add const& e) {
                                valSzr->write(serialization_context, buffer, e.new_value);
                            },
                            [this, &buffer](typename Event::Update const& e) {
                                valSzr->write(serialization_context, buffer, e.new_value);
                            },
                            [](typename Event::Remove const& e) {},
                    }, e.v);

//                logSend.trace { logmsg(op, nextVersion-1, it.index, it.newValueOpt) }
                });
            });
        });

        get_wire()->advise(lifetime, *this);

        if (!optimizeNested)
            this->view(lifetime,
                 [this](Lifetime lf, size_t index, V const &value) { /*value.bindPolymorphic(lf, this, "[$index]");*/ });
    }

    virtual void on_wire_received(Buffer const &buffer) {
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
                V value = valSzr->read(serialization_context, buffer);
                (index < 0) ? list.add(value) : list.add(index, value);
                break;
            }
            case Op::Update: {
                V value = valSzr->read(serialization_context, buffer);
                list.set(index, value);
                break;
            }
            case Op::Remove: {
                list.removeAt(index);
                break;
            }
        }
    }

    virtual void advise(Lifetime lifetime, std::function<void(typename IViewableList<V>::Event)> handler) {
        if (is_bound()) assert_threading();
        list.advise(lifetime, handler);
    }

    virtual bool add(V const &element) { return local_change<bool>([&]() { return list.add(element); }); }

    virtual bool add(size_t index, V const &element) {
        return local_change<bool>([&]() { return list.add(index, element); });
    }

    virtual bool remove(V const &element) { return local_change<bool>([&]() { return list.remove(element); }); }

//    virtual bool removeAll(elements: Collection<V>): Boolean = local_change { list.removeAll(elements) }
    virtual V removeAt(size_t index) { return local_change<V>([&]() { return list.removeAt(index); }); }

//    virtual bool retainAll(elements: Collection<V>): Boolean = local_change { list.retainAll(elements) }

    virtual V get(size_t index) { return local_change<V>([&]() { return list.get(index); }); };

    virtual V set(size_t index, V const &element) {
        return local_change<V>([&]() { return list.set(index, element); });
    }

    virtual void clear() { return local_change([&]() { list.clear(); }); }

    virtual size_t size() { return local_change<size_t>([&]() { return list.size(); }); }

    virtual bool empty() { return local_change<bool>([&]() { return list.empty(); }); }

    std::vector<V> toList() { return local_change<std::vector<V>>([&]() { return list.toList(); }); }
};


#endif //RD_CPP_RDLIST_H
