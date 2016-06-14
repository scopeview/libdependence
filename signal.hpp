#ifndef __LIBDEPENDENCE_SIGNAL_HPP__
#define __LIBDEPENDENCE_SIGNAL_HPP__

#include <list>
#include <functional>
#include <algorithm>

namespace libdependence {

////////////////////////////////////////////////////////////////
template<typename T, typename SignalF>
class signal;

template<typename Result, typename Class, typename... Args>
class signal<Class, Result(Class::*)(Args...)> {
    typedef Result(Class::*SignalFunctor)(Args...);
    typedef std::function<void(const Class&)> slot_function_type;
    using address_t = char*;
    using hash_code_t = size_t;
    typedef std::tuple<slot_function_type, address_t, hash_code_t> entry_type;

    Class& m_obj;
    SignalFunctor m_signal_f;
    std::list<entry_type> m_entry_list;

public:
    signal(Class& o, SignalFunctor f) : m_obj(o), m_signal_f(f) {}

    ////////////////
    // mem_fn
    template<typename SlotResult, typename Slot>
    void add_slot(Slot& slot, SlotResult(Slot::*slot_mem_fn)(const Class&)) {
	typedef SlotResult(Slot::*slot_functor)(const Class&);
	auto bindf = std::bind(slot_mem_fn, slot, std::placeholders::_1);
	m_entry_list.emplace_back(slot_function_type(bindf), 
				  reinterpret_cast<char*>(&slot), 
				  typeid(slot_functor).hash_code());
    }

    template<typename SlotResult, typename Slot, typename... SlotArgs>
    void remove_slot(Slot& slot, SlotResult(Slot::*slot_mem_fn)(SlotArgs...)) {
	typedef SlotResult(Slot::*slot_functor)(SlotArgs...);
	m_entry_list.erase(std::remove_if(m_entry_list.begin(), 
					  m_entry_list.end(),
					  [&](entry_type& entry_) {
					      if (std::get<1>(entry_) == reinterpret_cast<char*>(&slot) && 
						  std::get<2>(entry_) == typeid(slot_functor).hash_code()) {
						  return true;
					      }
					      return false;
					  }),
			   m_entry_list.end());
    }


    ////////////////
    // normal fn
    template<typename SlotResult, typename... SlotArgs>
    void add_slot(SlotResult(*slot_fn)(SlotArgs...)) {
	auto bindf = std::bind(slot_fn, std::placeholders::_1);
	m_entry_list.emplace_back(slot_function_type(bindf), 
				  reinterpret_cast<char*>(slot_fn), 
				  0);
    }

    template<typename SlotResult, typename... SlotArgs>
    void remove_slot(SlotResult(*slot_fn)(SlotArgs...)) {
	m_entry_list.erase(std::remove_if(m_entry_list.begin(), 
					  m_entry_list.end(),
					  [&](entry_type& entry_) {
					      if (std::get<1>(entry_) == reinterpret_cast<char*>(slot_fn)) {
						  return true;
					      }
					      return false;
					  }),
			   m_entry_list.end());
    }

    ////////////////
    Result invoke(Args&&... args) {
	(m_obj.*m_signal_f)(args...);
	std::for_each(m_entry_list.begin(), m_entry_list.end(), [&](entry_type& entry_) {
		std::get<0>(entry_)(m_obj);
	    });
    }
};

////////////////////////////////////////////////////////////////
// 
////////////////////////////////////////////////////////////////
// mem_fn
template<typename R1, typename C1, typename... Args,
	 typename R2, typename C2>
signal<C1, R1(C1::*)(Args...)> 
connect(C1& c1, R1(C1::*c1_mem_fn)(Args...), 
	C2& c2, R2(C2::*c2_mem_fn)(const C1&)) {
    typedef R1(C1::*C1_Functor)(Args...);

    signal<C1, C1_Functor> signal_(c1, c1_mem_fn);
    signal_.add_slot(c2, c2_mem_fn);
    return signal_;
}

////////////////
// normal fn
template<typename R1, typename C1, typename... Args,
	 typename R2>
signal<C1, R1(C1::*)(Args...)> 
connect(C1& c1, R1(C1::*c1_mem_fn)(Args...), 
	R2(*c2_fn)(const C1&)) {
    typedef R1(C1::*C1_Functor)(Args...);

    signal<C1, C1_Functor> signal_(c1, c1_mem_fn);
    signal_.add_slot(c2_fn);
    return signal_;
}

}
#endif	/* __LIBDEPENDENCE_SIGNAL_HPP__ */
