#include <cassert>
#include <chrono>
//#include "../common/cmdline.h"
//#include "../common/print.hpp"
//#include "../common/io.h"

#include "../include/rlbwt_iterator.hpp"
#include "../include/bwt.hpp"
#include "stool/src/io.hpp"
#include "stool/src/cmdline.h"


using namespace std;
using namespace stool;
using namespace stool::rlbwt;

using CHAR = char;
using INDEX = uint64_t;

std::vector<INDEX> constructLCPX(const std::string &text,const  std::vector<INDEX> &sa,const  std::vector<INDEX> &isa)
{
    std::vector<INDEX> lcp;
    lcp.resize(text.size(), 0);
    INDEX n = text.size();
    INDEX k = 0;
    stool::Counter counter;
    if(text.size() > 1000000)std::cout << "Constructing LCP Array"  << std::flush;

    int pk = 0;

    for (INDEX i = 0; i < n; i++)
    {
        if(n > 1000000)counter.increment();

        INDEX x = isa[i];


        if (x == 0)
        {
            lcp[x] = 0;
        }
        else
        {
            //std::cout << n << "/" << x << "/" << sa[x] << "/" << sa[x-1] << "/" << k << std::endl;
            //std::cout << ((sa[x] + k) - pk ) << ", " << std::flush;
            //pk = sa[x] + k;
            std::cout << ((sa[x] + k) - pk ) << ", " << std::flush;
            pk = sa[x] + k;

            while (sa[x] + k < n && sa[x - 1] + k < n && text[sa[x] + k] == text[sa[x - 1] + k])
            {
                std::cout << ((sa[x] + k) - pk ) << ", " << std::flush;
                pk = sa[x] + k;


                k++;
            }
        }
        lcp[x] = k;

        assert(x == 0 || (x > 0 && ( ( n - sa[x-1]) >= k )  ) );

        if (k > 0)
            k--;
    }


    if(n > 1000000)std::cout << "[END]" << std::endl;
    return lcp;
}

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

    //string text = "";
    std::cout << "Loading : " << inputFile << std::endl;
    string text = stool::load_string_from_file(inputFile, false);
    
    vector<INDEX> sa = stool::rlbwt::SuffixArrayConstructor::naive_sa<INDEX>(text);
    vector<INDEX> isa = stool::rlbwt::SuffixArrayConstructor::construct_isa(sa);

    auto lcpArray = constructLCPX(text,sa,isa);
    //Printer::print(lcpArray);

}
