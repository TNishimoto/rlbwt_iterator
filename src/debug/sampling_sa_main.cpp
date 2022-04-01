#include <cassert>
#include <chrono>
//#include "../common/cmdline.h"
//#include "../common/print.hpp"
//#include "../common/io.h"

#include "../include/rlbwt_iterator.hpp"
#include "../include/bwt.hpp"

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
    p.add<string>("mode", 'm', "mode", false, "faster");

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
    using RLBWT_STR = stool::rlbwt::RLBWT<std::vector<CHAR>, std::vector<INDEX>>;
    RLBWT_STR rlestr = stool::rlbwt::Constructor::load_RLBWT_from_file<CHAR, INDEX>(inputFile);

    std::pair<std::vector<INDEX>, std::vector<INDEX>> ssa;
    auto start = std::chrono::system_clock::now();

    if (mode == "standard")
    {
        using POWVEC = typename RLBWT_STR::run_vec_type;
        stool::rlbwt::BackwardISA<POWVEC, std::vector<INDEX>> tpb;
        tpb.construct_from_rlbwt(&rlestr, false);
        std::pair<std::vector<INDEX>, std::vector<INDEX>> r = stool::rlbwt::ForwardSA<>::iterator::construct_sampling_sa_lorder(rlestr, tpb.begin(), tpb.end());
        tpb.clear();
        ssa.first.swap(r.first);
        ssa.second.swap(r.second);
    }
    else if (mode == "analyze")
    {
        std::vector<uint64_t> thresholds;
        thresholds.push_back(8);
        thresholds.push_back(16);
        thresholds.push_back(32);
        thresholds.push_back(64);

        for (auto threshold : thresholds)
        {
            std::vector<bool> filterVec;
            auto lf_mapper = stool::rlbwt::RLBWTFunctions2::construct_rle_lf_lorder<RLBWT_STR>(rlestr, filterVec, threshold);

            uint64_t sum = 0;
            for (uint64_t i = 0; i < rlestr.rle_size(); i++)
            {
                if (filterVec[i])
                {
                    sum += rlestr.get_run(i);
                }
            }
            std::cout << "Threshold: " << threshold << std::endl;
            std::cout << "Skipped predecessor queries: " << sum << std::endl;
        }
    }
    else
    {
        mode = "faster";
        using POWVEC = typename RLBWT_STR::run_vec_type;
        stool::rlbwt::BackwardISA<POWVEC, std::vector<INDEX>> tpb;
        tpb.construct_from_rlbwt(&rlestr, true);
        std::pair<std::vector<INDEX>, std::vector<INDEX>> r = stool::rlbwt::ForwardSA<>::iterator::construct_sampling_sa_lorder(rlestr, tpb.begin(), tpb.end());
        tpb.clear();
        ssa.first.swap(r.first);
        ssa.second.swap(r.second);
    }
    if (rlestr.rle_size() < 100)
    {
        stool::Printer::print(ssa.first);
        stool::Printer::print(ssa.second);
    }

    auto end = std::chrono::system_clock::now();
    double elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    uint64_t check_sum = 0;
    for (uint64_t i = 0; i < ssa.first.size(); i++)
    {
        check_sum += ssa.first[i] + ssa.second[i];
    }

    std::cout << "OK!" << std::endl;

    std::cout << "\033[36m";
    std::cout << "=============RESULT===============" << std::endl;
    std::cout << "File : " << inputFile << std::endl;
    std::cout << "Mode : " << mode << std::endl;

    std::cout << "The length of the input text : " << rlestr.str_size() << std::endl;
    std::cout << "The number of runs : " << rlestr.rle_size() << std::endl;
    std::cout << "Checksum : " << check_sum << std::endl;

    std::cout << "Excecution time : " << elapsed << "ms" << std::endl;
    std::cout << "==================================" << std::endl;
    std::cout << "\033[39m" << std::endl;
}
