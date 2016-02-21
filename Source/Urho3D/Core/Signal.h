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
/// original repo https://github.com/miguelmartin75/Wink-Signals
/// uses http://www.codeproject.com/Articles/7150/Member-Function-Pointers-and-the-Fastest-Possible
/// other implementaion which doesn't use dynamic allocated memory :
///	http://www.codeproject.com/Articles/11015/The-Impossibly-Fast-C-Delegates

#pragma once

#include <algorithm>
#include <vector>
#include <utility>
#include <set>
#include "Delegate.h"

namespace Urho3D
{

    /// TODO: connect to other signal. prevent connecting to self.prevent infinite emit loop !? 
    /// TODO: disconnect on slot object destroyed. 

    // Declare FastDelegate as a class template.  It will be specialized
    // later for all number of arguments.
    template <typename Signature>
    class Signal;

#if 0

     template<typename R>
    class Signal< R() >
    {
    public:
        // Make using the slot type a bit easier via typedef.
        typedef fastdelegate::FastDelegate0 < R > SlotType;

        // Allow users access to the specific type of this delegate.
        typedef Signal SelfType;
       
        Signal() { }

        /// Connect to member function.
        template < class X, class Y >
        void Connect(Y * pthis,R(X::* function_to_bind)())
        { 
            slots_.insert(SlotType(pthis, function_to_bind));
        }

        /// Connect to const member function.
        template < class X, class Y >
        void Connect(const Y *pthis, R(X::* function_to_bind)() const)
        {
            slots_.insert(SlotType(pthis, function_to_bind));
        }

        /// Connect to static function.
        void Connect(R(*function_to_bind)())       
        {
        
            slots_.insert(SlotType(function_to_bind));
        }

        /// Connect to slot.
        void Connect(SlotType& slot)
        {
            slots_.insert(slot);
        }

        /// Connect to slot.
        SelfType& operator+=(SlotType& slot)
        {
            slots_.insert(slot);
            return *this;
        }

        /// Emit signal to all slots.
        void Emit() const
        {
            for (std::set< SlotType >::const_iterator i = slots_.begin(); i != slots_.end(); )
                (*(i++))();
        }

        /// Emit signal to all slots.
        void operator() () const
        {
            for (std::set< SlotType >::const_iterator i = slots_.begin(); i != slots_.end(); )
                (*(i++))(p1, p2);
        }

        /// Disconnect from slot.
        void Disconnect(SlotType& slot) { slots_.erase(slot); }

        /// Disconnect to slot.
        SelfType& operator-=(SlotType& slot)
        {
            slots_.erase(slot);
            return *this;
        }

        /// Disconnect from member function.
        template< class X, class Y >
        void Disconnect(Y * obj, R(X::*function_to_bind)())
        {
            slots_.erase(SlotType(obj, function_to_bind));
        }

        /// Disconnect from const member function.
        template< class X, class Y >
        void Disconnect(Y * obj, R(X::*function_to_bind)() const)
        {
            slots_.erase(SlotType(obj, function_to_bind));
        }

        /// Disconnect from static function.
        void Disconnect(R(*function_to_bind)())
        {
            slots_.erase(SlotType(function_to_bind));
        }

        /// Disconnect from all slots.
        void Clear() { slots_.clear(); }

        /// Return whether signal has any Connections.
        bool Empty() const { return slots_.empty(); }

    protected:       
        std::set< SlotType > slots_;
    };

    template<typename R, class Param1, class Param2>
    class Signal< R(Param1, Param2) >
    {
    public:
        // Make using the slot type a bit easier via typedef.
        typedef fastdelegate::FastDelegate2 < Param1, Param2, R  > SlotType;

        // Allow users access to the specific type of this delegate.
        typedef Signal SelfType;

        Signal() { }

        Signal(const SelfType &base) : slots_(base.slots_) { }

        void operator =(SelfType& base)
        {
            Clear();
            slots_.insert(base.slots_.begin(), base.slots_.end());
        }

        /// Connect to member function.
        template < class X, class Y >
        void Connect(Y * pthis, R(X::* function_to_bind)(Param1 p1, Param2 p2))
        {
            slots_.insert(SlotType(pthis, function_to_bind));
        }

