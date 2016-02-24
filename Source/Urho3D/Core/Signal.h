///
/// Wink Signals
/// Copyright (C) 2013-2014 Miguel Martin (miguel@miguel-martin.com)
///
///
/// This software is provided 'as-is', without any express or implied warranty.
/// In no event will the authors be held liable for any damages arising from the
/// use of this software.
///
/// Permission is hereby granted, free of charge, to any person
/// obtaining a copy of this software and associated documentation files (the "Software"),
/// to deal in the Software without restriction, including without limitation the rights
/// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
/// copies of the Software, and to permit persons to whom the Software is
/// furnished to do so, subject to the following conditions:
///
/// 1. The origin of this software must not be misrepresented;
///    you must not claim that you wrote the original software.
///    If you use this software in a product, an acknowledgment
///    in the product documentation would be appreciated but is not required.
///
/// 2. Altered source versions must be plainly marked as such,
///	   and must not be misrepresented as being the original software.
///
/// 3. The above copyright notice and this permission notice shall be included in
///    all copies or substantial portions of the Software.
///
/// Edited by svifylabs for urho3d
/// Benchmarks:
///     https://github.com/NoAvailableAlias/signal-slot-benchmarks
/// References: 
///     https://github.com/miguelmartin75/Wink-Signals
///     https://github.com/GarageGames/Torque3D/blob/2044b2691e1a29fa65d1bdd163f0d834995433ce/Engine/source/core/util/tSignal.h
///     https://github.com/realXtend/tundra-urho3d/blob/master/src/TundraCore/Signals/Signals.h
///     http://www.codeproject.com/Articles/7150/Member-Function-Pointers-and-the-Fastest-Possible
///     http://www.codeproject.com/Articles/11015/The-Impossibly-Fast-C-Delegates
///     http://hoyvinglavin.com/2012/08/06/jl_signal/
#pragma once

#include "../Container/Vector.h"
#include "../Container/Vector.h"
#include "Delegate.h"
#include <assert.h>

namespace Urho3D
{
/// TODO: connect to other signal. prevent connecting to self. prevent infinite emit loop !?
/// TODO: disconnect on slot object destroyed. handle automatic disconnect form Signal when destroyed
/// TODO: handle disconnect on emit
/// TODO: delegate container
/// 
/*
template <class InputIterator, class T>
T accumulate (InputIterator first, InputIterator last, T init)
{
while (first!=last) {
init = init + *first;  // or: init=binary_op(init,*first) for the binary_op version
++first;
}
return init;
}
*/

template<class T>
class DelegateRemapper : public DelegateMemento
{
public:
    DelegateRemapper() : mOffset(0) {}

    void Set(T * t, const DelegateMemento & memento)
    {
        SetMementoFrom(memento);
        if (m_pthis)
            mOffset = ((int)m_pthis) - ((int)t);
    }

    void Rethis(T * t)
    {
        if (m_pthis)
            m_pthis = (detail::GenericClass *)(mOffset + (int)t);
    }

protected:
    int mOffset;
};


template <typename Signature>
class Signal;

template<typename R>
class Signal< R() >
{
public:
    /// Make using the slot type a bit easier via typedef.
    typedef Delegate0 <  R  > SlotType;
    /// Allow users access to the specific type of this delegate.
    typedef Signal SelfType;
    // Constructor.
    Signal() { }
    /// Copy Constructor.
    Signal(const SelfType &base): slots_(base.slots_) { }
    /// Connect to member function.
    template < class X, class Y >
    void Connect(Y * pthis, R(X::* function_to_bind)())
    {
        if (!pthis) return; slots_.Push(SlotType(pthis, function_to_bind));
    }
    /// Connect to const member function.
    template < class X, class Y >
    void Connect(const Y *pthis, R(X::* function_to_bind)() const)
    {
        if (!pthis) return; slots_.Push(SlotType(pthis, function_to_bind));
    }
    /// Connect to static function.
    void Connect(R(*function_to_bind)())
    {
        slots_.Push(SlotType(function_to_bind));
    }
    /// Connect to slot.
    void Connect(SlotType& slot)
    {
        slots_.Push(slot);
    }
    /// Connect to signal.
    void Connect(SelfType& signal)
    {
        assert(&signal != this);
        if (&signal == this)
            return;
        Connect(&signal, &SelfType::Emit);
    }
    /// Emit signal to all slots.
    void Emit() const
    {
        for (Vector< SlotType >::ConstIterator i = slots_.Begin(); i != slots_.End();++i )
            (*i)();        
    }
    /// Emit signal and collect all return values.
    void Aggregate(PODVector<R>& container )
    {
        for (Vector< SlotType >::ConstIterator i = slots_.Begin(); i != slots_.End(); ++i)
            container.Push((*i)());
    }
    /// Disconnect from slot.
    void Disconnect(SlotType& slot) { slots_.Remove(slot); }
    /// Disconnect from signal.
    void Disconnect(SelfType& signal) { Disconnect(&signal, &SelfType::Emit); }
    /// Disconnect from member function.
    template< class X, class Y >
    void Disconnect(Y * obj, R(X::*function_to_bind)())
    {
        slots_.Remove(SlotType(obj, function_to_bind));
    }
    /// Disconnect from const member function.
    template< class X, class Y >
    void Disconnect(Y * obj, R(X::*function_to_bind)() const)
    {
        slots_.Remove(SlotType(obj, function_to_bind));
    }
    /// Disconnect from static function.
    void Disconnect(R(*function_to_bind)())
    {
        slots_.Remove(SlotType(function_to_bind));
    }
    /// Disconnect from all slots.
    void Clear() { slots_.Clear(); }
    /// Return whether signal has any Connections.
    bool Empty() const { return slots_.Empty(); }
    /// Copy from base into this.
    void Merge(const SelfType& base)
    {
        slots_.Push(base.slots_);
    }
    /// Returns true if the signal already contains this slot.
    bool Contains(SlotType& slot) const
    {
        return  slots_.Find(slot) != slots_.End();
    }
    /// Returns true if the signal already contains this slot.
    bool Contains(SelfType& signal)
    {
        return Contains(&signal, &SelfType::Emit);
    }
    /// Returns true if the signal already contains this slot.
    template< class X, class Y >
    bool Contains(Y * obj, R(X::*function_to_bind)())
    {
        return  slots_.Find(SlotType(obj, function_to_bind)) != slots_.End();
    }
    /// Returns true if the signal already contains this slot.
    template< class X, class Y >
    bool Contains(Y * obj, R(X::*function_to_bind)() const)
    {
        return  slots_.Find(SlotType(obj, function_to_bind)) != slots_.End();
    }

