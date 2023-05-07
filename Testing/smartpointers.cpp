#include <iostream>

#define _D(x) std::cout << x << std::endl;

class MyClass
{
public:
	int number = 0;

	MyClass(int nr)
	{
		number = nr;
		_D("Constructor called");
	}
	~MyClass()
	{
		_D("Destructor called");
	}

	void stuff()
	{
		_D("Stuff called");
	}
};

std::unique_ptr<MyClass> pmyclass;

int main()
{
	pmyclass = std::unique_ptr<MyClass>(new MyClass(5));

	pmyclass->stuff();

	_D(pmyclass);
}