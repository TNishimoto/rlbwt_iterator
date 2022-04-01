#include <cassert>
#include <chrono>

#include "stool/include/io.hpp"
#include "stool/include/cmdline.h"
#include "stool/include/debug.hpp"
#include "stool/include/value_array.hpp"

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
uint64_t create_random_value(uint64_t max)
{
    std::vector<uint64_t> r;
    std::random_device rnd;
    uint64_t p = max+1;
    while(p >= max){
        p = rnd() % max;
    }
    return p;
}


int main(int argc, char *argv[])
{
    uint64_t len = 12;
    std::vector<uint64_t> r;
    // = create_random_integer_vector(100, 10);
    r.push_back(0);
    for(uint64_t i=1;i<len;i++){
                r.push_back(create_random_value(i));

    }

    stool::Printer::print(r);
    std::vector<uint64_t> left,right;
    uint64_t top = stool::rlbwt::CartesianTree::construct(r, left, right);
    stool::Printer::print(left);
    stool::Printer::print(right);
    std::cout << top << std::endl;



}