    /// Assign Operator.
    void operator =(SelfType& base)
    {
        Clear();
        slots_.Push(base.slots_);
    }
    /// Connect to slot.
    SelfType& operator+=(SlotType& slot)
    {
        slots_.Push(slot);
        return *this;
    }
    /// Connect to signal.
    SelfType& operator+=(SelfType& signal)
    {
        assert(&signal != this);
        if (&signal == this)
            return *this;
        Connect(&signal, &SelfType::Emit);
        return *this;
    }
    /// Emit signal to all slots.
    void operator() () const {
        for (Vector< SlotType >::ConstIterator i = slots_.Begin(); i != slots_.End();++i )
            (*i)();
    }
    /// Disconnect to slot.
    SelfType& operator-=(SlotType& slot)
    {
        slots_.Remove(slot);
        return *this;
    }
    /// Disconnect to signal.
    SelfType& operator-=(SelfType& signal)
    {
        Disconnect(&signal, &SelfType::Emit);
        return *this;
    }

protected:
    Vector< SlotType > slots_;
};

template<typename R, class Param1 >
class Signal< R(Param1) >
{
public:
    /// Make using the slot type a bit easier via typedef.
    typedef Delegate1 < Param1, R  > SlotType;
    /// Allow users access to the specific type of this delegate.
    typedef Signal SelfType;
    // Constructor.
    Signal() { }
    /// Copy Constructor.
    Signal(const SelfType &base): slots_(base.slots_) { }
    /// Connect to member function.
    template < class X, class Y >
    void Connect(Y * pthis, R(X::* function_to_bind)(Param1 p1))
    {
        if (!pthis) return;
        slots_.Push(SlotType(pthis, function_to_bind));
    }
    /// Connect to const member function.
    template < class X, class Y >
    void Connect(const Y *pthis, R(X::* function_to_bind)(Param1 p1) const)
    {
        if (!pthis) return;
        slots_.Push(SlotType(pthis, function_to_bind));
    }
    /// Connect to static function.
    void Connect(R(*function_to_bind)(Param1 p1))
    {
        slots_.Push(SlotType(function_to_bind));
    }
    /// Connect to slot.
    void Connect(SlotType& slot)
    {
        slots_.Push(slot);
    }
    /// Connect to signal.
    void Connect(SelfType& signal)
    {
        assert(&signal != this);
        if (&signal == this)
            return;
        Connect(&signal, &SelfType::Emit);
    }
    /// Emit signal to all slots.
    void Emit(Param1 p1) const
    {
        for (Vector< SlotType >::ConstIterator i = slots_.Begin(); i != slots_.End();++i )
            (*i)(p1);    
    }
    /// Emit signal and collect all return values.
    void Aggregate(Param1 p1,PODVector<R>& container)
    {
        for (Vector< SlotType >::ConstIterator i = slots_.Begin(); i != slots_.End(); ++i)
            container.Push((*i)(p1));
    }
    /// Disconnect from slot.
    void Disconnect(SlotType& slot) { slots_.Remove(slot); }
    /// Disconnect from signal.
    void Disconnect(SelfType& signal) { Disconnect(&signal, &SelfType::Emit); }
    /// Disconnect from member function.
    template< class X, class Y >
    void Disconnect(Y * obj, R(X::*function_to_bind)(Param1 p1))
    {
        slots_.Remove(SlotType(obj, function_to_bind));
    }
    /// Disconnect from const member function.
    template< class X, class Y >
    void Disconnect(Y * obj, R(X::*function_to_bind)(Param1 p1) const)
    {
        slots_.Remove(SlotType(obj, function_to_bind));
    }
    /// Disconnect from static function.
    void Disconnect(R(*function_to_bind)(Param1 p1))
    {
        slots_.Remove(SlotType(function_to_bind));
    }
    /// Disconnect from all slots.
    void Clear() { slots_.Clear(); }
    /// Return whether signal has any Connections.
    bool Empty() const { return slots_.Empty(); }
    /// Copy from base into this.
    void Merge(const SelfType& base)
    {
        slots_.Push(base.slots_);
    }
    /// Returns true if the signal already contains this slot.
    bool Contains(SlotType& slot) const
    {
        return  slots_.Find(slot) != slots_.End();
    }
    /// Returns true if the signal already contains this slot.
    bool Contains(SelfType& signal)
    {
        return Contains(&signal, &SelfType::Emit);
    }
    /// Returns true if the signal already contains this slot.
    template< class X, class Y >
    bool Contains(Y * obj, R(X::*function_to_bind)(Param1 p1))
    {
        return  slots_.Find(SlotType(obj, function_to_bind)) != slots_.End();
    }
    /// Returns true if the signal already contains this slot.
    template< class X, class Y >
    bool Contains(Y * obj, R(X::*function_to_bind)(Param1 p1) const)
    {
        return  slots_.Find(SlotType(obj, function_to_bind)) != slots_.End();
    }

    /// Assign Operator.
    void operator =(SelfType& base)
    {
        Clear();
        slots_.Push(base.slots_);
    }
    /// Connect to slot.
    SelfType& operator+=(SlotType& slot)
    {
        slots_.Push(slot);
        return *this;
    }
    /// Connect to signal.
    SelfType& operator+=(SelfType& signal)
    {
        assert(&signal != this);
        if (&signal == this)
            return *this;
        Connect(&signal, &SelfType::Emit);
        return *this;
    }
    /// Emit signal to all slots.
    void operator()(Param1 p1) const {
        for (Vector< SlotType >::ConstIterator i = slots_.Begin(); i != slots_.End();++i )
            (*i)(p1);       
    }
    /// Disconnect to slot.
    SelfType& operator-=(SlotType& slot)
    {
        slots_.Remove(slot);
        return *this;
    }
    /// Disconnect to signal.
    SelfType& operator-=(SelfType& signal)
    {
        Disconnect(&signal, &SelfType::Emit);
        return *this;
    }
protected:
     
    Vector< SlotType > slots_;
};

template<typename R, class Param1, class Param2>
class Signal< R(Param1, Param2) >
{
public:
    /// Make using the slot type a bit easier via typedef.
    typedef Delegate2 < Param1, Param2, R  > SlotType;
    /// Allow users access to the specific type of this delegate.
    typedef Signal SelfType;
    // Constructor.
    Signal() { }
    /// Copy Constructor.
    Signal(const SelfType &base): slots_(base.slots_) { }
    /// Connect to member function.
    template < class X, class Y >
    void Connect(Y * pthis, R(X::* function_to_bind)(Param1 p1, Param2 p2))
    {
        if (!pthis) return;
        slots_.Push(SlotType(pthis, function_to_bind));
    }
    /// Connect to const member function.
    template < class X, class Y >
    void Connect(const Y *pthis, R(X::* function_to_bind)(Param1 p1, Param2 p2) const)
    {
        if (!pthis) return;
        slots_.Push(SlotType(pthis, function_to_bind));
    }
    /// Connect to static function.
    void Connect(R(*function_to_bind)(Param1 p1, Param2 p2))
    {
        slots_.Push(SlotType(function_to_bind));
    }
    /// Connect to slot.
    void Connect(SlotType& slot)
    {
        slots_.Push(slot);
    }
    /// Connect to signal.
    void Connect(SelfType& signal)
    {
        assert(&signal != this);
        if (&signal == this)
            return;
        Connect(&signal, &SelfType::Emit);
    }
    /// Emit signal to all slots.
    void Emit(Param1 p1, Param2 p2) const
    {
        for (Vector< SlotType >::ConstIterator i = slots_.Begin(); i != slots_.End();++i )
            (*i)(p1, p2);      
    }
    /// Emit signal and collect all return values.
    void Aggregate(Param1 p1, Param2 p2, PODVector<R>& container)
    {
        for (Vector< SlotType >::ConstIterator i = slots_.Begin(); i != slots_.End(); ++i)
            container.Push((*i)(p1, p2));
    }
    /// Disconnect from slot.
    void Disconnect(SlotType& slot) { slots_.Remove(slot); }
    /// Disconnect from signal.
    void Disconnect(SelfType& signal) { Disconnect(&signal, &SelfType::Emit); }
    /// Disconnect from member function.
    template< class X, class Y >
    void Disconnect(Y * obj, R(X::*function_to_bind)(Param1 p1, Param2 p2))
    {
        slots_.Remove(SlotType(obj, function_to_bind));
    }
    /// Disconnect from const member function.
    template< class X, class Y >
    void Disconnect(Y * obj, R(X::*function_to_bind)(Param1 p1, Param2 p2) const)
    {
        slots_.Remove(SlotType(obj, function_to_bind));
    }
    /// Disconnect from static function.
    void Disconnect(R(*function_to_bind)(Param1 p1, Param2 p2))
    {
        slots_.Remove(SlotType(function_to_bind));
    }
    /// Disconnect from all slots.
    void Clear() { slots_.Clear(); }
    /// Return whether signal has any Connections.
    bool Empty() const { return slots_.Empty(); }
    /// Copy from base into this.
    void Merge(const SelfType& base)
    {
        slots_.Push(base.slots_);
    }
    /// Returns true if the signal already contains this slot.
    bool Contains(SlotType& slot) const
    {
        return  slots_.Find(slot) != slots_.End();
    }
    /// Returns true if the signal already contains this slot.
    bool Contains(SelfType& signal)
    {
        return Contains(&signal, &SelfType::Emit);
    }
    /// Returns true if the signal already contains this slot.
    template< class X, class Y >
    bool Contains(Y * obj, R(X::*function_to_bind)(Param1 p1, Param2 p2))
    {
        return  slots_.Find(SlotType(obj, function_to_bind)) != slots_.End();
    }
    /// Returns true if the signal already contains this slot.
    template< class X, class Y >
    bool Contains(Y * obj, R(X::*function_to_bind)(Param1 p1, Param2 p2) const)
    {
        return  slots_.Find(SlotType(obj, function_to_bind)) != slots_.End();
    }

    /// Assign Operator.
    void operator =(SelfType& base)
    {
        Clear();
        slots_.Push(base.slots_);
    }
    /// Connect to slot.
    SelfType& operator+=(SlotType& slot)
    {
        slots_.Push(slot);
        return *this;
    }
    /// Connect to signal.
    SelfType& operator+=(SelfType& signal)
    {
        assert(&signal != this);
        if (&signal == this)
            return *this;
        Connect(&signal, &SelfType::Emit);
        return *this;
    }
    /// Emit signal to all slots.
    void operator() (Param1 p1, Param2 p2) const 
    {
        for (Vector< SlotType >::ConstIterator i = slots_.Begin(); i != slots_.End();++i )
            (*i)(p1, p2);       
    }
    /// Disconnect to slot.
    SelfType& operator-=(SlotType& slot)
    {
        slots_.Remove(slot);
        return *this;
    }
    /// Disconnect to signal.
    SelfType& operator-=(SelfType& signal)
    {
        Disconnect(&signal, &SelfType::Emit);
        return *this;
    }
protected:
     
