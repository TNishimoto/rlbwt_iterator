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
#include "stool/src/elias_fano_vector.hpp"

using namespace std;
using namespace stool;
using namespace stool::rlbwt;

bool SHOW = false;

template <typename DATA>
void test1(DATA &data, DATA &correct_data, std::string name)
{
    std::cout << "\033[47m"
              << "\033[31m"
              << "\033[1m"
              << "Test: " << name
              << "\033[0m" << std::endl;

    //BackwardISA<typename RLBWT_TYPE::run_vec_type> w;
        //Printer::print(data);
        //Printer::print(correct_data);

    bool b = stool::equal_check(correct_data, data);
    if (b)
    {
        std::cout << "\033[32m"
                  << "\033[1m"
                  << "OK! "
                  << "\033[0m" << std::endl;
    }else{

    }
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


template <typename RLBWT_TYPE>
void test(RLBWT_TYPE &rlestr, string &text, string name){
        using INDEX = typename RLBWT_TYPE::index_type;
        std::cout << "Test: " << name << std::endl;

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


        //BackwardText<typename RLBWT_TYPE::char_vec_type, typename RLBWT_TYPE::run_vec_type> w1;


        BackwardText<typename RLBWT_TYPE::char_vec_type, typename RLBWT_TYPE::run_vec_type> w1;
        w1.construct_from_rlbwt(&rlestr, false);
        std::string test_text;
        test_text.resize(text.size(), 0);
        uint64_t p1 = text.size()-1;
        for(auto it : w1) test_text[p1--] = it;
        test1(test_text, text, "Text");

        //backward_text_result = test1(rlestr, w1, "backward text");

        vector<INDEX> test_isa;
        test_isa.resize(isa.size(), 0);
        uint64_t p2 = isa.size()-1;
        BackwardISA<typename RLBWT_TYPE::run_vec_type> w2;
        w2.construct_from_rlbwt(&rlestr, false);
        for(auto it : w2) test_isa[p2--] = it;
        test1(test_isa, isa, "ISA");


        vector<INDEX> test_sa;
        ForwardSA<> w3;
        w3.construct_from_rlbwt(&rlestr, false);
        for(auto it : w3) test_sa.push_back(it);
        test1(test_sa, sa, "SA");

        vector<INDEX> test_lcp;
        ForwardLCPArray<> w4;
        w4.construct_from_rlbwt(&rlestr, false);
        for(auto it : w4) test_lcp.push_back(it);
        test1(test_lcp, lcp, "LCP");

        text.pop_back();
}

int main(int argc, char *argv[])
{
using CHAR = char;
//using INDEX = uint64_t;
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


        using RLBWT_TYPE = RLBWT<>;
        RLBWT_TYPE rlestr;
        test(rlestr, text, "Vector");

        //using RLBWT_TYPE1 = RLBWT<std::vector<CHAR>, SDVectorSeq>;
        //RLBWT_TYPE1 rlestr1;
        //test(rlestr1, text, "SDSL");


        using RLBWT_TYPE2 = RLBWT<std::vector<CHAR>, stool::EliasFanoVector>;
        RLBWT_TYPE2 rlestr2;
        test(rlestr2, text, "ELIAS");

        //std::cout << "SDSL" << std::endl;
        //testWithSDSL(text);
        //std::cout << "with EliasFano" << std::endl;
        //testWithEliasFano(text);

        //test_load(inputFile, bwt);

    }else{
        uint64_t len = 10000;
        uint64_t alphabetSize = 3;
        for(int i=0;i<100;i++){
            std::vector<char> text_vec = stool::create_deterministic_integers<char>(len, alphabetSize + 97, 97, i);
            string text;
            for(auto it : text_vec) text.push_back(it);

            using RLBWT_TYPE = RLBWT<>;
            RLBWT_TYPE rlestr;
            test(rlestr, text, "Vector");
            //test(text);

        }
    }
}
