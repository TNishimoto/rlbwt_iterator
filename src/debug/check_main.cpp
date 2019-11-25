#include <cassert>
#include <chrono>
//#include "../common/cmdline.h"
//#include "../common/print.hpp"
//#include "../common/io.h"

#include "stool/src/io.hpp"
#include "stool/src/cmdline.h"
#include "stool/src/debug.hpp"

#include "../include/rlbwt_iterator.hpp"
#include "../include/bwt.hpp"
#include <sdsl/bit_vectors.hpp>
#include "../include/elias_fano_vector.hpp"


using namespace std;
using namespace stool;
using namespace stool::rlbwt;

bool SHOW = false;
/*
template <typename T>
bool equalCheck(const vector<T> &vec1, const vector<T> &vec2)
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
*/


template <typename RLBWT_TYPE>
bool test_text(RLBWT_TYPE &rlestr, string &text)
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
    BackwardText<typename RLBWT_TYPE::char_vec_type, typename RLBWT_TYPE::run_vec_type > w;
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

    bool b = stool::equal_check(text, text2);

    if (b)
    {
        std::cout << "\033[32m"
                  << "\033[1m"
                  << "OK! "
                  << "\033[0m" << std::endl;
    }
    return b;
}


template <typename CHAR = char, typename INDEX = uint64_t, typename RLBWT_TYPE = RLBWT<CHAR, INDEX>>
bool test_isa(RLBWT_TYPE &rlestr, vector<INDEX> &isa)
{

    std::cout << "\033[47m"
              << "\033[31m"
              << "\033[1m"
              << "Test: ISA"
              << "\033[0m" << std::endl;

    
    BackwardISA<typename RLBWT_TYPE::run_vec_type> w;

    std::vector<uint64_t> p;
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
    bool b = stool::equal_check(isa, isa2);
    if (b)
    {
        std::cout << "\033[32m"
                  << "\033[1m"
                  << "OK! "
                  << "\033[0m" << std::endl;
    }
    return true;
}


template <typename CHARVEC, typename POWVEC>
bool test_sa(RLBWT<CHARVEC, POWVEC> &rlestr, vector<typename POWVEC::value_type> &sa)
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

    bool b = stool::equal_check(sa, sa2);
    if (b)
    {
        std::cout << "\033[32m"
                  << "\033[1m"
                  << "OK! "
                  << "\033[0m" << std::endl;
    }
    return b;
}


template <typename RLBWT_STR>
bool test_lcp(RLBWT_STR &rlestr, vector<uint64_t> &lcp)
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

    bool b = stool::equal_check(lcp, lcp2);
    if (b)
    {
        std::cout << "\033[32m"
                  << "\033[1m"
                  << "OK! "
                  << "\033[0m" << std::endl;
    }
    return b;
}


template <typename CHAR = char, typename INDEX = uint64_t>
bool test_load(string filepath, string &bwt)
{

    std::cout << "\033[47m"
              << "\033[31m"
              << "\033[1m"
              << "Test: load RLBWT"
              << "\033[0m" << std::endl;
    //string text = "";
    //stool::IO::load(filepath, text);

    string text = stool::load_string_from_file(filepath, false);
    //vector<INDEX> sa = stool::rlbwt::SuffixArrayConstructor::naive_sa<INDEX>(text);
    //string bwt = stool::rlbwt::SuffixArrayConstructor::construct_bwt(text, sa);

    RLBWT<CHAR, INDEX> rlestr2;
    Constructor::construct_from_string(rlestr2, text);

    //text.push_back((char)0);
    //string bwt = stool::rlbwt::SuffixArrayConstructor::construct_bwt(text);
    RLBWT<CHAR, INDEX> rlestr1;
    Constructor::construct_from_bwt<CHAR, INDEX>(rlestr1, bwt);

    bool b1 = stool::equal_check<char>(*rlestr1.get_char_vec(), *rlestr2.get_char_vec());
    bool b2 = stool::equal_check<uint64_t>(*rlestr1.get_run_vec(), *rlestr2.get_run_vec());
    if (b1 && b2)
    {
        std::cout << "\033[32m"
                  << "\033[1m"
                  << "OK! "
                  << "\033[0m" << std::endl;
    }
    return b1 && b2;
}



template <typename CHAR = char, typename INDEX = uint64_t>
void test(string &text){

        RLBWT<> rlestr;
        Constructor::construct_from_string(rlestr, text);

        text.push_back((char)0);

        std::cout << "Text length = " << text.size() << std::endl;
        if (text.size() <= 100)
        {
            std::cout << "Text: ";
            std::cout << text << std::endl;
        }

        RLBWT<>::check_text_for_rlbwt(text);

        std::cout << "Constructing SA by naive soring..." << std::endl;
        vector<INDEX> sa = stool::rlbwt::SuffixArrayConstructor::naive_sa<INDEX>(text);

        string bwt = stool::rlbwt::SuffixArrayConstructor::construct_bwt(text, sa);
        vector<INDEX> isa = stool::rlbwt::SuffixArrayConstructor::construct_isa(sa);
        vector<INDEX> lcp = stool::rlbwt::SuffixArrayConstructor::construct_lcp(text, sa, isa);

        //std::cout << "/" << sizeof(typename RLBWT<CHAR, INDEX>::char_vec_type) << std::endl;
        //BackwardISA<INDEX> w;
        test_isa(rlestr, isa);
        test_text(rlestr, text);

        test_sa(rlestr, sa);
        test_lcp(rlestr, lcp);
        text.pop_back();
}