    Vector< SlotType > slots_;
};

template<typename R, class Param1, class Param2, class Param3>
class Signal< R(Param1, Param2, Param3) >
{
public:
    /// Make using the slot type a bit easier via typedef.
    typedef Delegate3 < Param1, Param2, Param3, R  > SlotType;
    /// Allow users access to the specific type of this delegate.
    typedef Signal SelfType;
    // Constructor.
    Signal() { }
    /// Copy Constructor.
    Signal(const SelfType &base): slots_(base.slots_) { }
    /// Connect to member function.
    template < class X, class Y >
    void Connect(Y * pthis, R(X::* function_to_bind)(Param1 p1, Param2 p2, Param3 p3))
    {
        if (!pthis) return;
        slots_.Push(SlotType(pthis, function_to_bind));
    }
    /// Connect to const member function.
    template < class X, class Y >
    void Connect(const Y *pthis, R(X::* function_to_bind)(Param1 p1, Param2 p2, Param3 p3) const)
    {
        if (!pthis) return;
        slots_.Push(SlotType(pthis, function_to_bind));
    }
    /// Connect to static function.
    void Connect(R(*function_to_bind)(Param1 p1, Param2 p2, Param3 p3))
    {
        slots_.Push(SlotType(function_to_bind));
    }
    /// Connect to slot.
    void Connect(SlotType& slot)
    {
        slots_.Push(slot);
    }
    /// Connect to signal.
    void Connect(SelfType& signal)
    {
        assert(&signal != this);
        if (&signal == this)
            return;
        Connect(&signal, &SelfType::Emit);
    }
    /// Emit signal to all slots.
    void Emit(Param1 p1, Param2 p2, Param3 p3) const
    {
        for (Vector< SlotType >::ConstIterator i = slots_.Begin(); i != slots_.End();++i )
            (*i)(p1, p2, p3);      
    }
    /// Emit signal and collect all return values.
    void Aggregate(Param1 p1, Param2 p2, Param3 p3, PODVector<R>& container)
    {
        for (Vector< SlotType >::ConstIterator i = slots_.Begin(); i != slots_.End(); ++i)
            container.Push((*i)(p1, p2, p3));
    }
    /// Disconnect from slot.
    void Disconnect(SlotType& slot) { slots_.Remove(slot); }
    /// Disconnect from signal.
    void Disconnect(SelfType& signal) { Disconnect(&signal, &SelfType::Emit); }
    /// Disconnect from member function.
    template< class X, class Y >
    void Disconnect(Y * obj, R(X::*function_to_bind)(Param1 p1, Param2 p2, Param3 p3))
    {
        slots_.Remove(SlotType(obj, function_to_bind));
    }
    /// Disconnect from const member function.
    template< class X, class Y >
    void Disconnect(Y * obj, R(X::*function_to_bind)(Param1 p1, Param2 p2, Param3 p3) const)
    {
        slots_.Remove(SlotType(obj, function_to_bind));
    }
    /// Disconnect from static function.
    void Disconnect(R(*function_to_bind)(Param1 p1, Param2 p2, Param3 p3))
    {
        slots_.Remove(SlotType(function_to_bind));
    }
    /// Disconnect from all slots.
    void Clear() { slots_.Clear(); }
    /// Return whether signal has any Connections.
    bool Empty() const { return slots_.Empty(); }
    /// Copy from base into this.
    void Merge(const SelfType& base)
    {
        slots_.Push(base.slots_);
    }
    /// Returns true if the signal already contains this slot.
    bool Contains(SlotType& slot) const
    {
        return  slots_.Find(slot) != slots_.End();
    }
    /// Returns true if the signal already contains this slot.
    bool Contains(SelfType& signal)
    {
        return Contains(&signal, &SelfType::Emit);
    }
    /// Returns true if the signal already contains this slot.
    template< class X, class Y >
    bool Contains(Y * obj, R(X::*function_to_bind)(Param1 p1, Param2 p2, Param3 p3))
    {
        return  slots_.Find(SlotType(obj, function_to_bind)) != slots_.End();
    }
    /// Returns true if the signal already contains this slot.
    template< class X, class Y >
    bool Contains(Y * obj, R(X::*function_to_bind)(Param1 p1, Param2 p2, Param3 p3) const)
    {
        return  slots_.Find(SlotType(obj, function_to_bind)) != slots_.End();
    }

    /// Assign Operator.
    void operator =(SelfType& base)
    {
        Clear();
        slots_.Push(base.slots_);
    }
    /// Connect to slot.
    SelfType& operator+=(SlotType& slot)
    {
        slots_.Push(slot);
        return *this;
    }
    /// Connect to signal.
    SelfType& operator+=(SelfType& signal)
    {
        assert(&signal != this);
        if (&signal == this)
            return *this;
        Connect(&signal, &SelfType::Emit);
        return *this;
    }
    /// Emit signal to all slots.
    void operator() (Param1 p1, Param2 p2, Param3 p3) const {
        for (Vector< SlotType >::ConstIterator i = slots_.Begin(); i != slots_.End();++i )
            (*i)(p1, p2, p3);       
    }
    /// Disconnect to slot.
    SelfType& operator-=(SlotType& slot)
    {
        slots_.Remove(slot);
        return *this;
    }
    /// Disconnect to signal.
    SelfType& operator-=(SelfType& signal)
    {
        Disconnect(&signal, &SelfType::Emit);
        return *this;
    }
protected:
     
    Vector< SlotType > slots_;
};

template<typename R, class Param1, class Param2, class Param3, class Param4>
class Signal< R(Param1, Param2, Param3, Param4) >
{
public:
    /// Make using the slot type a bit easier via typedef.
    typedef Delegate4 < Param1, Param2, Param3, Param4, R  > SlotType;
    /// Allow users access to the specific type of this delegate.
    typedef Signal SelfType;
    // Constructor.
    Signal() { }
    /// Copy Constructor.
    Signal(const SelfType &base): slots_(base.slots_) { }
    /// Connect to member function.
    template < class X, class Y >
    void Connect(Y * pthis, R(X::* function_to_bind)(Param1 p1, Param2 p2, Param3 p3, Param4 p4))
    {
        if (!pthis) return;
        slots_.Push(SlotType(pthis, function_to_bind));
    }
    /// Connect to const member function.
    template < class X, class Y >
    void Connect(const Y *pthis, R(X::* function_to_bind)(Param1 p1, Param2 p2, Param3 p3, Param4 p4) const)
    {
        if (!pthis) return;
        slots_.Push(SlotType(pthis, function_to_bind));
    }
    /// Connect to static function.
    void Connect(R(*function_to_bind)(Param1 p1, Param2 p2, Param3 p3, Param4 p4))
    {
        slots_.Push(SlotType(function_to_bind));
    }
    /// Connect to slot.
    void Connect(SlotType& slot)
    {
        slots_.Push(slot);
    }
    /// Connect to signal.
    void Connect(SelfType& signal)
    {
        assert(&signal != this);
        if (&signal == this)
            return;
        Connect(&signal, &SelfType::Emit);
    }
    /// Emit signal to all slots.
    void Emit(Param1 p1, Param2 p2, Param3 p3, Param4 p4) const
    {
        for (Vector< SlotType >::ConstIterator i = slots_.Begin(); i != slots_.End();++i )
            (*i)(p1, p2, p3, p4);        
    }
    /// Emit signal and collect all return values.
    void Aggregate(Param1 p1, Param2 p2, Param3 p3, Param4 p4, PODVector<R>& container)
    {
        for (Vector< SlotType >::ConstIterator i = slots_.Begin(); i != slots_.End(); ++i)
            container.Push((*i)(p1, p2, p3, p4));
    }
    /// Disconnect from slot.
    void Disconnect(SlotType& slot) { slots_.Remove(slot); }
    /// Disconnect from signal.
    void Disconnect(SelfType& signal) { Disconnect(&signal, &SelfType::Emit); }
    /// Disconnect from member function.
    template< class X, class Y >
    void Disconnect(Y * obj, R(X::*function_to_bind)(Param1 p1, Param2 p2, Param3 p3, Param4 p4))
    {
        slots_.Remove(SlotType(obj, function_to_bind));
    }
    /// Disconnect from const member function.
    template< class X, class Y >
    void Disconnect(Y * obj, R(X::*function_to_bind)(Param1 p1, Param2 p2, Param3 p3, Param4 p4) const)
    {
        slots_.Remove(SlotType(obj, function_to_bind));
    }
    /// Disconnect from static function.
    void Disconnect(R(*function_to_bind)(Param1 p1, Param2 p2, Param3 p3, Param4 p4))
    {
        slots_.Remove(SlotType(function_to_bind));
    }
    /// Disconnect from all slots.
    void Clear() { slots_.Clear(); }
    /// Return whether signal has any Connections.
    bool Empty() const { return slots_.Empty(); }
    /// Copy from base into this.
    void Merge(const SelfType& base)
    {
        slots_.Push(base.slots_);
    }
    /// Returns true if the signal already contains this slot.
    bool Contains(SlotType& slot) const
    {
        return  slots_.Find(slot) != slots_.End();
    }
    /// Returns true if the signal already contains this slot.
    bool Contains(SelfType& signal)
    {
        return Contains(&signal, &SelfType::Emit);
    }
    /// Returns true if the signal already contains this slot.
    template< class X, class Y >
    bool Contains(Y * obj, R(X::*function_to_bind)(Param1 p1, Param2 p2, Param3 p3, Param4 p4))
    {
        return  slots_.Find(SlotType(obj, function_to_bind)) != slots_.End();
    }
    /// Returns true if the signal already contains this slot.
    template< class X, class Y >
    bool Contains(Y * obj, R(X::*function_to_bind)(Param1 p1, Param2 p2, Param3 p3, Param4 p4) const)
    {
        return  slots_.Find(SlotType(obj, function_to_bind)) != slots_.End();
    }

