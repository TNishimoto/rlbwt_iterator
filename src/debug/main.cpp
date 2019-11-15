#include <cassert>
#include <chrono>
#include "cmdline.h"
#include "io.h"
#include "bwt.hpp"
#include "iterator_generator.hpp"
#include "print.hpp"
#include "online_lcp_interval.hpp"

using namespace std;
using namespace stool;
using namespace stool::rlbwt;

/*
class Foo
{
  public:
    uint64_t i = 0;
    Foo &operator++()
    {
        this->i++;
        return *this;
    }
    Foo() = default;
    Foo(const Foo &) = delete;
    Foo(Foo &&) = delete;
};

uint64_t totalLCP(RLBWT<char> &rlestr){
    auto w2 = IteratorGenerator<char>(rlestr);
    vector<uint64_t> v = LCPIterator::construct_sampling_lcp_array(rlestr, w2.char_rbegin(), w2.char_rend());
    uint64_t sum = 0;
    for(auto p : v){
        if(p > 1000000) std::cout << p << ", ";
        sum += p;
    }
    std::cout << std::endl;
    //uint64_t sum = std::accumulate(v.begin(), v.end(), 0);
    return sum;
}
*/



int main(int argc, char *argv[])
{
    
    
    cmdline::parser p;
    p.add<string>("input_file", 'i', "input file name", true);

    p.parse_check(argc, argv);
    string inputFile = p.get<string>("input_file");
    
    
    std::ifstream ifs(inputFile);
    bool inputFileExist = ifs.is_open();
    if (!inputFileExist)
    {
        std::cout << inputFile << " cannot open." << std::endl;
        return -1;
    }

    string text = "";
    std::cout << "Loading : " << inputFile << std::endl;
    stool::IO::load(inputFile, text);
    text.push_back(0);

    string rev_text = text;
    std::reverse(rev_text.begin(), rev_text.end());

    //vector<string> r1,r2;
    
    vector<LCPInterval> r1 = PostorderMSIterator::naive_compute_maximal_substrings2(text);
    vector<LCPInterval> r2 = PostorderMSIterator::naive_compute_maximal_substrings2(rev_text);
    


    string bwt = stool::rlbwt::SuffixArrayConstructor::construct_bwt(text);
    RLBWT<char> rlestr;
    rlestr.construct(bwt);
    auto w = IteratorGenerator<char>(rlestr);
    uint64_t r3 = 0;
    for (auto it = w.maximal_substring_begin(); it != w.maximal_substring_end(); ++it)
    {
        r3++;
    }

    uint64_t r4 = 0;

    string rev_bwt = stool::rlbwt::SuffixArrayConstructor::construct_bwt(rev_text);
    RLBWT<char> rev_rlestr;
    rev_rlestr.construct(rev_bwt);
    auto rev_w = IteratorGenerator<char>(rev_rlestr);
    for (auto it = rev_w.maximal_substring_begin(); it != rev_w.maximal_substring_end(); ++it)
    {
        r4++;
    }

    std::cout << r1.size() << "/" << r2.size() << "/" << r3 << "/" << r4<< std::endl;

}
