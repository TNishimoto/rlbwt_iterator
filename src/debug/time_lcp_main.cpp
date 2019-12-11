#include <cassert>
#include <chrono>
#include "../common/print_rlbwt.hpp"


#include "stool/src/io.hpp"
#include "stool/src/cmdline.h"
#include "stool/src/debug.hpp"
#include "../include/sampling_lcp_construction/sampling_lcp2.hpp"

#include "../include/rlbwt_iterator.hpp"



using namespace std;
using namespace stool;
using namespace stool::rlbwt;

using CHAR = char;
using INDEX = uint64_t;
bool SHOW = false;



int main(int argc, char *argv[])
{
    using CHAR = char;
    using INDEX = uint64_t;
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

    stool::rlbwt::RLBWT<std::vector<CHAR>, std::vector<INDEX> > rlestr = stool::rlbwt::Constructor::load_RLBWT_from_file<CHAR, INDEX>(inputFile);
    std::vector<uint64_t> slcp;
    auto start = std::chrono::system_clock::now();
    if(mode == "old"){
        std::vector<uint64_t> correct_slcp = stool::rlbwt::SamplingLCP<RLBWT<>>::construct_sampling_lcp_array_lorder(rlestr);
        slcp.swap(correct_slcp);
    }else{
        mode = "new";
        std::vector<uint64_t> slcp_new = stool::rlbwt::SamplingLCP2<RLBWT<>>::construct_sampling_lcp_array_lorder(rlestr);
        slcp.swap(slcp_new);
    }
    auto end = std::chrono::system_clock::now();
    double elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    uint64_t textSize = rlestr.str_size();

    std::cout << "\033[36m";
    std::cout << "=============RESULT===============" << std::endl;
    std::cout << "File : " << inputFile << std::endl;
    std::cout << "Mode : " << mode << std::endl;

    std::cout << "The length of the input text : " << textSize << std::endl;
    double charperms = (double)textSize / elapsed;
    std::cout << "The number of runs : " << rlestr.rle_size() << std::endl;
    std::cout << "Ratio : " << (double)rlestr.rle_size() / (double)textSize << std::endl;

    std::cout << "Excecution time : " << elapsed << "ms" << std::endl;
    //std::cout << "[" << charperms << "chars/ms]" << std::endl;
    std::cout << "==================================" << std::endl;
    std::cout << "\033[39m" << std::endl;

}