    /// Assign Operator.
    void operator =(SelfType& base)
    {
        Clear();
        slots_.Push(base.slots_);
    }
    /// Connect to slot.
    SelfType& operator+=(SlotType& slot)
    {
        slots_.Push(slot);
        return *this;
    }
    /// Connect to signal.
    SelfType& operator+=(SelfType& signal)
    {
        assert(&signal != this);
        if (&signal == this)
            return *this;
        Connect(&signal, &SelfType::Emit);
        return *this;
    }
    /// Emit signal to all slots.
    void operator() (Param1 p1, Param2 p2, Param3 p3, Param4 p4) const {
        for (Vector< SlotType >::ConstIterator i = slots_.Begin(); i != slots_.End();++i )
            (*i)(p1, p2, p3, p4);       
    }
    /// Disconnect to slot.
    SelfType& operator-=(SlotType& slot)
    {
        slots_.Remove(slot);
        return *this;
    }
    /// Disconnect to signal.
    SelfType& operator-=(SelfType& signal)
    {
        Disconnect(&signal, &SelfType::Emit);
        return *this;
    }
protected:
     
    Vector< SlotType > slots_;
};

template<typename R, class Param1, class Param2, class Param3, class Param4, class Param5>
class Signal< R(Param1, Param2, Param3, Param4, Param5) >
{
public:
    /// Make using the slot type a bit easier via typedef.
    typedef Delegate5 < Param1, Param2, Param3, Param4, Param5, R  > SlotType;
    /// Allow users access to the specific type of this delegate.
    typedef Signal SelfType;
    // Constructor.
    Signal() { }
    /// Copy Constructor.
    Signal(const SelfType &base): slots_(base.slots_) { }
    /// Connect to member function.
    template < class X, class Y >
    void Connect(Y * pthis, R(X::* function_to_bind)(Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5))
    {
        if (!pthis) return;
        slots_.Push(SlotType(pthis, function_to_bind));
    }
    /// Connect to const member function.
    template < class X, class Y >
    void Connect(const Y *pthis, R(X::* function_to_bind)(Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5) const)
    {
        if (!pthis) return;
        slots_.Push(SlotType(pthis, function_to_bind));
    }
    /// Connect to static function.
    void Connect(R(*function_to_bind)(Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5))
    {
        slots_.Push(SlotType(function_to_bind));
    }
    /// Connect to slot.
    void Connect(SlotType& slot)
    {
        slots_.Push(slot);
    }
    /// Connect to signal.
    void Connect(SelfType& signal)
    {
        assert(&signal != this);
        if (&signal == this)
            return;
        Connect(&signal, &SelfType::Emit);
    }
    /// Emit signal to all slots.
    void Emit(Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5) const
    {    
        for (Vector< SlotType >::ConstIterator i = slots_.Begin(); i != slots_.End();++i )
            (*i)(p1, p2, p3, p4, p5);       
    }
    /// Emit signal and collect all return values.
    void Aggregate(Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5, PODVector<R>& container)
    {
        for (Vector< SlotType >::ConstIterator i = slots_.Begin(); i != slots_.End(); ++i)
            container.Push((*i)(p1, p2, p3, p4, p5));
    }
    /// Disconnect from slot.
    void Disconnect(SlotType& slot) { slots_.Remove(slot); }
    /// Disconnect from signal.
    void Disconnect(SelfType& signal) { Disconnect(&signal, &SelfType::Emit); }
    /// Disconnect from member function.
    template< class X, class Y >
    void Disconnect(Y * obj, R(X::*function_to_bind)(Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5))
    {
        slots_.Remove(SlotType(obj, function_to_bind));
    }
    /// Disconnect from const member function.
    template< class X, class Y >
    void Disconnect(Y * obj, R(X::*function_to_bind)(Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5) const)
    {
        slots_.Remove(SlotType(obj, function_to_bind));
    }
    /// Disconnect from static function.
    void Disconnect(R(*function_to_bind)(Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5))
    {
        slots_.Remove(SlotType(function_to_bind));
    }
    /// Disconnect from all slots.
    void Clear() { slots_.Clear(); }
    /// Return whether signal has any Connections.
    bool Empty() const { return slots_.Empty(); }
    /// Copy from base into this.
    void Merge(const SelfType& base)
    {
        slots_.Push(base.slots_);
    }
    /// Returns true if the signal already contains this slot.
    bool Contains(SlotType& slot) const
    {
        return  slots_.Find(slot) != slots_.End();
    }
    /// Returns true if the signal already contains this slot.
    bool Contains(SelfType& signal)
    {
        return Contains(&signal, &SelfType::Emit);
    }
    /// Returns true if the signal already contains this slot.
    template< class X, class Y >
    bool Contains(Y * obj, R(X::*function_to_bind)(Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5))
    {
        return  slots_.Find(SlotType(obj, function_to_bind)) != slots_.End();
    }
    /// Returns true if the signal already contains this slot.
    template< class X, class Y >
    bool Contains(Y * obj, R(X::*function_to_bind)(Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5) const)
    {
        return  slots_.Find(SlotType(obj, function_to_bind)) != slots_.End();
    }

    /// Assign Operator.
    void operator =(SelfType& base)
    {
        Clear();
        slots_.Push(base.slots_);
    }
    /// Connect to slot.
    SelfType& operator+=(SlotType& slot)
    {
        slots_.Push(slot);
        return *this;
    }
    /// Connect to signal.
    SelfType& operator+=(SelfType& signal)
    {
        assert(&signal != this);
        if (&signal == this)
            return *this;
        Connect(&signal, &SelfType::Emit);
        return *this;
    }
    /// Emit signal to all slots.
    void operator() (Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5) const {
        for (Vector< SlotType >::ConstIterator i = slots_.Begin(); i != slots_.End();++i )
            (*i)(p1, p2, p3, p4, p5);       
    }
    /// Disconnect to slot.
    SelfType& operator-=(SlotType& slot)
    {
        slots_.Remove(slot);
        return *this;
    }
    /// Disconnect to signal.
    SelfType& operator-=(SelfType& signal)
    {
        Disconnect(&signal, &SelfType::Emit);
        return *this;
    }
protected:
     
    Vector< SlotType > slots_;
};

template<typename R, class Param1, class Param2, class Param3, class Param4, class Param5, class Param6>
class Signal< R(Param1, Param2, Param3, Param4, Param5, Param6) >
{
public:
    /// Make using the slot type a bit easier via typedef.
    typedef Delegate6 < Param1, Param2, Param3, Param4, Param5, Param6, R  > SlotType;
    /// Allow users access to the specific type of this delegate.
    typedef Signal SelfType;
    // Constructor.
    Signal() { }
    /// Copy Constructor.
    Signal(const SelfType &base): slots_(base.slots_) { }
    /// Connect to member function.
    template < class X, class Y >
    void Connect(Y * pthis, R(X::* function_to_bind)(Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5, Param6 p6))
    {
        if (!pthis) return;
        slots_.Push(SlotType(pthis, function_to_bind));
    }
    /// Connect to const member function.
    template < class X, class Y >
    void Connect(const Y *pthis, R(X::* function_to_bind)(Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5, Param6 p6) const)
    {
        if (!pthis) return;
        slots_.Push(SlotType(pthis, function_to_bind));
    }
    /// Connect to static function.
    void Connect(R(*function_to_bind)(Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5, Param6 p6))
    {
        slots_.Push(SlotType(function_to_bind));
    }
    /// Connect to slot.
    void Connect(SlotType& slot)
    {
        slots_.Push(slot);
    }
    /// Connect to signal.
    void Connect(SelfType& signal)
    {
        assert(&signal != this);
        if (&signal == this)
            return;
        Connect(&signal, &SelfType::Emit);
    }
    /// Emit signal to all slots.
    void Emit(Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5, Param6 p6) const
    {
        for (Vector< SlotType >::ConstIterator i = slots_.Begin(); i != slots_.End();++i )
            (*i)(p1, p2, p3, p4, p5, p6);       
    }
    /// Emit signal and collect all return values.
    void Aggregate(Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5, Param6 p6, PODVector<R>& container)
    {
        for (Vector< SlotType >::ConstIterator i = slots_.Begin(); i != slots_.End(); ++i)
            container.Push((*i)(p1, p2, p3, p4, p5, p6));
    }
    /// Disconnect from slot.
    void Disconnect(SlotType& slot) { slots_.Remove(slot); }
    /// Disconnect from signal.
    void Disconnect(SelfType& signal) { Disconnect(&signal, &SelfType::Emit); }
    /// Disconnect from member function.
    template< class X, class Y >
    void Disconnect(Y * obj, R(X::*function_to_bind)(Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5, Param6 p6))
    {
        slots_.Remove(SlotType(obj, function_to_bind));
    }
    /// Disconnect from const member function.
    template< class X, class Y >
    void Disconnect(Y * obj, R(X::*function_to_bind)(Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5, Param6 p6) const)
    {
        slots_.Remove(SlotType(obj, function_to_bind));
    }
    /// Disconnect from static function.
    void Disconnect(R(*function_to_bind)(Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5, Param6 p6))
    {
        slots_.Remove(SlotType(function_to_bind));
    }
    /// Disconnect from all slots.
    void Clear() { slots_.Clear(); }
    /// Return whether signal has any Connections.
    bool Empty() const { return slots_.Empty(); }
    /// Copy from base into this.
    void Merge(const SelfType& base)
    {
        slots_.Push(base.slots_);
    }
    /// Returns true if the signal already contains this slot.
    bool Contains(SlotType& slot) const
    {
        return  slots_.Find(slot) != slots_.End();
    }
    /// Returns true if the signal already contains this slot.
    bool Contains(SelfType& signal)
    {
        return Contains(&signal, &SelfType::Emit);
    }
    /// Returns true if the signal already contains this slot.
    template< class X, class Y >
    bool Contains(Y * obj, R(X::*function_to_bind)(Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5, Param6 p6))
    {
        return  slots_.Find(SlotType(obj, function_to_bind)) != slots_.End();
    }
    /// Returns true if the signal already contains this slot.
    template< class X, class Y >
    bool Contains(Y * obj, R(X::*function_to_bind)(Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5, Param6 p6) const)
    {
        return  slots_.Find(SlotType(obj, function_to_bind)) != slots_.End();
    }

