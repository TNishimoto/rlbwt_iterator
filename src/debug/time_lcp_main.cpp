#include <cassert>
#include <chrono>
#include "../common/print_rlbwt.hpp"

#include "stool/include/io.hpp"
#include "stool/include/cmdline.h"
#include "stool/include/debug.hpp"
#include "../include/sampling_lcp/succinct_slcp_constructor.hpp"

#include "../include/rlbwt_iterator.hpp"
#include "../include/weiner/weiner.hpp"
#include "../include/weiner/hyper_weiner.hpp"

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

    //
    uint64_t rlbwt_memory = 0;
    uint64_t rle_size = 0;
    uint64_t textSize = 0;
    std::vector<uint64_t> slcp;
    auto start = std::chrono::system_clock::now();
    if (mode == "old")
    {
        stool::rlbwt::RLBWT<std::vector<CHAR>, std::vector<INDEX>> rlestr = stool::rlbwt::Constructor::load_RLBWT_from_file<CHAR, INDEX>(inputFile);
        //rlbwt_memory = rlestr.get_using_memory();
        rle_size = rlestr.rle_size();
        textSize = rlestr.str_size();
        std::vector<uint64_t> correct_slcp = stool::rlbwt::PracticalSamplingLCPConstructor<RLBWT<>>::construct_sampling_lcp_array_lorder(rlestr);
        slcp.swap(correct_slcp);
    }
    else if (mode == "sdsl")
    {

        //using RLBWT_STR = stool::rlbwt::RLBWT<std::vector<CHAR>, stool::EliasFanoVector>;
        using RLBWT_STR = stool::rlbwt::RLBWT<std::vector<CHAR>, std::vector<INDEX>>;
        RLBWT_STR rlestr = stool::rlbwt::Constructor::load_RLBWT_from_file<CHAR, INDEX>(inputFile);
        //rlbwt_memory = rlestr.get_using_memory();

        std::vector<uint64_t> slcp_new = stool::rlbwt::SuccinctSLCPConstructor<RLBWT_STR>::construct_sampling_lcp_array_lorder(rlestr, true);
        rle_size = rlestr.rle_size();
        textSize = rlestr.str_size();
        slcp.swap(slcp_new);
    }
    else if (mode == "weiner"){
        using RLBWT_STR = stool::rlbwt::RLBWT<std::vector<CHAR>, std::vector<INDEX>>;
        RLBWT_STR rlestr = stool::rlbwt::Constructor::load_RLBWT_from_file<CHAR, INDEX>(inputFile);

        std::cout << "plain : " << (rlestr.rle_size() * sizeof(uint64_t)) << "Bytes" << std::endl; 

        //std::vector<uint64_t> slcp_new = stool::rlbwt::SamplingLCPArrayConstructor<RLBWT_STR>::construct_sampling_lcp_array_lorder(rlestr);
        std::vector<uint64_t> slcp_new = stool::rlbwt::HyperSamplingLCPArrayConstructor<RLBWT_STR>::construct_sampling_lcp_array_lorder(rlestr, true);

        rle_size = rlestr.rle_size();
        textSize = rlestr.str_size();
        slcp.swap(slcp_new);

    }
    else
    {
        using RLBWT_STR = stool::rlbwt::RLBWT<std::vector<CHAR>, std::vector<INDEX>>;
        RLBWT_STR rlestr = stool::rlbwt::Constructor::load_RLBWT_from_file<CHAR, INDEX>(inputFile);

        std::cout << "plain : " << (rlestr.rle_size() * sizeof(uint64_t)) << "Bytes" << std::endl; 
        //std::cout << "elias : " << (rlestr.get_run_vec()->get_using_memory() ) << "Bytes" << std::endl; 
        //rlbwt_memory = rlestr.get_using_memory();

        mode = "new";

        std::vector<uint64_t> slcp_new = stool::rlbwt::SuccinctSLCPConstructor<RLBWT_STR>::construct_sampling_lcp_array_lorder(rlestr, false);
        rle_size = rlestr.rle_size();
        textSize = rlestr.str_size();
        slcp.swap(slcp_new);
    }
    auto end = std::chrono::system_clock::now();
    double elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    std::cout << "\033[36m";
    std::cout << "=============RESULT===============" << std::endl;
    std::cout << "File : " << inputFile << std::endl;
    std::cout << "Mode : " << mode << std::endl;

    std::cout << "The length of the input text : " << textSize << std::endl;
    //double charperms = (double)textSize / elapsed;
    std::cout << "The number of runs : " << rle_size << std::endl;
    std::cout << "Ratio : " << (double)rle_size / (double)textSize << std::endl;
    std::cout << "The memory of RLBWT : " << rlbwt_memory << std::endl;

    std::cout << "Excecution time : " << elapsed << "ms" << std::endl;
    //std::cout << "[" << charperms << "chars/ms]" << std::endl;
    std::cout << "==================================" << std::endl;
    std::cout << "\033[39m" << std::endl;
}
