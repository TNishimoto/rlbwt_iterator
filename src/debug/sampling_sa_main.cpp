#include <cassert>
#include <chrono>
//#include "../common/cmdline.h"
//#include "../common/print.hpp"
//#include "../common/io.h"

#include "../include/rlbwt_iterator.hpp"
#include "../include/bwt.hpp"
#include "../include/sampling_lcp/succinct_slcp_constructor.hpp"
#include "../include/weiner/weiner.hpp"
#include "../include/weiner/hyper_weiner.hpp"

#include "stool/src/io.hpp"
#include "stool/src/cmdline.h"
#include "stool/src/debug.hpp"

using namespace std;
using namespace stool;
using namespace stool::rlbwt;

using CHAR = char;
using INDEX = uint64_t;

int main(int argc, char *argv[])
{
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

    std::cout << "Loading : " << inputFile << std::endl;
    string text = stool::load_string_from_file(inputFile, false);

    RLBWT<> rlestr;
    //Constructor::construct_from_bwt<CHAR, INDEX>(rlestr, bwt);
    Constructor::construct_from_string(rlestr, text);

    text.push_back((char)0);

    std::cout << "Text length = " << text.size() << std::endl;
    if (text.size() <= 100)
    {
        std::cout << "Text: ";
        std::cout << text << std::endl;
    }

    RLBWT<>::check_text_for_rlbwt(text);

    if (true)
    {
        using POWVEC = typename RLBWT_STR::run_vec_type;
        stool::rlbwt::BackwardISA<POWVEC, std::vector<INDEX>> tpb;
        tpb.construct_from_rlbwt(rlestr);
        std::pair<std::vector<INDEX>, std::vector<INDEX>> r = stool::rlbwt::ForwardSA::construct_sampling_sa_lorder(rlestr, tpb.begin(), tpb.end());
        tpb.clear();
    }
}