    /// Assign Operator.
    void operator =(SelfType& base)
    {
        Clear();
        slots_.Push(base.slots_);
    }
    /// Connect to slot.
    SelfType& operator+=(SlotType& slot)
    {
        slots_.Push(slot);
        return *this;
    }
    /// Connect to signal.
    SelfType& operator+=(SelfType& signal)
    {
        assert(&signal != this);
        if (&signal == this)
            return *this;
        Connect(&signal, &SelfType::Emit);
        return *this;
    }
    /// Emit signal to all slots.
    void operator() (Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5, Param6 p6) const {
        for (Vector< SlotType >::ConstIterator i = slots_.Begin(); i != slots_.End();++i )
            (*i)(p1, p2, p3, p4, p5, p6);       
    }
    /// Disconnect to slot.
    SelfType& operator-=(SlotType& slot)
    {
        slots_.Remove(slot);
        return *this;
    }
    /// Disconnect to signal.
    SelfType& operator-=(SelfType& signal)
    {
        Disconnect(&signal, &SelfType::Emit);
        return *this;
    }
protected:
     
    Vector< SlotType > slots_;
};



template< >
class Signal< bool() >
{
public:
    /// Make using the slot type a bit easier via typedef.
    typedef Delegate0 <  bool  > SlotType;
    /// Allow users access to the specific type of this delegate.
    typedef Signal SelfType;
    // Constructor.
    Signal() { }
    /// Copy Constructor.
    Signal(const SelfType &base) : slots_(base.slots_) { }
    /// Connect to member function.
    template < class X, class Y >
    void Connect(Y * pthis, bool(X::* function_to_bind)())
    {
        if (!pthis) return;
        slots_.Push(SlotType(pthis, function_to_bind));
    }
    /// Connect to const member function.
    template < class X, class Y >
    void Connect(const Y *pthis, bool(X::* function_to_bind)() const)
    {
        if (!pthis) return;
        slots_.Push(SlotType(pthis, function_to_bind));
    }
    /// Connect to static function.
    void Connect(bool(*function_to_bind)())
    {
        slots_.Push(SlotType(function_to_bind));
    }
    /// Connect to slot.
    void Connect(SlotType& slot)
    {
        slots_.Push(slot);
    }
    /// Connect to signal.
    void Connect(SelfType& signal)
    {
        assert(&signal != this);
        if (&signal == this)
            return;
        Connect(&signal, &SelfType::Emit);
    }
    /// Emit signal to all slots.
    bool Emit()
    {
        for (Vector< SlotType >::ConstIterator i = slots_.Begin(); i != slots_.End(); ++i)
            if (!(*i)())
                return false;
        return true;
    }
    /// Disconnect from slot.
    void Disconnect(SlotType& slot) { slots_.Remove(slot); }
    /// Disconnect from signal.
    void Disconnect(SelfType& signal) { Disconnect(&signal, &SelfType::Emit); }
    /// Disconnect from member function.
    template< class X, class Y >
    void Disconnect(Y * obj, bool(X::*function_to_bind)())
    {
        slots_.Remove(SlotType(obj, function_to_bind));
    }
    /// Disconnect from const member function.
    template< class X, class Y >
    void Disconnect(Y * obj, bool(X::*function_to_bind)() const)
    {
        slots_.Remove(SlotType(obj, function_to_bind));
    }
    /// Disconnect from static function.
    void Disconnect(bool(*function_to_bind)())
    {
        slots_.Remove(SlotType(function_to_bind));
    }
    /// Disconnect from all slots.
    void Clear() { slots_.Clear(); }
    /// Return whether signal has any Connections.
    bool Empty() const { return slots_.Empty(); }
    /// Copy from base into this.
    void Merge(const SelfType& base)
    {
        slots_.Push(base.slots_);
    }
    /// Returns true if the signal already contains this slot.
    bool Contains(SlotType& slot) const
    {
        return  slots_.Find(slot) != slots_.End();
    }
    /// Returns true if the signal already contains this slot.
    bool Contains(SelfType& signal)
    {
        return Contains(&signal, &SelfType::Emit);
    }
    /// Returns true if the signal already contains this slot.
    template< class X, class Y >
    bool Contains(Y * obj, bool(X::*function_to_bind)())
    {
        return  slots_.Find(SlotType(obj, function_to_bind)) != slots_.End();
    }
    /// Returns true if the signal already contains this slot.
    template< class X, class Y >
    bool Contains(Y * obj, bool(X::*function_to_bind)() const)
    {
        return  slots_.Find(SlotType(obj, function_to_bind)) != slots_.End();
    }

    /// Assign Operator.
    void operator =(SelfType& base)
    {
        Clear();
        slots_.Push(base.slots_);
    }
    /// Connect to slot.
    SelfType& operator+=(SlotType& slot)
    {
        slots_.Push(slot);
        return *this;
    }
    /// Connect to signal.
    SelfType& operator+=(SelfType& signal)
    {
        assert(&signal != this);
        if (&signal == this)
            return *this;
        Connect(&signal, &SelfType::Emit);
        return *this;
    }
    /// Disconnect to slot.
    SelfType& operator-=(SlotType& slot)
    {
        slots_.Remove(slot);
        return *this;
    }
    /// Disconnect to signal.
    SelfType& operator-=(SelfType& signal)
    {
        Disconnect(&signal, &SelfType::Emit);
        return *this;
    }
protected:

