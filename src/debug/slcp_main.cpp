#include <cassert>
#include <chrono>
//#include "../common/cmdline.h"
//#include "../common/print.hpp"
//#include "../common/io.h"

#include "../include/rlbwt_iterator.hpp"
#include "../include/bwt.hpp"
#include "../include/sampling_lcp/succinct_slcp_constructor.hpp"
#include "../include/weiner/weiner.hpp"
#include "../include/weiner/hyper_weiner.hpp"

#include "stool/include/io.hpp"
#include "stool/include/cmdline.h"
#include "stool/include/debug.hpp"


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
    string mode = p.get<string>("mode");

    std::ifstream ifs(inputFile);
    bool inputFileExist = ifs.is_open();
    if (!inputFileExist)
    {
        std::cout << inputFile << " cannot open." << std::endl;
        return -1;
    }

    //string text = "";
    std::cout << "Loading : " << inputFile << std::endl;
std::vector<char> _text;
        stool::IO::load(inputFile, _text);
        std::string text;
        for (auto &it : _text)
        {
            text.push_back(it);
        }

    RLBWT<> rlestr;
    //Constructor::construct_from_bwt<CHAR, INDEX>(rlestr, bwt);
    Constructor::construct_from_string(rlestr, text);

    text.push_back((char)0);

    std::cout << "Text length = " << text.size() << std::endl;
    if (text.size() <= 100)
    {
        std::cout << "Text: ";
        std::cout << text << std::endl;
    }

    RLBWT<>::check_text_for_rlbwt(text);

    /*
    ForwardLCPArray<> fsa;
    fsa.construct_from_rlbwt(&rlestr, false);

    fsa.print_info();
    */

    std::vector<uint64_t> correct_slcp = stool::rlbwt::PracticalSamplingLCPConstructor<RLBWT<>>::construct_sampling_lcp_array_lorder(rlestr);


    //std::vector<uint64_t> slcp = stool::rlbwt::SuccinctSLCPConstructor<RLBWT<>>::construct_sampling_lcp_array_lorder(rlestr, mode == "sdsl");
    std::vector<uint64_t> slcp = stool::rlbwt::HyperSamplingLCPArrayConstructor<RLBWT<>>::construct_sampling_lcp_array_lorder(rlestr);

    //stool::Printer::print(slcp);
    /*
    auto slcp2 = stool::rlbwt::HyperWeiner<RLBWT<>>::construct_sampling_lcp_array(rlestr);
    auto slcp3 = stool::rlbwt::Weiner<RLBWT<>>::construct_sampling_lcp_array(rlestr);
    if(slcp2.size() < 100){
        stool::Printer::print(slcp2);
        stool::Printer::print(slcp3);


    }


    bool b1 = stool::equal_check(slcp2, slcp3);
    if(b1){
        std::cout << "OK!" << std::endl;
    }
    */
    
    //stool::Printer::print(correct_slcp);
    //stool::Printer::print(slcp2);


    std::vector<uint64_t> lf = RLBWTFunctions::construct_rle_lf_mapper(rlestr);
    std::vector<uint64_t> slcp_forder = stool::rlbwt::permutate(std::move(slcp), lf);
    std::vector<uint64_t> correct_slcp_forder = stool::rlbwt::permutate(std::move(correct_slcp), lf);


    bool b1 = stool::equal_check(correct_slcp_forder, slcp_forder );
    if(b1){
        std::cout << "OK!" << std::endl;
    }
    if(correct_slcp_forder.size() < 100){
        stool::Printer::print(correct_slcp_forder);
        stool::Printer::print(slcp_forder);


    }
    

}
