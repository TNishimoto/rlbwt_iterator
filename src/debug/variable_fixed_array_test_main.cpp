#include <cassert>
#include <chrono>

#include "stool/src/io.hpp"
#include "stool/src/cmdline.h"
#include "stool/src/debug.hpp"
#include "stool/src/value_array.hpp"

#include "../include/sampling_lcp/variable_fixed_array.hpp"

bool SHOW = false;

std::vector<uint64_t> create_random_integer_vector(uint64_t max, uint64_t len)
{
    std::vector<uint64_t> r;
    std::random_device rnd;
    for (uint64_t i = 0; i < len; ++i)
    {
        r.push_back(rand() % max);
    }
    return r;
}
bool getBit(uint64_t x, int8_t nth)
{
    return ((x >> nth) & 0x00000001) > 0;
}

std::string toBinaryString(uint64_t x)
{
    std::string s;
    s.resize(64, '0');
    for (uint64_t i = 0; i < 64; i++)
    {
        s[i] = getBit(x, i) ? '1' : '0';
    }
    return s;
}

int main(int argc, char *argv[])
{
    std::vector<uint64_t> r = create_random_integer_vector(300, 10);
    for (auto it : r)
    {
        std::cout << it << "/" << toBinaryString(it) << "/" << (int)stool::BinaryOperations::getBitWidth(it) << std::endl;
    }
    stool::Printer::print(r);
    stool::VariableFixedArray vfa;
    auto bits = stool::VariableFixedArray::create_width_bits(r);
    sdsl::bit_vector::select_1_type b_sel(&bits);
    vfa.build(&b_sel, r.size());
    for(uint64_t i=0;i<vfa.size();i++){
        vfa.change(i, r[i]);
    }
    
    std::random_device rnd;
    for(uint64_t i=0;i<100;i++){
        uint64_t p = rnd() % r.size();
        if(r[p] > 0){
            r[p]--;
            vfa.change(p, r[p]);
            assert(vfa[p] == r[p]);          
        }
    for(uint64_t i=0;i<vfa.size();i++){
        std::cout << vfa[i] << ", " << std::flush;
    }    
    std::cout << std::endl;

    }

    /*
    uint64_t v = UINT64_MAX;
    std::cout << toBinaryString(100) << std::endl;
    uint64_t xx = stool::BinaryOperations::write_bits(v, 100, 5, 10);
    std::cout << toBinaryString(xx) << std::endl;
    std::cout << stool::BinaryOperations::get_bits(xx, 5, 10) << std::endl;
    */


    /*
    std::vector<uint64_t> arr;
    arr.resize(100, 0);

    std::random_device rnd;
    for (uint64_t i = 0; i < 100; i++)
    {
        uint64_t v = rnd() % (uint64_t)UINT32_MAX;
        uint64_t pos = rnd() % 500;
        stool::BinaryOperations::write_bits(arr, v, pos, 32);
        uint64_t y = stool::BinaryOperations::get_bits(arr, pos, 32);
        stool::BinaryOperations::write_bits(arr, 0, pos, 32);
        assert(v == y);
    }
    std::cout << "OK!" << std::endl;
    */

    //stool::NaiveVariableFixedArray vfa;
    //vfa.build(r);
}
