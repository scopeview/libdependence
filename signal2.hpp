#ifndef __LIBDEPENDENCE_SIGNAL2_HPP__
#define __LIBDEPENDENCE_SIGNAL2_HPP__

#include <list>
#include <functional>
#include <algorithm>

namespace libdependence {

namespace helper {

template<typename SlotResult, typename Slot, typename Class, typename... Args>
struct binder_mem_fn;

template<typename SlotResult, typename Slot, typename Class>
struct binder_mem_fn<SlotResult, Slot, Class> {
    static auto bind(Slot& slot, SlotResult(Slot::*slot_mem_fn)(const Class&))
	-> decltype(std::bind(slot_mem_fn, slot, std::placeholders::_1)) {
	return std::bind(slot_mem_fn, slot, std::placeholders::_1);
    }
};

template<typename SlotResult, typename Slot, typename Class, typename A1>
struct binder_mem_fn<SlotResult, Slot, Class, A1> {
    static auto bind(Slot& slot, SlotResult(Slot::*slot_mem_fn)(const Class&, A1)) 
	-> decltype(std::bind(slot_mem_fn, slot, std::placeholders::_1, std::placeholders::_2)) {
	return std::bind(slot_mem_fn, slot, std::placeholders::_1, std::placeholders::_2);
    }
};

template<typename SlotResult, typename Slot, typename Class, typename A1, typename A2>
struct binder_mem_fn<SlotResult, Slot, Class, A1, A2> {
    static auto bind(Slot& slot, SlotResult(Slot::*slot_mem_fn)(const Class&, A1, A2))
	-> decltype(std::bind(slot_mem_fn, slot, 
			      std::placeholders::_1,
			      std::placeholders::_2,
			      std::placeholders::_3)) {
	return std::bind(slot_mem_fn, slot, 
			 std::placeholders::_1,
			 std::placeholders::_2,
			 std::placeholders::_3);
    }
};

////////////////////////////////////////////////////////////////
template<typename SlotResult, typename Class, typename... Args>
struct binder_fn;

template<typename SlotResult, typename Class, typename A1>
struct binder_fn<SlotResult, Class, A1> {
    static auto bind(SlotResult(*slot_fn)(const Class&, A1))
	-> decltype(std::bind(slot_fn,
			      std::placeholders::_1,
			      std::placeholders::_2)) {
	return std::bind(slot_fn,
			 std::placeholders::_1,
			 std::placeholders::_2);
    }
};

template<typename SlotResult, typename Class, typename A1, typename A2>
struct binder_fn<SlotResult, Class, A1, A2> {
    static auto bind(SlotResult(*slot_fn)(const Class&, A1, A2))
	-> decltype(std::bind(slot_fn,
			      std::placeholders::_1,
			      std::placeholders::_2,
			      std::placeholders::_3)) {
	return std::bind(slot_fn,
			 std::placeholders::_1,
			 std::placeholders::_2,
			 std::placeholders::_3);
    }
};

}

////////////////////////////////////////////////////////////////
// signal2
template<typename T, typename SignalF>
class signal2;

template<typename Result, typename Class, typename... Args>
class signal2<Class, Result(Class::*)(Args...)> {
    typedef Result(Class::*SignalFunctor)(Args...);
    typedef std::function<void(const Class&, Args...)> slot_function_type;
    using address_t = char*;
    using hash_code_t = size_t;
    typedef std::tuple<slot_function_type, address_t, hash_code_t> entry_type;

    Class& m_obj;
    SignalFunctor m_signal_f;
    std::list<entry_type> m_entry_list;

public:
    signal2(Class& o, SignalFunctor f) : m_obj(o), m_signal_f(f) {}

    ////////////////
    template<typename SlotResult, typename Slot>
    void add_slot(Slot& slot, SlotResult(Slot::*slot_mem_fn)(const Class&, Args...)) {
	typedef SlotResult(Slot::*slot_functor)(const Class&, Args...);
	auto bindf = helper::binder_mem_fn<SlotResult, Slot, Class, Args...>::bind(slot, slot_mem_fn);
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
    template<typename SlotResult>
    void add_slot(SlotResult(*slot_fn)(const Class&, Args...)) {
	auto bindf = helper::binder_fn<SlotResult, Class, Args...>::bind(slot_fn);
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
		std::get<0>(entry_)(m_obj, args...);
	    });
    }
};

////////////////////////////////////////////////////////////////
// 
////////////////////////////////////////////////////////////////
template<typename R1, typename C1, typename... Args,
	 typename R2, typename C2>
signal2<C1, R1(C1::*)(Args...)> 
connect2(C1& c1, R1(C1::*c1_mem_fn)(Args...), 
	 C2& c2, R2(C2::*c2_mem_fn)(const C1&, Args...)) {
    typedef R1(C1::*C1_Functor)(Args...);

    signal2<C1, C1_Functor> signal2_(c1, c1_mem_fn);
    signal2_.add_slot(c2, c2_mem_fn);
    return signal2_;
}

template<typename R1, typename C1, typename... Args,
	 typename R2>
signal2<C1, R1(C1::*)(Args...)> 
connect2(C1& c1, R1(C1::*c1_mem_fn)(Args...), 
	 R2(*c2_fn)(const C1&, Args...)) {
    typedef R1(C1::*C1_Functor)(Args...);

    signal2<C1, C1_Functor> signal2_(c1, c1_mem_fn);
    signal2_.add_slot(c2_fn);
    return signal2_;
}

}
#endif	/* __LIBDEPENDENCE_SIGNAL2_HPP__ */
