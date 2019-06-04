#include <cassert>
#include <chrono>
#include "common/cmdline.h"
#include "common/io.h"
#include "include/rlbwt.hpp"
#include "include/bwt.hpp"
//#include "src/constructor.hpp"
#include "online_rlbwt.hpp"

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

    std::cout << text << std::endl;

    //if(isReverse)stool::StringFunctions::reverse(text);

    //vector<LZFactor> factors;

    auto start = std::chrono::system_clock::now();
    RLBWT<char> rlestr;

    /*
    vector<char> cVec;
    vector<uint64_t> nVec;
    itmmti::online_rlbwt_from_file(inputFile, cVec, nVec, 1);
    Printer::print(cVec);
    Printer::print(nVec);
    */

    if (inputType == "text")
    {
        text += (char)0;
        if (outputType == "rlbwt")
        {
            if (outputFile.size() == 0)
            {
                outputFile = inputFile + ".rlbwt";
            }
            Constructor::construct_from_file<char, uint64_t>(rlestr, inputFile);

            Printer::print(*rlestr.get_char_vec());
            Printer::print(*rlestr.get_run_vec() );

            rlestr.write(outputFile);
        }
        else
        {
            if (outputFile.size() == 0)
            {
                outputFile = inputFile + ".bwt";
            }
            string bwt = "";
            bwt = itmmti::online_bwt(text);

            stool::IO::write(outputFile, bwt);
        }
    }
    else
    {
        if (outputFile.size() == 0)
        {
            outputFile = inputFile + ".rlbwt";
        }

        Constructor::construct_from_bwt<char, uint64_t>(rlestr, text);
        rlestr.write(outputFile);
    }
    auto end = std::chrono::system_clock::now();
    double elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    std::cout << "\033[36m";
    std::cout << "=============RESULT===============" << std::endl;
    std::cout << "File : " << inputFile << std::endl;
    std::cout << "FileType : " << inputType << std::endl;
    std::cout << "Output : " << outputFile << std::endl;
    std::cout << "OutputType : " << outputType << std::endl;

    std::cout << "The length of the input text : " << text.size() << std::endl;
    double charperms = (double)text.size() / elapsed;
    std::cout << "The number of runs : " << rlestr.rle_size() << std::endl;
    std::cout << "Excecution time : " << elapsed << "ms";
    std::cout << "[" << charperms << "chars/ms]" << std::endl;
    std::cout << "==================================" << std::endl;
    std::cout << "\033[39m" << std::endl;
}