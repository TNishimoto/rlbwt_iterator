#include <cassert>
#include <chrono>
//#include "../common/cmdline.h"
//#include "../common/print.hpp"
//#include "../common/io.h"

#include "stool/include/io.hpp"
#include "stool/include/cmdline.h"
#include "stool/include/debug.hpp"

#include "../include/rlbwt_iterator.hpp"
#include "../include/bwt.hpp"
#include <sdsl/bit_vectors.hpp>
#include "../include/elias_fano_vector.hpp"

using namespace std;
using namespace stool;
using namespace stool::rlbwt;

bool SHOW = false;

/*
template <typename RLBWT_TYPE>
std::pair<uint64_t, uint64_t> test_text(RLBWT_TYPE &rlestr)
{
    std::cout << "\033[47m"
              << "\033[31m"
              << "\033[1m"
              << "Test: Text"
              << "\033[0m" << std::endl;
    auto start = std::chrono::system_clock::now();

    BackwardText<typename RLBWT_TYPE::char_vec_type, typename RLBWT_TYPE::run_vec_type> w;
    w.construct_from_rlbwt(&rlestr, false);
    uint64_t hash = 0;

    for (auto it : w)
    {
        hash ^= it;
    }
    auto end = std::chrono::system_clock::now();
    uint64_t time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    return std::pair<uint64_t, uint64_t>(time, hash);
}
*/

struct TestResult
{
    uint64_t preprocess_time;
    uint64_t iterate_time;
    uint64_t hash;

    TestResult()
    {
    }
    TestResult(uint64_t _preprocess_time, uint64_t _iterate_time, uint64_t _hash)
    {
        this->preprocess_time = _preprocess_time;
        this->iterate_time = _iterate_time;
        this->hash = _hash;
    }
};
template <typename RLBWT_TYPE, typename DATA>
TestResult test1(RLBWT_TYPE &rlestr, DATA &data, std::string name)
{
    std::cout << "\033[47m"
              << "\033[31m"
              << "\033[1m"
              << "Test: " << name
              << "\033[0m" << std::endl;
    auto start1 = std::chrono::system_clock::now();

    //BackwardISA<typename RLBWT_TYPE::run_vec_type> w;

    std::vector<uint64_t> p;
    data.construct_from_rlbwt(&rlestr, false);
    auto end1 = std::chrono::system_clock::now();
    uint64_t time1 = std::chrono::duration_cast<std::chrono::milliseconds>(end1 - start1).count();

    auto start2 = std::chrono::system_clock::now();
    uint64_t hash = 0;
    for (auto it : data)
    {
        hash ^= it;
    }
    data.clear();
    auto end2 = std::chrono::system_clock::now();
    uint64_t time2 = std::chrono::duration_cast<std::chrono::milliseconds>(end2 - start2).count();

    return TestResult(time1, time2, hash);
}

TestResult backward_text_result;
TestResult backward_isa_result;
TestResult forward_sa_result;
TestResult forward_lcp_result;

template <typename RLBWT_TYPE>
TestResult test(RLBWT_TYPE &rlestr, std::string filename, std::string name)
{

std::vector<char> _text;
        stool::IO::load(filename, _text);
        std::string text;
        for (auto &it : _text)
        {
            text.push_back(it);
        }
    Constructor::construct_from_string(rlestr, text);

    BackwardText<typename RLBWT_TYPE::char_vec_type, typename RLBWT_TYPE::run_vec_type> w1;
    backward_text_result = test1(rlestr, w1, "backward text");

    BackwardISA<typename RLBWT_TYPE::run_vec_type> w2;
    backward_isa_result = test1(rlestr, w2, "backward isa");

    ForwardSA<> w3;
    forward_sa_result = test1(rlestr, w3, "forward sa");

    ForwardLCPArray<> w4;
    forward_lcp_result = test1(rlestr, w4, "forward lcp");
}

int main(int argc, char *argv[])
{
    using CHAR = char;
    using INDEX = uint64_t;
    cmdline::parser p;
    p.add<string>("input_file", 'i', "input file name", false, "");
    p.add<string>("mode", 'm', "mode", true);
    p.add<string>("type", 'p', "type", true);

    p.parse_check(argc, argv);
    string inputFile = p.get<string>("input_file");
    string mode = p.get<string>("mode");
    string type = p.get<string>("type");

    if (mode == "input")
    {
        if (inputFile == "")
            throw std::runtime_error("Please input filename");

        if (type == "vector")
        {
            using RLBWT_TYPE = RLBWT<>;
            RLBWT_TYPE rlestr;
            test(rlestr, inputFile, "vector");
        }
        else if (type == "sdsl")
        {
            using RLBWT_TYPE = RLBWT<std::vector<CHAR>, SDVectorSeq>;
            RLBWT_TYPE rlestr;
            //Constructor::construct_from_string(rlestr, text);

            test(rlestr, inputFile, "sdsl");
        }
        else
        {
            type = "elias-fano";
            using RLBWT_TYPE = RLBWT<std::vector<CHAR>, stool::EliasFanoVector>;
            RLBWT_TYPE rlestr;
            test(rlestr, inputFile, "elias");
        }

        std::cout << "\033[31m";
        std::cout << "______________________RESULT______________________" << std::endl;
        std::cout << "File \t\t\t\t\t : " << inputFile << std::endl;
        std::cout << "mode \t\t\t\t\t : " << mode << std::endl;
        std::cout << "vector type \t\t\t\t\t : " << type << std::endl;
        std::cout << "Backward Text[preprocess] \t\t\t\t : " << backward_text_result.preprocess_time << "[ms]" << std::endl;
        std::cout << "Backward Text[iterate] \t\t\t\t\t : " << backward_text_result.iterate_time << "[ms]" << std::endl;
        std::cout << "Backward ISA[preprocess] \t\t\t\t : " << backward_isa_result.preprocess_time << "[ms]" << std::endl;
        std::cout << "Backward ISA[iterate] \t\t\t\t\t : " << backward_isa_result.iterate_time << "[ms]" << std::endl;

        std::cout << "Forward SA[preprocess] \t\t\t\t\t : " << forward_sa_result.preprocess_time << "[ms]" << std::endl;
        std::cout << "Forward SA[iterate] \t\t\t\t\t : " << forward_sa_result.iterate_time << "[ms]" << std::endl;

        std::cout << "Forward LCP[preprocess] \t\t\t\t : " << forward_lcp_result.preprocess_time << "[ms]" << std::endl;
        std::cout << "Forward LCP[iterate] \t\t\t\t\t : " << forward_lcp_result.iterate_time << "[ms]" << std::endl;

        std::cout << "_______________________________________________________" << std::endl;
        std::cout << "\033[39m" << std::endl;
    }
    else
    {
    }
}
