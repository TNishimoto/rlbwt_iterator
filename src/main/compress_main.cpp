#include <cassert>
#include <chrono>
//#include "../common/cmdline.h"
//#include "../common/io.h"
//#include "src/constructor.hpp"
#include "stool/include/io.hpp"
#include "stool/include/cmdline.h"
#include "OnlineRlbwt/online_rlbwt.hpp"
#include "../include/rlbwt_iterator.hpp"

using namespace std;
using namespace stool;
using namespace stool::rlbwt;

int main(int argc, char *argv[])
{
    cmdline::parser p;
    p.add<string>("input_file", 'i', "input file name", true);
    p.add<string>("output_file", 'o', "output file name (the default output name is 'input_file.ext')", false, "");
    p.add<string>("input_type", 't', "input_type", false, "text");
    p.add<string>("output_type", 'y', "output_type", false, "rlbwt");

    p.parse_check(argc, argv);
    string inputFile = p.get<string>("input_file");
    string outputFile = p.get<string>("output_file");
    string inputType = p.get<string>("input_type");
    string outputType = p.get<string>("output_type");

    uint64_t textSize = 0;

    outputType = outputType == "rlbwt" ? "rlbwt" : "bwt";
    inputType = inputType == "text" ? "text" : "rlbwt";

    auto start = std::chrono::system_clock::now();
    RLBWT<> rlestr;

    if (inputType == "text")
    {
        Constructor::construct_from_file(rlestr, inputFile);
    }
    else
    {
        std::vector<char> _text;
        stool::IO::load(inputFile, _text);
        std::string text;
        for (auto &it : _text)
        {
            text.push_back(it);
        }
        Constructor::construct_from_bwt(rlestr, text);
    }
    textSize = rlestr.str_size();

    if (outputType == "rlbwt")
    {
        if (outputFile.size() == 0)
        {
            outputFile = inputFile + ".rlbwt";
        }
        rlestr.write(outputFile);
    }
    else
    {
        if (outputFile.size() == 0)
        {
            outputFile = inputFile + ".bwt";
        }

        using BWT_RLBWT = stool::rlbwt::ForwardBWT<>;
        BWT_RLBWT bwt_rlbwt(&rlestr);

        std::vector<char> bwt;
        for (auto it : bwt_rlbwt)
        {
            bwt.push_back(it);
        }
        stool::write_vector(outputFile, bwt, false);
    }

    auto end = std::chrono::system_clock::now();
    double elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    std::cout << "\033[36m";
    std::cout << "=============RESULT===============" << std::endl;
    std::cout << "File : " << inputFile << std::endl;
    std::cout << "FileType : " << inputType << std::endl;
    std::cout << "Output : " << outputFile << std::endl;
    std::cout << "OutputType : " << outputType << std::endl;

    std::cout << "The length of the input text : " << textSize << std::endl;
    double charperms = (double)textSize / elapsed;
    std::cout << "The number of runs : " << rlestr.rle_size() << std::endl;
    std::cout << "Excecution time : " << elapsed << "ms";
    std::cout << "[" << charperms << "chars/ms]" << std::endl;
    std::cout << "==================================" << std::endl;
    std::cout << "\033[39m" << std::endl;
}