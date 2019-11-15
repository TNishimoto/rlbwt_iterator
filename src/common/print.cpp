#include "print.hpp"

namespace stool
{

template <>
void Printer::print_item<uint64_t>(uint64_t item)
{
	std::cout << item << std::endl;
}
template <>
void Printer::print_item<char>(char item)
{
	std::cout << item << std::endl;
}
/*
template <>
void Printer::print<char>(const std::vector<char> &items)
{
	std::string s = "";
	s += "[";
	for (uint64_t i = 0; i < (uint64_t)items.size(); i++)
	{
		if ((uint64_t)items[i] == 0)
		{
			s += "#";
		}
		else
		{
			s.push_back(items[i]);
		}
		if (i != items.size() - 1)
			s += ", ";
	}
	s += "]";
	std::cout << s << std::endl;
}
*/

} // namespace stool