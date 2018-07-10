//
// Created by jetbrains on 10.07.2018.
//

#ifndef RD_CPP_VIEWABLE_COLLECTIONS_H
#define RD_CPP_VIEWABLE_COLLECTIONS_H

#include "interfaces.h"

#include <functional>
#include <set>
#include <unordered_set>
#include <list>

enum class AddRemove {
    ADD, REMOVE
};

template<typename T>
class IViewableSet : std::unordered_set<T>, IViewable<T>, ISource<typename IViewableSet<T>::Event> {

    class Event {
        AddRemove kind;
        T value;
    };

    virtual void advise(Lifetime *lt, std::function<void(AddRemove, T)> handler);


    virtual void view(Lifetime *lt, std::function<void(Lifetime *, T)> handler);
};

template<typename T>
class IMutableViewableSet : public std::unordered_set<T>, public IViewableSet<T> {
};

template<typename K, typename V>
class KeyValuePair {
    K key;
    V value;
};

template<typename K, typename V>
class IViewableMap : std::unordered_map<K, V>, IViewable<KeyValuePair<K, V>>, ISource<typename IViewableMap<K, V>::Event> {
    //TODO Event
    virtual void view(Lifetime* lt, std::function<void(Lifetime*, std::pair<K, V>)> handler);

    void advise_add_remove(Lifetime* lt, std::function<void(AddRemove, K, V)> handler);

    void view(Lifetime* lt, std::function<void(Lifetime*, K, V)> handler);
};

template<typename V>
class IViewableList<V> : std::list<V>, IViewable<std::pair<int, V>>, ISource<typename IViewableList<V>::Event> {
    ISource<IViewableList<V>::Event>* change;

    //TODO Event

    void advise_add_remove(Lifetime *lt, std::function<void(AddRemove, int, V)> handler);

    void view(Lifetime *lt, std::function<void(Lifetime *lt, std::pair<int, V>)> handler);

    void view(Lifetime *lt, std::function<void(Lifetime *, int, V)> handler);
};

template <typename K, typename V>
class IAsyncViewableMap: IViewableMap <K, V>, IAsyncSource<IViewableMap<K, V>::Event> {};

template <typename K, typename V>
class IMutableViewableMap : std::unordered_map<K, V>, IViewableMap <K, V> {};

template <typename V>
class IMutableViewableList : std::list<V>, IViewableList <V> {};


#endif //RD_CPP_VIEWABLE_COLLECTIONS_H