template <typename CHAR = char, typename INDEX = uint64_t>
void testWithSDSL(string &text){


        RLBWT<std::vector<CHAR>, SDVectorSeq > rlestr;


        std::vector<CHAR> cVec;
        std::vector<INDEX> nVec;
        Constructor::construct_vectors_for_rlbwt(text, cVec, nVec);


        using RUNVEC = SDVectorSeq;
        RUNVEC nVec2;
        nVec2.construct(nVec);
        rlestr.set(std::move(cVec), std::move(nVec2) );


        text.push_back((char)0);

        std::cout << "Text length = " << text.size() << std::endl;
        if (text.size() <= 100)
        {
            std::cout << "Text: ";
            std::cout << text << std::endl;
        }

        RLBWT<>::check_text_for_rlbwt(text);

        std::cout << "Constructing SA by naive soring..." << std::endl;
        vector<INDEX> sa = stool::rlbwt::SuffixArrayConstructor::naive_sa<INDEX>(text);

        string bwt = stool::rlbwt::SuffixArrayConstructor::construct_bwt(text, sa);
        vector<INDEX> isa = stool::rlbwt::SuffixArrayConstructor::construct_isa(sa);
        vector<INDEX> lcp = stool::rlbwt::SuffixArrayConstructor::construct_lcp(text, sa, isa);

        //BackwardISA<INDEX, SDVectorSeq> w;

        test_isa(rlestr, isa);
        test_text(rlestr, text);

        test_sa(rlestr, sa);
        test_lcp(rlestr, lcp);
        text.pop_back();
}

template <typename CHAR = char, typename INDEX = uint64_t>
void testWithEliasFano(string &text){


        RLBWT<std::vector<CHAR>, stool::EliasFanoVector > rlestr;

        std::vector<CHAR> cVec;
        std::vector<INDEX> nVec;
        Constructor::construct_vectors_for_rlbwt(text, cVec, nVec);


        using RUNVEC = stool::EliasFanoVector;
        RUNVEC nVec2;
        nVec2.construct(&nVec);
        rlestr.set(std::move(cVec), std::move(nVec2) );

        //Constructor::construct_from_string_with_efv(rlestr, text);

        text.push_back((char)0);

        std::cout << "Text length = " << text.size() << std::endl;
        if (text.size() <= 100)
        {
            std::cout << "Text: ";
            std::cout << text << std::endl;
        }

        RLBWT<>::check_text_for_rlbwt(text);

        std::cout << "Constructing SA by naive soring..." << std::endl;
        vector<INDEX> sa = stool::rlbwt::SuffixArrayConstructor::naive_sa<INDEX>(text);

        string bwt = stool::rlbwt::SuffixArrayConstructor::construct_bwt(text, sa);
        vector<INDEX> isa = stool::rlbwt::SuffixArrayConstructor::construct_isa(sa);
        vector<INDEX> lcp = stool::rlbwt::SuffixArrayConstructor::construct_lcp(text, sa, isa);

        //BackwardISA<INDEX, SDVectorSeq> w;

        test_isa(rlestr, isa);
        test_text(rlestr, text);

        test_sa(rlestr, sa);
        test_lcp(rlestr, lcp);
        text.pop_back();
}

int main(int argc, char *argv[])
{
using CHAR = char;
using INDEX = uint64_t;
    cmdline::parser p;
    p.add<string>("input_file", 'i', "input file name", false, "");
    p.add<string>("mode", 'm', "mode", true);

    p.parse_check(argc, argv);
    string inputFile = p.get<string>("input_file");
    string mode = p.get<string>("mode");

    if (mode == "input")
    {
        if(inputFile == "") throw std::runtime_error("Please input filename");

        string text = stool::load_string_from_file(inputFile, false);
        test(text);
        std::cout << "SDSL" << std::endl;
        testWithSDSL(text);
        std::cout << "with EliasFano" << std::endl;
        testWithEliasFano(text);

        //test_load(inputFile, bwt);

    }else{
        uint64_t len = 10000;
        uint64_t alphabetSize = 3;
        for(int i=0;i<100;i++){
            std::vector<char> text_vec = stool::create_deterministic_integers<char>(len, alphabetSize + 97, 97, i);
            string text;
            for(auto it : text_vec) text.push_back(it);
            test(text);

        }
    }
}
