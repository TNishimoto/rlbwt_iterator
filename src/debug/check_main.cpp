#include <cassert>
#include <chrono>
#include "../common/cmdline.h"
#include "../common/print.hpp"
#include "../common/io.h"

#include "../include/rlbwt_iterator.hpp"
#include "../include/bwt.hpp"



using namespace std;
using namespace stool;
using namespace stool::rlbwt;

using CHAR = char;
using INDEX = uint64_t;
bool SHOW = false;

template <typename T>
bool equalCheck(const vector<T> &vec1,const vector<T> &vec2)
{
    if (vec1.size() != vec2.size())
    {
        string s = string("String sizes are different!") + ", collect = " + std::to_string(vec1.size()) + ", test = " + std::to_string(vec2.size());
        
        throw std::logic_error(s);
    }
    for (uint64_t i = 0; i < vec1.size(); i++)
    {
        if (vec1[i] != vec2[i])
        {
            string msg = "collect_vec[" + std::to_string(i) + "] != test_vec[" + std::to_string(i) + "]";

            throw std::logic_error("Values are different! " + msg);
        }
    }
    return true;
}



bool equalCheck(string &vec1, string &vec2)
{
    if (vec1.size() != vec2.size())
    {
        string s = string("String sizes are different!") + ", collect = " + std::to_string(vec1.size()) + ", test = " + std::to_string(vec2.size());
        throw std::logic_error(s);
    }

    for (uint64_t i = 0; i < vec1.size(); i++)
    {
        if (vec1[i] != vec2[i])
        {
            throw std::logic_error("Values are different!");
        }
    }
    return true;
}

bool test_text(RLBWT<CHAR, INDEX> &rlestr, string &text)
{
    std::cout << "\033[47m"
              << "\033[31m"
              << "\033[1m"
              << "Test: Text"
              << "\033[0m" << std::endl;
    //string bwt = stool::rlbwt::SuffixArrayConstructor::construct_bwt(text);
    //RLBWT<char, uint64_t> rlestr;
    //Constructor::construct_from_bwt<CHAR, INDEX>(rlestr, bwt);
    //std::cout << bwt << std::endl;
    //rlestr.construct(bwt);
    BackwardText<char, uint64_t, vector<char>, vector<uint64_t>> w;
    w.construct_from_rlbwt(&rlestr, false);
    string text2 = w.to_string();

    if (text.size() <= 100 && SHOW)
    {
        std::cout << std::endl;
        std::cout << "\033[32m"
                  << "\033[1m"
                  << "Collect Text: "
                  << std::endl;
        std::cout << text << std::endl;
        std::cout << "\033[35m"
                  << "\033[1m"
                  << "Computed Text from RLBWT: "
                  << std::endl;
        std::cout << text2 << std::endl;
    }

    bool b = equalCheck(text, text2);

    if (b)
    {
        std::cout << "\033[32m"
                  << "\033[1m"
                  << "OK! "
                  << "\033[0m" << std::endl;
    }
    return b;
}

bool test_isa(RLBWT<CHAR, INDEX> &rlestr, vector<INDEX> &isa)
{

    std::cout << "\033[47m"
              << "\033[31m"
              << "\033[1m"
              << "Test: ISA"
              << "\033[0m" << std::endl;

    BackwardISA<> w;
    w.construct_from_rlbwt(&rlestr, false);
    vector<INDEX> isa2 = w.to_isa();

    if (isa.size() <= 100 && SHOW)
    {
        std::cout << std::endl;
        std::cout << "\033[32m"
                  << "\033[1m"
                  << "Collect ISA: "
                  << std::endl;
        stool::Printer::print(isa);
        std::cout << "\033[35m"
                  << "\033[1m"
                  << "Computed ISA from RLBWT: "
                  << std::endl;
        stool::Printer::print(isa2);
    }
    bool b = equalCheck(isa, isa2);
    if (b)
    {
        std::cout << "\033[32m"
                  << "\033[1m"
                  << "OK! "
                  << "\033[0m" << std::endl;
    }
    return true;
}

bool test_sa(RLBWT<CHAR, INDEX> &rlestr, vector<INDEX> &sa )
{
    std::cout << "\033[47m"
              << "\033[31m"
              << "\033[1m"
              << "Test: SA"
              << "\033[0m";
    ForwardSA<> w2;
    w2.construct_from_rlbwt(&rlestr, false);
    vector<uint64_t> sa2 = w2.to_sa();

    if (sa.size() <= 100 && SHOW)
    {
        std::cout << std::endl;
        std::cout << "\033[32m"
                  << "\033[1m"
                  << "Collect SA: "
                  << std::endl;
        stool::Printer::print(sa);
        std::cout << "\033[35m"
                  << "\033[1m"
                  << "Computed SA from RLBWT: " << std::endl;
        stool::Printer::print(sa2);
    }

    bool b = equalCheck(sa, sa2);
    if (b)
    {
        std::cout << "\033[32m"
                  << "\033[1m"
                  << "OK! "
                  << "\033[0m" << std::endl;
    }
    return b;
}

