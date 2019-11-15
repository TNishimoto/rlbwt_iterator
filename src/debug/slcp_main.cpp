#include <cassert>
#include <chrono>
#include "../common/cmdline.h"
#include "../common/print.hpp"
#include "../common/io.h"

#include "../include/rlbwt_iterator.hpp"



using namespace std;
using namespace stool;
using namespace stool::rlbwt;

using CHAR = char;
using INDEX = uint64_t;
bool SHOW = false;



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
    /*
    std::cout << "Text length = " << text.size() << std::endl;
    if (text.size() <= 100)
    {
        std::cout << "Text: ";
        std::cout << text << std::endl;
    }
    */

    RLBWT<char>::check_text_for_rlbwt(text);
    vector<uint64_t> slcp = SamplingLCP<>::construct_sampling_lcp_array_lorder(rlestr);

    std::cout << "text length: " << text.size() << std::endl;
    std::cout << "rle length: " << rlestr.rle_size() << std::endl;


    /*
    vector<INDEX> sa = stool::rlbwt::SuffixArrayConstructor::naive_sa<INDEX>(text);
    std::cout << "sa" << std::endl;
    Printer::print(sa);

    std::string bwt = stool::rlbwt::SuffixArrayConstructor::naive_bwt(text);
    std::cout << bwt << std::endl;

    std::vector<uint64_t> rlbwt_runs;
    std::vector<char> rlbwt_chars;
    stool::rlbwt::SuffixArrayConstructor::naive_rlbwt(text, rlbwt_chars, rlbwt_runs);

    Printer::print_chars(rlbwt_chars);
    Printer::print(rlbwt_runs);
    */
    /*
    ForwardLCPArray<> w2;
    w2.construct_from_rlbwt(&rlestr, false);
    vector<uint64_t> lcp2 = w2.to_lcp_array();
    stool::Printer::print(lcp2);
    */

    /*
    */

    /*
    test_sampling_lcp(text);
    test_sa2(text);

    test_ms(text);
    */
}
