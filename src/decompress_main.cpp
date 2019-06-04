#include <cassert>
#include <chrono>
#include "common/cmdline.h"
#include "common/io.h"
//#include "rlbwt.hpp"
#include "include/bwt.hpp"
//#include "src/constructor.hpp"
#include "include/backward_text.hpp"
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
    p.add<bool>("faster", 'm', "mode", false, false);

    p.parse_check(argc, argv);
    string inputFile = p.get<string>("input_file");
    string outputFile = p.get<string>("output_file");
    bool mode = p.get<bool>("faster");
    string modeName = "";

    if (outputFile.size() == 0)
    {
        outputFile = inputFile + ".txt";
    }

    std::ifstream ifs(inputFile);
    bool inputFileExist = ifs.is_open();
    if (!inputFileExist)
    {
        std::cout << inputFile << " cannot open." << std::endl;
        return -1;
    }

    //string text = "";
    std::cout << "Loading : " << inputFile << std::endl;
    //RLBWT<char> rlestr;

    // Preprocess

    auto start = std::chrono::system_clock::now();
    //rlestr.load(inputFile);
    RLBWT<char> rlestr = Constructor::load_RLBWT_from_file(inputFile);
    BackwardText<char> backer;
    backer.construct_from_rlbwt(&rlestr, true);
    //Constructor::load_from_file(rlestr,inputFile);
    rlestr.check_rlbwt();
    uint64_t textSize = rlestr.str_size();
    uint64_t rleSize = rlestr.rle_size();

    //BackwardText<char> generator(rlestr, mode);

    ofstream out(outputFile, ios::out | ios::binary);
    if (!out)
        return 1;
    // Decomperss

    IO::online_write<char, BackwardTextIterator<char>>(out, backer.begin(), backer.end(), 8192, false);
    auto end = std::chrono::system_clock::now();
    double decompressionTime = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();


    std::cout << "\033[36m";
    std::cout << "=============RESULT===============" << std::endl;
    std::cout << "File: " << inputFile << std::endl;
    std::cout << "Output: " << outputFile << std::endl;

    std::cout << "Mode: " << (mode ? "Faster" : "Normal") << std::endl;
    std::cout << "The number of runs: " << rleSize << std::endl;
    std::cout << "The length of the output text: " << textSize << std::endl;
    double totalRunningTime = decompressionTime;
    double charperms = (double)textSize / totalRunningTime;
    std::cout << "Total Running time: " << totalRunningTime << "ms";
    std::cout << "[" << charperms << "chars/ms]" << std::endl;
#ifdef DEBUG    
    std::cout << "Cache Miss Counter: " << stool::rlbwt::total_cache_miss_counter1 << std::endl;
    std::cout << "rate: " << (stool::rlbwt::total_cache_miss_counter1 / textSize) << std::endl;
#endif

    std::cout << "==================================" << std::endl;
    std::cout << "\033[39m" << std::endl;
    
}