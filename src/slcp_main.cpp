#include <cassert>
#include <chrono>
#include "common/cmdline.h"
#include "common/io.h"
#include "include/rlbwt.hpp"
#include "include/bwt.hpp"
//#include "iterator_generator.hpp"
//#include "src/constructor.hpp"

using namespace std;
using namespace stool;
using namespace stool::rlbwt;

int main(int argc, char *argv[])
{
    /*
    cmdline::parser p;
    p.add<string>("input_file", 'i', "input file name", true);
    p.add<string>("output_file", 'o', "output file name (the default output name is 'input_file.ext')", false, "");
    p.add<bool>("print", 'p', "print output file", false, false);

    p.parse_check(argc, argv);
    string inputFile = p.get<string>("input_file");
    string outputFile = p.get<string>("output_file");

    //bool is_print = p.get<bool>("print");

    if (outputFile.size() == 0)
    {
        outputFile = inputFile + ".slcp";

    }

    auto start = std::chrono::system_clock::now();
    std::cout << "Construct RLBWT" << std::endl;
    RLBWT<char> rlestr;
    Constructor::load_from_file(rlestr,inputFile);


    std::cout << "Construct sampling lcp" << std::endl;
    vector<uint64_t> slcp = SamplingLCP<char>::construct_sampling_lcp_array_lorder(rlestr);
    std::cout << "Construct sampling lcp[END]" << std::endl;

    BackwardISA<char> tpb(rlestr);

    std::pair<vector<uint64_t>, vector<uint64_t>>  ssa = SAIterator<>::construct_sampling_sa(rlestr, tpb.begin(), tpb.end());
    ssa.first.resize(0);
    ssa.first.shrink_to_fit();

    vector<uint64_t> slcp_yorder = SamplingLCP<char>::to_succ_sampling_lcp_array_yorder(std::move(slcp),rlestr, ssa.second);

    IO::write(outputFile, slcp_yorder);

    auto end = std::chrono::system_clock::now();
    double decompressionTime = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();


    std::cout << "\033[36m";
    std::cout << "=============RESULT===============" << std::endl;
    std::cout << "File: " << inputFile << std::endl;
    std::cout << "Output: " << outputFile << std::endl;
    double charperms = (double)(rlestr.str_size()) / decompressionTime;
    std::cout << "Total Running time: " << decompressionTime << "ms";
    std::cout << "[" << charperms << "chars/ms]" << std::endl;
    std::cout << "==================================" << std::endl;
    std::cout << "\033[39m" << std::endl;
    */
}