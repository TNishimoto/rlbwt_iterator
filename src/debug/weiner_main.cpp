#include <cassert>
#include <chrono>

#include "../include/rlbwt_iterator.hpp"
#include "../include/bwt.hpp"
#include "../include/weiner/weiner.hpp"

#include "stool/src/io.hpp"
#include "stool/src/cmdline.h"
#include "stool/src/debug.hpp"


using namespace std;
using namespace stool;
using namespace stool::rlbwt;

using CHAR = char;
using INDEX = uint64_t;

int main(int argc, char *argv[])
{
    cmdline::parser p;
    p.add<string>("input_file", 'i', "input file name", true);
    p.add<string>("mode", 'm', "mode", false, "xx");

    p.parse_check(argc, argv);
    string inputFile = p.get<string>("input_file");

    std::ifstream ifs(inputFile);
    bool inputFileExist = ifs.is_open();
    if (!inputFileExist)
    {
        std::cout << inputFile << " cannot open." << std::endl;
        return -1;
    }

    std::cout << "Loading : " << inputFile << std::endl;
    string text = stool::load_string_from_file(inputFile, false);

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

    vector<INDEX> sa = stool::rlbwt::SuffixArrayConstructor::naive_sa<INDEX>(text);
    vector<INDEX> isa = stool::rlbwt::SuffixArrayConstructor::construct_isa(sa);
    vector<INDEX> lcpArray = stool::rlbwt::SuffixArrayConstructor::construct_lcp(text, sa, isa);


    auto weiner = stool::rlbwt::Weiner<RLBWT<>>(rlestr);
    vector<INDEX> testLCPArray = weiner.construct_lcp_array();
    stool::equal_check(lcpArray, testLCPArray);
    std::cout << "OK!" << std::endl;
}
