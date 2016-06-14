#include "struct.hpp"
#include "libdependence/_.hpp"

void on_update(const A& a)
{
    std::cout << __PRETTY_FUNCTION__ << ": " << a.m_number << std::endl;
}

void on_update_2(const A& a, int n, double d)
{
    std::cout << __PRETTY_FUNCTION__ << ": int " << n << " double: " << d << std::endl;
}

void test_signal()
{
    A a;
    B b;
    std::cout << __PRETTY_FUNCTION__ << "----------------:" << __LINE__ << std::endl;
    {
	auto signal_ = libdependence::connect(a, &A::update, b, &B::on_update);
	signal_.invoke(1);

	signal_.remove_slot(b, &B::on_update);
	signal_.invoke(2);

	signal_.add_slot(b, &B::on_update);
	signal_.invoke(3);

	signal_.remove_slot(b, &B::on_update);
	signal_.invoke(4);

	////////////////
	signal_.add_slot(&on_update);
	signal_.invoke(5);

	signal_.remove_slot(&on_update);
	signal_.invoke(6);
    }
    std::cout << __PRETTY_FUNCTION__ << "----------------:" << __LINE__ << std::endl;
    {
	auto signal_ = libdependence::connect(a, &A::update, &on_update);
	signal_.invoke(1);

	signal_.remove_slot(&on_update);
	signal_.invoke(2);

	signal_.add_slot(&on_update);
	signal_.invoke(3);

	signal_.remove_slot(&on_update);
	signal_.invoke(4);
	
	////////////////
	signal_.add_slot(b, &B::on_update);
	signal_.invoke(5);

	signal_.remove_slot(b, &B::on_update);
	signal_.invoke(6);
	
    }
}

void test_signal2()
{
    A a;
    B b;
    std::cout << __PRETTY_FUNCTION__ << "----------------:" << __LINE__ << std::endl;
    {
	auto signal2_ = libdependence::connect2(a, &A::update_2, b, &B::on_update_2);
	signal2_.invoke((int)1, 1.2);

	signal2_.remove_slot(b, &B::on_update_2);
	signal2_.invoke(2, 2.3);

	signal2_.add_slot(b, &B::on_update_2);
	signal2_.invoke(3, 3.4);

	signal2_.remove_slot(b, &B::on_update_2);
	signal2_.invoke(4, 4.5);

	////////////////
	signal2_.add_slot(&on_update_2);
	signal2_.invoke(5, 5.5);

	signal2_.remove_slot(&on_update_2);
	signal2_.invoke(6, 6.7);
    }
    std::cout << __PRETTY_FUNCTION__ << "----------------:" << __LINE__ << std::endl;
    {
	auto signal2_ = libdependence::connect2(a, &A::update_2, &on_update_2);
	signal2_.invoke((int)1, 1.2);

	signal2_.remove_slot(&on_update_2);
	signal2_.invoke(2, 2.3);

	signal2_.add_slot(&on_update_2);
	signal2_.invoke(3, 3.4);

	signal2_.remove_slot(&on_update_2);
	signal2_.invoke(4, 4.5);

	////////////////
	signal2_.add_slot(b, &B::on_update_2);
	signal2_.invoke(5, 5.5);

	signal2_.remove_slot(b, &B::on_update_2);
	signal2_.invoke(6, 6.7);
    }
}

int main(int argc, char * argv[])
{
    test_signal();
    test_signal2();

    return 0;
}
