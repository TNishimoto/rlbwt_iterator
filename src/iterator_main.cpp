#include <cassert>
#include <chrono>
#include "common/cmdline.h"
#include "common/io.h"
#include "include/rlbwt.hpp"
#include "include/bwt.hpp"

using namespace std;
using namespace stool;
using namespace stool::rlbwt;

int main(int argc, char *argv[])
{
#ifdef DEBUG
    std::cout << "\033[41m";
    std::cout << "DEBUG MODE!";
    std::cout << "\e[m" << std::endl;
#endif
    cmdline::parser p;
    p.add<string>("input_file", 'i', "input file name", true);
    p.add<string>("output_file", 'o', "output file name (the default output name is 'input_file.ext')", false, "");
    p.add<string>("mode", 'm', "mode", false, "sa");
    p.add<bool>("faster", 'f', "mode2", false, false);

    p.add<bool>("print", 'p', "print output file", false, false);

    p.parse_check(argc, argv);
    string inputFile = p.get<string>("input_file");
    string outputFile = p.get<string>("output_file");
    string mode = p.get<string>("mode");

    bool faster = p.get<bool>("faster");

    bool is_print = p.get<bool>("print");

    if (outputFile.size() == 0)
    {
        if (mode == "sa")
        {
            outputFile = inputFile + ".sa";
        }
        else if (mode == "lcp")
        {
            outputFile = inputFile + ".lcp";
        }
        else if (mode == "lcp_interval")
        {
            outputFile = inputFile + ".lcpi";
        }
        else if (mode == "ms_interval")
        {
            outputFile = inputFile + ".msi";
        }
        else
        {
            outputFile = inputFile + "__";
        }
    }
    /*
    ofstream out(outputFile, ios::out | ios::binary);
    if (!out)
        return 1;

    auto start = std::chrono::system_clock::now();
    RLBWT<char> rlestr;
    rlestr.load(inputFile);

    uint64_t output_num = 0;
    uint64_t rle_size = rlestr.rle_size();
    uint64_t str_size = rlestr.str_size();
    */
    /*
    if (mode == "sa")
    {
        ForwardSA<char> backer(rlestr, faster);
        uint64_t x = 0;
        for (uint64_t p : backer)
        {
            x += p;
        }
        std::cout << "total SA: " << x << std::endl;

        //output_num = IO::online_write<uint64_t, SAIterator<>>(out, backer.begin(), backer.end(), 8192, is_print);
    }
    else if (mode == "lcp")
    {
        ForwardLCPArray<char> backer(rlestr);

        output_num = IO::online_write<uint64_t, LCPIterator<>>(out, backer.begin(), backer.end(), 8192, is_print);
    }
    else if (mode == "lcp_interval")
    {
        IteratorGenerator<char> backer(rlestr);

        output_num = IO::online_write<LCPInterval, PostorderSTIterator>(out, backer.lcp_interval_begin(), backer.lcp_interval_end(), 8192, is_print);
    }
    else if (mode == "ms_interval")
    {
        IteratorGenerator<char> backer(rlestr);

        output_num = IO::online_write<LCPInterval, PostorderMSIterator>(out, backer.maximal_substring_begin(), backer.maximal_substring_end(), 8192, is_print);
    }
    out.close();

    auto end = std::chrono::system_clock::now();
    double decompressionTime = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    std::cout << "\033[36m";
    std::cout << "=============RESULT===============" << std::endl;
    std::cout << "File: " << inputFile << std::endl;
    std::cout << "Output: " << outputFile << std::endl;
    std::cout << "Mode: " << mode << std::endl;
    std::cout << "Speed: " << (faster ? "Faster" : "Normal") << std::endl;

    std::cout << "The number of runs: " << rle_size << std::endl;
    std::cout << "The length of the output text: " << str_size << std::endl;
    std::cout << "The number of output items: " << output_num << std::endl;

    double charperms = (double)(str_size) / decompressionTime;
    std::cout << "Total Running time: " << decompressionTime << "ms";
    std::cout << "[" << charperms << "chars/ms]" << std::endl;
#ifdef DEBUG
    std::cout << "Cache Miss Counter: " << stool::rlbwt::total_cache_miss_counter2 << std::endl;
    std::cout << "rate: " << (stool::rlbwt::total_cache_miss_counter2 / str_size ) << std::endl;
#endif
    std::cout << "==================================" << std::endl;
    std::cout << "\033[39m" << std::endl;
    */
}