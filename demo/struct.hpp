#ifndef __STRUCT_HPP__
#define __STRUCT_HPP__

#include <iostream>

struct A {
    int m_number;
    
    int update(int n) {
	std::cout << __PRETTY_FUNCTION__ << ": n = " << n << std::endl;
	m_number = n;
	return n;
    }

    int update_2(int n, double d) {
	std::cout << __PRETTY_FUNCTION__ << ": n = " << n << std::endl;
	m_number = n;
	return n;
    }

};

struct B {
    void on_update(const A& a) {
	std::cout << __PRETTY_FUNCTION__ << ": " << a.m_number << std::endl;
    }
    void on_update_2(const A& a, int n, double d) {
	std::cout << __PRETTY_FUNCTION__ << ": int " << n << " double: " << d << std::endl;
    }
};

#endif	/* __STRUCT_HPP__ */