bool test_lcp(RLBWT<CHAR, INDEX> &rlestr, vector<INDEX> &lcp)
{
    std::cout << "\033[47m"
              << "\033[31m"
              << "\033[1m"
              << "Test: LCP Array"
              << "\033[0m";

    ForwardLCPArray<> w2;
    w2.construct_from_rlbwt(&rlestr, false);
    vector<uint64_t> lcp2 = w2.to_lcp_array();

    if (lcp.size() <= 100 && SHOW)
    {
        std::cout << std::endl;
        std::cout << "\033[32m"
                  << "\033[1m"
                  << "Collect LCP array: " << std::endl;
        stool::Printer::print(lcp);
        std::cout << "\033[35m"
                  << "\033[1m"
                  << "Computed LCP array from RLBWT: " << std::endl;
        stool::Printer::print(lcp2);
        std::cout << "\033[0m" << std::endl;
    }

    bool b = equalCheck(lcp, lcp2);
    if (b)
    {
        std::cout << "\033[32m"
                  << "\033[1m"
                  << "OK! "
                  << "\033[0m" << std::endl;
    }
    return b;
}
bool test_load(string filepath, string& bwt){

    std::cout << "\033[47m"
              << "\033[31m"
              << "\033[1m"
              << "Test: load RLBWT"
              << "\033[0m" << std::endl;
    string text = "";
    stool::IO::load(filepath, text);
 
    
    RLBWT<CHAR, INDEX> rlestr2;
    Constructor::construct_from_string(rlestr2, text);


    //text.push_back((char)0);
    //string bwt = stool::rlbwt::SuffixArrayConstructor::construct_bwt(text);
    RLBWT<CHAR, INDEX> rlestr1;
    Constructor::construct_from_bwt<CHAR, INDEX>(rlestr1, bwt);

    bool b1 = equalCheck<char>(*rlestr1.get_char_vec(), *rlestr2.get_char_vec());
    bool b2 = equalCheck<uint64_t>(*rlestr1.get_run_vec(), *rlestr2.get_run_vec() );
    if (b1 && b2)
    {
        std::cout << "\033[32m"
                  << "\033[1m"
                  << "OK! "
                  << "\033[0m" << std::endl;
    }
    return b1 && b2;
    
}

/*
bool test_sampling_lcp(string &text)
{

    vector<uint64_t> sa = stool::rlbwt::SuffixArrayConstructor::construct_sa(text);
    vector<uint64_t> isa = stool::rlbwt::SuffixArrayConstructor::construct_isa(sa);
    vector<uint64_t> lcp = stool::rlbwt::SuffixArrayConstructor::construct_lcp(text, sa, isa);

    //stool::rlbwt::result_lcp_vec = lcp;

    string bwt = stool::rlbwt::SuffixArrayConstructor::construct_bwt(text);
    RLBWT<char> rlestr;
    rlestr.construct(bwt);
    auto w = IteratorGenerator<char>(rlestr);

    //vector<uint64_t> fpos_vec = RLBWTFunctions<char>::construct_fpos_array(rlestr);
    std::pair<vector<uint64_t>, vector<uint64_t>> sampling_sa = SAIterator<>::construct_sampling_sa(rlestr, w.pos_rbegin(), w.pos_rend());
    //vector<uint64_t> _xpsa = SAIterator<>::construct_sampling_sa(rlestr, w.pos_rbegin(), w.pos_rend());
    //vector<uint64_t> _start_partitions = SAIterator<>::construct_xf_mapper(sampling_sa.first);

    //SamplingLCP<char> slcp(rlestr, fpos_vec, sampling_sa.first);
    vector<uint64_t> slcp = SamplingLCP<char>::construct_sampling_lcp_array_lorder(rlestr);
    vector<uint64_t> slcp_yorder = SamplingLCP<char>::to_succ_sampling_lcp_array_yorder(std::forward<vector<uint64_t>>(slcp), rlestr, sampling_sa.second);

    //Printer::print(slcp);

    w.load_sampling_lcp(std::move(slcp_yorder));

    vector<uint64_t> lcp2;
    for (auto p = w.lcp_begin(); p != w.lcp_end(); ++p)
    {
        lcp2.push_back(*p);
    }

    //Printer::print(lcp2);
    bool b = equalCheck(lcp, lcp2);
    if (b)
    {
        std::cout << "OK!" << std::endl;
    }

    return b;
}
*/
int main(int argc, char *argv[])
{
    cmdline::parser p;
    p.add<string>("input_file", 'i', "input file name", true);
    p.add<string>("mode", 'm', "mode", false, "xx");

    p.parse_check(argc, argv);
    string inputFile = p.get<string>("input_file");
    string mode = p.get<string>("mode");

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

    RLBWT<CHAR, INDEX> rlestr;
    //Constructor::construct_from_bwt<CHAR, INDEX>(rlestr, bwt);
    Constructor::construct_from_string<CHAR, INDEX>(rlestr, text);

    text.push_back((char)0);

    std::cout << "Text length = " << text.size() << std::endl;
    if (text.size() <= 100)
    {
        std::cout << "Text: ";
        std::cout << text << std::endl;
    }

    RLBWT<char>::check_text_for_rlbwt(text);

    
    std::cout << "Constructing SA by naive soring..." << std::endl;
    vector<INDEX> sa = stool::rlbwt::SuffixArrayConstructor::naive_sa<INDEX>(text);
    
    string bwt = stool::rlbwt::SuffixArrayConstructor::construct_bwt(text, sa);
    vector<INDEX> isa = stool::rlbwt::SuffixArrayConstructor::construct_isa(sa);
    vector<INDEX> lcp = stool::rlbwt::SuffixArrayConstructor::construct_lcp(text, sa, isa);
    //vector<LCPInterval<INDEX>> intervals = LCPInterval<INDEX>::createLCPIntervals(sa, lcp);
    //vector<LCPInterval<INDEX>> maximalSubstrings = PostorderMSIterator<>::naive_compute_maximal_substrings2(text);


    test_isa(rlestr, isa);
    test_text(rlestr, text);

    test_sa(rlestr,sa);
    test_lcp(rlestr, lcp);
    //test_lcp_interval(rlestr, intervals);
    //test_ms(rlestr, maximalSubstrings);
    test_load(inputFile, bwt);
    

    /*
    */

    /*
    test_sampling_lcp(text);
    test_sa2(text);

    test_ms(text);
    */
}