        /// Connect to const member function.
        template < class X, class Y >
        void Connect(const Y *pthis, R(X::* function_to_bind)(Param1 p1, Param2 p2) const)
        {
            slots_.insert(SlotType(pthis, function_to_bind));
        }

        /// Connect to static function.
        void Connect(R(*function_to_bind)(Param1 p1, Param2 p2))
        {
            slots_.insert(SlotType(function_to_bind));
        }

        /// Connect to slot.
        void Connect(SlotType& slot)
        {
            slots_.insert(slot);
        }

        /// Connect to signal.
        void Connect(SelfType& signal)
        {
            assert(&signal != this);
            if (&signal == this)
                return;
            this->Connect(&signal, &SelfType::Emit);
        }

        /// Connect to slot.
        SelfType& operator+=(SlotType& slot)
        {
            slots_.insert(slot);
            return *this;
        }
     
        /// Connect to signal.
        SelfType& operator+=(SelfType& signal)
        {
            assert(&signal != this);
            if (&signal == this)
                return *this;
            this->Connect(&signal, &SelfType::Emit);
            return *this;
        }

        /// Emit signal to all slots.
        void Emit(Param1 p1, Param2 p2) const
        {
            for (std::set< SlotType >::const_iterator i = slots_.begin(); i != slots_.end(); )
                (*(i++))( p1, p2);
        }

        /// Emit signal to all slots.
        void operator() (Param1 p1, Param2 p2) const {
            for (std::set< SlotType >::const_iterator i = slots_.begin(); i != slots_.end(); )
                (*(i++))(p1, p2);
        }

        /// Disconnect from slot.
        void Disconnect(SlotType& slot) {  slots_.erase(slot); }

        /// Disconnect to slot.
        SelfType& operator-=(SlotType& slot)
        {
            slots_.erase(slot);
            return *this;
        }

        /// Disconnect from signal.
        void Disconnect(SelfType& signal) { this->Disconnect(&signal, &SelfType::Emit); }

        /// Disconnect to signal.
        SelfType& operator-=(SelfType& signal)
        {
            this->Disconnect(&signal, &SelfType::Emit);
            return *this;
        }

        /// Disconnect from member function.
        template< class X, class Y >
        void Disconnect(Y * obj, R(X::*function_to_bind)(Param1 p1, Param2 p2))
        {
            slots_.erase(SlotType(obj, function_to_bind));
        }

        /// Disconnect from const member function.
        template< class X, class Y >
        void Disconnect(Y * obj, R(X::*function_to_bind)(Param1 p1, Param2 p2) const)
        {
            slots_.erase(SlotType(obj, function_to_bind));
        }

        /// Disconnect from static function.
        void Disconnect(R (*function_to_bind)(Param1 p1, Param2 p2) )
        {
            slots_.erase(SlotType(function_to_bind));
        }

        /// Disconnect from all slots.
        void Clear() { slots_.clear(); }

        /// Return whether signal has any Connections.
        bool Empty() const { return slots_.empty(); }

        void Merge(const SelfType& base)
        {
            slots_.insert(base.slots_.begin(), base.slots_.end());
        }

        /// Returns true if the signal already contains this slot.
        bool Contains(SlotType& slot) const
        {
           return  slots_.find(slot ) != slots_.end();
        }

        /// Returns true if the signal already contains this slot.
        bool Contains(SelfType& signal) const
        {
            return Contains(&signal, &SelfType::Emit);
        }

        /// Returns true if the signal already contains this slot.
        template< class X, class Y >
        bool Contains(Y * obj, R(X::*function_to_bind)(Param1 p1, Param2 p2))
        {
            return  slots_.find(SlotType(obj, function_to_bind)) != slots_.end();
        }

        /// Returns true if the signal already contains this slot.
        template< class X, class Y >
        bool Contains(Y * obj, R(X::*function_to_bind)(Param1 p1, Param2 p2) const)
        {
            return  slots_.find(SlotType(obj, function_to_bind)) != slots_.end();
        }

