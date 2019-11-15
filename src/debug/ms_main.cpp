#include <cassert>
#include <chrono>
#include "common/cmdline.h"
#include "common/io.h"
#include "include/rlbwt.hpp"
#include "include/bwt.hpp"
#include "include/postorder_maximal_substrings.hpp"

using namespace std;
using namespace stool;
using namespace stool::rlbwt;

int main(int argc, char *argv[])
{
    cmdline::parser p;
    p.add<string>("input_file", 'i', "input file name", true);
    p.add<string>("output_file", 'o', "output file name (the default output name is 'input_file.ext')", false, "");
    p.add<string>("slcp_file", 'm', "mode", false, "sa");
    p.add<bool>("print", 'p', "print output file", false, false);

    p.parse_check(argc, argv);
    string inputFile = p.get<string>("input_file");
    string outputFile = p.get<string>("output_file");
    string slcpFile = p.get<string>("slcp_file");

    bool is_print = p.get<bool>("print");

    if (outputFile.size() == 0)
    {
        outputFile = inputFile + ".msi";
    }

    ofstream out(outputFile, ios::out | ios::binary);
    if (!out)
        return 1;

    auto start = std::chrono::system_clock::now();
    
    RLBWT<char> rlestr = Constructor::load_RLBWT_from_file(inputFile);
    ///PostorderMaximalSubstrings<> ms = Constructor::construct_postorder_maximal_substrings<char,uint64_t, RLBWT<> >(&rlestr);

    rlbwt::PostorderMaximalSubstrings<> ms;
    
    ms.construct_from_rlbwt(&rlestr, false);
    /*
    vector<uint64_t> slcp;
    IO::load<uint64_t>(slcpFile, slcp);
    backer.load_sampling_lcp(std::move(slcp));
    */

    uint64_t output_num = 0;
    output_num = IO::online_write<LCPInterval<uint64_t>, PostorderMSIterator<uint64_t, vector<uint64_t> > >(out, ms.begin(), ms.end(), 1000, is_print);

    out.close();

    auto end = std::chrono::system_clock::now();
    double decompressionTime = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    std::cout << "\033[36m";
    std::cout << "=============RESULT===============" << std::endl;
    std::cout << "File: " << inputFile << std::endl;
    std::cout << "Output: " << outputFile << std::endl;
    //std::cout << "Mode: " << mode << std::endl;

    std::cout << "The number of runs: " << rlestr.rle_size() << std::endl;
    std::cout << "The length of the output text: " << rlestr.str_size() << std::endl;
    std::cout << "The number of output items: " << output_num << std::endl;

    double charperms = (double)(rlestr.str_size()) / decompressionTime;
    std::cout << "Total Running time: " << decompressionTime << "ms";
    std::cout << "[" << charperms << "chars/ms]" << std::endl;
    std::cout << "==================================" << std::endl;
    std::cout << "\033[39m" << std::endl;
}