    Vector< SlotType > slots_;
};

template< class Param1 >
class Signal< bool(Param1) >
{
public:
    /// Make using the slot type a bit easier via typedef.
    typedef Delegate1 < Param1, bool  > SlotType;
    /// Allow users access to the specific type of this delegate.
    typedef Signal SelfType;
    // Constructor.
    Signal() { }
    /// Copy Constructor.
    Signal(const SelfType &base): slots_(base.slots_) { }
    /// Connect to member function.
    template < class X, class Y >
    void Connect(Y * pthis, bool(X::* function_to_bind)(Param1 p1))
    {
        if (!pthis) return;
        slots_.Push(SlotType(pthis, function_to_bind));
    }
    /// Connect to const member function.
    template < class X, class Y >
    void Connect(const Y *pthis, bool(X::* function_to_bind)(Param1 p1) const)
    {
        if (!pthis) return;
        slots_.Push(SlotType(pthis, function_to_bind));
    }
    /// Connect to static function.
    void Connect(bool(*function_to_bind)(Param1 p1))
    {
        slots_.Push(SlotType(function_to_bind));
    }
    /// Connect to slot.
    void Connect(SlotType& slot)
    {
        slots_.Push(slot);
    }
    /// Connect to signal.
    void Connect(SelfType& signal)
    {
        assert(&signal != this);
        if (&signal == this)
            return;
        Connect(&signal, &SelfType::Emit);
    }
    /// Emit signal to all slots.
    bool Emit(Param1 p1) 
    {
        for (Vector< SlotType >::ConstIterator i = slots_.Begin(); i != slots_.End();++i )
            if (!(*i)(p1))
                return false;
        return true;
    }
    /// Disconnect from slot.
    void Disconnect(SlotType& slot) { slots_.Remove(slot); }
    /// Disconnect from signal.
    void Disconnect(SelfType& signal) { Disconnect(&signal, &SelfType::Emit); }
    /// Disconnect from member function.
    template< class X, class Y >
    void Disconnect(Y * obj, bool(X::*function_to_bind)(Param1 p1))
    {
        slots_.Remove(SlotType(obj, function_to_bind));
    }
    /// Disconnect from const member function.
    template< class X, class Y >
    void Disconnect(Y * obj, bool(X::*function_to_bind)(Param1 p1) const)
    {
        slots_.Remove(SlotType(obj, function_to_bind));
    }
    /// Disconnect from static function.
    void Disconnect(bool(*function_to_bind)(Param1 p1))
    {
        slots_.Remove(SlotType(function_to_bind));
    }
    /// Disconnect from all slots.
    void Clear() { slots_.Clear(); }
    /// Return whether signal has any Connections.
    bool Empty() const { return slots_.Empty(); }
    /// Copy from base into this.
    void Merge(const SelfType& base)
    {
        slots_.Push(base.slots_);
    }
    /// Returns true if the signal already contains this slot.
    bool Contains(SlotType& slot) const
    {
        return  slots_.Find(slot) != slots_.End();
    }
    /// Returns true if the signal already contains this slot.
    bool Contains(SelfType& signal)
    {
        return Contains(&signal, &SelfType::Emit);
    }
    /// Returns true if the signal already contains this slot.
    template< class X, class Y >
    bool Contains(Y * obj, bool(X::*function_to_bind)(Param1 p1))
    {
        return  slots_.Find(SlotType(obj, function_to_bind)) != slots_.End();
    }
    /// Returns true if the signal already contains this slot.
    template< class X, class Y >
    bool Contains(Y * obj, bool(X::*function_to_bind)(Param1 p1) const)
    {
        return  slots_.Find(SlotType(obj, function_to_bind)) != slots_.End();
    }

    /// Assign Operator.
    void operator =(SelfType& base)
    {
        Clear();
        slots_.Push(base.slots_);
    }
    /// Connect to slot.
    SelfType& operator+=(SlotType& slot)
    {
        slots_.Push(slot);
        return *this;
    }
    /// Connect to signal.
    SelfType& operator+=(SelfType& signal)
    {
        assert(&signal != this);
        if (&signal == this)
            return *this;
        Connect(&signal, &SelfType::Emit);
        return *this;
    }
    /// Disconnect to slot.
    SelfType& operator-=(SlotType& slot)
    {
        slots_.Remove(slot);
        return *this;
    }
    /// Disconnect to signal.
    SelfType& operator-=(SelfType& signal)
    {
        Disconnect(&signal, &SelfType::Emit);
        return *this;
    }
protected:
     
    Vector< SlotType > slots_;
};

template< class Param1, class Param2>
class Signal< bool(Param1, Param2) >
{
public:
    /// Make using the slot type a bit easier via typedef.
    typedef Delegate2 < Param1, Param2, bool  > SlotType;
    /// Allow users access to the specific type of this delegate.
    typedef Signal SelfType;
    // Constructor.
    Signal() { }
    /// Copy Constructor.
    Signal(const SelfType &base): slots_(base.slots_) { }
    /// Connect to member function.
    template < class X, class Y >
    void Connect(Y * pthis, bool(X::* function_to_bind)(Param1 p1, Param2 p2))
    {
        if (!pthis) return;
        slots_.Push(SlotType(pthis, function_to_bind));
    }
    /// Connect to const member function.
    template < class X, class Y >
    void Connect(const Y *pthis, bool(X::* function_to_bind)(Param1 p1, Param2 p2) const)
    {
        if (!pthis) return;
        slots_.Push(SlotType(pthis, function_to_bind));
    }
    /// Connect to static function.
    void Connect(bool(*function_to_bind)(Param1 p1, Param2 p2))
    {
        slots_.Push(SlotType(function_to_bind));
    }
    /// Connect to slot.
    void Connect(SlotType& slot)
    {
        slots_.Push(slot);
    }
    /// Connect to signal.
    void Connect(SelfType& signal)
    {
        assert(&signal != this);
        if (&signal == this)
            return;
        Connect(&signal, &SelfType::Emit);
    }
    /// Emit signal to all slots.
    bool Emit(Param1 p1, Param2 p2) 
    {
        for (Vector< SlotType >::ConstIterator i = slots_.Begin(); i != slots_.End();++i )
        if (!(*i)(p1, p2))
            return false;
        return true;
    }
    /// Disconnect from slot.
    void Disconnect(SlotType& slot) { slots_.Remove(slot); }
    /// Disconnect from signal.
    void Disconnect(SelfType& signal) { Disconnect(&signal, &SelfType::Emit); }
    /// Disconnect from member function.
    template< class X, class Y >
    void Disconnect(Y * obj, bool(X::*function_to_bind)(Param1 p1, Param2 p2))
    {
        slots_.Remove(SlotType(obj, function_to_bind));
    }
    /// Disconnect from const member function.
    template< class X, class Y >
    void Disconnect(Y * obj, bool(X::*function_to_bind)(Param1 p1, Param2 p2) const)
    {
        slots_.Remove(SlotType(obj, function_to_bind));
    }
    /// Disconnect from static function.
    void Disconnect(bool(*function_to_bind)(Param1 p1, Param2 p2))
    {
        slots_.Remove(SlotType(function_to_bind));
    }
    /// Disconnect from all slots.
    void Clear() { slots_.Clear(); }
    /// Return whether signal has any Connections.
    bool Empty() const { return slots_.Empty(); }
    /// Copy from base into this.
    void Merge(const SelfType& base)
    {
        slots_.Push(base.slots_);
    }
    /// Returns true if the signal already contains this slot.
    bool Contains(SlotType& slot) const
    {
        return  slots_.Find(slot) != slots_.End();
    }
    /// Returns true if the signal already contains this slot.
    bool Contains(SelfType& signal)
    {
        return Contains(&signal, &SelfType::Emit);
    }
    /// Returns true if the signal already contains this slot.
    template< class X, class Y >
    bool Contains(Y * obj, bool(X::*function_to_bind)(Param1 p1, Param2 p2))
    {
        return  slots_.Find(SlotType(obj, function_to_bind)) != slots_.End();
    }
    /// Returns true if the signal already contains this slot.
    template< class X, class Y >
    bool Contains(Y * obj, bool(X::*function_to_bind)(Param1 p1, Param2 p2) const)
    {
        return  slots_.Find(SlotType(obj, function_to_bind)) != slots_.End();
    }

    /// Assign Operator.
    void operator =(SelfType& base)
    {
        Clear();
        slots_.Push(base.slots_);
    }
    /// Connect to slot.
    SelfType& operator+=(SlotType& slot)
    {
        slots_.Push(slot);
        return *this;
    }
    /// Connect to signal.
    SelfType& operator+=(SelfType& signal)
    {
        assert(&signal != this);
        if (&signal == this)
            return *this;
        Connect(&signal, &SelfType::Emit);
        return *this;
    }
    /// Disconnect to slot.
    SelfType& operator-=(SlotType& slot)
    {
        slots_.Remove(slot);
        return *this;
    }
    /// Disconnect to signal.
    SelfType& operator-=(SelfType& signal)
    {
        Disconnect(&signal, &SelfType::Emit);
        return *this;
    }
protected:
     
