#include <cassert>
#include <chrono>
#include "common/cmdline.h"
#include "common/io.h"
//#include "interval_tree.hpp"
#include <random>
using namespace std;
using namespace stool;

struct Interval{
    uint64_t l;
    uint64_t r;

    uint64_t L() const {return this->l;}
    uint64_t R() const {return this->r;}

    Interval(){

    }
    Interval(uint64_t _l, uint64_t _r){
        this->l = _l;
        this->r = _r;

    }
    string to_string()
    {

        return "[" + std::to_string(l) + ", " + std::to_string(r) + "]";
    }
};

int main(int argc, char *argv[])
{
    cmdline::parser p;
    p.add<uint64_t>("the number of elements", 'i', "number", false, 100);
    p.add<uint64_t>("max r", 'o', "max_size", false, 100);

    p.parse_check(argc, argv);
    uint64_t num = p.get<uint64_t>("the number of elements");
    uint64_t size = p.get<uint64_t>("max r");

    vector<Interval> items;
    std::random_device rnd;
    std::mt19937 mt(rnd());
    std::uniform_int_distribution<> rand(0, size);
    for (uint64_t i = 0; i < num; ++i) {
        uint64_t l = rand(mt);
        uint64_t r = rand(mt);
        if(l > r){
            uint64_t tmp = l;
            l = r;
            r = tmp;
        }
        items.push_back(Interval(l,r));


    }
    IntervalTreeFunctions::sortL(items);
    for(auto it : items){
        std::cout << it.to_string() << std::endl;
    }
}