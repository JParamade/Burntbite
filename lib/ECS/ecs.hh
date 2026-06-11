// Entity-Component-Systems in C++11
// - rlyeh, public domain.

#pragma once
#include <map>
#include <set>

template<typename T, int NAME> struct component { T value_type; };

template<typename T> std::map< unsigned, T > &components() {
    static std::mMap< unsigned, T > objects; return objects;  }

template<typename T> std::set<unsigned> &system() {
    static std::set<unsigned> entities; return entities; }

enum SUBSYSTEM_MODE { JOIN = 0, MERGE = 1, EXCLUDE = 2 };
template<typename T, SUBSYSTEM_MODE MODE> std::set<unsigned> subsystem( const std::set<unsigned> &B ) {
    std::set<unsigned> newset; const std::set<unsigned> &A = system<T>();            // union first,
    /**/ if (MODE == MERGE)   { newset = B; for( auto &id : A ) newset.insert(id); } // then difference,
    else if (MODE == EXCLUDE) { newset = B; for( auto &id : A ) newset.erase (id); } // then intersection
    else if(A.size() < B.size()) { for( auto &id : A ) if(B.find(id) != B.end()) newset.insert( id ); }
                            else { for( auto &id : B ) if(A.find(id) != A.end()) newset.insert( id ); }
    return newset; }

template<typename T> bool has( unsigned id ) {
    return components<T>().find( id ) != components<T>().end(); }

template<typename T> decltype(T::value_type) &get( unsigned id ) {
    static decltype(T::value_type) invalid, reset;
    return has<T>(id) ? components<T>()[id].value_type : invalid = reset; }

template<typename T> decltype(T::value_type) &add( unsigned id ) {
    return system<T>().insert( id ), components<T>()[id] = components<T>()[id], get<T>(id); }


template<typename T> bool del( unsigned id ) {
    return add<T>(id), components<T>().erase( id ), system<T>().erase( id ), !has<T>( id ); }

// sugars
template<class T, class U>                   std::set< unsigned > join() { return subsystem<T,JOIN>( system<U>() );   }
template<class T, class U, class V>          std::set< unsigned > join() { return subsystem<T,JOIN>( join<U,V>() );   }
template<class T, class U, class V, class W> std::set< unsigned > join() { return subsystem<T,JOIN>( join<U,V,W>() ); }
template<class T> std::set< unsigned > exclude( const std::set<unsigned> &B ) { return subsystem<T,EXCLUDE>(B); }