    Vector< SlotType > slots_;
};

template<class Param1, class Param2, class Param3>
class Signal< bool(Param1, Param2, Param3) >
{
public:
    /// Make using the slot type a bit easier via typedef.
    typedef Delegate3 < Param1, Param2, Param3, bool  > SlotType;
    /// Allow users access to the specific type of this delegate.
    typedef Signal SelfType;
    // Constructor.
    Signal() { }
    /// Copy Constructor.
    Signal(const SelfType &base): slots_(base.slots_) { }
    /// Connect to member function.
    template < class X, class Y >
    void Connect(Y * pthis, bool(X::* function_to_bind)(Param1 p1, Param2 p2, Param3 p3))
    {
        if (!pthis) return;
        slots_.Push(SlotType(pthis, function_to_bind));
    }
    /// Connect to const member function.
    template < class X, class Y >
    void Connect(const Y *pthis, bool(X::* function_to_bind)(Param1 p1, Param2 p2, Param3 p3) const)
    {
        if (!pthis) return;
        slots_.Push(SlotType(pthis, function_to_bind));
    }
    /// Connect to static function.
    void Connect(bool(*function_to_bind)(Param1 p1, Param2 p2, Param3 p3))
    {
        slots_.Push(SlotType(function_to_bind));
    }
    /// Connect to slot.
    void Connect(SlotType& slot)
    {
        slots_.Push(slot);
    }
    /// Connect to signal.
    void Connect(SelfType& signal)
    {
        assert(&signal != this);
        if (&signal == this)
            return;
        Connect(&signal, &SelfType::Emit);
    }
    /// Emit signal to all slots.
    bool Emit(Param1 p1, Param2 p2, Param3 p3) const
    {
        for (Vector< SlotType >::ConstIterator i = slots_.Begin(); i != slots_.End();++i )
            if(!(*i)(p1, p2, p3))
            return false;
        return true;
    }
    /// Disconnect from slot.
    void Disconnect(SlotType& slot) { slots_.Remove(slot); }
    /// Disconnect from signal.
    void Disconnect(SelfType& signal) { Disconnect(&signal, &SelfType::Emit); }
    /// Disconnect from member function.
    template< class X, class Y >
    void Disconnect(Y * obj, bool(X::*function_to_bind)(Param1 p1, Param2 p2, Param3 p3))
    {
        slots_.Remove(SlotType(obj, function_to_bind));
    }
    /// Disconnect from const member function.
    template< class X, class Y >
    void Disconnect(Y * obj, bool(X::*function_to_bind)(Param1 p1, Param2 p2, Param3 p3) const)
    {
        slots_.Remove(SlotType(obj, function_to_bind));
    }
    /// Disconnect from static function.
    void Disconnect(bool(*function_to_bind)(Param1 p1, Param2 p2, Param3 p3))
    {
        slots_.Remove(SlotType(function_to_bind));
    }
    /// Disconnect from all slots.
    void Clear() { slots_.Clear(); }
    /// Return whether signal has any Connections.
    bool Empty() const { return slots_.Empty(); }
    /// Copy from base into this.
    void Merge(const SelfType& base)
    {
        slots_.Push(base.slots_);
    }
    /// Returns true if the signal already contains this slot.
    bool Contains(SlotType& slot) const
    {
        return  slots_.Find(slot) != slots_.End();
    }
    /// Returns true if the signal already contains this slot.
    bool Contains(SelfType& signal)
    {
        return Contains(&signal, &SelfType::Emit);
    }
    /// Returns true if the signal already contains this slot.
    template< class X, class Y >
    bool Contains(Y * obj, bool(X::*function_to_bind)(Param1 p1, Param2 p2, Param3 p3))
    {
        return  slots_.Find(SlotType(obj, function_to_bind)) != slots_.End();
    }
    /// Returns true if the signal already contains this slot.
    template< class X, class Y >
    bool Contains(Y * obj, bool(X::*function_to_bind)(Param1 p1, Param2 p2, Param3 p3) const)
    {
        return  slots_.Find(SlotType(obj, function_to_bind)) != slots_.End();
    }
    /// Assign Operator.
    void operator =(SelfType& base)
    {
        Clear();
        slots_.Push(base.slots_);
    }
    /// Connect to slot.
    SelfType& operator+=(SlotType& slot)
    {
        slots_.Push(slot);
        return *this;
    }
    /// Connect to signal.
    SelfType& operator+=(SelfType& signal)
    {
        assert(&signal != this);
        if (&signal == this)
            return *this;
        Connect(&signal, &SelfType::Emit);
        return *this;
    }
    /// Disconnect to slot.
    SelfType& operator-=(SlotType& slot)
    {
        slots_.Remove(slot);
        return *this;
    }
    /// Disconnect to signal.
    SelfType& operator-=(SelfType& signal)
    {
        Disconnect(&signal, &SelfType::Emit);
        return *this;
    }
protected:
     
    Vector< SlotType > slots_;
};

template< class Param1, class Param2, class Param3, class Param4>
class Signal< bool(Param1, Param2, Param3, Param4) >
{
public:
    /// Make using the slot type a bit easier via typedef.
    typedef Delegate4 < Param1, Param2, Param3, Param4, bool  > SlotType;
    /// Allow users access to the specific type of this delegate.
    typedef Signal SelfType;
    // Constructor.
    Signal() { }
    /// Copy Constructor.
    Signal(const SelfType &base): slots_(base.slots_) { }
    /// Connect to member function.
    template < class X, class Y >
    void Connect(Y * pthis, bool(X::* function_to_bind)(Param1 p1, Param2 p2, Param3 p3, Param4 p4))
    {
        if (!pthis) return;
        slots_.Push(SlotType(pthis, function_to_bind));
    }
    /// Connect to const member function.
    template < class X, class Y >
    void Connect(const Y *pthis, bool(X::* function_to_bind)(Param1 p1, Param2 p2, Param3 p3, Param4 p4) const)
    {
        if (!pthis) return;
        slots_.Push(SlotType(pthis, function_to_bind));
    }
    /// Connect to static function.
    void Connect(bool(*function_to_bind)(Param1 p1, Param2 p2, Param3 p3, Param4 p4))
    {
        slots_.Push(SlotType(function_to_bind));
    }
    /// Connect to slot.
    void Connect(SlotType& slot)
    {
        slots_.Push(slot);
    }
    /// Connect to signal.
    void Connect(SelfType& signal)
    {
        assert(&signal != this);
        if (&signal == this)
            return;
        Connect(&signal, &SelfType::Emit);
    }
    /// Emit signal to all slots.
    bool Emit(Param1 p1, Param2 p2, Param3 p3, Param4 p4) const
    {
        for (Vector< SlotType >::ConstIterator i = slots_.Begin(); i != slots_.End();++i )
        if (!(*i)(p1, p2, p3, p4))
            return false;
        return true;
    }
    /// Disconnect from slot.
    void Disconnect(SlotType& slot) { slots_.Remove(slot); }
    /// Disconnect from signal.
    void Disconnect(SelfType& signal) { Disconnect(&signal, &SelfType::Emit); }
    /// Disconnect from member function.
    template< class X, class Y >
    void Disconnect(Y * obj, bool(X::*function_to_bind)(Param1 p1, Param2 p2, Param3 p3, Param4 p4))
    {
        slots_.Remove(SlotType(obj, function_to_bind));
    }
    /// Disconnect from const member function.
    template< class X, class Y >
    void Disconnect(Y * obj, bool(X::*function_to_bind)(Param1 p1, Param2 p2, Param3 p3, Param4 p4) const)
    {
        slots_.Remove(SlotType(obj, function_to_bind));
    }
    /// Disconnect from static function.
    void Disconnect(bool(*function_to_bind)(Param1 p1, Param2 p2, Param3 p3, Param4 p4))
    {
        slots_.Remove(SlotType(function_to_bind));
    }
    /// Disconnect from all slots.
    void Clear() { slots_.Clear(); }
    /// Return whether signal has any Connections.
    bool Empty() const { return slots_.Empty(); }
    /// Copy from base into this.
    void Merge(const SelfType& base)
    {
        slots_.Push(base.slots_);
    }
    /// Returns true if the signal already contains this slot.
    bool Contains(SlotType& slot) const
    {
        return  slots_.Find(slot) != slots_.End();
    }
    /// Returns true if the signal already contains this slot.
    bool Contains(SelfType& signal)
    {
        return Contains(&signal, &SelfType::Emit);
    }
    /// Returns true if the signal already contains this slot.
    template< class X, class Y >
    bool Contains(Y * obj, bool(X::*function_to_bind)(Param1 p1, Param2 p2, Param3 p3, Param4 p4))
    {
        return  slots_.Find(SlotType(obj, function_to_bind)) != slots_.End();
    }
    /// Returns true if the signal already contains this slot.
    template< class X, class Y >
    bool Contains(Y * obj, bool(X::*function_to_bind)(Param1 p1, Param2 p2, Param3 p3, Param4 p4) const)
    {
        return  slots_.Find(SlotType(obj, function_to_bind)) != slots_.End();
    }

    /// Assign Operator.
    void operator =(SelfType& base)
    {
        Clear();
        slots_.Push(base.slots_);
    }
    /// Connect to slot.
    SelfType& operator+=(SlotType& slot)
    {
        slots_.Push(slot);
        return *this;
    }
    /// Connect to signal.
    SelfType& operator+=(SelfType& signal)
    {
        assert(&signal != this);
        if (&signal == this)
            return *this;
        Connect(&signal, &SelfType::Emit);
        return *this;
    }
    /// Disconnect to slot.
    SelfType& operator-=(SlotType& slot)
    {
        slots_.Remove(slot);
        return *this;
    }
    /// Disconnect to signal.
    SelfType& operator-=(SelfType& signal)
    {
        Disconnect(&signal, &SelfType::Emit);
        return *this;
    }
protected:
     
