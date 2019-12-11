#include <cassert>
#include <chrono>
//#include "../common/cmdline.h"
//#include "../common/print.hpp"
//#include "../common/io.h"

#include "../include/rlbwt_iterator.hpp"
#include "../include/bwt.hpp"
#include "../include/sampling_lcp_construction/succinct_interval_tree.hpp"

#include "stool/src/io.hpp"
#include "stool/src/cmdline.h"
#include "stool/src/debug.hpp"

#include <sdsl/bp_support.hpp>

using namespace std;
using namespace stool;
using namespace stool::rlbwt;
using namespace sdsl;


bool check(uint64_t x, SuccinctIntervalTree<uint64_t, uint8_t, std::vector<uint64_t>, std::vector<uint64_t>> &tree, NaiveIntervalTree &nit){
    //std::cout << "check x = " << x << std::endl;
    auto r = tree.report_and_remove(x);  
    std::vector<std::pair<uint64_t, uint64_t>> r1;
    for(auto it : r){
        r1.push_back(tree.get_item(it));        
    }

    auto r2 = nit.report_and_remove(x);  
        std::sort(r1.begin(), r1.end(), [&](auto const &lhs, auto const &rhs) {
            return lhs.first < rhs.first;
        });
        std::sort(r2.begin(), r2.end(), [&](auto const &lhs, auto const &rhs) {
            return lhs.first < rhs.first;
        });
        /*
        
    */

    if(r1.size() != r2.size()){
        std::cout << "succicnt report is .." << std::endl;
        for(auto it : r1){
            std::cout << "[" << it.first << ".." << it.second << "]" << std::endl;
    }
        std::cout << "naive report is .." << std::endl;

        for(auto it : r2){
            std::cout << "[" << it.first << ".." << it.second << "]" << std::endl;
    }
        throw std::logic_error("size error");
    }
    for(uint64_t i=0;i<r1.size();i++){
        if(r1[i].first != r2[i].first || r1[i].second != r2[i].second){
std::cout << "succicnt report is .." << std::endl;
        for(auto it : r1){
            std::cout << "[" << it.first << ".." << it.second << "]" << std::endl;
    }
        std::cout << "naive report is .." << std::endl;

        for(auto it : r2){
            std::cout << "[" << it.first << ".." << it.second << "]" << std::endl;
    }
        throw std::logic_error("content error");
        }
    }
    return true;

}

int main(int argc, char *argv[])
{
    uint64_t len = 500;
    std::string randStr = stool::CreateRandomString(len, 4);
    std::vector<std::pair<uint64_t, uint64_t>> p = SuccinctIntervalTreeDebug::to_intervals(randStr);
    for(auto it : p){
            std::cout << "[" << it.first << ".." << it.second << "]" << std::flush;

    }
    std::vector<uint64_t> leftArr;
    std::vector<uint64_t> rightArr;
    for(auto it : p){
        leftArr.push_back(it.first);
        rightArr.push_back(it.second);
    }
    
    SuccinctIntervalTree<uint64_t, uint8_t, std::vector<uint64_t>, std::vector<uint64_t>> tree;
    tree.initialize(len );
    std::vector<bool> flag_vec;
    flag_vec.resize(p.size(), true);
    tree.construct(&leftArr, &rightArr, flag_vec);
    NaiveIntervalTree nit;
    nit.construct(p);
    //auto root = tree.get_root();
    //std::cout << root.to_string() << std::endl;
    /*
    for(auto it : tree){
        uint64_t rank = tree.get_rank(it.line_rank, it.height);
        if(tree.get_item_count(rank) > 0){
        std::cout << tree.get_info(it.line_rank, it.height)<< std::endl;
        }
    }
    */
    for(uint64_t i=0;i<len;i++){
        check(i, tree, nit);
    }
    std::cout << "OK!" << std::endl;
    /*
    auto r = tree.report_and_remove(50);  
    for(auto it : r){
            auto item = p[it]; 
            std::cout << "[" << item.first << ".." << item.second << "]" << std::endl;

    }
    std::cout << std::endl;
    auto r2 = nit.report_and_remove(50);  
    for(auto it : r2){
            std::cout << "[" << it.first << ".." << it.second << "]" << std::endl;

    }
    */

    
    /*
    auto beg = tree.begin();
    auto end = tree.end();

    for(uint64_t i=0;i<100;i++){
        std::cout << (*beg).to_string() << std::endl;
        ++beg;

        if(!(beg != end)){
            break;
        }
    }
    */
    //std::cout << 
    //              0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9
    //              ( ( ( ) ( ) ) ( ) ( ( ( ) ( ) ) ( ) ) )
    //bit_vector b = {1,1,1,0,1,0,0,1,0,1,1,1,0,1,0,0,1,0,0,0};
    /*
    bit_vector b = {1,1,1,0,0,0};

    bp_support_sada<> bps(&b); // <- pointer to b
    for (size_t i=0; i < 3; ++i)
        cout << bps.find_open(i)<< " ";
    cout << endl;
    cout << bps.rank(0) << ", " // inclusive rank for BPS!!!
         << bps.select(4) << endl;
        */
}