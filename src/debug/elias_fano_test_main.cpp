#include <cassert>
#include <chrono>

#include "stool/src/io.hpp"
#include "stool/src/cmdline.h"
#include "stool/src/debug.hpp"
#include "stool/src/value_array.hpp"

#include "../include/elias_fano_vector.hpp"


bool SHOW = false;

std::vector<uint64_t> create_random_integer_vector(uint64_t max, uint64_t ratio)
{
    std::vector<uint64_t> r;
    std::random_device rnd;
    for (uint64_t i = 0; i < max; ++i)
    {
        if (rnd() % ratio == 0)
        {
            r.push_back(i);
        }
    }
    return r;
}


int main(int argc, char *argv[])
{
    std::vector<uint64_t> r = create_random_integer_vector(30000, 1000);
    stool::Printer::print(r);

    stool::ValueArray va;
    va.set(r, true);

    std::vector<uint64_t> r2;
    va.decode(r2);
    stool::Printer::print(r2);
    /*
    for(int i=0;i<r2.size();i++){
        std::cout << stool::toBinaryString(r2[i]) << std::endl;
    }
    */
    stool::EliasFanoVector efs;
    efs.construct(&r2);

    stool::EliasFanoVector efs2(std::move(efs));


    std::vector<uint64_t> r3 = efs2.to_vector();
    stool::Printer::print(r3);

    std::vector<uint64_t> r4;
    for(auto it : efs2) r4.push_back(it);
    stool::Printer::print(r4);



}
