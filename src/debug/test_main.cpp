#include <cassert>
#include <chrono>
#include "../common/cmdline.h"
#include "../common/print.hpp"
#include "../common/io.h"
#include "../common/print_rlbwt.hpp"

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

    std::cout << "Text length = " << text.size() << std::endl;
    if (text.size() <= 100)
    {
        std::cout << "Text: ";
        std::cout << text << std::endl;
    }

    RLBWT<char>::check_text_for_rlbwt(text);

    std::string bwt = stool::rlbwt::SuffixArrayConstructor::naive_bwt(text);

    std::vector<uint64_t> rlbwt_runs;
    std::vector<char> rlbwt_chars;
    stool::rlbwt::SuffixArrayConstructor::naive_rlbwt(text, rlbwt_chars, rlbwt_runs);
    //Printer::print_chars(rlbwt_chars);
    //Printer::print(rlbwt_runs);

    
    //std::cout << "sa" << std::endl;
    //Printer::print(sa);

    std::vector<uint64_t> lf_mapper = RLBWTFunctions::construct_rle_lf_mapper<INDEX>(rlestr);
    std::vector<uint64_t> fl_mapper = RLBWTFunctions::construct_rle_fl_mapper<INDEX>(rlestr);
    std::vector<uint64_t> fpos_array = RLBWTFunctions::construct_fpos_array<INDEX>(rlestr);


    
    vector<INDEX> sa = stool::rlbwt::SuffixArrayConstructor::naive_sa<INDEX>(text);
    //string bwt = stool::rlbwt::SuffixArrayConstructor::construct_bwt(text, sa);
    vector<INDEX> isa = stool::rlbwt::SuffixArrayConstructor::construct_isa(sa);
    vector<INDEX> lcp = stool::rlbwt::SuffixArrayConstructor::construct_lcp(text, sa, isa);
    


    vector<uint64_t> slcp = SamplingLCP<>::construct_sampling_lcp_array_lorder(rlestr);

    vector<uint64_t> slcp_forder = stool::rlbwt::permutate(std::move(slcp), lf_mapper);

    //ForwardLCPArray<> w2;
    //w2.construct_from_rlbwt(&rlestr, false);
    //vector<uint64_t> lcp2 = w2.to_lcp_array();
    //vector<uint64_t> slcp = w2.copy_slcp_array();
    //stool::Printer::print(slcp);
    RLBWTPrinter::printText(text);
    RLBWTPrinter::printRLBWTIndexes(rlbwt_runs);
    RLBWTPrinter::printOnRLBWT(rlbwt_chars, rlbwt_runs, "L(RLBWT)");
    RLBWTPrinter::printOnRLBWT(lf_mapper, rlbwt_runs, "lf_mapper");
    RLBWTPrinter::printOnRLBWT(fl_mapper, rlbwt_runs, "fl_mapper");
    RLBWTPrinter::printOnRLBWT(fpos_array, rlbwt_runs, "fpos_array");
    RLBWTPrinter::printWithFOrder(slcp_forder, rlbwt_runs, lf_mapper, "slcp_array");
    RLBWTPrinter::print(lcp, "lcp_array");




    /*
    */

    /*
    test_sampling_lcp(text);
    test_sa2(text);

    test_ms(text);
    */
}
