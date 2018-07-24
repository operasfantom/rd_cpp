//
// Created by jetbrains on 23.07.2018.
//

#ifndef RD_CPP_RDBINDABLEBASE_H
#define RD_CPP_RDBINDABLEBASE_H


#include <any>
#include "IRdBindable.h"

class RdBindableBase : public IRdBindable/*, IPrintable*/ {
protected:
    //bound state: main

    IRdDynamic *parent = nullptr;

    std::optional<Lifetime> bind_lifetime;

    //bound state: inferred

    bool is_bound();

    IProtocol *get_protocol();

    std::vector<std::pair<std::string, std::any> > bindable_children;

    SerializationCtx get_serialization_ctx();

    /*val containingExt: RdExtBase?
                       get() {
                               var cur: IRdDynamic? = this
                    while (cur is RdBindableBase) {
                        if (cur is RdExtBase) return cur
                        cur = cur.parent
                    }
                    return null
                       }*/

public:
    RdBindableBase() = default;

    //need to implement in subclasses
    RdBindableBase(Lifetime lifetime);

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

    RdBindableBase *withId(RdId id) {
//        require(this->rd_id == RdId::get_null()) {"this.id != RdId.NULL_ID, but ${this.rdid}"}
//        require(id != RdId.NULL_ID) {"id != RdId.NULL_ID"}

        this->rd_id = id;
        return this;
    }

    RdBindableBase *statics(int32_t id) {
//        require(id > 0 && id < RdId.MAX_STATIC_ID) { "Expected id > 0 && id < RdId.MaxStaticId, got $id" }
        return withId(RdId(static_cast<int64_t >(id)));
    }

    RdBindableBase *withIdFromName(std::string const &name) {
        return withId(RdId::Null().mix(name));
    }
};


#endif //RD_CPP_RDBINDABLEBASE_H
