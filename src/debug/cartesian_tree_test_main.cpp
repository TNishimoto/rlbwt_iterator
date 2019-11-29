#include <cassert>
#include <chrono>

#include "stool/src/io.hpp"
#include "stool/src/cmdline.h"
#include "stool/src/debug.hpp"
#include "stool/src/value_array.hpp"

#include "../include/cartesian_tree.hpp"


bool SHOW = false;

std::vector<uint64_t> create_random_integer_vector(uint64_t max, uint64_t len)
{
    std::vector<uint64_t> r;
    std::random_device rnd;
    for (uint64_t i = 0; i < len; ++i)
    {
        r.push_back(rnd() % max);
    }
    return r;
}


int main(int argc, char *argv[])
{
    std::vector<uint64_t> r = create_random_integer_vector(100, 10);
    stool::Printer::print(r);
    std::vector<uint64_t> left,right;
    uint64_t top = stool::rlbwt::CartesianTree::construct(r, left, right);
    stool::Printer::print(left);
    stool::Printer::print(right);
    std::cout << top << std::endl;



}
