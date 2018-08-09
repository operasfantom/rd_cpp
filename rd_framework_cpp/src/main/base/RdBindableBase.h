//
// Created by jetbrains on 23.07.2018.
//

#ifndef RD_CPP_RDBINDABLEBASE_H
#define RD_CPP_RDBINDABLEBASE_H


#include <any>
#include "IRdBindable.h"

class RdBindableBase : public virtual IRdBindable/*, IPrintable*/ {
protected:
    IRdDynamic *parent = nullptr;

    std::optional<Lifetime> bind_lifetime;

    //bound state: inferred

    bool is_bound() const;

    IProtocol *get_protocol() const;

    std::vector<std::pair<std::string, std::any> > bindable_children;

    SerializationCtx const &get_serialization_context() const;

public:
    //region ctor/dtor

    RdBindableBase() {
        location = RName("<<not bound>>");
    };

    virtual ~RdBindableBase() = default;
    //endregion

    //need to implement in subclasses
    virtual void init(Lifetime lifetime);

    virtual void bind(Lifetime lf, IRdDynamic *parent, const std::string &name);


    /*concurrent*/std::map<std::string, std::any> extensions;

//    getOrCreateExtension

    virtual void identify(IIdentities *identities, RdId id);

    /*void print(PrettyPrinter printer) {
        printer.print(toString())
        printer.print(" (")
        printer.print(rdid.toString())
        printer.print(")")
    }*/
};

//T : RdBindableBase
template<typename T>
T &withId(T &that, RdId id) {
//    assert(that->rd_id == RdId::Null() && "this.id != RdId.NULL_ID, but ${this.rdid}");
//    assert(that->id != RdId::Null() && "id != RdId.NULL_ID");

    that.rd_id = id;
    return that;
}

template<typename T>
T &statics(T &that, int32_t id) {
//    assert(id > 0 && id < RdId::MAX_STATIC_ID && "Expected id > 0 && id < RdId.MaxStaticId, got $id" );
    return withId(that, RdId(static_cast<int64_t >(id)));
}

template<typename T>
T &withIdFromName(T &that, std::string const &name) {
    return withId(that, RdId::Null().mix(name));
}


#endif //RD_CPP_RDBINDABLEBASE_H
