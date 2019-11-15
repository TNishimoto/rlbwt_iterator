#include <cassert>
#include <chrono>
#include "../src/common/cmdline.h"
#include "../src/common/print.hpp"
#include "../src/common/io.h"

#include "rlbwt_iterator/rlbwt_iterator.hpp"



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

    std::cout << "Text length = " << text.size() << std::endl;
    if (text.size() <= 100)
    {
        std::cout << "Text: ";
        std::cout << text << std::endl;
    }

    RLBWT<char>::check_text_for_rlbwt(text);

    ForwardLCPArray<> w2;
    w2.construct_from_rlbwt(&rlestr, false);
    vector<uint64_t> lcp2 = w2.to_lcp_array();
    stool::Printer::print(lcp2);
    /*
    */

    /*
    test_sampling_lcp(text);
    test_sa2(text);

    test_ms(text);
    */
}