    Vector< SlotType > slots_;
};

template<class Param1, class Param2, class Param3, class Param4, class Param5>
class Signal< bool(Param1, Param2, Param3, Param4, Param5) >
{
public:
    /// Make using the slot type a bit easier via typedef.
    typedef Delegate5 < Param1, Param2, Param3, Param4, Param5, bool  > SlotType;
    /// Allow users access to the specific type of this delegate.
    typedef Signal SelfType;
    // Constructor.
    Signal() { }
    /// Copy Constructor.
    Signal(const SelfType &base): slots_(base.slots_) { }
    /// Connect to member function.
    template < class X, class Y >
    void Connect(Y * pthis, bool(X::* function_to_bind)(Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5))
    {
        if (!pthis) return;
        slots_.Push(SlotType(pthis, function_to_bind));
    }
    /// Connect to const member function.
    template < class X, class Y >
    void Connect(const Y *pthis, bool(X::* function_to_bind)(Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5) const)
    {
        if (!pthis) return;
        slots_.Push(SlotType(pthis, function_to_bind));
    }
    /// Connect to static function.
    void Connect(bool(*function_to_bind)(Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5))
    {
        slots_.Push(SlotType(function_to_bind));
    }
    /// Connect to slot.
    void Connect(SlotType& slot)
    {
        slots_.Push(slot);
    }
    /// Connect to signal.
    void Connect(SelfType& signal)
    {
        assert(&signal != this);
        if (&signal == this)
            return;
        Connect(&signal, &SelfType::Emit);
    }
    /// Emit signal to all slots.
    bool Emit(Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5) const
    {
        for (Vector< SlotType >::ConstIterator i = slots_.Begin(); i != slots_.End();++i )
        if (!(*i)(p1, p2, p3, p4, p5))
            return false;
        return true;
    }
    /// Disconnect from slot.
    void Disconnect(SlotType& slot) { slots_.Remove(slot); }
    /// Disconnect from signal.
    void Disconnect(SelfType& signal) { Disconnect(&signal, &SelfType::Emit); }
    /// Disconnect from member function.
    template< class X, class Y >
    void Disconnect(Y * obj, bool(X::*function_to_bind)(Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5))
    {
        slots_.Remove(SlotType(obj, function_to_bind));
    }
    /// Disconnect from const member function.
    template< class X, class Y >
    void Disconnect(Y * obj, bool(X::*function_to_bind)(Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5) const)
    {
        slots_.Remove(SlotType(obj, function_to_bind));
    }
    /// Disconnect from static function.
    void Disconnect(bool(*function_to_bind)(Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5))
    {
        slots_.Remove(SlotType(function_to_bind));
    }
    /// Disconnect from all slots.
    void Clear() { slots_.Clear(); }
    /// Return whether signal has any Connections.
    bool Empty() const { return slots_.Empty(); }
    /// Copy from base into this.
    void Merge(const SelfType& base)
    {
        slots_.Push(base.slots_);
    }
    /// Returns true if the signal already contains this slot.
    bool Contains(SlotType& slot) const
    {
        return  slots_.Find(slot) != slots_.End();
    }
    /// Returns true if the signal already contains this slot.
    bool Contains(SelfType& signal)
    {
        return Contains(&signal, &SelfType::Emit);
    }
    /// Returns true if the signal already contains this slot.
    template< class X, class Y >
    bool Contains(Y * obj, bool(X::*function_to_bind)(Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5))
    {
        return  slots_.Find(SlotType(obj, function_to_bind)) != slots_.End();
    }
    /// Returns true if the signal already contains this slot.
    template< class X, class Y >
    bool Contains(Y * obj, bool(X::*function_to_bind)(Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5) const)
    {
        return  slots_.Find(SlotType(obj, function_to_bind)) != slots_.End();
    }
    /// Assign Operator.
    void operator =(SelfType& base)
    {
        Clear();
        slots_.Push(base.slots_);
    }
    /// Connect to slot.
    SelfType& operator+=(SlotType& slot)
    {
        slots_.Push(slot);
        return *this;
    }
    /// Connect to signal.
    SelfType& operator+=(SelfType& signal)
    {
        assert(&signal != this);
        if (&signal == this)
            return *this;
        Connect(&signal, &SelfType::Emit);
        return *this;
    }
    /// Disconnect to slot.
    SelfType& operator-=(SlotType& slot)
    {
        slots_.Remove(slot);
        return *this;
    }
    /// Disconnect to signal.
    SelfType& operator-=(SelfType& signal)
    {
        Disconnect(&signal, &SelfType::Emit);
        return *this;
    }
protected:
     
    Vector< SlotType > slots_;
};

template< class Param1, class Param2, class Param3, class Param4, class Param5, class Param6>
class Signal< bool(Param1, Param2, Param3, Param4, Param5, Param6) >
{
public:
    /// Make using the slot type a bit easier via typedef.
    typedef Delegate6 < Param1, Param2, Param3, Param4, Param5, Param6, bool  > SlotType;
    /// Allow users access to the specific type of this delegate.
    typedef Signal SelfType;
    // Constructor.
    Signal() { }
    /// Copy Constructor.
    Signal(const SelfType &base): slots_(base.slots_) { }
    /// Connect to member function.
    template < class X, class Y >
    void Connect(Y * pthis, bool(X::* function_to_bind)(Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5, Param6 p6))
    {
        if (!pthis) return;
        slots_.Push(SlotType(pthis, function_to_bind));
    }
    /// Connect to const member function.
    template < class X, class Y >
    void Connect(const Y *pthis, bool(X::* function_to_bind)(Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5, Param6 p6) const)
    {
        if (!pthis) return;
        slots_.Push(SlotType(pthis, function_to_bind));
    }
    /// Connect to static function.
    void Connect(bool(*function_to_bind)(Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5, Param6 p6))
    {
        slots_.Push(SlotType(function_to_bind));
    }
    /// Connect to slot.
    void Connect(SlotType& slot)
    {
        slots_.Push(slot);
    }
    /// Connect to signal.
    void Connect(SelfType& signal)
    {
        assert(&signal != this);
        if (&signal == this)
            return;
        Connect(&signal, &SelfType::Emit);
    }
    /// Emit signal to all slots.
    bool Emit(Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5, Param6 p6) const
    {
        for (Vector< SlotType >::ConstIterator i = slots_.Begin(); i != slots_.End();++i )
            if (!(*i)(p1, p2, p3, p4, p5, p6))
                return false;
        return true;
    }
    /// Disconnect from slot.
    void Disconnect(SlotType& slot) { slots_.Remove(slot); }
    /// Disconnect from signal.
    void Disconnect(SelfType& signal) { Disconnect(&signal, &SelfType::Emit); }
    /// Disconnect from member function.
    template< class X, class Y >
    void Disconnect(Y * obj, bool(X::*function_to_bind)(Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5, Param6 p6))
    {
        slots_.Remove(SlotType(obj, function_to_bind));
    }
    /// Disconnect from const member function.
    template< class X, class Y >
    void Disconnect(Y * obj, bool(X::*function_to_bind)(Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5, Param6 p6) const)
    {
        slots_.Remove(SlotType(obj, function_to_bind));
    }
    /// Disconnect from static function.
    void Disconnect(bool(*function_to_bind)(Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5, Param6 p6))
    {
        slots_.Remove(SlotType(function_to_bind));
    }
    /// Disconnect from all slots.
    void Clear() { slots_.Clear(); }
    /// Return whether signal has any Connections.
    bool Empty() const { return slots_.Empty(); }
    /// Copy from base into this.
    void Merge(const SelfType& base)
    {
        slots_.Push(base.slots_);
    }
    /// Returns true if the signal already contains this slot.
    bool Contains(SlotType& slot) const
    {
        return  slots_.Find(slot) != slots_.End();
    }
    /// Returns true if the signal already contains this slot.
    bool Contains(SelfType& signal)
    {
        return Contains(&signal, &SelfType::Emit);
    }
    /// Returns true if the signal already contains this slot.
    template< class X, class Y >
    bool Contains(Y * obj, bool(X::*function_to_bind)(Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5, Param6 p6))
    {
        return  slots_.Find(SlotType(obj, function_to_bind)) != slots_.End();
    }
    /// Returns true if the signal already contains this slot.
    template< class X, class Y >
    bool Contains(Y * obj, bool(X::*function_to_bind)(Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5, Param6 p6) const)
    {
        return  slots_.Find(SlotType(obj, function_to_bind)) != slots_.End();
    }
    /// Assign Operator.
    void operator =(SelfType& base)
    {
        Clear();
        slots_.Push(base.slots_);
    }
    /// Connect to slot.
    SelfType& operator+=(SlotType& slot)
    {
        slots_.Push(slot);
        return *this;
    }
    /// Connect to signal.
    SelfType& operator+=(SelfType& signal)
    {
        assert(&signal != this);
        if (&signal == this)
            return *this;
        Connect(&signal, &SelfType::Emit);
        return *this;
    }
    /// Disconnect to slot.
    SelfType& operator-=(SlotType& slot)
    {
        slots_.Remove(slot);
        return *this;
    }
    /// Disconnect to signal.
    SelfType& operator-=(SelfType& signal)
    {
        Disconnect(&signal, &SelfType::Emit);
        return *this;
    }
protected:
     
    Vector< SlotType > slots_;
};
}