    protected:

        std::set< SlotType > slots_;
    };
#endif

    /// Signal slot handle automatic disconnect form Signal when destroyed
  template <typename Signature>
   class Signal
	{
	public:
      typedef fastdelegate::FastDelegate < Signature > SlotType;
      typedef Signal SelfType;

		/// Connects a slot to the Signal
		template <typename... Args>
		void Connect(Args&&... args)
      {
			_slots.emplace_back(std::forward<Args>(args)...);
		}
		/// Disconnects a slot from the Signal
		template <typename... Args>
		void Disconnect(Args&&... args)
        {
			_slots.erase(std::find(_slots.begin(), _slots.end(), SlotType(std::forward<Args>(args)...)));
		}
      /// Return count of connections.
      unsigned Connections() { _slots.size(); }
      /// Disconnect from all slots.
      void Clear() { _slots.clear(); }
      /// Return whether signal has any Connections.
      bool Empty() const { return _slots.empty(); }
      /// Merge.
      void Merge(const SelfType& base)
      {
          _slots.insert(base._slots.begin(), base._slots.end());
      }
      /// Returns true if the signal already contains this slot.
      bool Contains(SlotType& slot) const
      {
          return  _slots.find(slot) != _slots.end();
      }
		/// Emits the events you wish to send to the call-backs
		template <class ...Args>
		void Emit(Args&&... args)
      {
			for(typename slot_array::   const_iterator i = _slots.begin(); i != _slots.end(); ++i)
				(*i)(std::forward<Args>(args)...);
			
		}
		/// Emits events you wish to send to call-backs
		template <class ...Args>
		void operator()(Args&&... args)
		{
			emit(std::forward<Args>(args)...);
		}
		/// comparision operators for sorting and comparing.
		bool operator==(const SelfType& signal) const
		{ 
          return _slots == signal._slots; 
      }
		bool operator!=(const SelfType& signal) const
		{
          return !operator==(signal); 
      }

	private:
		/// defines an array of slots
		typedef std::vector<SlotType> slot_array;

		/// The slots connected to the Signal
		slot_array _slots;
	};

   template<typename... Paras>
   class Signal<bool(Paras...)>
   {
   public:
       typedef fastdelegate::FastDelegate < bool(Paras...) > SlotType;
       typedef Signal SelfType;

       /// Connects a slot to the Signal
       template <typename... Args>
       void Connect(Args&&... args)
       {
           _slots.emplace_back(std::forward<Args>(args)...);
       }
       /// Disconnects a slot from the Signal
       template <typename... Args>
       void Disconnect(Args&&... args)
       {
           _slots.erase(std::find(_slots.begin(), _slots.end(), SlotType(std::forward<Args>(args)...)));
       }
       /// Return count of connections.
       unsigned Connections() { _slots.size(); }
       /// Disconnect from all slots.
       void Clear() { _slots.clear(); }
       /// Return whether signal has any Connections.
       bool Empty() const { return _slots.empty(); }
      /// Merge.
      void Merge(const SelfType& base)
      {
          _slots.insert(base._slots.begin(), base._slots.end());
      }
      /// Returns true if the signal already contains this slot.
      bool Contains(SlotType& slot) const
      {
          return  _slots.find(slot) != _slots.end();
      }
       /// Emits the events you wish to send to the call-backs
       bool Emit(Paras&&... args)
       {
           for (typename slot_array::const_iterator i = _slots.begin(); i != _slots.end(); ++i)
           {
               if (!(*i)(std::forward<Paras>(args)...))
                   return false;
           }
           return true;
       }
       /// Emits events you wish to send to call-backs
       void operator()(Paras&&... args)
       {
           emit(std::forward<Paras>(args)...);
       }
       /// comparision operators for sorting and comparing.
       bool operator==(const SelfType& signal) const
       {
           return _slots == signal._slots;
       }
       bool operator!=(const SelfType& signal) const
       {
           return !operator==(signal);
       }

   private:
       /// defines an array of slots
       typedef std::vector<SlotType> slot_array;

       /// The slots connected to the Signal
       slot_array _slots;
   };
}
